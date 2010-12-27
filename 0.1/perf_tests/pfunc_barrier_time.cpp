#include <iostream>
#include <pfunc.hpp>

extern double wsmprtc ();

#define NUM_ITERS 100000
struct work_t : virtual pfunc::pfunc_work_func_t {
  work_t () : average_time (0.0) {}

  virtual void run (void) {
    for (int i=0; i<NUM_ITERS; ++i) {
      double barrier_time = wsmprtc ();
      pfunc::pfunc_barrier ();
      average_time += wsmprtc () - barrier_time;
    }
    std::cout << "Rank: " << pfunc::pfunc_task_rank () 
              << " time: " << average_time/NUM_ITERS << std::endl;
  }

  virtual ~work_t () {}

  private:
  double average_time;
};

int main (int argc, char** argv) {
  try {
  unsigned int num_threads = atoi (argv[1]);
  pfunc::pfunc_init (1, &num_threads);
  pfunc::pfunc_handle_t* handles = new pfunc::pfunc_handle_t [num_threads];
  work_t* work_packets = new work_t [num_threads];
  pfunc::pfunc_group_t group(1234, num_threads, BARRIER_STEAL);
  for (int i=0; i<num_threads; ++i) 
    pfunc::pfunc_run (handles[i], group, work_packets[i]);
  for (int i=0; i<num_threads; ++i) pfunc::pfunc_wait (handles[i]);
  delete [] handles;
  delete [] work_packets;
  pfunc::pfunc_clear ();
  } catch (const pfunc::pfunc_exception_t& error) {
    std::cout << error.what () << std::endl;
    std::cout << error.trace () << std::endl;
    std::cout << error.code () << std::endl;
  }
  return 0;
}
