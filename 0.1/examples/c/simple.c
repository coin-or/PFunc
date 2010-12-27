#include <pfunc/pfunc.h>
#include <stdio.h>

static int global_counter;

void my_function (void* arg) {
  unsigned int rank, size, id;
  pfunc_cilk_taskmgr_t* taskmgr = (pfunc_cilk_taskmgr_t*) arg;

  pfunc_cilk_thread_id(*taskmgr, &id);
  pfunc_cilk_group_rank(*taskmgr, &rank);
  pfunc_cilk_group_size(*taskmgr, &size);

  printf ("(Thread %d): Here: %u of %u, Counter = %d\n", 
        id, rank, size, pfunc_fetch_and_add_32 (&global_counter, 1));
}

int main () {
  pfunc_cilk_taskmgr_t taskmgr;
  pfunc_cilk_attr_t attr;
  pfunc_cilk_task_t tasks[10];
  pfunc_cilk_group_t group;
  pfunc_cilk_work_t worker;
  unsigned int num_queues = 1;
  const unsigned int num_threads_per_queue[] = {4};
  int i;

  pfunc_cilk_taskmgr_init (&taskmgr, num_queues, num_threads_per_queue, NULL);
  pfunc_cilk_attr_init (&attr);
  pfunc_cilk_attr_grouped_set (attr, 1);
  pfunc_cilk_group_init (&group);
  pfunc_cilk_group_size_set (group, 10);

  printf ("Using C-style spawn\n");
  for (i=0; i<10; ++i) {
    pfunc_cilk_task_init (&(tasks[i]));
    pfunc_cilk_spawn_c (taskmgr, tasks[i], attr, group, my_function, &taskmgr);
  }

  for (i=0; i<10; ++i) pfunc_cilk_wait (taskmgr, tasks[i]);

  printf ("Using C++-style spawn\n");
  pfunc_cilk_work_init (&worker);
  pfunc_cilk_work_func_set (worker, my_function);
  pfunc_cilk_work_arg_set (worker, &taskmgr);

  for (i=0; i<10; ++i) 
    pfunc_cilk_spawn_cxx (taskmgr, tasks[i], attr, group, worker);
 
  for (i=0; i<10; ++i) {
    pfunc_cilk_wait (taskmgr, tasks[i]);
    pfunc_cilk_task_clear (&(tasks[i]));
  }

  pfunc_cilk_work_clear (&worker);
  pfunc_cilk_group_clear (&group);
  pfunc_cilk_attr_clear (&attr);
  pfunc_cilk_taskmgr_clear (&taskmgr);

  return 0;
}
