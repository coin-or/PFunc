#ifndef PFUNC_PREDICATE_T_HPP
#define PFUNC_PREDICATE_T_HPP

/** 
 * \file predicate.hpp Different predicates for pulling tasks out of task 
 * queues.
 * \author Prabhanjan Kambadur.
 */

#include <pfunc/task_queue_set.hpp>

namespace pfunc { namespace detail {

  /**
   * Predicate type for getting tasks out at a "regular" scheduling point.
   * ValueType is task and PolicyName can be anything.
   */
  template <typename PolicyName, typename ValueType>
  struct regular_predicate_pair {
    typedef bool result_type;
    typedef ValueType* value_type;

    /**
     * Initialize the previous task.
     */
    regular_predicate_pair (value_type previous_task=NULL) {}

    /** 
     * Can this task be given to the calling thread? The answer is YES,
     * always YES, by default. If you like to change it, specialize!
     * @param[in] arg Pointer to the task that is being chosen.
     */
    bool own_pred (value_type current_task) const { return true; }

    /** 
     * Same as own_pred ()
     * @param[in] arg Pointer to the task that is being chosen.
     */
    bool steal_pred (value_type current_task) const { 
      return own_pred (current_task); 
    }
  };

  /**
   * Predicate type for getting tasks out at a "waiting" scheduling point.
   */
  template <typename PolicyName, typename ValueType>
  struct waiting_predicate_pair {
    typedef bool result_type;
    typedef ValueType* value_type;

    /**
     * Initialize the previous task.
     */
    waiting_predicate_pair (value_type previous_task=NULL) {}

    /** 
     * Can this task be given to the calling thread? The answer is YES,
     * always YES, by default. If you like to change it, specialize!
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool own_pred (value_type current_task) const { return true; }

    /** 
     * Same as own_pred ()
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool steal_pred (value_type current_task) const { 
      return own_pred (current_task); 
    }
  };

  /**
   * Predicate type for getting tasks out at a "group" scheduling point.
   */
  template <typename PolicyName, typename ValueType>
  struct group_predicate_pair {
    typedef bool result_type;
    typedef ValueType* value_type;
    value_type previous_task;

    /**
     * Initialize the previous task.
     */
    group_predicate_pair (value_type previous_task) : 
                                  previous_task (previous_task) {}

    /** 
     * Can this task be given to the calling thread? The answer is "As 
     * long as the previous and current task's are not in the same 
     * group". If they are in the same group, there can be a deadlock.
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool own_pred (value_type current_task) const {
      return !(previous_task->get_group() == current_task->get_group()) ; 
    } 

    /** 
     * Same as own_pred ()
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool steal_pred (value_type current_task) const { 
      return own_pred (current_task); 
    } 
  };

  /*************************************************************************
   * MODIFICATIONS FOR CILKS 
   *************************************************************************/

  /**
   * Define the waiting predicates for the cilkS task_queue_set. 
   */
  template <typename ValueType> 
  struct waiting_predicate_pair <cilkS, ValueType> { 
    typedef typename task_traits<ValueType>::attribute attribute; 
    typedef typename attribute::level_type level_type; 
    typedef bool result_type;
    typedef ValueType* value_type; 
   
    value_type previous_task; 
  
    /**
     * Initialize the previous task.
     */
    waiting_predicate_pair (value_type previous_task) : 
                                  previous_task (previous_task) {}

    /** 
     * have to ensure that we are only stealing a task that is at the same 
     * level in the spawn tree or lower. This is important to prevent thread
     * stack explosion.
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool own_pred (value_type current_task) const { 
      bool ret_val = false; 
      level_type previous_task_level = previous_task->get_attr().get_level (); 
      level_type current_task_level = current_task->get_attr().get_level (); 
      if (previous_task_level <= current_task_level) ret_val = true; 
      return ret_val; 
    } 

    /**
     * Same as own_pred()
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool steal_pred (value_type current_task) const { 
      return own_pred (current_task); 
    }
  };

  /**
   * Define the group stealing predicate for the cilk queue type. We
   * have to ensure that we are only stealing a task that is at the same 
   * level in the spawn tree or lower. This is important to prevent thread
   * stack explosion. Also important is to not steal anything in the same 
   * group as the task executing the group.
   */
  template <typename ValueType> 
  struct group_predicate_pair <cilkS, ValueType> { 
    typedef typename task_traits<ValueType>::attribute attribute; 
    typedef typename attribute::level_type level_type; 
    typedef bool result_type;
    typedef ValueType* value_type; 
   
