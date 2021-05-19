/*
 * environ.c
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */

#include <stdlib.h>
#include "tlpi_hdr.h"

extern char **environ;

struct found_env {
	int index;
	int keep;
	struct found_env *next;
	struct found_evn *prev;
	char *str;
};

static int
env_found(const char *enviro, const char *check)
{
	char *c, *x;

	for(c= enviro, x = check; *c != NULL && *x != NULL; c++, x++)
	{
		if(*c!=*x)
			return -1;
	}
	return 0;
}

static void
my_putenv(const char *name, const char *value)
{
	int total_length;
	int offset;
	char *str;

	total_length = strlen(name)+strlen(value)+2;
	str = malloc(total_length);
	if(str == NULL)
	{
		errExit("malloc");
	}
	offset = 0;

	for(int i = 0; i < strlen(name); i++)
	{
		str[i]=name[i];
		offset++;
	}

	str[offset]='=';
	offset++;

	for(int i = 0; i < strlen(value); i++)
	{
		str[i+offset] = value[i];
	}

	str[total_length-1] = '\0';
	putenv(str);
}

int
my_unsetenv(const char *name)
{
	int env_ct, found_ct;

	char **new_env;
	char **old_env;

	struct found_env *found_list;
	struct found_env *head;
	struct found_env *current;
	struct found_env *tail;


	env_ct = 0;
	found_ct = 0;
	old_env = environ;

	found_list = malloc(sizeof(struct found_env));
	head = found_list;
	head->index = -1;
	head->prev = NULL;
	current = found_list;
	tail = current;

	for(int i = 0; *old_env != NULL; i++, old_env++)
	{
		env_ct++;

		current->next = malloc(sizeof(struct found_env));
		current->next->prev = current;
		current = current->next;

		current->index = i;
		current->keep = 1;
		current->str = environ[i];
		current->next = NULL;

		tail = current;

		if(env_found(environ[i],name) == 0)
		{
			found_ct++;
			current->keep = 0;
			printf("Found:%d\n", current->index);
		}

		char *x;
		x = environ[i];
		printf("%s\n",x);
	}

	if(found_ct != 0)
	{
		char ** current_env;
		new_env = malloc(sizeof(char**)*((env_ct-found_ct)+1));
		current_env = new_env;

		current = head;
		for(int i = 0; i < env_ct; i++)
		{
			current = current->next;
			if(current->keep == 1)
			{
				*current_env = current->str;
				current_env++;
			}
			//else
				//free(current->str);

		}
		current_env = NULL;
		environ = new_env;
	}

	while(tail->prev != NULL)
	{
		tail = tail->prev;
		free(tail->next);
	}
	free(tail);
	return 0;
}

int
my_setenv(const char *name, const char *value, int overwrite)
{
	char *var;
	var = getenv(name);
	if(var == NULL)
	{
		my_putenv(name,value);
	} else if(overwrite == 1) {
		my_unsetenv(name);
		my_putenv(name,value);
	} else return 0;

	return 0;
}
