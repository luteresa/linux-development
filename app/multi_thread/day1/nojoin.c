#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

void* worker (void* unusued)
{
   // pthread_detach(pthread_self());
    // Do nothing
}

int main()
{
    pthread_t thread_id;

    for(int i=0; i < 200000; i++)
    {
            if (pthread_create(&thread_id, NULL, &worker, NULL) != 0) {
	    	fprintf(stderr, "pthread_create failed at %d\n", i);
		perror("pthread_create");
		break;
            }
    }
    sleep(1000);
    return 0;
}
