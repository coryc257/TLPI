/*
 * realpath.c
 *
 *  Created on: May 27, 2021
 *      Author: cory
 */

#include <unistd.h>
#include "tlpi_hdr.h"
#include <limits.h>

static char
**string_split(char *string, char splitter, int *size)
{
	struct pitem {
		char *item;
		struct pitem *next;
		struct pitem *tail;
	};

	int get_offset(char *str, int len, char splitter)
	{
		int offset = 0;
		for (int j = 0; j < len; j++)
		{
			if(str[j] == splitter)
				++offset;
			else
				return offset;
		}
	}

	int add_p_item (struct pitem *head, char *str)
	{
		struct pitem *next;
		if((next = malloc(sizeof(struct pitem))) == NULL)
			return -1;
		next->item = str;
		if (head->next == NULL)
		{
			head->tail = head->next = next;
			return 0;
		}
		head->tail->next = next;
		head->tail = next;
		return 0;
	}

	char *make_string_from_part(char *str, int len)
	{
		char *cstr;
		if((cstr = malloc(sizeof(char)*(len+1))) == NULL)
			return NULL;
		for (int j = 0; j < len; j++) {
			cstr[j] = str[j];
		}
		cstr[len] = '\0';
		return cstr;
	}

	char **construct_array(struct pitem *item, int count)
	{
		struct pitem *cur;
		void *swap;
		char **ret, **this;
		if (item == NULL)
			return NULL;
		if((ret = malloc(sizeof(char*)*count)) == NULL)
			return NULL;
		this = ret;
		cur = item;
		for (int j = 0; j < count; j++){
			*this = cur->item;
			swap = cur;
			cur = cur->next;
			free(swap);
			this++;
		}
		return ret;
	}

	struct pitem head;
	int len, s_offset, part_index, count;
	char *new;
	head.item = NULL;
	head.next = NULL;
	len = strlen(string);
	part_index = 0;
	count = 0;

	for (int j = 0; j < len; j++){
		if (string[j] == splitter || j+1 == len){
			s_offset = get_offset(&string[j], len-j, splitter);
			if (string[part_index] != '/')
				if (add_p_item(&head,make_string_from_part(&string[part_index],((j+1==len) && string[j] != '\n' ? j+1 : j)-part_index)) == -1)
					goto ss_fail;
				else
				{
					count++;
					if (j+1 == len)
						break;
				}
			j += s_offset - 1;
			part_index = j+1;
		}
	}

	*size = count;
	return construct_array(head.next,count);
	ss_fail:
	return NULL;
}

char
*realpath(const char *path, char *resolved_path)
{
	char **combine_string(char **first, char **second, int fl, int sl)
	{
		char **combined, **c;
		char **f = first, **s = second;
		if((combined = malloc(sizeof(char*)*(fl+sl))) == NULL)
			return NULL;
		c = combined;
		for (int j = 0; j < fl; j++)
		{
			*c = *f;
			f++;
			c++;
		}
		for (int j = 0; j < sl; j++)
		{
			*c = *s;
			s++;
			c++;
		}
		return combined;
	}

	int cur_len, path_len, c_len, r_pos;
	char **cur_dir, **sup_path, **combined;
	char *cstr, *ret, *plink;

	cur_dir = string_split((cstr = get_current_dir_name()), '/', &cur_len);
	sup_path = string_split(path, '/', &path_len);

	if ((cur_dir == NULL && strlen(cstr) > 1) || sup_path == NULL)
	{
		return NULL;
	}

	if (path[0] == '/' || strlen(cstr) == 1)
	{
		combined = sup_path;
		c_len = path_len;
	}
	else
	{
		combined = combine_string(cur_dir,sup_path, cur_len, path_len);
		free(cur_dir);
		free(sup_path);
		c_len = cur_len+path_len;
	}

	for (int j = 0; j < c_len; j++)
	{
		int t_len = strlen(combined[j]);
		if (t_len == 2 && combined[j][0] == '.' && combined[j][1] == '.')
		{
			free(combined[j]);
			combined[j] = NULL;
			if (j == 0)
				continue;
			free(combined[j-1]);
			combined[j-1] = NULL;
		}
		else if (t_len == 1 && combined[j][0] == '.')
		{
			free(combined[j]);
			combined[j] = NULL;
		}
	}

	if (resolved_path == NULL)
	{
		resolved_path = malloc(sizeof(char)*PATH_MAX);
	}
	plink = malloc(sizeof(char)*PATH_MAX);
	resolved_path[0] = '/';
	r_pos = 1;
	for (int j = 0; j < c_len; j++)
	{
		int i = 0;
		if (combined[j] != NULL)
		{
			while(combined[j][i] != '\0')
				resolved_path[r_pos++] = combined[j][i++];

			if (readlink(resolved_path,plink,PATH_MAX) != -1)
			{
				resolved_path[0] = '\0';
				strcpy(resolved_path,plink);
				plink[0] = '\0';
				r_pos = strlen(resolved_path);
				free(combined[j]);
				continue;
			}


			if (j + 1 != c_len)
				resolved_path[r_pos++] = '/';
			free(combined[j]);
		}
	}
	free(combined);
	resolved_path[r_pos] = '\0';
	return resolved_path;
}
