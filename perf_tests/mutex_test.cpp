#include <cstdio>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>
#include <pthread.h>

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

double wsmprtc(void) {
struct timeval tp;
static long start=0, startu;

  if (!start) {
    gettimeofday(&tp, NULL);
    start = tp.tv_sec;
    startu = tp.tv_usec;
    return(0.0);
  }
  gettimeofday(&tp, NULL);
  return( ((double) (tp.tv_sec - start)) + (tp.tv_usec-startu)*1.e-6 );
}

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

  double time = wsmprtc ();
  for (int i=0; i<8; ++i) 
    pthread_create (threads+i, NULL, pthread_func, NULL);
  
  for (int i=0; i<8; ++i)
    pthread_join (threads[i], NULL);  

  printf ("%ld: %lf\n", counter, wsmprtc() - time);

  return 0;
}
