#ifndef PFUNC_PARALLEL_WHILE_HPP
#define PFUNC_PARALLEL_WHILE_HPP

#include <pfunc/pfunc.hpp>
#include <iostream>

namespace pfunc {
/**
 * A structure that implements the parallel_while loop. To initialize, a 
 * range of InputIterators is given. Each element in the given range is 
 * executed in parallel by spawning tasks. 
 *
 * Types:
 * PFuncInstanceType: The type of PFunc library instance. The library instance
 *                    must use pfunc::use_default for the Functor! That is, it 
 *                    should use virtual operator().
 * InputIterator: An input iterator that has value_type.
 * WhileExecutable: A functor that is a model of WhileExecutable concept.
 * 
 */
template <typename PFuncInstanceType, /*type of PFunc instance*/
          typename InputIterator, /*type of the iterator*/
          typename WhileExecutable> /*type of the function object*/
struct parallel_while : pfunc::virtual_functor {
  public:
  typedef typename PFuncInstanceType::taskmgr TaskMgrType;
  typedef typename PFuncInstanceType::task TaskType;
  typedef typename InputIterator::value_type ValueType;

  private:
  InputIterator first;
  InputIterator last;
  const WhileExecutable& func;
  TaskMgrType& taskmgr;

  /**
   * A structure to execute the base case of WhileExecutable. We need this 
   * structure because PFunc only accepts void operator()(void), but we 
   * need void operator()(const ValueType&). This is the wrapper that 
   * allows execution by PFunc.
   */
  struct while_wrapper : pfunc::virtual_functor {
    private:
    const WhileExecutable& func;
    const ValueType& value;

    public:
    /**
     * Constructor
     * @param[in] func The func that is executed on value.
     * @param[in] value The value on which func is to be executed.
     */
    while_wrapper (const WhileExecutable& func, const ValueType& value) :
      func (func), value (value) {}

    /**
     * The wrapper operator() function that invokes func() with value.
     */
    void operator()(void) { func (value); }
  };

  public:
  /**
   * Constructor:
   * @param[in] first The iterator pointing to the first element.
   * @param[in] last The iterator pointing to the last element.
   * @param[in] func The function to execute on each object.
   * @param[in] taskmgr The task manager to use for this parallel_while.
   *
   * TODO: Make parallel_while work with global task manager.
   */
  parallel_while (InputIterator first,
                  InputIterator last,
                  const WhileExecutable& func,
                  TaskMgrType& taskmgr) : 
     first (first), last(last), func(func), taskmgr (taskmgr) {}

  /**
   * Operator that does the parallelization.
   */
  void operator() (void) {
    std::vector<TaskType*> tasks;
    std::vector<while_wrapper*> functors;
    // Go through each element in the list and spawn a new task for each 
    // element in [first, last). One optimization that we might want to 
    // perform is that we pick elements from this list in parallel instead 
    // of sequentially --- but that is for later.
    int task_index = 0;
    while (first != last) {
      tasks.push_back (new TaskType());
      functors.push_back (new while_wrapper (func, *first));
      pfunc::spawn (taskmgr, 
                    *(tasks [task_index]),
                    *(functors [task_index]));
      ++first;
      ++task_index;
    }

    // Now that all of them have been spawned, wait on them.
    pfunc::wait_all (taskmgr, tasks.begin(), tasks.end());

    // Deallocate everything.
    for (int i=0; i<task_index; ++i) {
      delete tasks[i];
      delete functors[i];
    }
  }
};
}

#endif // PFUNC_PARALLEL_WHILE_HPP
