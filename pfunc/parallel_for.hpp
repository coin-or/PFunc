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
 * @param[in] space The iteration space. space is a model of Space.
 * @param[in] func The function object to be applied to every element.
 *                 This function object has to take in an object of space_1D.
 *                 i.e., void operator (const space_1D& space) { ... } must 
 *                 be defined on func. func is a model of the ForExecutable 
 *                 concept.
 *
 * NOTE: This function currently uses a local task manager.
 *
 * NOTE: To use parallel_for, the Functor used in PFuncInstanceType must be 
 * pfunc::use_default! If a definite type is given, parallel_for fails to 
 * execute.
 *
 */
template <typename PFuncInstanceType /*type of PFunc instantiated*/,
          typename ForExecutable/*type of the function*/,
          typename SpaceType /*type of the space*/>
struct parallel_for : pfunc::virtual_functor {
  public:
  typedef typename PFuncInstanceType::taskmgr TaskMgrType;
  typedef typename PFuncInstanceType::task TaskType;

  private:
  SpaceType space; 
  const ForExecutable& func;
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
                const ForExecutable& func,
                TaskMgrType& taskmgr) : space(space), 
                                        func(func), 
                                        taskmgr (taskmgr) {}

  void operator() (void) {
    if (space.can_split ()) {
      // Split into subspaces.
      typename SpaceType::subspace_container subspaces = space.split ();
      assert (1<=SpaceType::arity);

      // Create a vector of tasks for each subspace but the first one.
      const int num_tasks = SpaceType::arity-1;
      TaskType subspace_tasks [num_tasks];

      // Save the first task to execute yourself, but do this last.
      typename SpaceType::subspace_container::iterator first=subspaces.begin();
      space = *first++;

      // Iterate and launch the tasks
      int task_index = 0;
      while (first != subspaces.end()) {
        parallel_for<PFuncInstanceType, ForExecutable, SpaceType> 
            current_subspace_for (*first++, func, taskmgr);
        pfunc::spawn (taskmgr, // the task manager to use
                      subspace_tasks[task_index++], // task handle
                      current_subspace_for); // the subspace for this comptn
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
