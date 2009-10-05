#ifndef PFUNC_CILK_HPP
#define PFUNC_CILK_HPP

#ifndef PFUNC_SCHEDULER_HPP
#error "This file can only be included from scheduler.hpp"
#endif

#include <deque> 
#include <pfunc/mutex.hpp>
#include <pfunc/environ.hpp>
#include <pfunc/exception.hpp>

namespace pfunc { namespace detail {

  /**
   * Specialization of scheduler for Cilk-style queues.
   */
  template <typename ValueType>
  struct scheduler <cilkS, ValueType> {
    typedef std::deque<ValueType> queue_type; /**< queue type */
    typedef typename queue_type::value_type value_type; /**< value type */
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
     * is always true for Cilk. Its there simply so that we have the same 
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
    bool test_and_get_front (const unsigned int& queue_num, 
                             Condition cnd, 
                             value_type& value) {
      bool ret_val = false;

      PFUNC_START_TRY_BLOCK()
      queue_type& queue = data[queue_num].queue;
      mutex& lock = data[queue_num].lock;

      lock.lock ();
      if (!queue.empty () && cnd(queue.front())) {
        value = queue.front ();
        queue.pop_front ();
        ret_val = true;
      }
      lock.unlock ();

      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduler,test_and_get_front)

      return ret_val;
    }

    /**
     * Check if there is something at the back of the given task queue 
     * that meets our predicate. If so, get it. Remember that the predicate
     * is always true for Cilk. Its there simply so that we have the same 
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
    bool test_and_get_back (const unsigned int& queue_num, 
                            Condition cnd, 
                            value_type& value) {
      bool ret_val = false;

      PFUNC_START_TRY_BLOCK()
      queue_type& queue = data[queue_num].queue;
      mutex& lock = data[queue_num].lock;

      lock.lock ();
      if (!queue.empty () && cnd(queue.back())) {
        value = queue.back ();
        queue.pop_back ();
        ret_val = true;
      }
      lock.unlock ();

      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduler,test_and_get_back)

      return ret_val;
    }

    /**
     * Get a suitable task from the queue. First, we check if a task can be 
     * retrieved from the task queue passed to us. If not, we check every 
     * other task queue for a task (this constitues a steal). The steals are 
     * always done from the opposite end of the dequeue. The condition 
     * predicate passe along is to be satisfied by the task.
     * 
     * \param [in] queue_num The first queue to check on.
     * \param [in] cnd The predicate to be satisfied.
     *
     * \return value_type The retrieved task if found.
     * \return NULL If no suitable task is found.
     *
     */
    template <typename ConditionPair>
    value_type get (const unsigned int& queue_num, 
                    ConditionPair cnd) {
      value_type task = NULL;

      PFUNC_START_TRY_BLOCK()
      for (int i=queue_num, num_attempts = 0; 
           num_attempts < static_cast<int>(num_queues); 
           ++i, ++num_attempts) {
        const unsigned int real_i = i % num_queues;
        if (real_i == queue_num) {
          if (test_and_get_front (real_i, cnd.first, task)) break;
        } else if (test_and_get_back (real_i, cnd.second, task)) break;
      }

      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduler,get)

      return task;
    }

    /**
     * Store the value at the front of the given queue
     *
     * \param [in] value The value (task ptr) to be stored.
     * \param [in] queue_num The task queue to use.
     *
     */
    void put (const value_type& value, const unsigned int& queue_num) {
      PFUNC_START_TRY_BLOCK()
      queue_type& queue = data[queue_num].queue;
      mutex& lock = data[queue_num].lock;

      lock.lock ();
      queue.push_front (value);
      lock.unlock ();

      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduler,scheduler)
    }
  };
} /* namespace detail */ }  /* namespace pfunc */

#endif /* PFUNC_CILK_HPP */
