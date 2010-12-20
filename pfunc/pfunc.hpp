#ifndef PFUNC_HPP
#define PFUNC_HPP

/**
 * \file pfunc.hpp
 * \brief Implementation of PFUNC -- A Task Parallel API
 * \author Prabhanjan Kambadur
 *
 */

/**
 * We include all the relevant header files here so that the users do not 
 * have to figure out which header files to include when they need functions
 */
#include <pfunc/pfunc_common.h>
#include <pfunc/exception.hpp>
#include <pfunc/mutex.hpp>
#include <pfunc/event.hpp>
#include <pfunc/thread.hpp>
#include <pfunc/trampolines.hpp>
#include <pfunc/group.hpp>
#include <pfunc/attribute.hpp>
#include <pfunc/task.hpp>
#include <pfunc/taskmgr.hpp>
#include <pfunc/generator.hpp>


namespace pfunc {

  /* Convenience */
  using detail::group;

  /**
   * \param [out] attr Attribute whose priority is to be set 
   * \param [in] pri Priority for the attribute
   */
  template <typename Attribute>
  static inline void attr_priority_set (Attribute& attr, 
                                 const typename Attribute::priority_type& pri) {
    attr.set_priority (pri);
  }

  /**
   * \param [in] attr Attribute whose priority is to be retrieved
   * \param [out] pri Priority for the given attribute
   */
  template <typename Attribute>
  static inline void attr_priority_get (const Attribute& attr, 
                                 typename Attribute::priority_type& pri) {
    pri = attr.get_priority ();
  }

  /**
   * \param [out] attr Attribute whose queue number is to be set 
   * \param [in] qnum Queue number for the attribute.
   */
  template <typename Attribute>
  static inline void attr_qnum_set (Attribute& attr, 
                             const typename Attribute::qnum_type& qnum) {
    attr.set_queue_number (qnum);
  }

  /**
   * \param [in] attr Attribute whose queue number is to be retrieved
   * \param [out] qnum Queue number for the given attribute
   */
  template <typename Attribute>
  static inline void attr_qnum_get (const Attribute& attr, 
                             typename Attribute::qnum_type& qnum) {
    qnum = attr.get_queue_number ();
  }

  /**
   * \param [out] attr Attribute whose number of waiters is to be set 
   * \param [in] nwait Number of waiters for the attribute.
   */
  template <typename Attribute>
  static inline void attr_num_waiters_set (Attribute& attr, 
                        const typename Attribute::num_waiters_type& nwait) {
    attr.set_num_waiters (nwait);
  }

  /**
   * \param [in] attr Attribute whose number of waiters is to be retrieved.
   * \param [out] nwait Number of waiters for the given attribute.
   */
  template <typename Attribute>
  static inline void attr_num_waiters_get (const Attribute& attr,
                              typename Attribute::num_waiters_type& nwait) {
    nwait = attr.get_num_waiters ();
  }

  /**
   * \param [out] attr Attribute whose nestedness is to be set.
   * \param [in] nested Nestedness of the attribute.
   */
  template <typename Attribute>
  static inline void attr_nested_set (Attribute& attr, 
                             const typename Attribute::nested_type& nested) {
    attr.set_nested (nested);
  }

  /**
   * \param [in] attr Attribute whose nestedness is to be retrieved.
   * \param [out] nested Nestedness for the given attribute.
   */
  template <typename Attribute>
  static inline void attr_nested_get (const Attribute& attr, 
                               typename Attribute::nested_type& nested) {
    nested = attr.get_nested ();
  }

  /**
   * \param [out] attr Attribute whose level is to be set.
   * \param [in] level Level the attribute.
   */
  template <typename Attribute>
  static inline void attr_level_set (Attribute& attr, 
                             const typename Attribute::level_type& level) {
    attr.set_level (level);
  }

  /**
   * \param [out] attr Attribute whose level is to be retrieved.
   * \param [in] level Level the attribute.
   */
  template <typename Attribute>
  static inline void attr_level_get (Attribute& attr, 
                             typename Attribute::level_type& level) {
    level = attr.get_level ();
  }

