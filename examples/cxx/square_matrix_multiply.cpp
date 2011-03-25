/**
 * @author Prabhanjan Kambadur
 * 
 * A program that computes the product of two matrices using divide and
 * conquer. Here is the brief sketch of how we go about computing A*B.
 * 
 * Let A = A11 | A12    and  B = B11 | B12
 *         _________             _________
 *         A21 | A22             B21 | B22
 *                      
 * Then, C = A*B = C11 | C12
 *                 ---------
 *                 C21 | C22
 * 
 * Where, C11 = B11*A11 + B11*A21
 *        C21 = B21*A21 + B21*A22
 *        C12 = B12*A11 + B12*A21
 *        C22 = B22*A21 + B22*A22
 *
 * Therefore, we recursively divide the computation of C = A*B into four 
 * quadrants, each of which require 2 matrix matrix multiplications. These
 * 8 matrix-matrix multiplications can be executed in two batches of 4 each
 * as they write to independent locations. That is, we compute C in the 
 * following manner:
 *
 * spawn C11 = B11*A11 
 * spawn C21 = B21*A21 
 * spawn C12 = B12*A11 
 * spawn C22 = B22*A21 
 * sync
 * spawn C11 += B11*A21
 * spawn C21 += B21*A22
 * spawn C12 += B12*A21
 * spawn C22 += B22*A22
 * sync 
 *
 * Matrix representation:
 * The matrix is represented in ROW-MAJOR format. That is, 
 * A = [1 2; 3 4] is laid out in memory as [1 2 3 4]; 
 *     
 * Base case:
 * Recursion is ended when we reach small enough matrices that we can multiply
 * them without further division. Ideally, this is done when A, B, and C can
 * fit into cache. The base case multiplication is done here using the 3-loop
 * formula, but can be replaced by DGEMM kernel as well.
 * 
 * Optimization:
 * As this example is meant for demonstration purposes only, we will only deal 
 * with matrices that are not only square, but also powers of 2.
 *
 * Inputs:
 * (1) 'n': The dimension of the square matrix. If this is not a power of two, 
 *          'n' is rounded up to the next power of two. The matrix will be 
 *          generated randomly.
 * (2) 'x': The dimensionality of the matrix below which sequential multiply 
 *          is used. As a rule, 'x' <= 'n'.
 * (3) 'nqueues': Number of queues to create
 * (4) 'nthreads': Number of threads PER QUEUE. Total number of threads is 
 *                 'nqueues' * 'nthreads'.
 * (5) 'print': The value > 0 results in A, B, and C being printed.
 */
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <pfunc/pfunc.hpp>
#include <pfunc/utility.h>

/**
 * First, let us get the definition of the PFunc instance.
 */
struct dgemm_op;
typedef 
pfunc::generator <pfunc::cilkS, /* Cilk-style scheduling */
                  pfunc::use_default, /* No task priorities needed */
                  dgemm_op /* function type*/> generator_type;
typedef generator_type::attribute attribute;
typedef generator_type::task task;
typedef generator_type::group group;
typedef generator_type::taskmgr taskmgr;
typedef generator_type::functor functor;

/**
 * Three global variables:
 * (1) 'n'
 * (2) 'x'
 * (3) 'print'
 */
static int problem_dim = 4; /* default 4x4 */
static int problem_base_case_dim = 4; /* default to problem_dim */
static bool please_print = false; /* do not print */

/**
 * A structure to hold the matrices. We initially have one big ROW-MAJOR 
 * representation of the matrices A, B, and C. As we continually divide 
 * each matrix into 4 equaly sized submatrices, the starting point, the 
 * stride and the dimension changes. Basically, with each subdivision, 
 * (1) The dimension halves.
 * (2) The stride doubles.
 * (3) The starting point changes according to the quadrant.
 *
 */
struct divisible_matrix {
  private:
  int dim;
  int stride;
  int start_index;
  bool transposed;
  std::vector<double>& matrix;

  public:

  /**
   * Constructor. 
   * @param[in] dim Dimension of this matrix
   * @param[in] stride Stride to access succesive rows of this matrix.
   * @param[in] start_index Starting point for this matrix.
   * @param[in] transposed True if this is a transposed representation.
   * @param[in] matrix Reference to the vector that holds the elements.
   */
  divisible_matrix (const int dim,
                    const int stride,
                    const int start_index,
                    std::vector<double>& matrix,
                    const bool transposed = false):
    dim (dim), stride(stride), start_index (start_index), 
    matrix (matrix), transposed (transposed) {}
  /**
   * Return a new matrix, which represents the current matrix's quadarant.
   * Quadrants are numbered as shown here: Q1 | Q2
   *                                       -------
   *                                       Q3 | Q4
   * @param[in] quadrant 1,2,3 or 4. Determines the starting point.
   */
  divisible_matrix split (const int quadrant) const {
    const int new_dim = dim/2;
    const int new_start_index = start_index + 
                                (1==quadrant) ? 0 :
                                  (2==quadrant) ? new_dim :
                                    (3==quadrant) ? (dim*dim)/2:
                                      (dim*dim)/2 + new_dim;
    return divisible_matrix (new_dim, 
                             stride, 
                             new_start_index, 
                             matrix,
                             transposed);
  }

  /**
   * A means to access the [i][j]th element of the matrix from the RHS.
   * @param[in] i The row number [0,dim)
   * @param[in] j The column number [0,dim)
   * @return [i][j]th element.
   */
  double operator() (int i, int j) const {
    assert (i<dim && j<dim);
    return (transposed) ? 
      matrix[start_index+(j*stride)+i]:matrix[start_index+(i*stride)+j];
  }

  /**
   * A means to access the [i][j]th element of the matrix from the LHS.
   * @param[in] i The row number [0,dim)
   * @param[in] j The column number [0,dim)
   * @return [i][j]th element.
   */
  double& operator() (int i, int j) {
    assert (i<dim && j<dim);
    return (transposed) ? 
      matrix[start_index+(j*stride)+i]:matrix[start_index+(i*stride)+j];
  }

  /**
   * @return Dimension of the matrix
   */
  int dimension () const { return dim; }

  /**
   * Pretty-print the matrix
   */
  void pretty_print () const {
    // Do some formatting
    std::ios_base::fmtflags original_flags = std::cout.flags();
    std::cout.precision(2);
    std::cout.setf (std::ios_base::fixed, std::ios_base::floatfield);
    for (int i=0; i<dim; ++i) {
      for (int j=0; j<dim; ++j) {
        std::cout << " " << (*this)(i,j);
      }
      std::cout << std::endl;
    }
    std::cout.flags(original_flags);
  }
};

/**
 * Function to multiply two divisible_matrix-es using the three loop formula
 * @param[in] A Sub-matrix A.
 * @param[in] B Sub-matrix A.
 * @param[out] C Sub-matrix C holding A*B. We assume C is initialized.
 */
static inline void dgemm_three_loop (const divisible_matrix& A,
                                     const divisible_matrix& B,
                                     divisible_matrix& C) {
  assert (A.dimension() == B.dimension() == C.dimension());
  const register int dim = A.dimension();
  for (int i=0; i<dim; ++i) {
    for (int j=0; j<dim; ++j) {
      register double sum=0.0;
      for (int k=0; k<dim; ++k) {
        sum += A(i,k)*B(k,j);
      }
      C(i,j) += sum;
    }
  }
}

/**
 * Parallel function executed by us to compute the matrix product. Explanation
 * of parallelism has been given at the top of the file. Briefly, the matrix 
 * is sub-divided into four quadrants and matrix product computed recursively.
 */
struct dgemm_op {
  private:
  const divisible_matrix& A; 
  const divisible_matrix& B; 
  divisible_matrix& C; 

  public:
  /**
   * Constructor 
   * @param[in] A The submatrix A
   * @param[in] B The submatrix B
   * @param[in] C The submatrix C, which has been initialized
   */
   dgemm_op (const divisible_matrix& A,
             const divisible_matrix& B,
             divisible_matrix& C) : A (A), B (B), C (C) {}

