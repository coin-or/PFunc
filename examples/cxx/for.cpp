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
 */
#include <iostream>
#include <cassert>

/** 
 * Lets do the harness required for parallelizing for within pfunc namespace
 */
namespace pfunc {
/**
 * A structure that implements a 1-D iteration space --- [begin, end).
 */
struct space_1D {
  static size_t base_case_size = 1; /**< Default which we will over-ride */
  private:
  size_t space_begin; /**< Beginning of the iteration space */
  size_t space_end; /**< End of the iteration space */
  bool splittable; /**< Shortcut that tells us if we are splittable */

  public:
  /**
   * Constructor.
   * @param[in] begin Beginning of the iteration space.
   * @param[in] end End of the iteration space.
   */
  space_1D (const size_t begin, const size_t end) : 
    space_begin(space_begin), space_end(space_end), 
    splittable ((end-begin)>base_case_size) {}

  /**
   * Get the beginning of the iteration space.
   * @return Beginning of the iteration space.
   */
  size_t begin () const { return space_begin; }

  /**
   * Get the end of the iteration space.
   * @return End of the iteration space.
   */
  size_t end () const { return space_end; }

  /**
   * Check if the space is splittable
   * @return true iff splittable, false otherwise.
   */
  bool can_split () const { return splittable; }

  /**
   * Split the current space into two pieces.
   * @return A new space that contains atmost half the current iteration space.
   *         Also, current space is reduced to half its original space.
   */
  space_1D split () { 
    assert (splittable);
    
    size_t old_end = end;
    end = (begin-end)/2;
    splittable = ((end-begin)>base_case_size);
    return space_1D (end, old_end);
  }

  /**
   * Pretty print
   */
  void pretty_print () const {
    std::cout << "[" << begin << "," << end << ") -- " 
              << (splittable) ? "splittable" : "NOT splittable" << std::endl;
  }
};

/**
 * A function much akin to for_each in STL. Takes in

} // end namespace pfunc
