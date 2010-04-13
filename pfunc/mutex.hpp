#ifndef PFUNC_MUTEX_HPP
#define PFUNC_MUTEX_HPP
  
/**
 * \file mutex.hpp
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

#if PFUNC_HAVE_ERRNO_H
#include <errno.h>
#endif

#if PFUNC_HAVE_FUTEX == 1
#include <pfunc/futex.h>

/*
 * TODO: There is an assumption here that int is 32-bits. Need to change this
 */

namespace pfunc {
  struct mutex : public detail::no_copy {
    private:
    ALIGN64 int val;
    static const int PFUNC_MUTEX_FREE = 0x0;
    static const int PFUNC_MUTEX_LOCKED = 0x1;
    static const int PFUNC_MUTEX_LOCKED_WITH_WAITERS = 0x2;
    static const int PFUNC_MAX_RELAXATIONS = 2*1024*1024; /* 2M */

    public:
    mutex () : val (PFUNC_MUTEX_FREE) {}

    void lock () {
      if (PFUNC_MUTEX_FREE != pfunc_compare_and_swap_32 (&val, 
                                              PFUNC_MUTEX_LOCKED, 
                                              PFUNC_MUTEX_FREE)) {
        do {
          int oldval = pfunc_compare_and_swap_32 (&val, 
                                       PFUNC_MUTEX_LOCKED_WITH_WAITERS, 
                                       PFUNC_MUTEX_LOCKED);
          if (PFUNC_MUTEX_FREE != oldval) {
            /** Try to spin for a while first */
            for (unsigned int i = 0; i < PFUNC_MAX_RELAXATIONS; i++) {
              if (PFUNC_MUTEX_LOCKED_WITH_WAITERS == val) cpu_relax ();
              else break;
            }
            /** Give up and sleep */
            futex_wait (reinterpret_cast<int*>(&val), 
                        PFUNC_MUTEX_LOCKED_WITH_WAITERS);
          }
        } while (PFUNC_MUTEX_FREE != pfunc_compare_and_swap_32 (&val, 
                                           PFUNC_MUTEX_LOCKED_WITH_WAITERS, 
                                           PFUNC_MUTEX_FREE));
      }
    }

    bool trylock () {
      if (PFUNC_MUTEX_FREE != pfunc_compare_and_swap_32 (&val, 
                                          PFUNC_MUTEX_LOCKED, 
                                          PFUNC_MUTEX_FREE)) return true;
      else return false;
    }

    void unlock () { 
      int oldval = pfunc_fetch_and_store_32 (&val, PFUNC_MUTEX_FREE);
      if (oldval <= PFUNC_MUTEX_LOCKED) return;
      else futex_wake (&val, PFUNC_MUTEX_LOCKED); 
    }
  };
} /* namespace pfunc */

#elif PFUNC_HAVE_PTHREADS == 1

namespace pfunc {
  struct mutex : public detail::no_copy {
    typedef pthread_mutex_t mutex_type; /*< Mutex type */
    mutex_type mtx; /**< Internal mutex */

    /**
     * Constructor
     */
    mutex () {
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_mutex_init (&mtx, NULL);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
    ("pfunc::detail::mutex::mutex::pthread_mutex_init at " FILE_AND_LINE(),
     "Error initializing the mutex",
     error);
#endif
    }

    /**
     * Destructor
     */
    ~mutex () {
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_mutex_destroy (&mtx);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
    ("pfunc::detail::mutex::mutex::pthread_mutex_destroy at " FILE_AND_LINE(),
     "Error destroying the mutex",
     error);
#endif
    }

    /**
     * Locks the mutex.
     */
    void lock () {
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_mutex_lock (&mtx);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
    ("pfunc::detail::mutex::mutex::pthread_mutex_lock at " FILE_AND_LINE(),
     "Error locking the mutex",
     error);
#endif
    }

