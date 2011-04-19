#ifndef PFUNC_SPACE_1D_HPP
#define PFUNC_SPACE_1D_HPP

#include <cassert>
#include <utility>
#include <vector>

namespace pfunc {
/**
 * A structure that implements a 1-D iteration space --- [begin, end).
 * It is a model of the interface Space (see Space.concept.ipp)
 */
struct space_1D {
  public:
  typedef std::vector<space_1D> subspace_container; /**< Container type */
  static size_t base_case_size; /**< Default which we will over-ride */

  const static size_t arity = 2;/**< Number of ways in which a space is split */
  const static size_t dimension = 1;/**< Dimensionality of the space */

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
  space_1D (const size_t space_begin, const size_t space_end) : 
    space_begin(space_begin), space_end(space_end), 
    splittable ((space_end-space_begin)>base_case_size) {}

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
   * Split the current space into two pieces and return an iterator pair that 
   * gives the right and the left subspaces.
   * @return A pair of iterators [begin, end) that will point to the split 
   *         subspaces.
   */
  subspace_container split () const { 
    // Make sure that the space is splittable
    assert (splittable);

    // Get a vector to store the subspaces
    subspace_container subspaces;

    const size_t split_point = space_begin + (space_end-space_begin)/2;
    const size_t left_space_begin = space_begin;
    const size_t left_space_end = split_point;
    const size_t right_space_begin = split_point;
    const size_t right_space_end = space_end;

    subspaces.push_back (space_1D (left_space_begin, left_space_end));
    subspaces.push_back (space_1D (right_space_begin, right_space_end));

    return subspaces;
  }

  /**
   * Pretty print
   */
  void pretty_print () const {
    std::cout << "[" << begin() << "," << end() << ") -- " 
              << ((splittable) ? "splittable" : "NOT splittable") << std::endl;
  }
};

/**
 * Initialize base_case_size to something sensible.
 */
size_t pfunc::space_1D::base_case_size = 100;

} // end namespace pfunc

#endif // PFUNC_SPACE_1D_HPP
