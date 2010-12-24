#include <iostream>
#include <pfunc/pfunc.hpp>
#include <pfunc/utility.h>

#define NUM_ITERS 100000

/** Forward declare work */
struct work_type;

/** Define the library instance */
typedef 
pfunc::generator <pfunc::use_default, 
                  pfunc::use_default, 
                  work_type> generator_type;
typedef generator_type::attribute attribute;
typedef generator_type::task task;
typedef generator_type::group group;
typedef generator_type::taskmgr taskmgr;
typedef generator_type::functor functor;

/** Declare a global task manager */
static taskmgr* global_tmanager;

/**
 * Structure that repeatedly executes a given number of barriers.
 */
struct work_type {
  work_type () : average_time (0.0) {}

  void operator () (void) {
    unsigned int rank;
    pfunc::group_rank (*global_tmanager, rank);
    for (int i=0; i<NUM_ITERS; ++i) {
      double barrier_time = micro_time ();
      pfunc::barrier (*global_tmanager);
      average_time += micro_time () - barrier_time;
    }
    std::cout << "Rank: " << rank 
              << " time: " << average_time/NUM_ITERS << std::endl;
  }

  private:
  double average_time;
};

int main (int argc, char** argv) {
  if (3 != argc) {
    std::cout << "Run the program like so"
              << std::endl
              << "./pfunc_barrier_time <nqueues> <nthreadsperqueue>"
              << std::endl;
  }
 
  // Read in the configuration
  const unsigned int num_queues = atoi (argv[1]);
  const unsigned int num_threads_per_queue = atoi (argv[2]);
  unsigned int* threads_per_queue = new unsigned int [num_queues];
  for (unsigned int i=0; i<num_queues; ++i) {
    threads_per_queue[i] = num_threads_per_queue;
  }

  // Initialize the task manager
  global_tmanager = new taskmgr (num_queues, threads_per_queue);

  // Create the tasks --- one per thread.
  task* tasks = new task [num_queues * num_threads_per_queue];

  // Create the work packets
  work_type* work = new work_type [num_queues * num_threads_per_queue];

  // Create a common attribute for all tasks
  attribute world_attribute (false, /* not nested */
                             true); /* belongs to a group */

  // Create the group to hold all the tasks
  group world_group (1234, 
                     num_queues*num_threads_per_queue, 
                     BARRIER_STEAL);

  // Spawn the tasks.
  for (int i=0; i<num_queues*num_threads_per_queue; ++i) {
    pfunc::spawn (*global_tmanager, 
                  tasks[i],
                  world_attribute,
                  world_group,
                  work[i]);
  }

  // Now, wait on the tasks to return
  for (int i=0; i<num_queues*num_threads_per_queue; ++i) {
    pfunc::wait (tasks[i]);
  }

  delete [] threads_per_queue;
  delete [] tasks;
  delete [] work;
  
  return 0;
}
