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
struct parallel_reduce : pfunc::virtual_functor {
  public:
  typedef typename PFuncInstanceType::taskmgr TaskMgrType;
  typedef typename PFuncInstanceType::task TaskType;

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
      typename SpaceType::subspace_container subspaces = space.split ();
      assert (SpaceType::arity>=1);

      // Create a vector of tasks for each subspace but the first one.
      const int num_tasks = SpaceType::arity-1;
      TaskType subspace_tasks [num_tasks];
      parallel_reduce<PFuncInstanceType, ReduceExecutable, SpaceType>*
        subspace_parallel_reducers [num_tasks];

      // Split func and create a vector of functors one for each task.
      std::vector<ReduceExecutable*> split_funcs;
      for (int i=0; i<num_tasks; ++i) 
        split_funcs.push_back (new ReduceExecutable(func.split ()));

      // Save the first task to execute yourself, but do this last.
      typename SpaceType::subspace_container::iterator first=subspaces.begin();
      space = *first++;

      // Iterate and launch the tasks
      int task_index = 0;
      while (first != subspaces.end()) {
        subspace_parallel_reducers [task_index] = new 
          parallel_reduce<PFuncInstanceType, ReduceExecutable, SpaceType> 
            (*first++, *(split_funcs[task_index]), taskmgr);
        pfunc::spawn (taskmgr, // the task manager to use
                      subspace_tasks[task_index], // task handle
                      *(subspace_parallel_reducers[task_index])); 
                        // the subspace for this comptn
        ++task_index;
      }

      (*this)(); // executing this loop ourselves.

      // Wait for completion of other tasks
      pfunc::wait_all (taskmgr, // the task manager to use
                       subspace_tasks, // beginning
                       subspace_tasks+num_tasks); // end

      // Join everything
      for (int i=0; i<num_tasks; ++i) { 
        func.join (*(split_funcs[i]));
        delete subspace_parallel_reducers[i];
        delete split_funcs[i];
      }
    } else {
      // No more splitting --- simply invoke the function on the given space.
      func (space);
    }
  }
};

} // namespace pfunc

#endif // PFUNC_PARALLEL_FOR_HPP
