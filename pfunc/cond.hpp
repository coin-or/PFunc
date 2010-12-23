#ifndef PFUNC_COND_HPP
#define PFUNC_COND_HPP
/**
 * \file cond.hpp
 * \brief Implementation of PFUNC Condition Variable
 * \author Prabhanjan Kambadur
 * 
 * On Linux, pfunc_cond_t is an one to one mapping with pthread_cond_t. 
 * One WINDOWS, things are slightly more complicated. This implementation
 * is based on the tutorial about condition variables on WINDOWS at 
 * http://www.s.wustl.edu/~schmidt/win32-cv-1.html. Upon further followup,
 * we discovered that the tutorial's code samples were present in ACE 
 * software. Hence, we are including the license of this software:
 *
 * ACE(TM), TAO(TM), CIAO(TM), and CoSMIC(TM) (henceforth referred to as "DOC
 * software") are copyrighted by Douglas C. Schmidt and his research group at
 * Washington University, University of California, Irvine, and Vanderbilt
 * University, Copyright (c) 1993-2008, all rights reserved. Since DOC
 * software is open-source, freely available software, you are free to use,
 * modify, copy, and distribute--perpetually and irrevocably--the DOC
 * software source code and object code produced from the source, as well as
 * copy and distribute modified versions of this software. You must, however,
 * include this copyright statement along with any code built using DOC
 * software that you release. No copyright statement needs to be provided if
 * you just ship binary executables of your software products.
 *
 * You can use DOC software in commercial and/or binary software releases and
 * are under no obligation to redistribute any of your source code that is
 * built using DOC software. Note, however, that you may not do anything to
 * the DOC software code, such as copyrighting it yourself or claiming
 * authorship of the DOC software code, that will prevent DOC software from
 * being distributed freely using an open-source development model. You
 * needn't inform anyone that you're using DOC software in your software,
 * though we encourage you to let us know so we can promote your project in
 * the DOC software success stories.
 *
 * The ACE, TAO, CIAO, and CoSMIC web sites are maintained by the DOC Group
 * at the Institute for Software Integrated Systems (ISIS) and the Center for
 * Distributed Object Computing of Washington University, St. Louis for the
 * development of open-source software as part of the open-source software
 * community. Submissions are provided by the submitter ``as is'' with no
 * warranties whatsoever, including any warranty of merchantability,
 * noninfringement of third party intellectual property, or fitness for any
 * particular purpose. In no event shall the submitter be liable for any
 * direct, indirect, special, exemplary, punitive, or consequential damages,
 * including without limitation, lost profits, even if advised of the
 * possibility of such damages. Likewise, DOC software is provided as is with
 * no warranties of any kind, including the warranties of design,
 * merchantability, and fitness for a particular purpose, noninfringement, or
 * arising from a course of dealing, usage or trade practice. Washington
 * University, UC Irvine, Vanderbilt University, their employees, and
 * students shall have no liability with respect to the infringement of
 * copyrights, trade secrets or any patents by DOC software or any part
 * thereof. Moreover, in no event will Washington University, UC Irvine, or
 * Vanderbilt University, their employees, or students be liable for any lost
 * revenue or profits or other special, indirect and consequential damages.
 *
 * DOC software is provided with no support and without any obligation on the
 * part of Washington University, UC Irvine, Vanderbilt University, their
 * employees, or students to assist in its use, correction, modification, or
 * enhancement. A number of companies around the world provide commercial
 * support for DOC software, however.
 *
 * DOC software is Y2K-compliant, as long as the underlying OS platform is
 * Y2K-compliant. Likewise, DOC software is compliant with the new US
 * daylight savings rule passed by Congress as "The Energy Policy Act of
 * 2005," which established new daylight savings times (DST) rules for the
 * United States that expand DST as of March 2007. Since DOC software obtains
 * time/date and calendaring information from operating systems users will
 * not be affected by the new DST rules as long as they upgrade their
 * operating systems accordingly.
 *
 * The names ACE(TM), TAO(TM), CIAO(TM), CoSMIC(TM), Washington University,
 * UC Irvine, and Vanderbilt University, may not be used to endorse or
 * promote products or services derived from this source without express
 * written permission from Washington University, UC Irvine, or Vanderbilt
 * University. This license grants no permission to call products or services
 * derived from this source ACE(TM), TAO(TM), CIAO(TM), or CoSMIC(TM), nor
 * does it grant permission for the name Washington University, UC Irvine, or
 * Vanderbilt University to appear in their names. 
 *
 */

#include <pfunc/config.h>

/** If we are using futexes, we do not need condition variables at all. 
 *  Moreover, we do not have what we need to implement condition variables
 *  if we are using futexes. 
 */
#if PFUNC_HAVE_FUTEX != 1

