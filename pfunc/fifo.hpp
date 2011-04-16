#ifndef PFUNC_FIFO_HPP
#define PFUNC_FIFO_HPP

#ifndef PFUNC_SCHEDULER_HPP
#error "This file can only be included from task_queue_set.hpp"
#endif

#include <queue> 
#include <pfunc/mutex.hpp>
#include <pfunc/environ.hpp>
#include <pfunc/exception.hpp>

namespace pfunc { namespace detail {

  /**
   * Specialization of task_queue_set for FIFO queues.
   */
  template <typename ValueType>
  struct task_queue_set <fifoS, ValueType> {
    typedef std::queue<ValueType*> queue_type; /**< queue type */
    typedef typename queue_type::value_type value_type; /**< value type */
    typedef unsigned int queue_index_type; /**< type to index into the list */
    typedef task_queue_set_data<queue_type> data_type; /**< task_queue_set data */

    ALIGN128 data_type* data; /**< Holds all the data required */
    ALIGN128 unsigned int num_queues; /**< Number of queues */
    PFUNC_DEFINE_EXCEPT_PTR()

    /**
     * Constructor
     *
     * \param [in] num_queues Number of task queues to create.
     */
    task_queue_set (unsigned int num_queues) PFUNC_CONSTRUCTOR_TRY_BLOCK(): 
      num_queues (num_queues) PFUNC_EXCEPT_PTR_INIT() {
      PFUNC_START_TRY_BLOCK()
      data = new data_type [num_queues];
      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(task_queue_set,task_queue_set)
    }
    PFUNC_CATCH_AND_RETHROW(task_queue_set,task_queue_set)

    /**
     * Destructor
     */
    ~task_queue_set ( ) { 
      PFUNC_START_TRY_BLOCK()
      delete [] data; 
      PFUNC_EXCEPT_PTR_CLEAR()
      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(task_queue_set,~task_queue_set)
    }

    /**
     * Check if there is something at the front of the given task queue 
     * that meets our predicate. If so, get it. Remember that the predicate
     * is always true for FIFO. Its there simply so that we have the same 
     * interface with other scheduling policies that require a predicate.
     *
     * \param [in] queue_num The task queue to check for tasks.
     * \param [in] cnd The predicate to be satisfied.
     * \param [out] value If a suitable task is found, its put here.
     * \param [in] own_queue Is true if removing element from own_queue.
     * 
     * \return true If a suitable task is found.
     * \return false If no suitable tasks could be found.
     */
    template <typename TaskPredicatePair>
    bool test_and_get (queue_index_type queue_num, 
                       const TaskPredicatePair& cnd, 
                       value_type& value,
                       bool own_queue) {
      bool ret_val = false;

      PFUNC_START_TRY_BLOCK()
      queue_type& queue = data[queue_num].queue;
      mutex& lock = data[queue_num].lock;

      lock.lock ();
      if (!queue.empty () && 
          ((own_queue)?cnd.own_pred(queue.front()):cnd.steal_pred(queue.front()))) {
        value = queue.front ();
        queue.pop ();
        ret_val = true;
      }
      lock.unlock ();

      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(scheduling,test_and_get)

      return ret_val;
    }

    /**
     * Get a suitable task from the queue. First, we check if a task can be 
     * retrieved from the task queue passed to us. If not, we check every 
     * other task queue for a task (this constitues a steal). 
     * 
     * \param [in] queue_num The first queue to check on.
     * \param [in] cnd The predicate to be satisfied.
     *
     * \return value_type The retrieved task if found.
     * \return NULL If no suitable task is found.
     *
     */
    template <typename TaskPredicatePair>
    value_type get (queue_index_type queue_num, 
                    const TaskPredicatePair& cnd) {
      value_type task = NULL;

      PFUNC_START_TRY_BLOCK()
      for (int i=queue_num, num_attempts = 0; 
           num_attempts < static_cast<int>(num_queues);
           ++i, ++num_attempts) {
        const unsigned int real_i = i % num_queues;
        if (test_and_get (real_i, cnd, task, (real_i==queue_num))) break;
      }

      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(task_queue_set,get)

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
      PFUNC_CATCH_AND_RETHROW(task_queue_set,task_queue_set)
    }
  };
} /* namespace detail */ } /* namespace pfunc */

#endif /* PFUNC_FIFO_HPP */
