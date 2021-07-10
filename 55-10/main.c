/*
 * main.c
 *
 *  Created on: Jul 9, 2021
 *      Author: cory
 */

#include <fcntl.h>
#include <glib-2.0/glib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <setjmp.h>

typedef struct __this {
	GList *file_list;
	int total_files;
} __THIS;

static __THIS this;
static jmp_buf out;

void
get_file_list(__THIS *ref, int option_offset, int argc, char *argv[])
{
	GString *snew;
	ref->file_list = g_list_alloc();
	ref->total_files = 0;

	for (int j = option_offset; j < argc; j++) {
		snew = g_string_new(argv[j]);
		if (g_list_append(ref->file_list,snew) == NULL) {
			write(STDERR_FILENO, "FAIL\n", 5);
			exit(EXIT_FAILURE);
		}
		ref->total_files++;
	}
}

void
for_each_file(gpointer xitem, gpointer context)
{
	GString *item = xitem;
	int *success = context;
	mode_t prev;
	int fd = -1;
	if (xitem == NULL) {
		return;
	}

	prev = umask((mode_t)0);
	if ((fd = open(item->str, O_CREAT | O_EXCL | O_TRUNC, ((mode_t)0) | S_ISGID)) != -1) {
		(*success)++;
		close(fd);
	}
	umask(prev);

	printf("File:%s\n", item->str);
}

void
unlink_bad(gpointer xitem, gpointer context)
{
	GString *item = xitem;
	int *total_remaining = context;
	if (item == NULL)
		return;
	if (*total_remaining == 0)
		longjmp(out,1);
	unlink(item->str);
	(*total_remaining)--;
}

void
try_lock_files( __THIS *ref)
{
	int success = 0;
	g_list_foreach(ref->file_list, for_each_file, &success);
	if (success != ref->total_files) {
		switch(setjmp(out)) {
		case 0:
			g_list_foreach(ref->file_list, unlink_bad, &success);
			break;
		default:
			break;
		}
		exit(EXIT_FAILURE);
	} else {
		exit(EXIT_SUCCESS);
	}
}

int
main(int argc, char *argv[])
{

	int option_offset = 1;

	get_file_list(&this, option_offset, argc, argv);
	try_lock_files(&this);

}