#include <pfunc/exception.hpp>
#include <pfunc/mutex.hpp>

#if PFUNC_WINDOWS == 1 
#include <Windows.h>

namespace pfunc { namespace detail {
  struct cond : public detail::no_copy {
    private:
    volatile int num_waiters; /**< Number of threads waiting on the condition */
    int MAX_COUNT; /**< Maximum number allowed to queue up on this condition */
    CRITICAL_SECTION num_waiters_lock; /**< Protects updates to num_waiters */
    HANDLE thread_queue; /**< Used to queue up the waiting threads */
    HANDLE all_awake_event; /**< Event that is used for broadcast purposes */
    BOOL was_broadcast; /**< Set to true if the wakeup was a broadcast */

    public:
    /**
     * Constructor
     */
    cond () {
      num_waiters = 0;
      MAX_COUNT = 1024;
      was_broadcast = FALSE;
      thread_queue = CreateSemaphore (NULL, /* No Security */
                                      0, /* Initial count */
                                      MAX_COUNT, /* Maximum count */
                                      NULL); /* Anonymous thread_queue */
      if (NULL == thread_queue)
#if PFUNC_USE_EXCEPTIONS == 1
        throw exception_generic_impl 
            ("pfunc::detail::cond::cond::CreateSemaphore at " FILE_AND_LINE(),
             "Error in initializing the thread_queue",
           GetLastError());
#else
        return;
#endif

      InitializeCriticalSection (&num_waiters_lock);

      all_awake_event = CreateEvent (NULL, /* No security */
                                  FALSE,/* auto-reset event */
                                  FALSE,/* non-signaled initially */
                                  NULL);/* No name */

      if (NULL == all_awake_event) 
#if PFUNC_USE_EXCEPTIONS == 1
        throw exception_generic_impl 
            ("pfunc::detail::cond::cond::CreateEvent at " FILE_AND_LINE(),
             "Error in initializing the event",
           GetLastError());
#else
        return;
#endif
    }

    /**
     * Destructor
     */
    ~cond () {
      num_waiters = 0;
      DeleteCriticalSection (&num_waiters_lock);
      if (TRUE != CloseHandle (thread_queue) || TRUE != CloseHandle (all_awake_event)) 
#if PFUNC_USE_EXCEPTIONS == 1
        throw exception_generic_impl 
            ("pfunc::detail::cond::~cond::CloseHandle at " FILE_AND_LINE(),
             "Error in destroying the condition variable",
             GetLastError());
#else
        return;
#endif
    }

    /**
     * Wait on the condition
     *
     * \param mtx Mutex that is associated with the condition
     */
    void wait (mutex& mtx) {
      HANDLE actual_mutex = mtx.get_internal_mutex();
      BOOL last_waiter = FALSE;
      DWORD ret_val;
  
      /* Increment num_waiters */
      EnterCriticalSection (&num_waiters_lock);
      ++num_waiters;
      LeaveCriticalSection (&num_waiters_lock);
  
      /* Atomically release the mutex and wait on the thread_queue until signal/broadcast */
      error_code_type error = SignalObjectAndWait (actual_mutex, /* Mutex to release */
                                                thread_queue, /* Sempahore to wait on */
                                                INFINITE, /* Amount of time to wait */
                                                FALSE);  /* Is not alertable */
      if (WAIT_OBJECT_0 != error)
#if PFUNC_USE_EXCEPTIONS == 1
        throw exception_generic_impl 
   ("pfunc::detail::cond::wait::SignalObjectAndWait at " FILE_AND_LINE(),
    "Error on atomically releasing the mutex and waiting on the thread_queue",
    GetLastError());
#else
         return;
#endif
  
      /* Post-processing */
      EnterCriticalSection (&num_waiters_lock);
      --num_waiters;
      last_waiter = (was_broadcast && num_waiters==0);
      LeaveCriticalSection (&num_waiters_lock);
  
      /* Check if we are the last one out the door in a broadcast. If so, we should let
         other threads proceed */
      if (last_waiter) { 
        error =  SignalObjectAndWait (all_awake_event, 
                                      actual_mutex, 
                                      INFINITE, 
                                      FALSE);
      if (WAIT_OBJECT_0 != error)
#if PFUNC_USE_EXCEPTIONS == 1
        throw exception_generic_impl 
    ("pfunc::detail::cond::wait::SignalObjectAndWait at " FILE_AND_LINE(),
     "Error while checking we are the last thread out of the broadcast",
     GetLastError());
#else
        return;
#endif
      } else {
        error = WaitForSingleObject (actual_mutex, INFINITE);
        if (WAIT_OBJECT_0 != error)
#if PFUNC_USE_EXCEPTIONS == 1
          throw exception_generic_impl 
      ("pfunc::detail::cond::wait::WaitForSingleObject at " FILE_AND_LINE(),
       "Error while waiting for the mutex",
       GetLastError());
#else
          return;
#endif
      }
    }

