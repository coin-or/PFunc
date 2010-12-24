#include <cstdio>
#include <stdlib.h>
#include <sys/resource.h>
#include <math.h>
#include <pthread.h>
#include <pfunc/utility.h>

struct my_mutex {
  pthread_mutex_t mutex;

  my_mutex () {
    pthread_mutex_init (&mutex, NULL);
  }

  ~my_mutex () {
    pthread_mutex_destroy (&mutex);
  }

  void lock () {
    pthread_mutex_lock (&mutex);
  }

  void unlock () {
    pthread_mutex_unlock (&mutex);
  }
};

struct my_exception {
  const char* what (void) const {
    return "What the hell";
  }
};

#ifdef MINE
static my_mutex global_mutex;
#else
static pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
static int64_t counter;

void* pthread_func (void* arg) {
#ifdef MINE
  global_mutex.lock ();
#else
  pthread_mutex_lock (&global_mutex);
#endif
  for (int i=0; i<10000000; ++i)
   ++counter;
#ifdef MINE
  global_mutex.unlock ();
#else
  pthread_mutex_unlock (&global_mutex);
#endif
}

int main () {
  pthread_t threads[8];

  double time = micro_time ();
  for (int i=0; i<8; ++i) 
    pthread_create (threads+i, NULL, pthread_func, NULL);
  
  for (int i=0; i<8; ++i)
    pthread_join (threads[i], NULL);  

  printf ("%ld: %lf\n", counter, micro_time () - time);

  return 0;
}
