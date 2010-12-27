#include <iostream>
#include <tbb/task_scheduler_init.h>
#include <tbb/task.h>
#include <sys/time.h>

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
  return( ((double) (tp.tv_sec - start)) + (tp.tv_usec-startu)*1.e-6);
}

struct fibcontinuation: public tbb::task {
  long* const sum;
  long x, y;
  fibcontinuation (long* sum) : sum (sum) {}
  tbb::task* execute () { *sum = x+y; return NULL; }
};

struct fibonacci : public tbb::task {
  long n;
  long* sum;

  public:
  fibonacci (long n, long* sum) : n(n), sum(sum) {}

  tbb::task* execute () {
    
    if (0 == n || 1 == n) {
      *sum = n;
      return NULL;
    } else {
      fibcontinuation& c = 
                *new (tbb::task::allocate_continuation()) fibcontinuation (sum);
      fibonacci& b = 
                *new (c.allocate_child()) fibonacci (n-1, &c.y);
      tbb::task::recycle_as_child_of (c);
      n -= 2;
      sum = &c.x;

      c.set_ref_count (2);
      c.spawn (b);
      return this;
    }
  }
};

int main (int argc, char**argv) {
  int n;
  unsigned int num_threads; 

  if (3 != argc) {
    std::cout << "Run the program like so" 
              << std::endl
              << "./tbb_fibonacci <nthds> <number>"
              << std::endl;
  }

  num_threads = atoi (argv[1]);
  n = atoi (argv[2]);
 
  tbb::task_scheduler_init init (num_threads);

  double time = wsmprtc ();
   
  long sum;
  fibonacci& fib = *new (tbb::task::allocate_root()) fibonacci(n, &sum); 
  tbb::task::spawn_root_and_wait (fib); 

  time = wsmprtc() - time;
 
  std::cout << time << std::endl;

  return 0;
}
