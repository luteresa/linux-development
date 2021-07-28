#include <pthread.h>
#include <unistd.h>

void* worker(void* unusued)
{
  // Do nothing
}

int main()
{
  pthread_t thread_id;

  for (int i = 0; i < 10; i++) {
    pthread_create(&thread_id, NULL, &worker, (void*)i);
    if (i != 4 && i != 7) pthread_join(thread_id, NULL);
  }
  sleep(1000);
  return 0;
}
