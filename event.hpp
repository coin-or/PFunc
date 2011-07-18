#ifndef PFUNC_EVENT_HPP
#define PFUNC_EVENT_HPP
  
/**
 * \file event.hpp
 * \brief Implementation of Events for PFUNC -- A Task Parallel API
 * \author Prabhanjan Kambadur
 */
#include <pfunc/config.h>
#include <pfunc/environ.hpp>
#include <pfunc/pfunc_common.h>
#include <pfunc/no_copy.hpp>
#include <pfunc/exception.hpp>
#include <pfunc/pfunc_atomics.h>

#if PFUNC_HAVE_LIMITS_H == 1
#include <limits.h>
#else
#define INT_MAX 0x0FFFFFFF
#endif

#if PFUNC_HAVE_FUTEX == 1 
#include <pfunc/futex.h>
#elif PFUNC_HAVE_PTHREADS == 1 || PFUNC_WINDOWS == 1
#include <pfunc/mutex.hpp>
#include <pfunc/cond.hpp>
#else
#error "Futexes, pthreads or windows threads are required"
#endif

namespace pfunc { namespace detail {

  static const int PFUNC_INACTIVE = 0; /**< Task is not an active task */
  static const int PFUNC_ACTIVE_INCOMPLETE = 1; /**< Task is running */
  static const int PFUNC_ACTIVE_COMPLETE = 2; /**< Task is complete */

  struct event_type {};
  struct testable_event : public event_type {} ;
  struct waitable_event : public event_type {} ;

  struct event_base : public no_copy {
    protected:
    ALIGN64 int event_state; /**< State of this event */
    ALIGN64 int num_waiters; /**<The number of waiters on this even */

    public:
    /**
     * Constructor
     */
    event_base ()  :event_state (PFUNC_INACTIVE), num_waiters (1) {}

    /**
     * Destructor
     */
    ~event_base ()  { }

    /**
     * Test if an event is complete. We just check for the state
     *
     * \return true if the event is complete
     * \return false if the event is incomplete or inactive
     */
    bool test ()  {
      if (event_state == PFUNC_ACTIVE_INCOMPLETE) return false;
      else if (event_state == PFUNC_INACTIVE) return true;
      else {
        if (1 == pfunc_fetch_and_add_32 (&num_waiters, -1))
          event_state = PFUNC_INACTIVE;
        return true;
      }
    }

    /**
     * Reset this event for reuse
     *
     * \param [in] nwait Number of waiters receiving completion notices
     */
    void reset (const unsigned int& nwait)  {
      num_waiters = nwait; 
      event_state = PFUNC_ACTIVE_INCOMPLETE;
    }
  };

  template<typename EventType> 
  struct event: public event_base { };

#if PFUNC_HAVE_FUTEX == 1

  template<> 
  struct event<waitable_event>: public event_base {
    /**
     * Wait for an event completion
     */ 
    void wait ()  {
      if (event_state == PFUNC_ACTIVE_INCOMPLETE) 
        futex_wait (&event_state, PFUNC_ACTIVE_INCOMPLETE);
      /* After wake up */
      if (0 == pfunc_fetch_and_add_32 (&num_waiters, -1)) 
        event_state = PFUNC_INACTIVE;
    }
  
    /**
     * Notify completion of an event
     */
    void notify ()  {
      pfunc_mem_fence ();
      pfunc_fetch_and_store_32 (&event_state, PFUNC_ACTIVE_COMPLETE);
      futex_wake (&event_state, INT_MAX);
    }
  }; /* waitable event */

#elif PFUNC_HAVE_PTHREADS == 1 || PFUNC_WINDOWS == 1

  template<> 
  struct event<waitable_event>: public event_base {
    mutex lck; /**< The lock to be used with condition variable */
    cond cnd; /**< Condition variable to be used for the wait */
    PFUNC_DEFINE_EXCEPT_PTR() /**< To propogate the exeption up the stack */

    /** Default constructor */
    event () PFUNC_CONSTRUCTOR_TRY_BLOCK() : 
          event_base () PFUNC_EXCEPT_PTR_INIT() { }
    PFUNC_CATCH_AND_RETHROW(event,event)

    /** Destructor */
    ~event () { PFUNC_EXCEPT_PTR_CLEAR() } 

    /**
     * Wait for an event completion
     */ 
    void wait () {
      PFUNC_START_TRY_BLOCK()
      if (event_state == PFUNC_ACTIVE_INCOMPLETE) 
        lck.lock ();
        if (PFUNC_ACTIVE_INCOMPLETE == event_state)
          cnd.wait (lck); 
        lck.unlock ();
      /* After wake up */
      if (0 == pfunc_fetch_and_add_32 (&num_waiters, -1)) 
        event_state = PFUNC_INACTIVE;
      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(event,wait)
    }
  
    /**
     * Notify completion of an event
     */
    void notify ()  {
      PFUNC_START_TRY_BLOCK()
      pfunc_fetch_and_store_32 (&event_state, PFUNC_ACTIVE_COMPLETE);
      lck.lock ();
      cnd.broadcast ();
      lck.unlock ();
      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(event,wait)
    }
  }; /* waitable event */

#else
#error "Futexes, Windows or pthreads are required"
#endif

  /**
   * Can only be tested -- no waiting facility 
   */
  template<>
  struct event<testable_event>: public event_base {
    /**
     * Notify completion of an event
     */
    void notify ()  {
      pfunc_mem_fence ();
      event_state = PFUNC_ACTIVE_COMPLETE;
    }
  }; /* testable event */
} /* namespace detail */ } /* namespace pfunc */

#endif // PFUNC_EVENT_HPP