    /**
     * Attempt to lock the mutex
     *
     * \return true if we successfully locked the mutex
     * \return false if we could not lock the mutex
     */
    bool trylock () {
      error_code_type error = pthread_mutex_lock (&mtx);
      bool ret_val = false;
      if (0 == error) ret_val = true;
      /** We can check for errors only if there is errno.h */
#if PFUNC_HAVE_ERRNO_H == 1
      else if (EBUSY == error) return false;
#if PFUNC_USE_EXCEPTIONS == 1
      else
        throw exception_generic_impl
    ("pfunc::detail::mutex::mutex::pthread_mutex_trylock at " FILE_AND_LINE(),
     "Error trying to lock the mutex",
     error);
#endif
#endif
      return ret_val;
    }

    /**
     * Unlocks the mutex
     */
    void unlock () {
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_mutex_unlock (&mtx);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
    ("pfunc::detail::mutex::mutex::pthread_mutex_unlock at " FILE_AND_LINE(),
     "Error unlocking the mutex",
     error);
#endif
    }

    /**
     * Returns the internal mutex. This is a hack required for pfunc_cond_t.
     *
     * \return Returns the underlying OS mutex variable.
     */
    mutex_type& get_internal_mutex ()  { return mtx; }
  };

} /* namespace pfunc */

#elif PFUNC_HAVE_WINDOWS_THREADS == 1

namespace pfunc {
  struct mutex : public detail::no_copy {
    typedef HANDLE  mutex_type; /*< Mutex type */
    mutex_type mtx; /**< Internal mutex */

    /**
     * Constructor
     */
    mutex () {
      mtx = CreateMutex (NULL, /* No security */
                         FALSE,/* Unowned */
                         NULL);/* Unnamed */
#if PFUNC_USE_EXCEPTIONS == 1
      if (NULL == mutex)
        throw exception_generic_impl
      ("pfunc::detail::mutex::mutex::CreateMutex at " FILE_AND_LINE(),
       "Error initializing the mutex",
       GetLastError());
#endif
    }

    /**
     * Destructor
     */
    ~mutex () {
      if (FALSE == CloseHandle (mtx))
#if PFUNC_USE_EXCEPTIONS == 1
      if (NULL == mutex) 
        throw exception_generic_impl
      ("pfunc::detail::mutex::mutex::CloseHandle at " FILE_AND_LINE(),
       "Error destroying the mutex",
       GetLastError());
#endif
    }

    /**
     * Locks the mutex.
     */
    void lock () {
      error_code_type error = WaitForSingleObject (mtx, INFINITE);
#if PFUNC_USE_EXCEPTIONS == 1
      if (WAIT_OBJECT_0 != error) 
        throw exception_generic_impl 
  ("pfunc::detail::mutex::lock::WaitForSingleObject at " FILE_AND_LINE(),
   "Error locking the mutex",
   GetLastError());
#endif
    }

    /**
     * Attempt to lock the mutex
     *
     * \return true if we successfully locked the mutex
     * \return false if we could not lock the mutex
     */
    bool trylock () {
      DWORD ret_val = WaitForSingleObject (mtx, 0);
      if (ret_val == WAIT_OBJECT_0) return true;
      else if (ret_val == WAIT_TIMEOUT) return false;
#if PFUNC_USE_EXCEPTIONS == 1
      else 
       throw exception_generic_impl 
 ("pfunc::detail::mutex::lock::WaitForSingleObject at " FILE_AND_LINE(),
  "Error locking the mutex",
  GetLastError());
#endif
    }

    /**
     * Unlocks the mutex
     */
    void unlock () {
      BOOL ret_val = ReleaseMutex (mtx);
#if PFUNC_USE_EXCEPTIONS == 1
      if (FALSE == ret_val)
        throw exception_generic_impl 
  ("pfunc::detail::mutex::lock::ReleaseMutex at " FILE_AND_LINE(),
   "Error unlocking the mutex",
   GetLastError());
#endif
    }

    /**
     * Returns the internal mutex. This is a hack required for pfunc_cond_t.
     */
    mutex_type& get_internal_mutex ()  { return mtx; }
  };
} /* namespace pfunc */
#else
#error "pthreads, windows threads or futexes are required"
#endif 

#endif // PFUNC_MUTEX_HPP
