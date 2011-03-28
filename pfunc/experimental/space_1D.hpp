#ifndef PFUNC_SPACE_1D_HPP
#define PFUNC_SPACE_1D_HPP

#include <cassert>

/** 
 * Lets do the harness required for parallelizing for within pfunc namespace
 */
namespace pfunc {
/**
 * A structure that implements a 1-D iteration space --- [begin, end).
 */
struct space_1D {
  public:
  static size_t base_case_size; /**< Default which we will over-ride */

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
   * Split the current space into two pieces.
   * @return A new space that contains atmost half the current iteration space.
   *         Also, current space is reduced to half its original space.
   */
  space_1D split () { 
    assert (splittable);

    size_t old_space_end = space_end;
    space_end = space_begin + (space_end-space_begin)/2;
    splittable = (space_end-space_begin)>base_case_size;
    return space_1D (space_end, old_space_end);
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