    value_type previous_task; 
  
    /**
     * Initialize the previous task.
     */
    group_predicate_pair (value_type previous_task) : 
                                  previous_task (previous_task) {}

    /**
     * Return true only if the current_task is lower in the execution tree 
     * and is not of the same group as the previous_task that we were executing
     * @param[in] current_task The candidate task that we want to check for 
     *                         suitability.
     */
    bool own_pred (value_type current_task) const { 
      bool ret_val = false; 
      level_type previous_task_level = previous_task->get_attr().get_level (); 
      level_type current_task_level = current_task->get_attr().get_level (); 
      if (previous_task_level <= current_task_level) 
        if (previous_task->get_group() != current_task->get_group()) 
          ret_val = true; 
      return ret_val; 
    } 

    /**
     * Same as own_pred()
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool steal_pred (value_type current_task) const { 
      return own_pred (current_task); 
    }
  };

  /*************************************************************************
   * MODIFICATIONS FOR PRIOS
   *************************************************************************/
  
  /**
   * Define the waiting stealing predicate for the priority queue type. We
   * have to ensure that we are stealing a task, which has at least the same
   * priority as the one that we are waiting on. This prevents deadlocks from
   * occuring.
   */
  template <typename ValueType> 
  struct waiting_predicate_pair <prioS, ValueType> { 
    typedef typename task_traits<ValueType>::attribute attribute; 
    typedef typename task_traits<ValueType>::functor functor; 
    typedef compare_task_ptr<attribute, functor> compare_type; 
    typedef bool result_type;
    typedef ValueType* value_type; 
   
    value_type previous_task; 
    compare_type comp;  
  
    /**
     * Initialize the previous task.
     */
    waiting_predicate_pair (value_type previous_task) : 
                                  previous_task (previous_task) {}
   
    /**
     * Only pick a task that has lower priority than the task which we are
     * waiting to complete.
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool own_pred (value_type current_task) const { 
      bool ret_val = false; 
      if (comp (previous_task, current_task)) 
        ret_val = true; 
      return ret_val; 
    } 

    /**
     * Same as own_pred()
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool steal_pred (value_type current_task) const { 
      return own_pred (current_task); 
    }
  };


  /**
   * Define the group stealing predicate for the priority queue type. We
   * have to ensure that we are stealing a task, which has at least the same
   * priority as the one that we are waiting on. This prevents deadlocks from
   * occuring.
   */
  template <typename ValueType> 
  struct group_predicate_pair <prioS, ValueType> { 
    typedef typename task_traits<ValueType>::attribute attribute; 
    typedef typename task_traits<ValueType>::functor functor; 
    typedef compare_task_ptr<attribute, functor> compare_type; 
    typedef bool result_type;
    typedef ValueType* value_type; 
   
    value_type previous_task; 
    compare_type comp;  

    /**
     * Initialize the previous task.
     */
    group_predicate_pair (value_type previous_task) : 
                                  previous_task (previous_task) {}
  
    /**
     * First, check that the task we are trying to pick is not in our group.
     * Then pick the task only if it has lower priority than the task which we
     * are waiting to complete. 
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool own_pred (value_type current_task) const { 
      bool ret_val = false; 
      if (comp (previous_task, current_task)) 
        if (previous_task->get_group() != current_task->get_group()) 
          ret_val = true; 
      return ret_val; 
    } 

    /**
     * Same as own_pred()
     * @param[in] current_task Pointer to the task that is being chosen.
     */
    bool steal_pred (value_type current_task) const { 
      return own_pred (current_task); 
    }
  };

} /* namespace detail */ } /* namespace pfunc */

#endif // PFUNC_PREDICATE_T_HPP
