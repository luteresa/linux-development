/*
 * thread_attr.c
 *
 * Create a thread using a non-default attributes object,
 * thread_attr. The thread reports its existence, and exits. The
 * attributes object specifies that the thread be created
 * detached, and, if the stacksize attribute is supported, the
 * thread is given a stacksize twice the minimum value.
 */
#include <limits.h>
#include <pthread.h>

#define _POSIX_THREAD_ATTR_STACKSIZE

void err_abort(int status, char *p)
{
	perror("%s\n", p);
	exit(status);
}

static void *access_overflow(void)
{
	char q[11*1024];
	int i = 10;
	for (i = 10;i>=0;i--) {
		printf("%s %d %p\n", __func__, i, &q[i*1024]);
		q[i*1024] = 'a';
	}
	while(1);
	printf("The thread is here2\n");
}

static void *thread_routine(void *arg)
{
	char p[5*1014];
	p[4999] = 'a';
	p[0] = 'a';

	access_overflow();
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thread_id1, thread_id2;
	pthread_attr_t thread_attr;
	struct sched_param thread_param;
	size_t stack_size, guard_size;
	int status;

	status = pthread_attr_init(&thread_attr);
	if (status != 0)
		err_abort(status, "Create attr");

	/*
	 * Create a detached thread.
	 */
	status =
	    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if (status != 0)
		err_abort(status, "Set detach");
#ifdef _POSIX_THREAD_ATTR_STACKSIZE
	/*
	 * If supported, determine the default stack size and report
	 * it, and then select a stack size for the new thread.
	 *
	 * Note that the standard does not specify the default stack
	 * size, and the default value in an attributes object need
	 * not be the size that will actually be used.  Solaris 2.5
	 * uses a value of 0 to indicate the default.
	 */
	status = pthread_attr_getstacksize(&thread_attr, &stack_size);
	if (status != 0)
		err_abort(status, "Get stack size");
	printf("Default stack size is %u; minimum is %u\n",
	       stack_size, PTHREAD_STACK_MIN);

	status = pthread_attr_getguardsize(&thread_attr, &guard_size);
	if (status != 0)
		err_abort(status, "Get guard size");
	printf("Default guard size is %u\n", guard_size);

	status = pthread_attr_setstacksize(&thread_attr, PTHREAD_STACK_MIN);
	if (status != 0)
		err_abort(status, "Set stack size");
#endif
	status = pthread_create(&thread_id1, &thread_attr, thread_routine, NULL);
	if (status != 0)
		err_abort(status, "Create thread");
	
	status = pthread_create(&thread_id2, &thread_attr, thread_routine, NULL);
	if (status != 0)
		err_abort(status, "Create thread");

	while(1);
	return 0;
}
