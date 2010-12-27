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
  
  /** Common base class for all the scheduling mechanisms */
  struct schedS {};

  /** Error scheduling class */
  struct errorS : public schedS {};

  /** FIFO scheduling offering queue semantics */
  struct fifoS : public schedS {};

  /** LIFO scheduling offering stack semantics */
  struct lifoS : public schedS {};

  /** PRIORITY support for tasks */
  struct prioS : public schedS {};

  /** CILK schedling for tasks */
  struct cilkS : public schedS {};

  /** Hash multimap based schedling for tasks */
  struct hashS : public schedS {};

  namespace detail {
    /**
     * Template class whose specializations give us the different scheduling
     * policy based task queues. SchedPolicyType is one of the schedS 
     * specializations and ValueType is always a pointer to a task. 
     */
    template <typename SchedPolicyType, 
              typename ValueType>
    struct scheduler {
      typedef errorS queue_type; /**< Declare error if no specialization is found */
    };

    /**
     * Data stored in a scheduler. QueueType is one of schedS.
     */
    template <typename QueueType>
    struct sched_data {
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
