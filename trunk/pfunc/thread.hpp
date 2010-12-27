#ifndef PFUNC_THREAD_HPP
#define PFUNC_THREAD_HPP
  
/**
 * \file thread.hpp
 * \brief Implementation of portable threads for PFUNC -- A Task Parallel API
 * \author Prabhanjan Kambadur
 */
#include <pfunc/config.h>
#include <pfunc/pfunc_common.h>
#if PFUNC_HAVE_WINDOWS_THREADS == 1
#include <Windows.h>
#elif PFUNC_HAVE_PTHREADS == 1
#include<pthread.h>
#if PFUNC_HAVE_SCHED_AFFINITY == 1
#include <sched.h>
#endif
#else
#error "Windows threads or pthreads are required"
#endif

#include <pfunc/no_copy.hpp>
#include <pfunc/exception.hpp>
#include <pfunc/mutex.hpp>

#if PFUNC_HAVE_TLS == 1
#include <vector>
__thread volatile unsigned int pfunc_thread_self_id;
#elif PFUNC_HAVE_HASH_MAP_H == 1
#include <ext/hash_map>
namespace std { using namespace __gnu_cxx; }
#else
#include <map>
#endif

#if PFUNC_DARWIN == 1 && PFUNC_HAVE_SYSCTL_H == 1
#include <sys/sysctl.h>
#endif

namespace pfunc { namespace detail {
  static const unsigned int  PFUNC_STACK_MIN = 2048*2048;
  static const unsigned int  PFUNC_STACK_MAX = 4096*4096;
  static const unsigned int  PFUNC_STACK_AVG = 2048*4096;
  static const unsigned int  PFUNC_NO_AFFINITY = ~0x0;

  /**
   * \brief Class that represents thread specific meta-data.
   *
   * When a thread is created, it is passed along its attributes in the form 
   * of thread_attr structure. This structure contains important info
   * such as ID, stacksize, queue out of which jobs need to be taken, etc.
   * 
   */ 
  struct thread_attr : no_copy {
    private:
    const unsigned int stack_size; /**< Size of the thread-specific stack */
    const unsigned int thread_id; /**< An unsigned int from (0..num_threads) */
    const unsigned int thread_affinity; /**< A number representing processor affinity */
    const unsigned int task_queue_number; /**< An unsigned int from (0..num_queues) */
    void* user_data; /**< Users can hang anything they want off of here */

    public:
    /**
     * Constructor
     *
     * @param [in] stack_size Size of the stack for the thread which inherits
     *            this struct.  Note that 0, 1 and 2 are reserved values for
     *            PFUNC_STACK_MAX, PFUNC_STACK_MIN and PFUNC_STACK_AVG.
     * @param [in] thread_id ID of the thread.
     * @param [in] thread_affinity Affinity of the thread to a processor.
     * @param [in] task_queue_number Queue number from which to poll for jobs.
     */
    thread_attr (const unsigned int& stack_size,
                 const unsigned int& thread_id,
                 const unsigned int& thread_affinity,
                 const unsigned int& task_queue_number)  :
                            stack_size (stack_size),
                            thread_id (thread_id),
                            thread_affinity (thread_affinity),
                            task_queue_number (task_queue_number) {}
 
    /**
     * @return Size of the stack for this attribute.
     */
    unsigned int get_stack_size () const  { return stack_size; }

    /**
     * @return Thread ID for this attribute.
     */
    unsigned int get_thread_id () const   { return thread_id; }

    /**
     * @return Processor Affinity for this attribute.
     */
    unsigned int get_thread_affinity () const  {return thread_affinity; }

