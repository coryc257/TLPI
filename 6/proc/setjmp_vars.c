#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf env;

static void
doJump(int nvar, int rvar, int vvar)
{
	printf("Inside doJump:{'nvar':'%d','rvar':'%d','vvar':'%d'}\n", nvar, rvar, vvar);
	longjmp(env,1);
}

int
__setjmp_vars__main(int argc, char **argv)
{
	int nvar;
	register int rvar;		/* put in a register if you can */
	volatile int vvar;		/* see text */

	nvar=111;
	rvar=222;
	vvar=333;

	if(setjmp(env)==0) { /* Code Executed after setjmp */
		nvar=777;
		rvar=888;
		vvar=999;
		doJump(nvar,rvar,vvar);
	} else {
		printf("after longjmp:{'nvar':'%d','rvar':'%d','vvar':'%d'}\n", nvar, rvar, vvar);
	}

	exit(EXIT_SUCCESS);
}
