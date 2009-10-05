#include <iostream>
#include <pfunc/pfunc.hpp>

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
  return( (static_cast<double>(tp.tv_sec - start)) + (tp.tv_usec-startu)*1.e-6);
}

struct fibonacci;

typedef 
pfunc::generator <pfunc::cilkS, pfunc::use_default, fibonacci> generator_type;
typedef generator_type::attribute attribute;
typedef generator_type::task task;
typedef generator_type::group group;
typedef generator_type::taskmgr taskmgr;
typedef generator_type::functor functor;

taskmgr* gbl_taskmgr; 
group gbl_default_group;

struct fibonacci {
  private:
  const int n;
  int fib_n;

  public:
  fibonacci (const int& n) : n(n), fib_n(0) {}

  int get_number () const { return fib_n; }

  void operator () (void) {

    if (0 == n || 1 == n) fib_n = n;
    else {
      task tsk;
      attribute nested_attr;
      
      fibonacci fib_n_1 (n-1); 
      fibonacci fib_n_2 (n-2); 

      pfunc::attr_level_set (nested_attr, ~0x0-(n-1));
      pfunc::spawn (*gbl_taskmgr, tsk, nested_attr, gbl_default_group, fib_n_1);

      fib_n_2();

      pfunc::wait (*gbl_taskmgr, tsk);

      fib_n = fib_n_1.get_number () + fib_n_2.get_number ();
    }
  }
};

int main (int argc, char**argv) {
  int n;
  unsigned int num_queues = 2; 
  unsigned int* num_threads_per_queue;

  if (4 != argc) {
    std::cout << "Run the program like so" 
              << std::endl
              << "./fibonacci <nqueues> <nthreadsperqueue> <number>"
              << std::endl;
  }

  num_queues = atoi (argv[1]);
  num_threads_per_queue = new unsigned int [num_queues];
  for (unsigned int i=0; i<num_queues; ++i) 
          num_threads_per_queue[i] = atoi (argv[2]);
  n = atoi (argv[3]);
 
  double time = wsmprtc ();
  gbl_taskmgr = new taskmgr (num_queues, num_threads_per_queue);
 
  fibonacci fib_n (n); 
 
  task root_task;
  pfunc::spawn (*gbl_taskmgr,  /** Taskmgr to use */
                root_task,     /** Task handle */
                attribute (false), /** Non-nested task */
                gbl_default_group, /** No use for groups */
                fib_n); /** The functor */
  pfunc::wait (*gbl_taskmgr, root_task);

  time = wsmprtc() - time;
 
  std::cout << "The fibonacci number is: " 
            << fib_n.get_number () 
            << " , it took "
            << time
            << " seconds"
            << std::endl;

  delete gbl_taskmgr;
  delete num_threads_per_queue;

  return 0;
}