    /**
     * Wake up one of the threads
     */
    void signal () {
      BOOL waiters_present = FALSE;
  
      EnterCriticalSection (&num_waiters_lock);
      waiters_present = num_waiters>0;
      LeaveCriticalSection (&num_waiters_lock);
  
      /* If there are waiters, then release one of them */
      if (waiters_present) {
        if (FALSE == ReleaseSemaphore (thread_queue, 1, 0)) 
#if PFUNC_USE_EXCEPTIONS == 1
          throw exception_generic_impl 
            ("pfunc::detail::cond::signal::ReleaseSemaphore at " FILE_AND_LINE(),
             "Error releasing the thread_queue",
             GetLastError());
#else
          return;
#endif
      }
    }

    /**
     * Broadcast the condition being satisfied to all waiters
     */
    void broadcast () {
      BOOL waiters_present = FALSE;
  
      /*This is needed to ensure that num_waiters and was_broadcast are
       * consistent */
      EnterCriticalSection (&num_waiters_lock);
      if (num_waiters>0) {
        was_broadcast = 1;
        waiters_present = TRUE;
      }
  
      if (waiters_present) {
        /* Wake up everyone */
        if (FALSE == ReleaseSemaphore (thread_queue, num_waiters, 0))
#if PFUNC_USE_EXCEPTIONS
            throw exception_generic_impl 
      ("pfunc::detail::cond::broadcast::ReleaseSemaphore at " FILE_AND_LINE(),
       "Error releasing the thread_queue",
       GetLastError());
#else
            return;
#endif
 
        LeaveCriticalSection (&num_waiters_lock);
 
        /* Wait for all awakened threads to acquire the counting thread_queue */
        if (WAIT_OBJECT_0 != WaitForSingleObject (all_awake_event, INFINITE)) 
#if PFUNC_USE_EXCEPTIONS == 1
            throw exception_generic_impl 
    ("pfunc::detail::cond::broadcast::WaitForSingleObject at" FILE_AND_LINE(),
      "Error waiting for the broadcast to go through",
      GetLastError());
#else
            return;
#endif
        /**
         * Its ok to touch was_broadcast without a lock since no one can wake
         * up to access it 
         */
        was_broadcast = FALSE;
      } else {
        LeaveCriticalSection (&num_waiters_lock);
      }
    }
  };
} /* namespace detail */ } /* namespace pfunc */

#elif PFUNC_HAVE_PTHREADS == 1
#include <pthread.h>

namespace pfunc { namespace detail {

  struct cond : public detail::no_copy {
    private:
    pthread_cond_t cnd; /**< Internal condition variable */

    public:
    /**
     * Constructor
     */
    cond () {
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_cond_init (&cnd, NULL);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error) 
        throw exception_generic_impl 
      ("pfunc::detail::cond::cond::pthread_cond_init at " FILE_AND_LINE(),
       "Error in initializing the condition variable",
       error);
#endif
    }

    /**
     * Destructor
     */
    ~cond () {
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_cond_destroy (&cnd);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error) 
        throw exception_generic_impl
            ("pfunc::detail::cond::~cond::pthread_cond_destroy at " FILE_AND_LINE(),
             "Error in destroying the condition variable",
             error);
#endif
    }

    /**
     * Wait on the condition
     *
     * \param mtx Mutex that is associated with the condition
     */
    void wait (mutex& mtx) {
      pthread_mutex_t& actual_mutex = mtx.get_internal_mutex();
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_cond_wait (&cnd, &actual_mutex);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error) 
        throw exception_generic_impl
            ("pfunc::detail::cond::wait::pthread_cond_wait at " FILE_AND_LINE(),
             "Error in waiting for the condition variable",
             error);
#endif
    }

    /**
     * Wake up one of the threads
     */
    void signal () {
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_cond_signal (&cnd);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl 
  ("pfunc::detail::cond::signal::pthread_cond_signal at " FILE_AND_LINE(),
   "Error in signaling the condition variable",
    error);
#endif
    }

    /**
     * Broadcast the condition being satisfied to all waiters
     */
    void broadcast () {
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_cond_broadcast (&cnd);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl 
   ("pfunc::detail::cond::broadcast::pthread_cond_signal at " FILE_AND_LINE(),
    "Error in broadcasting the condition variable",
    error);
#endif
    }
  };
} /* namespace detail */ } /* namespace pfunc */

#else
#error "Platform not supported"
#endif

#endif /* if PFUNC_HAVE_FUTEX == 1 */

#endif // PFUNC_COND_HPP
