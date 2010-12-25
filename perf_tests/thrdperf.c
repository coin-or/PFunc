#include <stdio.h>
#include <stdlib.h>
#include <pfunc/pfunc.h>
#include <pfunc/utility.h>

#ifndef NITERS
#define NITERS 100
#endif

#ifndef NJOBS
#define NJOBS 5000
#endif

/**
 * void work_func (void* work_func_arg);
 * \param work_func_arg Argument to the function
 */
void work_func (void* work_func_arg) { }

/**
 * A NOOP function.
 * \param a ignored.
 */
void serial_foo (int a) { }

/**
 * A wrapper around serial function.
 * \param args Argument to pass to serial_foo.
 */
void parallel_foo (void* args) {
  int a;
  pfunc_unpack (args, "int", &a);
  serial_foo (a);
}

int main () {
  /** iteration indices and time */
  double tt;
  int i, j;

  /** pfunc declarations */
  pfunc_cilk_taskmgr_t taskmgr;
  pfunc_cilk_task_t tasks[NJOBS];
  pfunc_cilk_attr_t attr;
  pfunc_cilk_group_t group;

  /** run parameters */
  unsigned int num_queues = 1;
  unsigned int threads_per_queue = 2;
  char* args[NJOBS];

  /** initialize pfunc */
  pfunc_cilk_taskmgr_init (&taskmgr,  // task manager 
                           num_queues,  // number of queues
                           &threads_per_queue, // number of threads per queue
                           NULL); // thread affinities

  /** initialize tasks, attribute and group */
  pfunc_cilk_attr_init (&attr);
  pfunc_cilk_group_init (&group);
  for (j=0; j<NJOBS; ++j) pfunc_cilk_task_init (&(tasks[j]));


  tt = micro_time ();
  for (i = 0; i < NITERS; i++) {
    /** spawn the tasks */
    for (j = 0; j < NJOBS; j++) {
#if MODE_ONE /** run an empty function to see the function overhead */
      pfunc_cilk_spawn_c (taskmgr, tasks[j], attr, group, work_func, NULL);
#else
      pfunc_pack (&(args[j]), "int", j);
      pfunc_cilk_spawn_c 
            (taskmgr, tasks[j], attr, group, parallel_foo, args[j]);
#endif
    }

    /** wait for the tasks to complete */
    for (j = 0; j < NJOBS; j++) pfunc_cilk_wait (taskmgr, tasks[j]); 
  }
  tt = micro_time () - tt;

  /** clear the variables */
  for (j=0; j<NJOBS; ++j) pfunc_cilk_task_clear (&tasks[j]);
  pfunc_cilk_attr_clear (&attr);
  pfunc_cilk_group_clear (&group);

  /** clear the task manager and print out the time */
  pfunc_cilk_taskmgr_clear (&taskmgr);
  printf ("Total time (secs) = %g, unit time (secs) = %g\n",
                    tt,tt/(NJOBS*NITERS));

  return 0;
}
