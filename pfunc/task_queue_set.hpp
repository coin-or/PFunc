#ifndef PFUNC_SCHEDULER_HPP
#define PFUNC_SCHEDULER_HPP

#include <pfunc/config.h>
#include <pfunc/environ.hpp>
#include <pfunc/pfunc_atomics.h>
#include <pfunc/mutex.hpp>

#if PFUNC_HAVE_ERRNO_H == 1
#include <errno.h>
#endif


namespace pfunc { 
  /** Error scheduling class */
  struct errorS {};

  /** FIFO scheduling offering queue semantics */
  struct fifoS {};

  /** LIFO scheduling offering stack semantics */
  struct lifoS {};

  /** PRIORITY support for tasks */
  struct prioS {};

  /** CILK schedling for tasks */
  struct cilkS {};

  namespace detail {
    /**
     * Template class whose specializations give us the different scheduling
     * policy based task queues. SchedPolicyType is one of the schedS 
     * specializations and ValueType is always a pointer to a task. 
     */
    template <typename PolicyName, 
              typename ValueType>
    struct task_queue_set {
      typedef errorS queue_index_type; /**< Declare error if no specialization is found */
      typedef errorS queue_type; /**< Declare error if no specialization is found */
    };

    /**
     * Data stored in a task_queue_set. QueueType is one of schedS.
     */
    template <typename QueueType>
    struct task_queue_set_data {
      ALIGN128 QueueType queue; /**< Internal queue */
      ALIGN128 mutex lock; /**< Lock associated with this internal queue */
    };
  } /* namespace detail */ 
} /* namespace pfunc */

#include "lifo.hpp"
#include "fifo.hpp"
#include "cilk.hpp"
#include "prio.hpp"

#endif /* PFUNC_SCHEDULER_HPP */
