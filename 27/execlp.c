/*
 * execlp.c
 *
 *  Created on: Jun 5, 2021
 *      Author: cory
 */
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
extern char **environ;
extern struct exit_function_list *__exit__funcs;


int my_execlp(const char *filename, const char *arg, ...)
{

	char **e, **pe, **cp;
	char **ag, **ca;
	char *ev, *x;
	int e_ct, a_ct;
	va_list args;
	e = environ;
	e_ct = 0;
	a_ct = 0;
	while (*e != NULL)
	{
		e_ct++;
		e++;
	}

	cp = pe = malloc(sizeof(char*)*(e_ct+1));
	e = environ;
	while (*e != NULL)
	{
		ev = strdup(*e);
		*cp = ev;
		e++;
		cp++;
	}
	*cp = NULL;

	va_start(args,arg);
	while(1){
		a_ct++;
		x = va_arg(args,char*);
		if (x == NULL)
			break;
		//printf("%s\n",x);
	}
	va_end(args);

	a_ct--;
	ag = ca = malloc(sizeof(char*)*(a_ct + 2 + (arg != NULL ? 1 : 0)));

	va_start(args,arg);

	ev = strdup(filename);
	*ca = ev;
	ca++;

	if (arg != NULL) {
		ev = strdup(arg);
		*ca = ev;
		ca++;
	}
	while (a_ct > 0)
	{
		x = va_arg(args, char*);
		ev = strdup(x);
		*ca = ev;
		ca++;
		a_ct--;
	}
	va_end(args);
	*ca = NULL;

	execve(filename, ag, pe);
	return 0;
}
