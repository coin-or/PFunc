#ifndef PFUNC_PARALLEL_FOR_HPP
#define PFUNC_PARALLEL_FOR_HPP

#include <pfunc/pfunc.hpp>
#include <iostream>

namespace pfunc {

/**
 * A function much akin to accumulate in STL. Takes in a range and a functor.
 * The assumption is that the functor has the access to the entire container
 * and hence all the harness needs to do is provide access to the correct 
 * range.
 * @param[in] space The iteration space. space is a model of Space concept.
 * @param[in] func The function object to be applied to every element.
 *                 This function object has to take in an object of space_1D.
 *                 i.e., void operator (const space_1D& space) { ... } must 
 *                 be defined on func. func is a model of ReduceExecutable
 *                 concept.
 *
 * NOTE: This function currently uses a local task manager.
 *
 * NOTE: To use parallel_reduce, the Functor used in PFuncInstanceType must be 
 * pfunc::use_default! If a definite type is given, parallel_reduce fails to 
 * execute.
 *
 */
template <typename PFuncInstanceType /*type of PFunc instantiated*/,
          typename ReduceExecutable/*type of the function*/,
          typename SpaceType /*type of the space*/>
struct parallel_reduce : pfunc::detail::virtual_functor {
  public:
  typedef typename PFuncInstanceType::taskmgr TaskMgrType;
  typedef typename PFuncInstanceType::task TaskType;
  typedef typename SpaceType::subspace_iterator subspace_iterator;
  typedef typename SpaceType::subspace_iterator_pair subspace_iterator_pair;

  private:
  SpaceType space; 
  ReduceExecutable& func;
  TaskMgrType& taskmgr;

  public:
  /**
   * Constructor
   * @param[in] space The space over which to iterate
   * @param[in] func The function to execute over elements in this space
   * @param[in] taskmgr The task manager to use for this parallel_reduce
   *
   * TODO: Make parallel_reduce work with global task manager.
   */
  parallel_reduce (SpaceType space, 
                   ReduceExecutable& func,
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

      // Split func and create a vector of functors one for each task.
      std::vector<ReduceExecutable> split_funcs;
      for (int i=0; i<num_tasks; ++i) 
        split_funcs.push_back (func.split ());

      // Save the first task to execute yourself, but do this last.
      assert (SpaceType::arity>=1);
      space = *(subspaces.first);
      ++(subspaces.first);

      // Iterate and launch the tasks
      int task_index = 0;
      while (subspaces.first != subspaces.second) {
        parallel_reduce<PFuncInstanceType, ReduceExecutable, SpaceType> 
            current_subspace_reduce (*(subspaces.first), 
                                     split_funcs[task_index], 
                                     taskmgr);
        pfunc::spawn (taskmgr, // the task manager to use
                      subspace_tasks[task_index], // task handle
                      current_subspace_reduce); // the subspace for this comptn
        ++(subspaces.first);
        ++task_index;
      }

      (*this)(); // executing this loop ourselves.

      // Wait for completion of other tasks
      pfunc::wait_all (taskmgr, // the task manager to use
                       subspace_tasks, // beginning
                       subspace_tasks+num_tasks); // end

      // Join everything
      for (int i=0; i<num_tasks; ++i) func.join (split_funcs[i]);
    } else {
      // No more splitting --- simply invoke the function on the given space.
      func (space);
    }
  }
};

} // namespace pfunc

#endif // PFUNC_PARALLEL_FOR_HPP