    /**
     * @return Job queue number for this attribute.
     */
    unsigned int get_task_queue_number () const  {
      return task_queue_number;
    }
  };

/*****************************************************************************************/
  /**
   * \brief Portably creates and destroys threads on WINDOWS and Linux
   *
   * Only the common subset of thread creation attributes are supported. 
   * 
   */ 
  struct thread : public no_copy {
#if PFUNC_HAVE_WINDOWS_THREADS == 1
    typedef HANDLE thread_handle_type;/**< Type of a thread handle */
    typedef DWORD (__stdcall *start_func_type)(void*); /**< start func */
    typedef DWORD native_thread_id_type; /**< Type of the native ID */
#elif PFUNC_HAVE_PTHREADS == 1
    typedef pthread_t thread_handle_type; /**< Type of a thread handle */
    typedef void* (*start_func_type)(void*); /**< start func type */
    typedef pthread_t native_thread_id_type; /**< Type of the native ID */
#endif

#if PFUNC_HAVE_TLS == 1
    typedef std::vector<thread_attr*> tls_attribute_map_type; /**< Type of thread local storage */
#elif PFUNC_HAVE_HASH_MAP_H == 1
    /**
     * Function object that takes in a pointer to a native thread id type 
     * and returns a hash value of the same 
     */
    struct my_hash_function {
      typedef size_t result_type; /**< Type of the result -- hash key */
      std::hash<size_t> my_hash; /**< The internal hash function used */

      /**
       * Takes in a value and hashes it to a key. This is required only on 
       * OS X where we cannot directly use std::hash on pthread_t!
       *
       * \param [in] key The native thread ID type (pthread_t)

       * \return Hashed value of the value.
       */
      result_type operator () (const native_thread_id_type& key) const {
#if PFUNC_DARWIN == 1
        return my_hash(reinterpret_cast<size_t>(key));
#else
        return my_hash(key);
#endif /* if defined(__DARWIN__) */
      }
    };
    typedef std::hash_map <native_thread_id_type, 
                           thread_attr*, 
                           my_hash_function> tls_attribute_map_type; /**< Type of thread local storage */
#else
    typedef std::map <native_thread_id_type, thread_attr*> 
                                             tls_attribute_map_type; /**< Type of thread local storage */
#endif

    typedef tls_attribute_map_type::value_type tls_value_type; /**< Type of the thread local information stored */
    tls_attribute_map_type tls_attr_map; /**< Thread local storage */

#if PFUNC_HAVE_TLS == 1
    /**
     * required only on if TLS is suported.
     *
     * \param [in] nthreads Number of threads that require thread local storage.
     */
    void initialize (const unsigned int& nthreads) {
      tls_attr_map.resize (nthreads);
    }
#endif

    /**
     * \brief Creates a single thread
     * 
     * \param [in,out] handle A handle to the new thread.
     * \param [in] attr Attributes with which the thread is created. 
     * \param [in] start_func_ptr Pointer to the startup function.
     * \param [in] start_func_arg Argument to the startup function.
     *
     */
    void create_thread (thread_handle_type& handle,
                        thread_attr* attr,
                        start_func_type start_func_ptr,
                        void* start_func_arg)  {

#if PFUNC_USE_EXCEPTIONS == 1
      /** Check all the variables */

      /** Attribute should be non-NULL */
      if (NULL == attr)
        throw exception_generic_impl ("pfunc::detail::thread::create_thread",
                                      "NULL attribute provided",
                                      PFUNC_INVALID_ATTR);

      /** Starting function executed by this thread should be non-NULL */
      if (NULL == start_func_ptr)
        throw exception_generic_impl ("pfunc::detail::thread::create_thread",
                                      "NULL start function for thread",
                                      PFUNC_INVALID_ARGUMENTS);

      /** Argument to the starting function should be non-NULL */
      if (NULL == start_func_arg)
        throw exception_generic_impl ("pfunc::detail::thread::create_thread",
                                      "NULL argument for thread start function",
                                      PFUNC_INVALID_ARGUMENTS);
#endif

#if PFUNC_HAVE_WINDOWS_THREADS == 1
      handle = CreateThread (NULL, /* Security Attributes */
                             attr->get_stack_size(), /* dwStackSize */
                             start_func_ptr, /* StartRoutine */
                             start_func_arg, /* Function Argument */
                             0, /* CreationFlags */
                             NULL); /* ThreadId */
#if PFUNC_USE_EXCEPTIONS == 1
      /** If we could not create the thread */
      if (NULL == handle)
        throw exception_generic_impl
                  ("pfunc::detail::thread::create_thread::CreateThread",
                   "Could not create the thread",
                   GetLastError ());
#endif

#elif PFUNC_HAVE_PTHREADS == 1
      error_code_type error;

      /* The only thing that we need to set is the stacksize */
      pthread_attr_t my_attr;

      /* Iniitalize the attribute */
      error = pthread_attr_init (&my_attr);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
                ("pfunc::detail::thread::create_thread::pthread_attr_init",
                 "Could not initialize the attribute",
                 error);
#endif
     
      size_t stack_size = static_cast <size_t> (attr->get_stack_size());
      error = pthread_attr_setstacksize (&my_attr, stack_size);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
                ("pfunc::detail::thread::create_thread::pthread_attr_init",
                 "Could not set the stacksize",
                 error);
#endif
     
      error = pthread_attr_setscope (&my_attr, PTHREAD_SCOPE_SYSTEM);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
                ("pfunc::detail::thread::create_thread::pthread_attr_init",
                 "Could not set thread scope",
                 error);
#endif

      /* Create the thread now */
      error = pthread_create 
                    (&handle, &my_attr, start_func_ptr, start_func_arg);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
                ("pfunc::detail::thread::create_thread::pthread_attr_init",
                 "Could not create the thread",
                 error);
#endif
     
      /* Free the attribute */
      error = pthread_attr_destroy (&my_attr);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
                ("pfunc::detail::thread::create_thread::pthread_attr_init",
                 "Could not destroy the attribute",
                 error);
#endif

#else
#error "Windows threads or pthreads are required"
#endif
    }

