#ifndef PFUNC_PRIO_T_HPP
#define PFUNC_PRIO_T_HPP

#ifndef PFUNC_SCHEDULER_HPP
#error "This file can only be included from scheduler.hpp"
#endif

#include <queue> 
#include <vector>
#include <pfunc/mutex.hpp>
#include <pfunc/environ.hpp>
#include <pfunc/task.hpp>
#include <pfunc/exception.hpp>

namespace pfunc { namespace detail {
  /**
   * Traits class to figure out the nested types "attribute" and "functor"
   * associated with a "task" type.
   */
  template <typename T>
  struct task_traits {
    typedef typename T::attribute attribute; /**< Type of the attribute */
    typedef typename T::functor functor; /**< Type of the functor */
  };

  /**
   * Specialization of task_traits for pointers to "task" type.
   */
  template <typename T>
  struct task_traits <T*> {
    typedef typename T::attribute attribute; /**< Type of the attribute */
    typedef typename T::functor functor; /**< Type of the functor */
  };

  /**
   * Predicate that compares two task pointers. If the first is "before" the 
   * "second" task structure, the predicate returns true.
   */
  template <typename Attribute, typename Functor>
  struct compare_task_ptr {

    typedef bool result_type; /**< Type of the result for operator() */
    typedef task<Attribute, Functor>* first_argument_type; /**< Type of the first argument */
    typedef first_argument_type second_argument_type /**< Type of the second argument */;

    /**
     * Determine whether first argument comes before the second according to 
     * priorities (task->attribute->priority) of the tasks to which these 
     * arguments point to. (task->attribute->compare_type) is used for 
     * comparison. 
     *
     * \param [in] ptr1 Pointer to the first task.
     * \param [in] ptr2 Pointer to the second task.
     *
     * \return true Task at ptr1 comes before task at ptr2.
     * \return false Task at ptr1 does not come before task at ptr2.
     */
    bool operator()(first_argument_type ptr1,
                    second_argument_type ptr2) const {
      typename Attribute::compare_type comp;
      return comp (ptr1->get_attr().get_priority(), 
                   ptr2->get_attr().get_priority());
    }
  };

  /**
   * Specialization of scheduler for priority queues.
   */
  template <typename ValueType>
  struct scheduler <prioS, ValueType> {
    typedef typename task_traits<ValueType>::attribute attribute; /**< Type of the task attribute */
    typedef typename task_traits<ValueType>::functor functor; /**< Type of the task functor */
    typedef compare_task_ptr<attribute, functor> compare_type; /**< Type of the priority comparison operator */
    typedef std::priority_queue<ValueType, 
                                std::vector<ValueType>, 
                                compare_type> queue_type; /**< Type of the priority_queue */
    typedef typename queue_type::value_type value_type; /**< Type of the items stored in the priority_queue */
    typedef unsigned int queue_index_type; /**< type to index into the queue */
    typedef sched_data<queue_type> data_type; /**< scheduler data */

    ALIGN128 data_type* data; /**< Holds all the data required */
    ALIGN128 unsigned int num_queues; /**< Number of queues */
    PFUNC_DEFINE_EXCEPT_PTR()

    /**
     * Constructor
     *
     * \param [in] num_queues Number of task queues to create.
     */
    scheduler (const unsigned int& num_queues) PFUNC_CONSTRUCTOR_TRY_BLOCK() : 
      num_queues (num_queues) PFUNC_EXCEPT_PTR_INIT() {
      PFUNC_START_TRY_BLOCK()
      data = new data_type [num_queues];
      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduler,scheduler)
    }
    PFUNC_CATCH_AND_RETHROW(scheduler,scheduler)

    /**
     * Destructor
     */
    ~scheduler ( ) { 
      PFUNC_START_TRY_BLOCK()
      delete [] data; 
      PFUNC_EXCEPT_PTR_CLEAR()
      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduler,~scheduler)
    }

    /**
     * Check if there is something at the front of the given task queue 
     * that meets our predicate. If so, get it. Remember that the predicate
     * is always true for LIFO. Its there simply so that we have the same 
     * interface with other scheduling policies that require a predicate.
     *
     * \param [in] queue_num The task queue to check for tasks.
     * \param [in] cnd The predicate to be satisfied.
     * \param [out] value If a suitable task is found, its put here.
     * 
     * \return true If a suitable task is found.
     * \return false If no suitable tasks could be found.
     */
    template <typename Condition>
    bool test_and_get (const unsigned int& queue_num, 
                       Condition cnd, 
                       value_type& value) {
      bool ret_val = false;

      PFUNC_START_TRY_BLOCK()
      queue_type& queue = data[queue_num].queue;
      mutex& lock = data[queue_num].lock;

      lock.lock ();
      if (!queue.empty () && cnd(queue.top())) {
        value = queue.top ();
        queue.pop ();
        ret_val = true;
      }
      lock.unlock ();

      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduler,test_and_get)

      return ret_val;
    }

    /**
     * Get a suitable task from the queue. First, we check if a task can be 
     * retrieved from the task queue passed to us. If not, we check every 
     * other task queue for a task (this constitues a steal). Note that the 
     * predicate for the steal is different from the one for regular task 
     * retrieval.
     * 
     * \param [in] queue_num The first queue to check on.
     * \param [in] cnd The predicate to be satisfied.
     *
     * \return value_type The retrieved task if found.
     * \return NULL If no suitable task is found.
     *
     */
    template <typename ConditionPair>
    value_type get (queue_index_type queue_num, 
                    const ConditionPair& cnd) {
      value_type task = NULL;

      PFUNC_START_TRY_BLOCK()
      for (int i=queue_num, num_attempts = 0; 
           num_attempts < static_cast<int>(num_queues);
           ++i, ++num_attempts) {
        const unsigned int real_i = i % num_queues;
        if (real_i == queue_num) {
          if (test_and_get (real_i, cnd.first, task)) break; 
        } else if (test_and_get (real_i, cnd.second, task)) break;
      }

      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduler,get)

      return task;
    }

    /**
     * Store the value at the front of the given queue
     *
     * \param [in] queue_num The task queue to use.
     * \param [in] value The value (task ptr) to be stored.
     *
     */
    void put (queue_index_type queue_num, const value_type& value) {
      PFUNC_START_TRY_BLOCK()
      queue_type& queue = data[queue_num].queue;
      mutex& lock = data[queue_num].lock;

      lock.lock ();
      queue.push (value);
      lock.unlock ();

      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduler,scheduler)
    }
  };
} /* namespace detail */ } /* namespace pfunc */

#endif /* PFUNC_PRIO_T_HPP */
