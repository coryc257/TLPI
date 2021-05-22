/*
 * main.c
 *
 *  Created on: May 21, 2021
 *      Author: cory
 */

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define pp(pos) printf("POS:%ld\n",pos)


typedef struct line_part {
	char *nnlt_line_part;
	int len;
	struct line_part *next_part;
	struct line_part *prev_part;
};
typedef struct current_line {
	struct line_part *head;
	struct line_part *cur;
	size_t total_size;
	off_t pos;
	long file_size;
	long first_size;
	long bytes_remaining;
	int fd;
	int block_size;
	long num_lines;

	struct line_part *output_cur;
};

static void
cl_print(struct current_line *cl)
{
	struct line_part *item, *swap;
	item = cl->output_cur;
	while (item != NULL)
	{
		for (int j = 0; j < item->len; j++)
		{
			printf("%c",item->nnlt_line_part[j]);
		}
		swap = item;
		item = item->prev_part;
		free(swap->nnlt_line_part);
		free(swap);
	}
}

static void
output_string(struct current_line *cl, char *str, int len, int index)
{
	struct line_part *x;
	char *copy;
	x = malloc(sizeof(struct line_part));
	if(x == NULL)
		errExit("malloc");


	if(index == 0)
	{
		x->len = len;
		x->nnlt_line_part = str;
	}
	else
	{
		copy = malloc(sizeof(char)*len);
		for(int j = 0; j < len; j++)
		{
			copy[j]=str[j];
		}
		x->len = len;
		x->nnlt_line_part = copy;
	}

	if(cl->output_cur == NULL)
	{
		cl->output_cur = x;
		cl->output_cur->prev_part = NULL;
	}
	else
	{
		x->prev_part = cl->output_cur;
		cl->output_cur = x;
	}
}

static int
cascade_print(struct current_line *cl,
		char *str, int len, int index,
		struct line_part *cascade, size_t *deleted)
{
	char *local;
	void *swap;
	local = str;

	if(str == NULL || len < 0)
		return -1;


	output_string(cl, "\n", 1, 1);
	*deleted = 0;
	output_string(cl,str,len,index);
	*deleted = *deleted + len;
	while(cascade != NULL)
	{
		cascade->next_part->prev_part = NULL;
		if(cascade->len < 0 || cascade->nnlt_line_part == NULL)
			return -1;
		output_string(cl,cascade->nnlt_line_part, cascade->len, 0);
		*deleted = *deleted + cascade->len;
		//free(cascade->nnlt_line_part);
		swap = cascade;
		cascade = cascade->prev_part;
		free(swap);
	}
	//printf("\n");

	cl->num_lines--;
	return 0;
}

static int
check_line(struct current_line *cl)
{
	size_t check_size 	= cl->total_size;
	size_t str_pos 		= 0;
	size_t eaten_bytes  = 0;
	struct line_part	*cur;
	struct line_part	*swap;
	size_t				total_deleted;

	cur = cl->cur;
	//while(check_size != 0 && cur != NULL)
	//{
		swap = cur;
		for(int j = cur->len-1; j >= 0; j--)
		{
			if(cur->nnlt_line_part[j] == '\n')
			{
				if(cascade_print(cl,&(cur->nnlt_line_part[j+1]),
						cur->len-j-1,j, cur->prev_part, &total_deleted) != 0)
					return -1;
				if(cl->num_lines <= 0)
					goto free_zone;
				cur->len = j;
				cl->total_size = cl->total_size - total_deleted;
				cl->head = cur;
			}
			/*if(check_size - 1 == 0)
			{
				printf("r");
			}*/
			check_size--;
		}
		if (cur->len == 0)
		{
			free_zone:
			//free(cur->nnlt_line_part);
			//free(cur);
			cur = NULL;
			cl->head = NULL;
			cl->cur = NULL;
			/*if(cur->next_part != NULL)
			{
				printf("x");
			}
			if(cur->prev_part != NULL)
			{
				printf("y");
			}*/
		}
		//else
			//cur = cur->next_part;
		/*else
		{
			printf("z");
		}*/

	//}
	return 0;
}

