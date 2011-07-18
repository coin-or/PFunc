#include <iostream>
#include <numeric>
#include <functional>
#include <pfunc/pfunc.hpp>
#include <pfunc/utility.h>

struct fibonacci;

typedef 
pfunc::generator <pfunc::cilkS, pfunc::use_default, fibonacci> generator_type;
typedef generator_type::attribute attribute;
typedef generator_type::task task;
typedef generator_type::group group;
typedef generator_type::taskmgr taskmgr;
typedef generator_type::functor functor;

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
      pfunc::global::spawn (tsk, nested_attr, fib_n_1);

      fib_n_2();

      pfunc::global::wait (tsk);

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
 
  double time = micro_time ();
  taskmgr my_taskmgr (num_queues, num_threads_per_queue);
  pfunc::global::init (my_taskmgr);
 
  fibonacci fib_n (n); 
 
  task root_task;
  pfunc::global::spawn (root_task,     /** Task handle */
                attribute (false), /** Non-nested task */
                fib_n); /** The functor */
  pfunc::global::wait (root_task);

  time = micro_time() - time;
 
  std::cout << "The fibonacci number is: " 
            << fib_n.get_number () 
            << " , it took "
            << time
            << " seconds"
            << std::endl;

  pfunc::global::clear ();
  delete num_threads_per_queue;

  return 0;
}