    /**
     * Exits the calling thread
     */
    void exit_thread () const  {
#if PFUNC_HAVE_WINDOWS_THREADS
      ExitThread (0);
#elif PFUNC_HAVE_PTHREADS
      pthread_exit (NULL);
#else
#error "Windows threads or pthreads are required"
#endif
    }

    /**
     * Exits the calling thread
     *
     * \param [in] handle handle of the thread that we are joining with.
     */
    void join_thread (const thread_handle_type& handle) const  {
#if PFUNC_HAVE_WINDOWS_THREADS == 1
      WaitForSingleObject (handle, INFINITE);
#elif PFUNC_HAVE_PTHREADS == 1
      PFUNC_CAPTURE_RETURN_VALUE(error) pthread_join (handle, NULL);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
                ("pfunc::detail::thread::join_thread::pthread_join",
                 "Could not join on the thread",
                 error);
#endif
#else
#error "Windows threads or pthreads are required"
#endif
    }

    /**
     * Get an ID associated with the handle being passed in. For 
     * PTHREADS, this is the handle itself. On Windows, its 
     * gotten through a function call. This function is never called
     * from the thread whose native ID we seek.
     *
     * \param [in] handle The handle of the thread whose ID is being sought
     * \return Native ID of the thread with handle "handle".
     */ 
    native_thread_id_type get_native_id(const thread_handle_type& handle)const{ 
#if PFUNC_HAVE_WINDOWS_THREADS == 1
      return GetThreadId (handle);
#elif PFUNC_HAVE_PTHREADS == 1
      return handle;
#else
#error "Windows threads or pthreads are required"
#endif
    }
 
    /**
     * Get an ID associated with the current thread.
     *
     * \return Native ID of the current thread (eg., pthread_self())
     */ 
    native_thread_id_type get_native_id () const  { 
#if PFUNC_HAVE_WINDOWS_THREADS == 1
      return GetCurrentThreadId ();
#elif PFUNC_HAVE_PTHREADS == 1
      return pthread_self (); 
#else
#error "Windows threads or pthreads are required"
#endif
    }
 
    /**
     * Set the attributes in the TLS. Called from within a thread
     *
     * \param [in] attr The attribute to be set.
     */ 
    void tls_set (thread_attr* attr)  {
#if PFUNC_HAVE_TLS == 1
      tls_attr_map[pfunc_thread_self_id] = attr;
#else
      tls_attr_map [get_native_id()] = attr; 
#endif
    }