static int
pull_bytes(struct current_line *cl, int pull_size, int block_size)
{
	struct line_part *lp;
	//int buf_size = pull_size;
	/*pull_size = (pull_size > cl->bytes_remaining ?
			cl->bytes_remaining - 1 : pull_size - 1);*/

	cl->pos = lseek(cl->fd, -1 * pull_size, SEEK_CUR);
	if(cl->pos == -1)
		return -1;

	if((lp = malloc(sizeof(struct line_part))) == NULL)
		return -1;

	if((lp->nnlt_line_part = malloc(sizeof(char)*block_size)) == NULL)
	{
		free(lp);
		return -1;
	}

	if(read(cl->fd, lp->nnlt_line_part, block_size) != block_size)
	{
		free(lp->nnlt_line_part);
		free(lp);
		return -1;
	}

	lp->len = block_size;
	cl->bytes_remaining -= block_size;

	cl->pos = lseek(cl->fd, -1 * block_size, SEEK_CUR);
	if(cl->pos == -1)
		return -1;

	if(cl->head == NULL)
	{
		cl->head = lp;
		cl->cur = lp;
		lp->prev_part = NULL;
		cl->total_size = block_size;
	}
	else
	{
		cl->cur->next_part = lp;
		lp->prev_part = cl->cur;
		cl->cur = cl->cur->next_part;
		cl->total_size += block_size;
	}



	return 0;
}

static void
init_line_struct(int *fd, struct current_line *cl, struct stat *st, int num_lines) {
	cl->block_size = (long) st->st_blksize;
	cl->pos = lseek(*fd, -1, SEEK_END);
	if (cl->pos == -1)
		errExit("pos");
	cl->fd = *fd;
	*fd = -9999;
	cl->first_size = ((long) cl->pos + 1) % (long) cl->block_size;
	cl->bytes_remaining = cl->pos + 1;
	cl->num_lines = num_lines;
}

static void
tail_file (char *file, long num_lines)
{
	int fd, block_size;
	long file_size, first_size;
	off_t pos;
	struct stat *st;
	struct current_line *cl;

	st = malloc(sizeof(struct stat));
	if (st == NULL)
		errExit("malloc\n");

	cl = malloc(sizeof(struct stat));
	if (cl == NULL)
		errExit("malloc\n");


	if ((fd = open(file, O_RDONLY | O_SYNC)) == -1)
		errExit("open: %s\n", file);
	if (fstat(fd, st) == -1)
		errExit("stat: %s\n", file);



	init_line_struct(&fd, cl, st, num_lines);
	free(st);

	if (pull_bytes(cl, (cl->first_size -1 ), (cl->first_size)) != 0)
		errExit("pull_bytes");
	if (check_line(cl) != 0)
		errExit("check_line\n");


	while(cl->bytes_remaining > 0 && cl->num_lines > 0)
	{
		if (pull_bytes(cl, (cl->block_size), (cl->block_size)) != 0)
			errExit("pull_bytes");
		if (check_line(cl) != 0)
			errExit("check_line\n");
	}

	if (cl->num_lines > 0 && check_line(cl) != 0)
		errExit("check_line\n");

	if(cl->head != NULL && cl->head->len > 0 && cl->num_lines > 0)
	{
		output_string(cl,cl->head->nnlt_line_part, cl->head->len, 0);
		//free(cl->head->nnlt_line_part);
		//free(cl->head);
	}

	cl_print(cl);

	return;
}

int
main(int argc, char *argv[])
{
	typedef struct args {
		long num_lines;
		char *file;
	} ARGS;

	void parse_args(int argc, char *argv[], ARGS *args) {
		int opt;
		extern char *optarg;
		extern int optind, opterr, optopt;

		args->num_lines = -1;

		while ((opt = getopt(argc, argv, "n:")) != -1)
		{
			switch (opt) {
			case 'n':
				if (optarg != NULL)
				{
					args->num_lines = getLong(optarg,GN_GT_0, "-n <num>\n");
				}
				break;
			}
		}

		if(args->num_lines != -1 && argc < 3)
			errExit("No File Provided\n");

		args->file = argv[optind];
	}

	ARGS *args;


	if(argc < 2)
		usageErr("%s [ -n num ] <file>\n");

	args = malloc(sizeof(ARGS));
	if (args == NULL)
		errExit("malloc\n");

	parse_args(argc, argv, args);

	if (args->file != NULL)
		tail_file(args->file, args->num_lines);
}
