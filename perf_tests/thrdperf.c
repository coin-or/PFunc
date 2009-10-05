#include <stdio.h>
#include <stdlib.h>
#include "pfunc.h"

extern double wsmprtc ();

/**
 * void work_func (void* work_func_arg);
 * \param work_func_arg Argument to the function
 */
void work_func (void* work_func_arg) { }

void serial_foo (int a) { }

void parallel_foo (void* args) {
  int a;
  pfunc_unpack (args, "int", &a);
  serial_foo (a);
}

int main () {
  double tt;
  int i;
  int j;
  const int niters = 100;
  const int njobs = 5000;
  pfunc_handle_t handles[5000];
  unsigned int threads_per_queue = 2;
  char* args[5000];

  pfunc_init(1, /* one queue */
             &threads_per_queue,/* two threads in this queue */
             NULL);

#if MODE_ONE
  for (j=0; j<njobs; ++j) pfunc_handle_init (&handles[j]);

  tt = wsmprtc();
  for (i = 0; i < niters; i++) {
    for (j = 0; j < njobs; j++) 
      pfunc_run (&handles[j], PFUNC_ATTR_DEFAULT, PFUNC_GROUP_DEFAULT, work_func, NULL);
    pfunc_wait_all (handles, njobs);
  }
  tt = wsmprtc() - tt;

  for (j=0; j<njobs; ++j) pfunc_handle_clear (handles[j]);

#elif MODE_TWO
  for (j=0; j<njobs; ++j) pfunc_handle_init (&handles[j]);

  tt = wsmprtc();
  for (i = 0; i < niters; i++) {
    for (j = 0; j < njobs; j++) {
      pfunc_pack (&args[i], "int", j);
      pfunc_run (&handles[j], PFUNC_ATTR_DEFAULT, PFUNC_GROUP_DEFAULT, parallel_foo, args[i]);
    }
    pfunc_wait_all (handles, njobs);
  }
  tt = wsmprtc() - tt;

  for (j=0; j<njobs; ++j) pfunc_handle_clear (handles[j]);
#elif MODE_THREE
  tt = wsmprtc();

  for (i = 0; i < niters; i++) {
    for (j = 0; j < njobs; j++) {
      pfunc_handle_init (&handles[j]);
      pfunc_run (&handles[j], PFUNC_ATTR_DEFAULT, PFUNC_GROUP_DEFAULT, work_func, NULL);
    }
    pfunc_wait_all (handles, njobs);
    for (j=0; j<njobs; ++j) pfunc_handle_clear (handles[j]);
  }

  tt = wsmprtc() - tt;
#else 
  tt = wsmprtc();

  for (i = 0; i < niters; i++) {
    for (j = 0; j < njobs; j++) {
      pfunc_handle_init (&handles[j]);
      pfunc_pack (&args[i], "int", j);
      pfunc_run (&handles[j], PFUNC_ATTR_DEFAULT, PFUNC_GROUP_DEFAULT, parallel_foo, args[i]);
    }
    pfunc_wait_all (handles, njobs);
    for (j=0; j<njobs; ++j) pfunc_handle_clear (handles[j]);
  }

  tt = wsmprtc() - tt;
#endif


  pfunc_clear ();
  printf ("Total, unit time = %g, %g\n",tt,tt/(njobs*niters));

  return 0;
}
