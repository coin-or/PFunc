/**
 * @author Prabhanjan Kambadur
 * 
 * Loop parallelism is an important form of parallelism that often results in
 * dramatic speedups. In fact, constructs such as OpenMP's "parallel for" have
 * been exclusively dedicated to parallelizing for loops, which occur 
 * frequently in HPC applications.
 *
 * Task parallelism is a powerful form of parallelism that subsumes loop
 * parallelism. In this example, we demonstrate how to elegantly realize 
 * loop parallelism using task parallelism over a linear iteration space.
 * 
 * IDEA:
 * Consider a standard 'for' statement that iterates over a *randomly
 * accessible* set of elements. It is quite important that the elements be
 * randomly accessible because parallelization may fail to yield significant
 * performance boost if iteration (or in layman's terms 'advancing the
 * pointer') takes longer than the computation itself.
 * 
 * Let the iteration space be dentoed by the semi-open interval [0,n). Then, 
 * we can devise an elegant divide and conquer mechanism to parallelize the 
 * computations in the following manner:
 * (1) At each level (starting with level 0), inspect the iteration space to 
 *     determine benefit of parallelization.
 * (2) If parallelization will help, split the interval into two and execute 
 *     iterations over the split iteration space in parallel.
 * (3) Repeat until the number of iterations in the iteration space are too 
 *     few to benefit from parallelization --- execute this space serially.
 *
 * In the true spirit of generic design, we devise the concept of 'space' 
 * that has a few basic elements that enable iteration. These are:
 * (1) begin()
 * (2) end()
 * (3) can_split()
 * (4) split() 
 *
 * The 'space' object is constructed initially with the interval [0,n). It is
 * also given the iteration space that is deemed 'parallelizable'. For example,
 * we can initialize a space with [0,100) with the base case being 25. In this 
 * case, the following execution occurs:
 *
 *                              [0,100)
 *                              (SPLIT)
 *                         [0,50)   [50,100)
 *                         (SPLIT)  (SPLIT)
 *                [0,25)   [25,50)  [50,75)   [75,100)
 * Finally, the four 'spaces' obtained in the end are executed serially. There 
 * is no requirement that the iteration space should perform a 2-way split; 
 * theoretically, an n-way split is also implementable.
 *
 * CRITIQUE:
 * Notice that in our example all the non-leaf nodes do not perform any
 * computation, but are simply control structures. These control structures 
 * are overhead, which increases with increase in the depth of the tree. An
 * alternative is to split the iteration space into equal size chunks at the 
 * very beginning. In fact, this is the model used in MPI-style algorithms and
 * can be mimiced in PFunc using GROUP structures.
 *
 * NOTE: Please see pfunc/experimental/ directory for space_1D and parallel_for
 */
#include <iostream>
#include <cassert>
#include <vector>
#include <pfunc/pfunc.hpp>
#include <pfunc/utility.h>
#include <pfunc/experimental/space_1D.hpp>
#include <pfunc/experimental/parallel_for.hpp>

/**
 * A scaling operator for a vector.
 */
struct vector_scale {
  private:
  std::vector<double>& my_vector;
  double scaling_factor;

  public:
  /**
   * Constructor
   * @param[in] 
   * @param[in] scaling_factor The factor by which each element is scaled.
   */
  vector_scale (std::vector<double>& my_vector, const double scaling_factor) :
    my_vector (my_vector), scaling_factor (scaling_factor) {}

  /**
   * Operator that takes in a space and scales the vector in this space
   */
  void operator() (const pfunc::space_1D& space) const {
    for (size_t i = space.begin(); i<space.end(); ++i) {
      my_vector[i] *= scaling_factor;
    }
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
 * (3) 'scaling': The factor by which we want to scale the function.
 * (4) 'nqueues': The number of task queues to create
 * (5) 'nthreads': The number of threads PER QUEUE. Total number of threads is
 *                 'nqueues'*'nthreads'.
 * (6) 'print': Whether to print out the array before and after scaling.
 */
int main (int argc, char** argv) {
  // All inputs must be given. Else, barf.
  if (7 != argc) {
    std::cout << "Please use this program as follows" << std::endl
              << "./for <n> <chunksize> <scaling> <nqueues> <nthreads> "
              << "<print(0|1)>" << std::endl;
    exit (3);
  }

  const int n = atoi(argv[1]);
  const int chunk_size = atoi(argv[2]);
  const double scaling_factor = static_cast<double>(atof(argv[3]));

  if (chunk_size > n) {
    std::cout << "To demonstrate parallel for properly, please choose a "
              << "chunk_size <= n" << std::endl;
    exit (3);
  }

  // Set up the base case size
  pfunc::space_1D::base_case_size = static_cast<size_t>(chunk_size);

  const unsigned int nqueues = static_cast<unsigned int>(atoi(argv[4]));
  const unsigned int threads_per_queue = 
                               static_cast<unsigned int>(atoi(argv[5]));
  unsigned int* threads_per_queue_array = new unsigned int [nqueues];
  for (int i=0; i<nqueues; ++i) threads_per_queue_array[i] = threads_per_queue;
  const bool please_print = static_cast<bool>(atoi(argv[6]));

  // Create the vector
  std::vector<double> my_vector (n);
  for (int i=0; i<n; ++i) my_vector[i] = get_next_rand();

  if (please_print) print_vector (my_vector.begin(), my_vector.end());

  // Initialize PFunc
  taskmgr global_taskmgr (nqueues, threads_per_queue_array);

  // Spawn the root task
  task root_task;
  attribute root_attribute (false /*nested*/, false /*grouped*/);
  pfunc::parallel_for<generator_type, vector_scale, pfunc::space_1D> 
    root_for (pfunc::space_1D (0,n), 
              vector_scale (my_vector, scaling_factor), 
              global_taskmgr);

  double time = micro_time();
  pfunc::spawn (global_taskmgr, root_task, root_attribute, root_for);
  pfunc::wait (global_taskmgr, root_task);
  time = micro_time() - time;

  std::cout << "Scaling of " << n << " elements in " << chunk_size 
            << " chunks took " << time << " seconds" << std::endl;

  if (please_print) print_vector (my_vector.begin(), my_vector.end());

  return 0;
}
