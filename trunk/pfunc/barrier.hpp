#ifndef PFUNC_BARRIER_HPP
#define PFUNC_BARRIER_HPP
  
/**
 * \file barrier.hpp
 * \brief Implementation of PFUNC mutex
 * \author Prabhanjan Kambadur
 *
 */
#include <pfunc/config.h>
#include <pfunc/pfunc_common.h>
#include <pfunc/no_copy.hpp>
#include <pfunc/exception.hpp>
#include <pfunc/pfunc_atomics.h>
#include <pfunc/environ.hpp>

#if PFUNC_HAVE_FUTEX == 1
#include <pfunc/futex.h>

namespace pfunc { namespace detail {
  struct barrier : public no_copy {
    ALIGN64 unsigned barrier_size; /** Number of threads in the barrier */
    ALIGN64 unsigned barrier_phase; /** Phase -- toggles so that we ensure correctness */
    ALIGN64 unsigned barrier_count; /** Number of threads currently in barrier */

    barrier () : barrier_size (0), barrier_phase (0), barrier_count (0) {}

    /**
     * \param [in] size The number of threads in the barrier.
     */
    void initialize (const unsigned int& size) { 
      barrier_size = barrier_count = size; 
    }
 
    /**
     * Execute the barrier.
     */
    void operator() () {
      unsigned my_phase = barrier_phase; 

      /** This is the last thread. Reset barrier_size and toggle the phase.*/
      if (1 == pfunc_fetch_and_add_32 (&barrier_count, -1)) {
        barrier_count = barrier_size;
        barrier_phase = ~barrier_phase;
        futex_wake (reinterpret_cast<int*>(&barrier_phase), INT_MAX);
      } else {/** This is NOT the last thread--wait for the phase to change */
        while (barrier_phase == my_phase) {
          /** Try to spin for a while first */
          for (unsigned int i = 0; i < 2000000; i++) {
            if (my_phase == barrier_phase) cpu_relax ();
            else break;
          }
          /** Give up and sleep */
          futex_wait (reinterpret_cast<int*>(&barrier_phase), my_phase);
        }
      }
    }
  };
} /* namespace detail */ } /* namespace pfunc */

#elif PFUNC_HAVE_PTHREADS == 1 || PFUNC_WINDOWS == 1
#include <pfunc/mutex.hpp>
#include <pfunc/cond.hpp>

namespace pfunc { namespace detail {
  struct barrier {
    ALIGN64 volatile bool barrier_phase; /**< Phase of the barrier */
    ALIGN64 volatile unsigned int barrier_count; /**< Current number of tasks */
    ALIGN64 unsigned int barrier_size; /**< Total number of tasks */
    mutex lck;
    cond cnd;
    PFUNC_DEFINE_EXCEPT_PTR()

    barrier () PFUNC_CONSTRUCTOR_TRY_BLOCK() : 
        barrier_phase (true), barrier_count (0), barrier_size (0) 
        PFUNC_EXCEPT_PTR_INIT() {}
    PFUNC_CATCH_AND_RETHROW(barrier,barrier)

    ~barrier () { PFUNC_EXCEPT_PTR_CLEAR() }

    void initialize (const unsigned int& size) { barrier_size = size; }
 
    /** We are implementing a waiting barrier for now */
    void operator() () {
      PFUNC_START_TRY_BLOCK()
      lck.lock (); 
      volatile bool my_phase = barrier_phase;
      ++barrier_count;
      if (barrier_count == barrier_size) {
        barrier_count = 0;
        barrier_phase = !barrier_phase;
        cnd.broadcast ();
        lck.unlock ();
      } else {
        while (my_phase == barrier_phase) cnd.wait (lck);
        lck.unlock ();
      }
      PFUNC_END_TRY_BLOCK()
      PFUNC_CATCH_AND_RETHROW(barrier,paranthesis)
    }
  };
} /* namespace detail */ } /* namespace pfunc */
#endif

#endif // PFUNC_HPP