  /**
   * \param [out] attr Attribute whose group is to be set/unset.
   * \param [in] grouped Determines the if the attribute is grouped.
   */
  template <typename Attribute>
  static inline void attr_grouped_set (Attribute& attr, 
                            const typename Attribute::grouped_type& grouped) {
    attr.set_grouped (grouped);
  }

  /**
   * \param [in] attr Attribute whose grouped nature is to be retrieved.
   * \param [out] grouped Determines the if the attribute is grouped.
   */
  template <typename Attribute>
  static inline void attr_grouped_get (const Attribute& attr,
                                typename Attribute::grouped_type& grouped) {
    grouped= attr.get_grouped ();
  }
  /**
   * \param [out] grp Group whose ID is to be set.
   * \param [in] id ID of the group.
   */
  static inline void group_id_set (group& grp, 
                                   const unsigned int& id) {
    grp.set_id (id);
  }

  /**
   * \param [in] grp Group whose ID is to be retrieved.
   * \param [out] id ID of the group.
   */
  static inline void group_id_get (const group& grp, 
                                   unsigned int& id) {
    id = grp.get_id ();
  }

  /**
   * \param [out] grp Group whose size is to be retrieved.
   * \param [in] size Size of the group.
   */
  static inline void group_size_set (group& grp, 
                                     const unsigned int& size) {
    grp.set_size (size);
  }

  /**
   * \param [in] grp Group whose size is to be retrieved.
   * \param [out] size Size of the group.
   */
  static inline void group_size_get (const group& grp,
                                     unsigned int& size) {
    size = grp.get_size ();
  }

  /**
   * \param [out] grp Group whose Barrier type is to be set.
   * \param [in] barr Barrier type of the group.
   */
  static inline void group_barrier_set (group& grp, 
                                        const unsigned int& barr) {
    grp.set_barrier (barr);
  }

  /**
   * \param [in] grp Group whose barrier type is to be retrieved.
   * \param [out] barr Barrier type of the group.
   */
  static inline void group_barrier_get (const group& grp,
                                        unsigned int& barr) {
    barr = grp.get_barrier ();
  }

/****************************************************************************
 * All the above functions make use of the taskmanager as a parameter. As 
 * a result, we have two versions of these functions. A global version that
 * uses as default task manager object and the (preferred) version that uses
 * a user supplied task manager object. Users are allowed to mix and match the
 * two calls, as long as they are consistent. That is, if you spawned a task
 * using the global task manager, please also wait on the task using the 
 * global task manager 
 ***************************************************************************/

 /**************************************************************************
  * Here are the local versions of the functions that use task manager 
  *************************************************************************/

  /**
   * Retrieves the ID of the currently executing thread.
   * \param [in] tmanager TaskManager that is currently running the threads.
   */ 
  template <typename TaskManager>
  static inline unsigned int thread_id (const TaskManager& tmanager) {
    unsigned int id;

    PFUNC_START_TRY_BLOCK()              
    id = const_cast<TaskManager&>(tmanager).current_thread_id (); 
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()

    return id;
  }

