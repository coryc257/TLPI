#include "tlpi_hdr.h"

extern char **environ;

int
__display_env__main(int argc, char **argv)
{
	char **ep;

	for(ep = environ; *ep != NULL; ep++)
	{
		puts(*ep);
	}

	//exit(EXIT_SUCCESS);
}
