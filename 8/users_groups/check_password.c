#define _BSD_SOURCE  /* Get getPass() declaration from <unistd.h> */
#define _XOPEN_SOURCE /* Get crypt() declaration from <unistd.h> */
//#define __USE_MISC 1
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include "tlpi_hdr.h"
#include "mem.h"
extern int errno;

int
__check_password__name(int argc, char **argv)
{
	char *username, *password, *encrypted, *p;
	struct passwd *pwd;
	struct spwd *spwd;
	Boolean authOK;
	size_t len;
	long lnmax;

	lnmax = sysconf(_SC_LOGIN_NAME_MAX);
	if(lnmax == -1)
		lnmax = 256;

	username = malloc(lnmax);
	if(username == NULL)
		errExit("malloc");

	printf("Username: ");
	fflush(stdout);
	//fgets(username, lnmax, stdin);
	if(fgets(username, lnmax, stdin) == NULL || *username == '\n')
	{
		exit(EXIT_FAILURE);		/* exit on EOF */
	}


	len = strlen(username);
	if (username[len-1] == '\n')
		username[len-1] = '\0'; /* remove trailing \n */

	pwd = getpwnam(username);
	if(pwd == NULL)
		fatal("couldn't get password record\n");
	spwd = getspnam(username);
	if(spwd == NULL && errno == EACCES)
		fatal("No permission to read the shadow file\n");

	if(spwd != NULL)	/* If there is a shadow password record */
		pwd->pw_passwd = spwd->sp_pwdp;  /* Use the shadow password */

	password = getpass("Password: ");

	encrypted = crypt((const char*)password, (const char*)pwd->pw_passwd);
	for(p = password; *p != '\0'; )
		*p++ = '\0';

	if(encrypted == NULL)
		errExit("crypt");

	authOK = strcmp(encrypted, pwd->pw_passwd) == 0;
	if(!authOK) {
		printf("Incorrect password you fucking slob!\n");
		exit(EXIT_FAILURE);
	}

	printf("Successfully Authenticated: UID=%ld\n", (long)pwd->pw_uid);
	exit(EXIT_SUCCESS);
}