  /** 
   * \param [in] tmanager The task manager which is running the current task.
   * \param [out] rank Rank of the task in the group.
   */
  template <typename TaskManager>
  static inline void group_rank (const TaskManager& tmanager,
                                 unsigned int& rank) {
    PFUNC_START_TRY_BLOCK()                                                 
    rank = const_cast<TaskManager&>(tmanager).current_task_group_rank ();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /** 
   * \param [in] tmanager The task manager which is running the current task.
   * \param [out] size Size of the group in which the task is a part of.
   */
  template <typename TaskManager>
  static inline void group_size (const TaskManager& tmanager,
                                 unsigned int& size) {
    PFUNC_START_TRY_BLOCK()                                                   
    size = const_cast<TaskManager&>(tmanager).current_task_group_size ();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /**
   * \param [in] taskmgr The taskmanager that is running the task.
   * \param [in,out] task The task to be waited on.
   */     
  template <typename TaskManager, typename TaskType>
  static inline void wait (TaskManager& tmanager, TaskType& task)  {
    PFUNC_START_TRY_BLOCK()                                                   
    task.wait (tmanager);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /**
   * Waits for one of the tasks specified in the set. The algorithm implemented
   * is one of test_all () followed by exponential backoff.
   *
   * \param [in] tmanager The task manager that is running the tasks.
   * \param [in] first First of the tasks to be waited on.
   * \param [in] last End marker for the tasks to be waited on (true last+1).
   * \param [out] completion_arr Array that contains the completion status.
   * 
   */     
  template <typename TaskManager, typename ForwardIterator>
  static inline void wait_any (TaskManager& tmanager,
                        ForwardIterator first, 
                        ForwardIterator last,
                        int* completion_arr)  {
    PFUNC_START_TRY_BLOCK()                                                  
    unsigned int loop_count = 1;
    while (!pfunc_test_all (tmanager, first, last, completion_arr)) {
    /** 
     * Exponential backoff can be implemented using "pause" command that both
     * Intel and AMD architectures provide. That being said, we want to not
     * including any architecture specific code. Quite the quandry :-/. Note
     * that on WINDOWS, we are not doing an exponential backoff since the
     * minimum resolution of the Sleep() function is 1 MilliSecond. So, we
     * always sleep for that amount of time since its quite large.
     */
#if PFUNC_LINUX == 1 
      pthread_yield();
#elif PFUNC_DARWIN == 1
      pthread_yield_np();
#elif PFUNC_WINDOWS == 1
      SwitchToThread();
#elif PFUNC_AIX == 1 && PFUNC_HAVE_SCHED_H
      sched_yield();
#else
      /** Do nothing */
#endif
      loop_count = 1;
    }
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
 }

 /**
  * Waits for all the tasks in the specified set to complete.
  *
  * \param [in] tmanager The task manager that is running the tasks.
  * \param [in] first Start marker of the tasks to be waited on
  * \param [in] last End marker for the tasks to be waited on (true last+1)
  */     
  template <typename TaskManager, typename ForwardIterator>
  static inline void wait_all (TaskManager& tmanager, 
                        ForwardIterator first, 
                        ForwardIterator last)  {
    PFUNC_START_TRY_BLOCK()                                                  
    while (first != last) wait (tmanager, *first++);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

 /**
  * Test for completion of the task associated with the task
  *
  * \param [in] tmanager The task manager that is running the tasks.
  * \param [in,out] task Task to be tested for completion.
  * \result True if the task completed, false otherwise.
  */     
 template <typename TaskManager, typename TaskType>
 static inline bool test (TaskManager& tmanager, TaskType& task)  {
   bool return_value = false;

   PFUNC_START_TRY_BLOCK()                                                  
   return_value = task.test (tmanager);
   PFUNC_END_TRY_BLOCK()
   PFUNC_CXX_CATCH_AND_RETHROW()

   return return_value;
 }

 /**
  * Test for completion of the tasks specified by the set of tasks.
  *
  * \param [in] tmanager The task manager that is running the tasks.
  * \param [in] first First of the tasks to be tested on.
  * \param [in] last End marker for the tasks to be tested on (true last+1).
  * \param [out] completion_arr Contains the results of the tests.
  */     
  template <typename TaskManager, typename ForwardIterator>
  static inline bool test_all (TaskManager& tmanager, 
                        ForwardIterator first, 
                        ForwardIterator last,
                        int* completion_arr)  {
    bool return_value = false;

   PFUNC_START_TRY_BLOCK()                                                  
    int i = 0;
    while (first != last) 
      completion_arr[i] = return_value = test (tmanager, *first++);
   PFUNC_END_TRY_BLOCK()
   PFUNC_CXX_CATCH_AND_RETHROW()

    return return_value;
  }

 /**
  * Executes a barrier across the tasks associated with the specified group.
  * The group is automatically gotten from the task that called barrier. For
  * context, the taskmanager running all the tasks has to be passed in as an
  * argument.
  *
  * \param [in] tmanager The task manager that is running the tasks.
  */
  template <typename TaskManager>
  static inline void barrier (TaskManager& tmanager)  {
    PFUNC_START_TRY_BLOCK()                                                  
    return tmanager.current_task_group_barrier ();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

 /**
  * Spawn the task specified. The specification consists of a task, the 
  * task attributes, the group the task belongs to and the functor that 
  * executes the task.
  *
  * \param [in] tmanager The task manager that is running the tasks.
  * \param [out] task Task to the task we are adding.
  * \param [in] attr Attributes with which to create this job.
  * \param [in,out] grp Group that contains the group of these tasks.
  * \param [in] func The work function to execute
  */
  template <typename TaskManager, 
            typename Task, 
            typename Attribute,
            typename Functor>
  static inline void spawn (TaskManager& tmanager,
                   Task& task,
                   const Attribute& attr,
                   group& grp,
                   Functor& func)  {
    PFUNC_START_TRY_BLOCK()                                                  
    tmanager.spawn_task (task, attr, grp, func);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

 /**************************************************************************
  * Here are the global versions of the functions that use task manager 
  *************************************************************************/

 /** Global variable that holds the task manager */
 detail::taskmgr_virtual_base* global_tmanager = NULL;

 /**
  * \brief Used to initialize a global task manager that can then be used 
  * in the following functions:
  * \see current_thread_id
  * \see group_rank
  * \see group_size
  * \see wait
  * \see wait_any
  * \see wait_all
  * \see test
  * \see test_all
  * \see run
  *
  * \param [in] tmanager The task manager that is to be used globally.
  */
  template <typename TaskManager>
  void init (TaskManager& tmanager)  {
    global_tmanager = &tmanager;
  }

  /**
   * \brief Clears the global task manager. Note that this does not destroy 
   * the task manager object itself. That is to be ensured by the user.
   */
  void clear ()  { 
    global_tmanager = NULL; 
  } 

  /**
   * Retrieves the ID of the currently executing thread.
   */ 
  static inline unsigned int thread_id ()  {
    unsigned int id;
    
    PFUNC_START_TRY_BLOCK()
    id = thread_id (*global_tmanager);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()

    return id;
  }

  /** 
   * \param [out] rank Rank of the task in the group.
   */
  static inline void group_rank (unsigned int& rank) {
    PFUNC_START_TRY_BLOCK()
    return group_rank (*global_tmanager, rank); 
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /** 
   * \param [out] size Size of the group in which the task is a part of.
   */
  static inline void group_size (unsigned int& size) {
    PFUNC_START_TRY_BLOCK()
    return group_size (*global_tmanager, size); 
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /**
   * \param [in,out] task The task to be waited on.
   */     
  template <typename TaskType>
  static inline void wait (TaskType& task)  {
    PFUNC_START_TRY_BLOCK()
    return wait (*global_tmanager, task);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /**
   * Waits for one of the tasks specified in the set. The algorithm implemented
   * is one of test_all () followed by exponential backoff.
   *
   * \param [in] first First of the tasks to be waited on.
   * \param [in] last End marker for the tasks to be waited on (true last+1).
   * \param [out] completion_arr Array that contains the completion status.
   * 
   */     
  template <typename ForwardIterator>
  static inline void wait_any (ForwardIterator first, 
                               ForwardIterator last,
                               int* completion_arr)  {
    PFUNC_START_TRY_BLOCK()
    return wait_any (*global_tmanager, first, last, completion_arr);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

 /**
  * Waits for all the tasks in the specified set to complete.
  *
  * \param [in] first First of the tasks to be waited on
  * \param [in] last End marker for the tasks to be waited on (true last+1)
  */     
  template <typename ForwardIterator>
  static inline void wait_all (ForwardIterator first, 
                               ForwardIterator last)  {
    PFUNC_START_TRY_BLOCK()
    return wait_all (*global_tmanager, first, last);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

 /**
  * Test for completion of the task associated with the task
  *
  * \param [in,out] task Task to be tested for completion.
  * \result True if the task completed, false otherwise.
  */     
  template <typename TaskType>
  static inline bool test (TaskType& task)  {
    bool ret_val;
    
    PFUNC_START_TRY_BLOCK()
    ret_val = test (*global_tmanager, task);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
    
    return ret_val;
 }

 /**
  * Test for completion of the tasks specified by the set of tasks.
  *
  * \param [in] first First of the tasks to be tested on.
  * \param [in] last End marker for the tasks to be tested on (true last+1).
  * \param [out] completion_arr Contains the results of the tests.
  */     
  template <typename ForwardIterator>
  static inline bool test_all (ForwardIterator first, 
                               ForwardIterator last,
                               int* completion_arr)  {
    bool ret_val;

    PFUNC_START_TRY_BLOCK()
    ret_val = test_all (*global_tmanager, first, last, completion_arr);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()

    return ret_val;
  }

 /**
  * Executes a barrier across the tasks associated with the specified group.
  * The group is automatically gotten from the task that called barrier. 
  */
  static inline void barrier ()  {
    PFUNC_START_TRY_BLOCK()
    return ::pfunc::barrier (*global_tmanager);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

 /**
  * Spawn the task specified. The specification consists of a task, the 
  * task attributes, the group the task belongs to and the functor that 
  * executes the task.
  *
  * \param [out] task Task to the task we are adding.
  * \param [in] attr Attributes with which to create this job.
  * \param [in,out] grp Group that contains the group of these tasks.
  * \param [in] func The work function to execute
  */
  template <typename Task, 
            typename Attribute,
            typename Functor>
  static inline void spawn (Task& task,
                          const Attribute& attr,
                          group& grp,
                          Functor& func)  {
    PFUNC_START_TRY_BLOCK()
    global_tmanager->spawn_task(reinterpret_cast<void*>(&task), 
                                reinterpret_cast<void*>(
                                  &(const_cast<Attribute&>(attr))), 
                                reinterpret_cast<void*>(&grp), 
                                reinterpret_cast<void*>(&func));
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /*
   * Set the maximum number of attempts before yielding for the specified 
   * task manager.
   *
   * \param [out] tmanager The task manager in question.
   * \param [out] attempts Contains the value of max attempts.
   */
  template <typename TaskManager>
  static inline void taskmgr_max_attempts_set (TaskManager& tmanager,
                                               const unsigned int& attempts) {
    PFUNC_START_TRY_BLOCK()
    tmanager.set_max_attempts (attempts);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /*
   * Get the maximum number of attempts before yielding for teh global runtime
   *
   * \param [out] attempts Contains the value of max attempts.
   */
  static inline void taskmgr_max_attempts_set (const unsigned int& attempts) {
    PFUNC_START_TRY_BLOCK()
    global_tmanager->set_max_attempts (attempts);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /*
   * Get the maximum number of attempts before yielding for the specified 
   * task manager.
   *
   * \param [out] tmanager The task manager in question.
   * \param [out] attempts Contains the value of max attempts.
   */
  template <typename TaskManager>
  static inline void taskmgr_max_attempts_get (TaskManager& tmanager,
                                               unsigned int& attempts) {
    PFUNC_START_TRY_BLOCK()
    attempts = tmanager.get_max_attempts ();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }

  /*
   * Get the maximum number of attempts before yielding for teh global runtime
   *
   * \param [out] attempts Contains the value of max attempts.
   */
  static inline void taskmgr_max_attempts_get (unsigned int& attempts) {
    PFUNC_START_TRY_BLOCK()
    attempts = global_tmanager->get_max_attempts ();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CXX_CATCH_AND_RETHROW()
  }
} // namespace pfunc
#endif // PFUNC_HPP
