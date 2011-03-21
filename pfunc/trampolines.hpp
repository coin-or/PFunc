#ifndef PFUNC_TRAMPOLINES_HPP
#define PFUNC_TRAMPOLINES_HPP

/** Required because progress_wait in taskmgr requires a testable event */
#include <pfunc/event.hpp>

/**
 * \file trampolines.hpp
 * \brief Implementation of Trampoline Functions For PFUNC.
 * \author Prabhanjan Kambadur
 */
namespace pfunc { namespace detail {

/*************************************************************************/
/**
 * \brief This strucure is used for dynamic casting purposes ONLY. 
 *
 * \details 
 * taskmgr_virtual_base is used as a trampoline when we use either 
 * WINDOWS or pthreads. It also serves the important purpose of enabling 
 * global task managers to be used. Note that since the task manager's type
 * is not determined by the users, we have to resort to using a virtual class
 * if we are to use global classes.
 */ 
struct taskmgr_virtual_base { 
  /** 
   * Virtual destructor
   */
  virtual ~taskmgr_virtual_base() {}; 

  /**
   * Required for pthread_create to ultimately execute via reroute_function_arg
   */
  virtual void operator() (void*) = 0; 

  /**
   * Returns the PFunc ID (always unsigned int) of the current thread id
   */
  virtual unsigned int current_thread_id () = 0;

  /**
   * Returns the rank of the task being currently executed by the calling 
   * thread in that particular task's group.
   */
  virtual unsigned int current_task_group_rank () = 0;

  /**
   * Returns the size of group of the task being currently executed by the
   * calling thread.
   */
  virtual unsigned int current_task_group_size () = 0;

  /**
   * Executes a barrier accross the group to which the currently executing 
   * task (and hence, thread) belong.
   */
  virtual void current_task_group_barrier () = 0;

  /**
   * Executes a task (from own queue or otherwise) while waiting on a task
   * to complete.
   */
  virtual void progress_wait (event<testable_event>&) = 0;

  /**
   * Executes a task (from own queue or otherwise) while waiting on a barrier
   * to complete. Note that the task executed is ALWAYS from a group other than
   * that of the calling task (ensures deadlock avoidance).
   */
  virtual void progress_barrier () = 0;

  /**
   * Runs the specified task. This function call is type UNSAFE and is used 
   * when global_taskmanager is used. We cannot help it as the derived class
   * of taskmgr_virtual_base class has template arguments. So, we don't know 
   * the type of the task or the functor.
   */
  virtual void spawn_task (void*, void*) = 0;

  /**
   * Runs the specified task. This function call is type UNSAFE and is used 
   * when global_taskmanager is used. We cannot help it as the derived class
   * of taskmgr_virtual_base class has template arguments. So, we don't know 
   * the type of the task, attribute, or the functor.
   */
  virtual void spawn_task (void*, void*, void*) = 0;

  /**
   * Runs the specified task. This function call is type UNSAFE and is used 
   * when global_taskmanager is used. We cannot help it as the derived class
   * of taskmgr_virtual_base class has template arguments. So, we don't know 
   * the type of the task, attribute, group or the functor.
   */
  virtual void spawn_task (void*, void*, void*, void*) = 0;

  /**
   * Sets the maximum number of attempts made by a thread before yielding.
   */
  virtual void set_max_attempts (const unsigned int&) = 0;

  /**
   * Gets the maximum number of attempts made by a thread before yielding.
   */
  virtual unsigned int get_max_attempts () const = 0;
};

/***********************************************************************/

/**
 *
 * Each thread ultimately ends up running "run()" function in pfunc_t. 
 * However, since both pthreads and WINDOWS threads cannot directly 
 * call member functions as their start functions, we use a trampoline
 * mechanism.
 */ 
struct reroute_function_arg {
  taskmgr_virtual_base* taskmgr_ptr; /**< pointer to the task manager */
  void* thd_attr; /**< pointer (cast as void) to thread_attr */

  /**
   * Constructor
   *
   * \param [in] taskmgr_ptr The pointer to the taskmgr object.
   * \param [in] thd_attr The pointer to the pfunc_thread_attr_t 
   *                     associated with this thread.
   */
  explicit reroute_function_arg (taskmgr_virtual_base* taskmgr_ptr,
                                 void* thd_attr) : 
                                     taskmgr_ptr (taskmgr_ptr),
                                     thd_attr (thd_attr) {}
};

/** 
 * \brief Virtual base class that is the default functor used.
 *
 * 
 */
struct virtual_functor { 
  virtual ~virtual_functor() {} ; 
  virtual void operator() (void) = 0; 
};

} /* namespace detail */ } /* namespace pfunc */

/*************************************************************************/

#ifdef PFUNC_WINDOWS
#define PFUNC_THREAD_FUNC_PREFIX DWORD __stdcall 
#else
#define PFUNC_THREAD_FUNC_PREFIX void*
#endif

namespace {
 /**
  * \brief Trampoline thread start routine that calls into taskmgr->run().
  *
  * \param object reroute_function_arg pointer.
  */
  extern "C" PFUNC_THREAD_FUNC_PREFIX reroute_function (void* object) {
    /* cast the parameter back to pfunc_reroute_function_arg_t */
    pfunc::detail::reroute_function_arg* arg = 
             static_cast<pfunc::detail::reroute_function_arg*>(object);
 
    /* call "run" with the argument passed to it */          
    (*(arg->taskmgr_ptr))(arg->thd_attr);
 
     /* never reaches here */
    return 0;
  }
}

#undef PFUNC_THREAD_FUNC_PREFIX

/************************************************************************/

#endif // PFUNC_TRAMPOLINES_HPP
