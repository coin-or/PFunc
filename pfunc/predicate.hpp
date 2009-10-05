#ifndef PFUNC_PREDICATE_T_HPP
#define PFUNC_PREDICATE_T_HPP

/** 
 * \file predicate.hpp Different predicates for pulling tasks out of task 
 * queues.
 * \author Prabhanjan Kambadur.
 */

#include <pfunc/scheduler.hpp>

namespace pfunc { namespace detail {

  /**
   * Predicate type for getting task out of your own queue. ValueType is 
   * the pointer to the task that we are checking the predicate for. This
   * predicate is always true for ALL scheduling policies.
   */
  template <typename ValueType>
  struct regular_get_own_predicate {
    /** Can this task be given to the calling thread? */
    bool operator ()(const ValueType&) const { return true; }
  };

  /**
   * Predicates are always given in pairs -- one for the thread's own 
   * queue -- this one is for the steal. 
   *
   * Regular stealing means that a thread is trying to pick up a task 
   * because its queue is empty -- NOT because its waiting on another 
   * task to complete. For that, we use the "waiting" predicates.
   *
   * The value of this predicate is always true.
   */
  template <typename ValueType> 
  struct regular_get_steal_predicate { \
    /** Can this task be given to the calling thread? */
    bool operator () (const ValueType&) const { return true; }
  };

/**
 * generate the stealing predicate for a task given a particular 
 * scheduling policy.
 */
#define PFUNC_DEFINE_STEAL_FOR_SCHED(policy,struct_name) \
  template <typename ValueType> \
  struct struct_name <policy,ValueType> { \
    struct_name (const ValueType&) {} \
    bool operator () (const ValueType&) const { return true; } \
  };

  /** 
   * Predicate template for getting tasks from the calling thread's own 
   * queue when waiting on a task.
   */
  template <typename SchedPolicyType, typename ValueType>
  struct waiting_get_own_predicate { };

  /** 
   * Predicate template for getting tasks from the another thread's 
   * queue when waiting on a task.
   */
  template <typename SchedPolicyType, typename ValueType>
  struct waiting_get_steal_predicate { };

  PFUNC_DEFINE_STEAL_FOR_SCHED(lifoS,waiting_get_own_predicate)
  PFUNC_DEFINE_STEAL_FOR_SCHED(lifoS,waiting_get_steal_predicate)
  PFUNC_DEFINE_STEAL_FOR_SCHED(fifoS,waiting_get_own_predicate)
  PFUNC_DEFINE_STEAL_FOR_SCHED(fifoS,waiting_get_steal_predicate)

/**
 * \def Define the waiting stealing predicate for the cilk queue type. We
 * have to ensure that we are only stealing a task that is at the same 
 * level in the spawn tree or lower. This is important to prevent thread
 * stack explosion.
 */
#define PFUNC_DEFINE_WAITING_STEAL_FOR_CILKS(struct_name) \
  template <typename ValueType> \
  struct struct_name <cilkS, ValueType> { \
    typedef typename task_traits<ValueType>::attribute attribute; \
    typedef typename attribute::level_type level_type; \
    typedef ValueType value_type; \
   \
    const value_type current; \
  \
    struct_name (const value_type& value) : current (value) {} \
   \
    bool operator ()(const value_type& next) const { \
      bool ret_val = false; \
      level_type current_level = current->get_attr().get_level (); \
      level_type next_level = next->get_attr().get_level (); \
      if (current_level <= next_level) ret_val = true; \
      return ret_val; \
    } \
  };

  PFUNC_DEFINE_WAITING_STEAL_FOR_CILKS(waiting_get_own_predicate)
  PFUNC_DEFINE_WAITING_STEAL_FOR_CILKS(waiting_get_steal_predicate)


/**
 * \def Define the waiting stealing predicate for the priority queue type. We
 * have to ensure that we are stealing a task, which has at least the same
 * priority as the one that we are waiting on. This prevents deadlocks from
 * occuring.
 */
#define PFUNC_DEFINE_WAITING_STEAL_FOR_PRIOS(struct_name) \
  template <typename ValueType> \
  struct struct_name <prioS, ValueType> { \
    typedef typename task_traits<ValueType>::attribute attribute; \
    typedef typename task_traits<ValueType>::functor functor; \
    typedef compare_task_ptr<attribute, functor> compare_type; \
    typedef ValueType value_type; \
   \
    const value_type current; \
    compare_type comp;  \
  \
    struct_name (const value_type& value) : current (value) {} \
   \
    bool operator ()(const value_type& next) const { \
      bool ret_val = false; \
      if (comp (current, next)) \
        ret_val = true; \
      return ret_val; \
    } \
  };

