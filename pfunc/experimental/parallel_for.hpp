#ifndef PFUNC_PARALLEL_FOR_HPP
#define PFUNC_PARALLEL_FOR_HPP

#include <pfunc/pfunc.hpp>
#include <iostream>

namespace pfunc {

/**
 * A function much akin to for_each in STL. Takes in a range and a functor.
 * The assumption is that the functor has the access to the entire container
 * and hence all the harness needs to do is provide access to the correct 
 * range.
 * @param[in] space The iteration space.
 * @param[in] func The function object to be applied to every element.
 *                 This function object has to take in an object of space_1D.
 *                 i.e., void operator (const space_1D& space) { ... } must 
 *                 be defined on func.
 *
 * NOTE: This function currently uses a local task manager.
 *
 * NOTE: To use parallel_for, the Functor used in PFuncInstanceType must be 
 * pfunc::use_default! If a definite type is given, parallel_for fails to 
 * execute.
 *
 * TODO: All we need to parallelize is the ability to split. The space does not
 * have to be one dimensional nor does the split have to be into two halves.
 * Change space_1D into a template parameter. Also, change such that instead 
 * of a binary split, we use an n-ary split.
 */
template <typename PFuncInstanceType /*type of PFunc instantiated*/,
          typename ForExecutable/*type of the function*/,
          typename SpaceType /*type of the space*/>
struct parallel_for : pfunc::detail::virtual_functor {
  public:
  typedef typename PFuncInstanceType::taskmgr TaskMgrType;
  typedef typename PFuncInstanceType::task TaskType;
  typedef typename SpaceType::subspace_iterator_pair subspace_iterator_pair;
  typedef typename subspace_iterator_pair::first_type subspace_iterator;

  private:
  SpaceType space; 
  ForExecutable func;
  TaskMgrType& taskmgr;

  public:
  /**
   * Constructor
   * @param[in] space The space over which to iterate
   * @param[in] func The function to execute over elements in this space
   * @param[in] taskmgr The task manager to use for this parallel_for
   *
   * TODO: Make parallel_for work with global task manager.
   */
  parallel_for (SpaceType space, 
                ForExecutable func,
                TaskMgrType& taskmgr) : space(space), 
                                        func(func), 
                                        taskmgr (taskmgr) {}

  void operator() (void) {
    if (space.can_split ()) {
      // Split into subspaces.
      subspace_iterator_pair subspaces = space.split ();

      // Create a vector of tasks for each subspace but the first one.
      const int num_tasks = SpaceType::arity-1;
      TaskType subspace_tasks [num_tasks];

      // Save the first task to execute yourself, but do this last.
      assert (SpaceType::arity>=1);
      space = *(subspaces.first);
      ++(subspaces.first);

      // Iterate and launch the tasks
      int task_index = 0;
      while (subspaces.first != subspaces.second) {
        parallel_for<PFuncInstanceType, ForExecutable, SpaceType> 
            current_subspace_for (*(subspaces.first), func, taskmgr);
        pfunc::spawn (taskmgr, // the task manager to use
                      subspace_tasks[task_index], // task handle
                      current_subspace_for); // the subspace for this comptn
        ++(subspaces.first);
        ++task_index;
      }

      (*this)(); // executing this loop ourselves.

      // Wait for completion of other tasks
      pfunc::wait_all (taskmgr, // the task manager to use
                       subspace_tasks, // beginning
                       subspace_tasks+num_tasks); // end
    } else {
      // No more splitting --- simply invoke the function on the given space.
      func (space);
    }
  }
};

} // namespace pfunc

#endif // PFUNC_PARALLEL_FOR_HPP
