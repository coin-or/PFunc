#include <pfunc/pfunc.h>
#include <stdio.h>
#include <stdlib.h>

enum {
  NUM_ITERS = 1000,
  N = 100000,
  p = 2
};

double local_dot [p];

void parallel_dot (void* arg) { 
  double* X;
  double* Y;
  pfunc_cilk_taskmgr_t taskmgr;

  unsigned int rank;
  unsigned int size;
  double dot;
  char* arg_buf = (char*)arg;
  unsigned int my_start_index;
  unsigned int my_stop_index;
  unsigned int i, j, k;

  /* First, we need to unpack the arguments */
  pfunc_unpack (arg_buf, /* the buffer */
                "double*, double*, void*", /* the format */
                &X, &Y, &taskmgr); /* the variables */

  pfunc_cilk_group_rank (taskmgr, &rank);
  pfunc_cilk_group_size (taskmgr, &size);

  my_start_index = ((N+size-1)/size) * rank;
  my_stop_index = ((((N+size-1)/size)*(rank+1)) < N-1) ? 
                    (((N+size-1)/size)*(rank+1)) : N-1;

  for (i=0; i<NUM_ITERS; ++i) {
    /* Compute the local dot product */
    local_dot[rank] = 0.0;
    for (j=my_start_index; j<my_stop_index; ++j) local_dot[rank] += X[j]*Y[j];
    
    /* barrier */
    pfunc_cilk_barrier (taskmgr);

    /* compute the dot -- its read only, so its fine */
    dot = 0.0;
    for (k=0; k<size; ++k) dot += local_dot[k];
    
    /* barrier again */
    pfunc_cilk_barrier (taskmgr);
  }
  printf ("Id: %d, dot: %f\n", rank, dot);
}

int main () {
  double* X;
  double* Y;

  pfunc_cilk_taskmgr_t taskmgr;
  pfunc_cilk_attr_t def_attr;
  pfunc_cilk_group_t def_group;
  pfunc_cilk_task_t tasks[p]; 

  unsigned int num_queues = 1;
  unsigned int num_threads_per_queue [] = {p};

  int i; 
  char* parallel_dot_args[p]; 

  X = (double*) malloc (sizeof (double) * N);
  Y = (double*) malloc (sizeof (double) * N);
  if (NULL == X || NULL == Y) {
    printf ("Could not allocate memory for X or Y\n");
    exit (3);
  }
  for (i=0; i<N; ++i) {
    X[i] = (1.0 * (rand() / (double) RAND_MAX));
    Y[i] = (1.0 * (rand() / (double) RAND_MAX));
  }

  pfunc_cilk_taskmgr_init (&taskmgr,
                       num_queues, 
                       num_threads_per_queue, 
                       NULL); 
  for (i=0; i<p; ++i) pfunc_cilk_task_init (&tasks[i]);
  pfunc_cilk_attr_init (&def_attr);
  pfunc_cilk_group_init (&def_group); 
  pfunc_cilk_group_id_set (def_group, 1234); 
  pfunc_cilk_group_size_set (def_group, p);

  /** ensure that the grouped attribute is turned on */
  pfunc_cilk_attr_grouped_set (def_attr, 1 /* turn on */);

  /**************************************************************************/
  pfunc_cilk_group_barrier_set (def_group, BARRIER_SPIN);
  for (i=0; i<p; ++i) {
    pfunc_pack (&parallel_dot_args[i],
                "double*, double*, void*",
                 X, Y, taskmgr); 
    pfunc_cilk_spawn_c (taskmgr, tasks[i], def_attr, def_group, 
                      parallel_dot, parallel_dot_args[i]);
  }
  pfunc_cilk_wait_all (taskmgr, tasks, p);
  /**************************************************************************/
  pfunc_cilk_group_barrier_set (def_group, BARRIER_WAIT);
  for (i=0; i<p; ++i) {
    pfunc_pack (&parallel_dot_args[i],
                "double*, double*, void*",
                 X, Y, taskmgr); 
    pfunc_cilk_spawn_c (taskmgr, tasks[i], def_attr, def_group, 
                      parallel_dot, parallel_dot_args[i]);
  }
  pfunc_cilk_wait_all (taskmgr, tasks, p);
  /**************************************************************************/
  pfunc_cilk_group_barrier_set (def_group, BARRIER_STEAL);
  for (i=0; i<p; ++i) {
    pfunc_pack (&parallel_dot_args[i],
                "double*, double*, void*",
                 X, Y, taskmgr); 
    pfunc_cilk_spawn_c (taskmgr, tasks[i], def_attr, def_group, 
                      parallel_dot, parallel_dot_args[i]);
  }
  pfunc_cilk_wait_all (taskmgr, tasks, p);
  /**************************************************************************/

  pfunc_cilk_group_clear (&def_group);
  pfunc_cilk_attr_clear (&def_attr);
  for (i=0; i<p; ++i) pfunc_cilk_task_clear (&(tasks[i]));
  pfunc_cilk_taskmgr_clear(&taskmgr);

  free (X);
  free (Y);

  return 0;
}