  PFUNC_DEFINE_WAITING_STEAL_FOR_PRIOS(waiting_get_own_predicate)
  PFUNC_DEFINE_WAITING_STEAL_FOR_PRIOS(waiting_get_steal_predicate)

  /** 
   * Predicate template for getting tasks from the calling thread's own 
   * queue when waiting on a barrier for a task.
   */
  template <typename SchedPolicyType, typename ValueType>
  struct barrier_get_own_predicate { };

  /** 
   * Predicate template for getting tasks from the a different task 
   * queue when waiting on a barrier for a task.
   */
  template <typename SchedPolicyType, typename ValueType>
  struct barrier_get_steal_predicate { };

/**
 * \def Define barrier steal for the most scheduling policies.
 */
#define PFUNC_DEFINE_BARRIER_STEAL_FOR_SCHED(policy,struct_name) \
  template <typename ValueType> \
  struct struct_name <policy,ValueType> { \
    \
    ValueType current; \
    \
    struct_name (const ValueType& value) : current (value) {} \
    \
    bool operator () (const ValueType& value=NULL) const { \
      return !(current->get_group() == value->get_group()) ; \
    } \
  };

  PFUNC_DEFINE_BARRIER_STEAL_FOR_SCHED(lifoS,barrier_get_own_predicate)
  PFUNC_DEFINE_BARRIER_STEAL_FOR_SCHED(lifoS,barrier_get_steal_predicate)
  PFUNC_DEFINE_BARRIER_STEAL_FOR_SCHED(fifoS,barrier_get_own_predicate)
  PFUNC_DEFINE_BARRIER_STEAL_FOR_SCHED(fifoS,barrier_get_steal_predicate)

/**
 * \def Define the barrier stealing predicate for the cilk queue type. We
 * have to ensure that we are only stealing a task that is at the same 
 * level in the spawn tree or lower. This is important to prevent thread
 * stack explosion. Also important is to not steal anything in the same 
 * group as the task executing the barrier.
 */
#define PFUNC_DEFINE_BARRIER_STEAL_FOR_CILKS(struct_name) \
  template <typename ValueType> \
  struct struct_name <cilkS, ValueType> { \
    typedef typename task_traits<ValueType>::attribute attribute; \
    typedef typename attribute::level_type level_type; \
    typedef ValueType value_type; \
   \
    const value_type current; \
  \
    struct_name (const value_type& value) : current (value) {} \
   \
    bool operator ()(const value_type& next) const { \
      bool ret_val = false; \
      level_type current_level = current->get_attr().get_level (); \
      level_type next_level = next->get_attr().get_level (); \
      if (current_level <= next_level) \
        if (current->get_group() != next->get_group()) ret_val = true; \
      return ret_val; \
    } \
  };

  PFUNC_DEFINE_BARRIER_STEAL_FOR_CILKS(barrier_get_own_predicate)
  PFUNC_DEFINE_BARRIER_STEAL_FOR_CILKS(barrier_get_steal_predicate)

/**
 * \def Define the barrier stealing predicate for the priority queue type. We
 * have to ensure that we are stealing a task, which has at least the same
 * priority as the one that we are waiting on. This prevents deadlocks from
 * occuring.
 */
#define PFUNC_DEFINE_BARRIER_STEAL_FOR_PRIOS(struct_name) \
  template <typename ValueType> \
  struct struct_name <prioS, ValueType> { \
    typedef typename task_traits<ValueType>::attribute attribute; \
    typedef typename task_traits<ValueType>::functor functor; \
    typedef compare_task_ptr<attribute, functor> compare_type; \
    typedef ValueType value_type; \
   \
    const value_type current; \
    compare_type comp;  \
  \
    struct_name (const value_type& value) : current (value) {} \
   \
    bool operator ()(const value_type& next) const { \
      bool ret_val = false; \
      if (comp (current, next)) \
        if (current->get_group() != next->get_group()) \
          ret_val = true; \
      return ret_val; \
    } \
  };

  PFUNC_DEFINE_BARRIER_STEAL_FOR_PRIOS(barrier_get_own_predicate)
  PFUNC_DEFINE_BARRIER_STEAL_FOR_PRIOS(barrier_get_steal_predicate)

} /* namespace detail */ } /* namespace pfunc */

#endif // PFUNC_PREDICATE_T_HPP