  /**
   * Check if we need to recurse any further. If so, spawn additional tasks.
   * Otherwise, directly multiply the two matrices.
   */
  void operator()(void) {
    assert (A.dimension() == B.dimension() == C.dimension());

    // Multiply directly if we have small enough matrices
    if (A.dimension() <= problem_base_case_dim) {
      dgemm_three_loop (A, B, C);
    } else { // Divide into four quadrants and recurse.
      // Split the matrices
      divisible_matrix q1_A = A.split (1);
      divisible_matrix q2_A = A.split (2);
      divisible_matrix q3_A = A.split (3);
      divisible_matrix q4_A = A.split (4);
      divisible_matrix q1_B = B.split (1);
      divisible_matrix q2_B = B.split (2);
      divisible_matrix q3_B = B.split (3);
      divisible_matrix q4_B = B.split (4);
      divisible_matrix q1_C = C.split (1);
      divisible_matrix q2_C = C.split (2);
      divisible_matrix q3_C = C.split (3);
      divisible_matrix q4_C = C.split (4);
      
      // Create the task functors
      //
      // q1_C | q2_C    q1_A | q2_A      q1_B | q2_B
      // -----|----- =  -----|------  *  -----|------
      // q3_C | q4_C    q3_A | q4_A      q3_B | q4_B
      //
      dgemm_op q1_op_1 (q1_A, q1_B, q1_C);
      dgemm_op q1_op_2 (q2_A, q1_B, q1_C);
      dgemm_op q2_op_1 (q1_A, q2_B, q2_C);
      dgemm_op q2_op_2 (q2_A, q2_B, q2_C);
      dgemm_op q3_op_1 (q3_A, q3_B, q3_C);
      dgemm_op q3_op_2 (q4_A, q3_B, q3_C);
      dgemm_op q4_op_1 (q3_A, q4_B, q4_C);
      dgemm_op q4_op_2 (q4_A, q4_B, q4_C);

      // Create the task objects
      task q1_task;
      task q2_task;
      task q3_task;
      // we will execute Q4 ourselves -- no need for a task

      // spawn the first four tasks
      pfunc::global::spawn (q1_task, q1_op_1);
      pfunc::global::spawn (q2_task, q2_op_1);
      pfunc::global::spawn (q3_task, q3_op_1);
      q4_op_1();
      pfunc::global::wait (q1_task);
      pfunc::global::wait (q2_task);
      pfunc::global::wait (q3_task);

      // spawn the next four tasks
      pfunc::global::spawn (q1_task, q1_op_2);
      pfunc::global::spawn (q2_task, q2_op_2);
      pfunc::global::spawn (q3_task, q3_op_2);
      q4_op_2();
      pfunc::global::wait (q1_task);
      pfunc::global::wait (q2_task);
      pfunc::global::wait (q3_task);
    }
  }
};

/**
 * main loop of this example. Reads in all the values, creates the matrices,
 * multiplies them and finally prints the results if needed.
 */
int main (int argc, char** argv) {
  // All inputs must be given. Else, barf.
  if (6 != argc) {
    std::cout << "Please use this program as follows" << std::endl
              << "./square_matrix_multiply <n> <x> <nqueues> <nthreads> "
              << "<print(0|1)>" << std::endl;
    exit(3);
  }

  problem_dim = get_closest_power_of_2 (atoi(argv[1]));
  problem_base_case_dim = atoi(argv[2]);
  assert (problem_dim >= problem_base_case_dim);
  const unsigned int nqueues = static_cast<unsigned int>(atoi(argv[3]));
  const unsigned int threads_per_queue = 
                               static_cast<unsigned int>(atoi(argv[4]));
  unsigned int* threads_per_queue_array = new unsigned int [nqueues];
  for (int i=0; i<nqueues; ++i) threads_per_queue_array[i] = threads_per_queue;
  please_print = static_cast<bool>(atoi(argv[5]));

  // Create the matrices
  const int num_elements = problem_dim*problem_dim;
  std::vector<double> A_storage (num_elements);
  std::vector<double> B_storage (num_elements);
  std::vector<double> C_storage (num_elements, 0.0);

  for (int i=0; i<num_elements; ++i) {
    A_storage[i] = get_next_rand();
    B_storage[i] = get_next_rand();
  }

  // Set up the matrices
  divisible_matrix A (problem_dim,problem_dim,0,A_storage);
  divisible_matrix B (problem_dim,problem_dim,0,B_storage, true/*transposed*/);
  divisible_matrix C (problem_dim,problem_dim,0,C_storage);

  // Initialize PFunc
  taskmgr global_taskmgr (nqueues, threads_per_queue_array);
  pfunc::global::init (global_taskmgr);

  // Spawn the root task
  task root_task;
  attribute root_attribute (false /*nested*/, false /*grouped*/);
  dgemm_op root_dgemm (A, B, C);
  double time = micro_time();
  pfunc::global::spawn (root_task, root_attribute, root_dgemm);
  pfunc::global::wait (root_task);
  time = micro_time() - time;

  std::cout << "Multiplication of two " << problem_dim << "x" << problem_dim 
            << " random matrices took " << time << " seconds" << std::endl;

  if (please_print) {
    A.pretty_print ();
    std::cout << std::endl;
    B.pretty_print ();
    std::cout << std::endl;
    C.pretty_print ();
  }

  // Clear
  pfunc::global::clear ();

  return 0;
}