    /**
     * Get the attributes in the TLS. Called from within a thread
     *
     * \return Attributes (thread_attr) associated with the current thread.
     */ 
    thread_attr* tls_get ()  { 
#if PFUNC_HAVE_TLS == 1
      return tls_attr_map [pfunc_thread_self_id];
#else
      return tls_attr_map [get_native_id ()];
#endif 
    }

#if PFUNC_HAVE_SCHED_AFFINITY == 1 && PFUNC_HAVE_SCHED_H == 1
    /**
     * Get the number of cores in the system
     *
     * \return The number of processors in the currently running system.
     *
     */
    int get_num_procs () const  {
      int num_procs = 0;
      error_code_type error;
      cpu_set_t orig_set;
      cpu_set_t my_set;
      if (0 > (error=sched_getaffinity (0, sizeof(my_set), &orig_set))) 
#if PFUNC_USE_EXCEPTIONS == 1
        throw exception_generic_impl
                ("pfunc::detail::thread::get_num_procs:",
                 "Could not get the current thread affinities",
                 error);
#else
        return num_procs;
#endif
      for (num_procs=0; ;++num_procs) {
        CPU_ZERO (&my_set);
        CPU_SET(num_procs, &my_set);
        if (0 > sched_setaffinity (0, sizeof(my_set), &my_set)) break;
      }
      error = sched_setaffinity (0, sizeof(my_set), &orig_set);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
                ("pfunc::detail::thread::get_num_procs:",
                 "Could not reset the affinities back to the original",
                 error);
#endif
        return num_procs;
    }

    /**
     * Set the affinity of the thread to the given processor
     *
     * \param [in] proc_id The processor ID to bind the current thread.
     */
    void set_affinity (const int& proc_id) const  {
      error_code_type error;
      cpu_set_t my_set;
      CPU_ZERO (&my_set);
      CPU_SET (proc_id, &my_set);
      error = sched_setaffinity (0, sizeof(my_set), &my_set);
#if PFUNC_USE_EXCEPTIONS == 1
        if (error)
          throw exception_generic_impl
                ("pfunc::detail::thread::get_num_procs:",
                 "Could not set thread affinity",
                 error);
#endif
    }

#elif PFUNC_DARWIN == 1 && PFUNC_HAVE_SYSCTL_H == 1
    /**
     * Get the number of cores in the system
     *
     * \return The number of processors in the currently running system.
     *
     */
    int get_num_procs () const  {
      int num_procs = 0;
      error_code_type error;
      int mib[] = {CTL_HW, HW_NCPU};
      size_t len = sizeof(int);
      error = sysctl(mib, 2, &num_procs, &len, NULL, 0);
#if PFUNC_USE_EXCEPTIONS == 1
      if (error)
        throw exception_generic_impl
                ("pfunc::detail::thread::get_num_procs:",
                 "Could not get the number of processors",
                 error);
#endif
      return num_procs;
    }

    /**
     * Set the affinity of the thread to the given processor
     *
     * \param [in] proc_id The processor ID to bind the current thread.
     */
    void set_affinity (const int& proc_id) const  {
      /** Do nothing */
    }

#elif PFUNC_WINDOWS == 1
    /**
     * Get the number of cores in the system
     *
     * \return The number of processors in the currently running system.
     *
     */
    int get_num_procs () const  {
      int num_procs = 0;
      error_code_type error;
      num_procs = error = GetActiveProcessorCount (ALL_PROCESSOR_GROUPS);
#if PFUNC_USE_EXCEPTIONS == 1
    if (0 == error)
        throw exception_generic_impl
                ("pfunc::detail::thread::get_num_procs:",
                 "Could not get the number of processors",
                 error);
#endif
    }

    /**
     * Set the affinity of the thread to the given processor
     *
     * \param [in] proc_id The processor ID to bind the current thread.
     */
    void set_affinity (const int& proc_id) const  {
      /** Do nothing */
    }

#else /* NONE of the systems that we know of */

    /**
     * Get the number of cores in the system
     *
     * \return The number of processors in the currently running system.
     *
     */
    int get_num_procs () const {
      return 0;
    }

    /**
     * Set the affinity of the thread to the given processor
     *
     * \param [in] proc_id The processor ID to bind the current thread.
     */
    void set_affinity (const int& proc_id) const  {
      /** Do nothing */
    }
#endif


    /**
     * Yield the execution of the current thread
     */
    void yield () {
#if PFUNC_LINUX == 1 
      pthread_yield();
#elif PFUNC_DARWIN == 1
      pthread_yield_np();
#elif PFUNC_WINDOWS == 1
      SwitchToThread();
#elif PFUNC_AIX == 1 && PFUNC_HAVE_SCHED_H
      sched_yield();
#else
      /** Do nothing */
#endif
    }
  };
} /* namespace detail */ } /* namespace pfunc */

#endif // PFUNC_THREAD_HPP
