/*
 * main.c
 *
 *  Created on: Jun 9, 2021
 *      Author: cory
 */

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>
#define __INIT_CONTROL_ALIGNMENT
typedef enum { FALSE, TRUE} Boolean;

/*typedef struct init_control_t {
	Boolean initialized;
	pthread_mutex_t lock;
} init_control_t;*/

typedef int __INIT_CONTROL_ALIGNMENT init_control_t;
#define INIT_CONTROL_INIT 0

init_control_t x = INIT_CONTROL_INIT;

static sigjmp_buf jbf;
static volatile int swap_sig;
void oti_hand(int sig)
{
	printf("BOOM\n");
	siglongjmp(jbf,sig);
}

void
oti_thread(void *arg)
{

	void (*init)(void) = arg;
	int oti_init = 0;
	struct sigaction act;
	sigset_t open, block;

	sigemptyset(&open);
	sigfillset(&block);
	sigfillset(&(act.sa_mask));
	act.sa_handler = oti_hand;
	act.sa_flags = 0;

	for (int j = 0; j < NSIG; j++) {
		sigaction(j,&act,NULL);
	}
	printf("ding\n");
	if(sigsetjmp(jbf,0) == 0) {
		(*init)();
		swap_sig = 0;
	}
	else {
		swap_sig = 1;
	}


}

int one_time_init(init_control_t *control, void (*init)(void))
{
static pthread_mutex_t otc_lock = PTHREAD_MUTEX_INITIALIZER;
	pthread_t slip;
	pthread_mutex_lock(&otc_lock);


	swap_sig = 0;


	if (*control == INIT_CONTROL_INIT)
	{
		*control = ~INIT_CONTROL_INIT;
		pthread_create(&slip,NULL,oti_thread,init);
		pthread_join(slip,NULL);
		if (swap_sig != 0)
		{
			*control = INIT_CONTROL_INIT;
			printf("FAIL OTI\n");
		}
	}
	else
	{
		printf("GOOD OTI\n");
	}

	pthread_mutex_unlock(&otc_lock);
	return ~(*control);
}

void
handler(void)
{
	printf("Once\n");
	int x;
	free(&x);
	printf("Once\n");
}

void
handler_good(void)
{
	printf("GOT IT\n");
}
int
main(int argc, char *argv[])
{
	printf("%d\n", one_time_init(&x,&handler));
	printf("%d\n", one_time_init(&x,&handler_good));
	printf("%d\n", one_time_init(&x,&handler_good));
}
