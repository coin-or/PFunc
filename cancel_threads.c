#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static __attribute__((aligned(128))) volatile int a = 0;
static __attribute__((aligned(128))) volatile int b = 0;
static int c;
static int d;

void* func (void* unused_arg) {

  pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL);

  while (1) {
    pthread_mutex_lock (&lock);
    printf ("*");
    ++a;
    pthread_mutex_unlock (&lock);
    pthread_testcancel ();
  }
}

int main () {
  pthread_t thd1;
  pthread_t thd2;
  pthread_attr_t attr;

  pthread_attr_init (&attr);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create (&thd1, &attr, func, NULL);
  pthread_create (&thd2, &attr, func, NULL);

  sleep (1);

  if (0 != pthread_cancel (thd1)) printf ("Error cancelling\n");
  if (0 != pthread_cancel (thd2)) printf ("Error cancelling\n");

  pthread_join (thd1, NULL);
  pthread_join (thd2, NULL);

  b = c = d = a;

  printf ("The value of a: %d\n", a);

  return 0;
}
