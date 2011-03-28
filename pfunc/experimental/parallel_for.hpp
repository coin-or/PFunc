#ifndef PFUNC_PARALLEL_FOR_HPP
#define PFUNC_PARALLEL_FOR_HPP

#include <pfunc/pfunc.hpp>

namespace pfunc {

/**
 * A function much akin to for_each in STL. Takes in a range and a functor.
 * The assumption is that the functor has the access to the entire container
 * and hence all the harness needs to do is provide access to the correct 
 * range.
 * @param[in] space The iteration space.
 * @param[in] func The function object to be applied to every element.
 *
 * NOTE: This function currently uses a local task manager.
 *
 * NOTE: To use parallel_for, the Functor used in PFuncInstanceType must be 
 * pfunc::use_default! If a definite type is given, parallel_for fails to 
 * execute.
 */
template <typename PFuncInstanceType /*type of PFunc instantiated*/,
          typename UnaryFunctor/*type of the function*/>
struct parallel_for : pfunc::detail::virtual_functor {
  public:
  typedef typename PFuncInstanceType::taskmgr TaskMgrType;
  typedef typename PFuncInstanceType::task TaskType;

  private:
  space_1D space; 
  UnaryFunctor func;
  TaskMgrType& taskmgr;

  public:
  /**
   * Constructor
   * @param[in] space The space over which to iterate
   * @param[in] func The function to execute over elements in this space
   */
  parallel_for (space_1D space, 
                UnaryFunctor func,
                TaskMgrType& taskmgr) : 
       space(space), func(func), taskmgr (taskmgr) {}

  void operator() (void) {
    if (space.can_split ()) {
      space_1D right_space = space.split ();

      // Spawn a task for executing the right space and execute the left
      // space yourself.
      TaskType right_space_task;
      parallel_for<PFuncInstanceType, UnaryFunctor> right_space_for 
                                            (right_space, func, taskmgr);

      pfunc::spawn (taskmgr, right_space_task, right_space_for);
      (*this)(); // executing this loop ourselves.
      pfunc::wait (taskmgr, right_space_task);

    } else {
      // No more splitting --- simply invoke the function on the given space.
      func (space);
    }
  }
};

} // namespace pfunc

#endif // PFUNC_PARALLEL_FOR_HPP
