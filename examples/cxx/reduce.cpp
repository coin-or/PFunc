/**
 * @author Prabhanjan Kambadur
 * 
 * For an explanation of Loop parallelism, please see for.cpp.
 */
#include <iostream>
#include <cassert>
#include <vector>
#include <pfunc/pfunc.hpp>
#include <pfunc/utility.h>
#include <pfunc/experimental/space_1D.hpp>
#include <pfunc/experimental/parallel_reduce.hpp>

/**
 * A scaling operator for a vector.
 */
struct accumulate {
  private:
  std::vector<double>& my_vector;
  double sum;

  public:
  /**
   * Constructor
   * @param[in] 
   * @param[in] scaling_factor The factor by which each element is scaled.
   */
 accumulate (std::vector<double>& my_vector, const double init) :
    my_vector (my_vector), sum (init) {}

  /**
   * Operator that takes in a space and scales the vector in this space
   */
  void operator() (const pfunc::space_1D& space) {
    for (size_t i = space.begin(); i<space.end(); ++i) sum += my_vector[i];
  }

  /**
   * Split --- create a functor that is properly initialized.
   * @return A functor that is correctly initialized.
   */
  accumulate split () const { return accumulate (my_vector, 0.0); }

  /**
   * Join from a previous iterator.
   * @param[in] other The functor from which we need to join.
   */
  void join (const accumulate& other) { sum += other.get_sum (); }

  /**
   * @return The sum of elements accumulated by this functor.
   */
  double get_sum () const { return sum; }

  /**
   * Assignable --- need it to make accumulate work with std::vector.
   * @param[in] other The other functor from which we need to assign.
   */
  accumulate& operator= (const accumulate& other) { 
    my_vector = other.my_vector;
    sum = other.sum;
  }
};

/**
 * Define the PFunc instance. Note that we HAVE TO USE PFUNC::USE_DEFAULT as
 * the type of the FUNCTOR because of the way in which pfunc::parallel_for
 * is defined!
 */
typedef 
pfunc::generator <pfunc::cilkS, /* Cilk-style scheduling */
                  pfunc::use_default, /* No task priorities needed */
                  pfunc::use_default /* any function type*/> generator_type;
typedef generator_type::attribute attribute;
typedef generator_type::task task;
typedef generator_type::taskmgr taskmgr;

/**
 * A function to pretty a range. The value type is double.
 * @param[in] first Pointer to the first element.
 * @param[in] last Pointer just past the last element.
 */
template <typename Iterator>
static inline void print_vector (Iterator first, Iterator last) {
  std::ios_base::fmtflags original_flags = std::cout.flags();
  std::cout.precision(5);
  std::cout.setf (std::ios_base::fixed, std::ios_base::floatfield);

  while (first != last) std::cout << *first++ << " ";
  std::cout << std::endl;

  std::cout.flags(original_flags);
}

/**
 * Main harness. Takes in the following parameters:
 * (1) 'n': How big an array to create --- [0,n)
 * (2) 'chunk': What is the base case size that we want to execute serially.
 *              'chunk' <= 'n'.
 * (3) 'nqueues': The number of task queues to create
 * (4) 'nthreads': The number of threads PER QUEUE. Total number of threads is
 *                 'nqueues'*'nthreads'.
 * (5) 'print': Whether to print out the array before and after scaling.
 */
int main (int argc, char** argv) {
  // All inputs must be given. Else, barf.
  if (6 != argc) {
    std::cout << "Please use this program as follows" << std::endl
              << "./for <n> <chunksize> <nqueues> <nthreads> "
              << "<print(0|1)>" << std::endl;
    exit (3);
  }

  const int n = atoi(argv[1]);
  const int chunk_size = atoi(argv[2]);

  if (chunk_size > n) {
    std::cout << "To demonstrate parallel for properly, please choose a "
              << "chunk_size <= n" << std::endl;
    exit (3);
  }

  // Set up the base case size
  pfunc::space_1D::base_case_size = static_cast<size_t>(chunk_size);

  const unsigned int nqueues = static_cast<unsigned int>(atoi(argv[3]));
  const unsigned int threads_per_queue = 
                               static_cast<unsigned int>(atoi(argv[4]));
  unsigned int* threads_per_queue_array = new unsigned int [nqueues];
  for (int i=0; i<nqueues; ++i) threads_per_queue_array[i] = threads_per_queue;
  const bool please_print = static_cast<bool>(atoi(argv[5]));

  // Create the vector
  std::vector<double> my_vector (n);
  for (int i=0; i<n; ++i) my_vector[i] = get_next_rand();

  // Initialize PFunc
  taskmgr global_taskmgr (nqueues, threads_per_queue_array);

  // Spawn the root task
  task root_task;
  attribute root_attribute (false /*nested*/, false /*grouped*/);
  accumulate root_accumulate (my_vector, 0.0);
  pfunc::parallel_reduce<generator_type, accumulate, pfunc::space_1D> 
  root_reduce (pfunc::space_1D (0,n), root_accumulate, global_taskmgr);

  double time = micro_time();
  pfunc::spawn (global_taskmgr, root_task, root_attribute, root_reduce);
  pfunc::wait (global_taskmgr, root_task);
  time = micro_time() - time;

  std::cout << "Accumulating " << n << " elements in " << chunk_size 
            << " chunks took " << time << " seconds" << std::endl;

  if (please_print) {
    print_vector (my_vector.begin(), my_vector.end());
    std::cout << "Sum = " << root_accumulate.get_sum () << std::endl;
  }

  return 0;
}
