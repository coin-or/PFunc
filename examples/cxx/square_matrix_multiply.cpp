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
 */
#include <iostream>
#include <cstdlib>
#include <vector>
#include <pfunc/pfunc.hpp>

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
 * Returns a random double in the range (0.0,1.0]. Notice that the random 
 * number generator is never initialized --- we don't really care about :).
 * @return random double in the range (0.0,1.0].
 */
static const double NORMALIZER = static_cast<double>(RAND_MAX);
static inline double get_next_rand () {
  return (static_cast<double>(rand())/NORMALIZER);
}

/**
 * Returns the next highest power of two for the given number.
 * @param[in] n The number for which we want to find the next closest power.
 * @return The power of 2 closest to 'n'. E.g., for 3, 4 is returned.
 */
static inline int get_closest_power_of_2 (const int n) {
  int closest_power_of_2 = 0x1;
  while (n > closest_power_of_2) closest_power_of_2 = closest_power_of_2 << 1;
  return closest_power_of_2;
}
 
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
  std::vector<double>& matrix;

  public:
  /**
   * Constructor. 
   * @param[in] dim Dimension of this matrix
   * @param[in] stride Stride to access succesive rows of this matrix.
   * @param[in] start_index Starting point for this matrix.
   * @param[in] matrix Reference to the vector that holds the elements.
   */
  divisible_matrix (const int dim,
                    const int stride,
                    const int start_index,
                    std::vector<double>& matrix) :
    dim (dim), stride(stride), start_index (start_index), matrix (matrix) {}

  /**
   * Return a new matrix, which represents the current matrix's quadarant.
   * Quadrants are numbered as shown here: Q1 | Q2
   *                                       -------
   *                                       Q3 | Q4
   * @param[in] quadrant 1,2,3 or 4. Determines the starting point.
   */
  divisible_matrix split (const int quadrant) const {
    const int new_dim = dim/2;
    const int new_stride = stride/2;
    const int new_start_index = start_index + 
                                (1==quadrant) ? 0 :
                                  (2==quadrant) ? new_dim :
                                    (3==quadrant) ? (dim*dim)/2:
                                      (dim*dim)/2 + new_dim;
    return divisible_matrix (new_dime, new_stride, new_start_index, matrix);
  }

  /**
   * A means to access the [i][j]th element of the matrix from the RHS.
   * @param[in] i The row number [0,dim)
   * @param[in] j The column number [0,dim)
   * @return [i][j]th element.
   */
  double operator[][] (int i, int j) const {
    assert (i<dim && j<dim);
    return matrix[start_index + (i*stride) + j];
  }

  /**
   * A means to access the [i][j]th element of the matrix from the LHS.
   * @param[in] i The row number [0,dim)
   * @param[in] j The column number [0,dim)
   * @return [i][j]th element.
   */
  double& operator[][] (int i, int j) {
    assert (i<dim && j<dim);
    return matrix[start_index + (i*stride) + j];
  }

  /**
   * @return Dimension of the matrix
   */
  int dimension () const { return dim; }
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
        sum += A[i][k]*B[k][j];
      }
      C[i][j] += sum;
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
  int base_case_dim;

  public:
  /**
   * Constructor 
   * @param[in] A The submatrix A
   * @param[in] B The submatrix B
   * @param[in] C The submatrix C, which has been initialized
   * @param[in] base_case_dim Dimension at which we directly multiply.
   */
   dgemm_op (const divisible_matrix& A,
             const divisible_matrix& B,
             divisible_matrix& C,
             const int base_case_dim) : 
     A (A), B (B), C (C), base_case_dim (base_case_dim) {}

  /**
   * Check if we need to recurse any further. If so, spawn additional tasks.
   * Otherwise, directly multiply the two matrices.
   */
  void operator()(void) {
    assert (A.dimension() == B.dimension() == C.dimension());

    // Multiply directly if we have small enough matrices
    if (A.dimension() <= base_case_dim) {
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
      dgemm_op q1_op_1 (q1_A, q1_B, q1_C, base_case_dim);
      dgemm_op q1_op_2 (q2_A, q1_B, q1_C, base_case_dim);
      dgemm_op q2_op_1 (q1_A, q2_B, q2_C, base_case_dim);
      dgemm_op q2_op_2 (q2_A, q2_B, q2_C, base_case_dim);
      dgemm_op q3_op_1 (q3_A, q3_B, q3_C, base_case_dim);
      dgemm_op q3_op_2 (q4_A, q3_B, q3_C, base_case_dim);
      dgemm_op q4_op_1 (q3_A, q4_B, q4_C, base_case_dim);
      dgemm_op q4_op_2 (q4_A, q4_B, q4_C, base_case_dim);


      // Create the task objects
      task q1_task;
      task q2_task;
      task q3_task;
      // we will execute Q4 ourselves -- no need for a task

      // spawn the first four tasks
      pfunc::spawn (q1_task, q1_op_1);
      pfunc::spawn (q2_task, q2_op_1);
      pfunc::spawn (q3_task, q3_op_1);
      q4_op_1();
      pfunc::wait (q1_task);
      pfunc::wait (q2_task);
      pfunc::wait (q3_task);

      // spawn the next four tasks
      pfunc::spawn (q1_task, q1_op_2);
      pfunc::spawn (q2_task, q2_op_2);
      pfunc::spawn (q3_task, q3_op_2);
      q4_op_2();
      pfunc::wait (q1_task);
      pfunc::wait (q2_task);
      pfunc::wait (q3_task);
    }
  }
};
