#include <sys/types.h>
#include <pwd.h>
#include "mem.h"
#include <string.h>

static void
replicate_char(char **p)
{
	char *x;
	size_t len;
	x = malloc((len = (strlen(*p)+1)));
	for(int j = 0; j < len; j++)
	{
		x[j]=(*p)[j];
	}
	*p=x;
}

static void
replicate_passwd(struct passwd **pwd, const struct passwd *cur)
{

	char *f, *t;
	*pwd = malloc(sizeof(struct passwd));
	t = (char*)(*pwd);
	f = (char*)cur;
	for(int j = 0; j < sizeof(struct passwd); j++)
	{
		*t = *f;
		t++, f++;
	}
	replicate_char(&(*pwd)->pw_name);
	replicate_char(&(*pwd)->pw_passwd);
	replicate_char(&(*pwd)->pw_gecos);
	replicate_char(&(*pwd)->pw_dir);
	replicate_char(&(*pwd)->pw_shell);
}

struct passwd
*my_getpwname(const char *name)
{
	struct passwd *cur, *found;

	found = NULL;
	setpwent(); /* Make sure we are at the start */
	while((cur = getpwent()) != NULL)
	{
		if(strcmp(cur->pw_name,name) == 0)
		{
			replicate_passwd(&found, (const struct passwd*)cur);
			//break; let's not and simulate someone else trying to fuck around with this
		}
	}
	endpwent();
	return found;
}


