#ifndef PFUNC_TASKMGR_HPP
#define PFUNC_TASKMGR_HPP

  
/**
 * \file taskmgr.hpp
 * \brief Implementation of Main Object For PFUNC -- A Task Parallel API
 * \author Prabhanjan Kambadur
 */
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <limits>
#include <utility>
#include <map>

#include <pfunc/pfunc_common.h>
#include <pfunc/pfunc_atomics.h>
#include <pfunc/exception.hpp>
#include <pfunc/mutex.hpp>
#include <pfunc/barrier.hpp>
#include <pfunc/thread.hpp>

#if PFUNC_USE_PAPI == 1
#include <pfunc/perf.hpp>
#endif

#include <pfunc/event.hpp>
#include <pfunc/group.hpp>
#include <pfunc/attribute.hpp>
#include <pfunc/task.hpp>
#include <pfunc/trampolines.hpp>
#include <pfunc/scheduler.hpp>
#include <pfunc/predicate.hpp>
#include <pfunc/environ.hpp>

/**
 * \brief Namespace for the C++ interface of PFUNC.
 * 
 * \details
 * Putting all the public structures here under the namespace ``pfunc'' since 
 * there are going to inevitably be a couple of name clashes between the C and
 * the C++ interface. For example, ``work_func_t'' is a class in the C++ 
 * interface and a function pointer in the C interface.
 *
 */ 
namespace pfunc { namespace detail {

/** 
 * \brief Main class that implements the tasking aspect
 *
 * @param SchedPolicy The scheduling policy to use.
 * @param Task The type of the task to use.
 *
 * This is the main struct that implements the functionality provided
 * in pfunc tool kit. Of the many things implemented in this class,
 * the important functions are:
 * 1. Create X number of threads and Y number of queues.
 * 2. Add a task to a queue.
 * 3. Wait for completions for those added tasks.
 *
 */    
template <typename SchedPolicy,
          typename Task>
struct taskmgr : public taskmgr_virtual_base  {
  typedef Task task; /**< type of task */
  typedef Task* task_ptr; /**< pointer to it */
  typedef typename task::functor functor; /**< Type of the functor */
  typedef SchedPolicy sched_policy; /**< Type of scheduler in use */
  typedef scheduler<sched_policy, task_ptr> queue_type; /**< scheduler */
  typedef typename task::attribute attribute; /**< To know the attribute */
  typedef typename attribute::priority_type priority_type; /**< To know what priority exit_job */
  typedef thread::native_thread_id_type native_thread_id_type; /**< used for storage */
  typedef regular_get_own_predicate<task_ptr> regular_own_predicate;
  typedef regular_get_steal_predicate<task_ptr> regular_steal_predicate;
  typedef waiting_get_own_predicate<sched_policy, task_ptr> waiting_own_predicate;
  typedef waiting_get_steal_predicate<sched_policy, task_ptr> waiting_steal_predicate;
  typedef barrier_get_own_predicate<sched_policy, task_ptr> barrier_own_predicate;
  typedef barrier_get_steal_predicate<sched_policy, task_ptr> barrier_steal_predicate;
  typedef std::pair<regular_own_predicate, regular_steal_predicate> regular_predicate;
  typedef std::pair<waiting_own_predicate, waiting_steal_predicate> waiting_predicate;
  typedef std::pair<barrier_own_predicate, barrier_steal_predicate> barrier_predicate;
  typedef typename thread::thread_handle_type thread_handle_type;

  private: 
  const unsigned int num_queues; /**< Number of task queues to create */
  unsigned int num_threads; /**< Number of work threads to create */
  unsigned int* threads_per_queue; /**< Holds the number of threads waiting on each queue */ 
  queue_type* task_queue; 
  thread_handle_type* thread_handles; /**< thread handles */
  thread_attr** thread_data; /**< Startup information for the threads */
  task* task_cache; /**< Used to extract the closest possible match */
  reroute_function_arg** thread_args; /**< Arguments to reroute_function */
  volatile unsigned int thread_start_count; /**< Used to ensure all threads start */
  thread_attr* main_thread_attr; /**< We will set some defaults for the main thread */
  thread thread_manager; /**< Creates and manages threads */
  barrier start_up_barrier; /**< Ensures all threads start together */
#if PFUNC_USE_PAPI == 1
  long long** perf_event_values; /**< a place holder for events */
  int num_events; /**< The number of events to monitor */
  int *event_codes; /**< The event codes to be monitored */
#endif
  /**
   * Used to implement user-level thread cancellation 
   */
  struct aligned_bool { 
    ALIGN128 volatile bool is_cancelled; 
    aligned_bool () : is_cancelled (false) {}
    void cancel () { is_cancelled = true; }
    bool operator()() const { return is_cancelled; }
  };
  aligned_bool* thread_state; /**< Denote thread cancellations */
  unsigned int task_max_attempts; /**< Number of attempts before backoff */
  PFUNC_DEFINE_EXCEPT_PTR() /**< Place to store the exception */

  /**
   * Used to wrap around testing_compl.test()
   */
  struct task_completion_predicate {
    event<testable_event>& compl_event; /**< Completion event */

    /**
     * \param [in,out] compl_event A testable completion event reference.
     */
    task_completion_predicate (event<testable_event>& compl_event) :
                                compl_event (compl_event) {}

    /**
     * \return true If the task completed.
     * \return false If the task has not completed.
     */
    bool operator ()() const { return compl_event.test(); }
  };

  public:
  /**
   * \brief Returns information regarding the current thread.
   *
   * \details
   * Every task is being executed by some thread or the other. This function 
   * returns the ID of the thread that is being executed. This is an uint.
   * If the thread querying for information is NOT a PFunc thread, we return
   * -1
   *
   * \return ID of the currently executing thread (0 -- NUM_THREADS-1)
   */ 
  unsigned int current_thread_id ()  {
    unsigned int tid;
    PFUNC_START_TRY_BLOCK()
    tid = (thread_manager.tls_get ())->get_thread_id ();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,current_thread_id)
    return tid;
  }

  /**
   * \brief Returns information regarding the current task being executed.
   *
   * \details
   * Every thread stores the current task that it is executing. This means that
   * we can get the handle, group and work attributes associated with every 
   * task that a particular thread is executing. Hence, tasks can query this
   * information without needing actually explictly passing any information.
   *
   * \return Pointer to the task being currently executed.
   */ 
  task_ptr current_task_information ()  {
    task_ptr tptr;
    PFUNC_START_TRY_BLOCK()
    tptr = &(task_cache[current_thread_id()]);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,current_task_information)
    return tptr;
  }

  /**
   * \brief Returns the rank of the calling task in its group.
   *
   * \details
   * We allow each task to get its rank and size from the running environment.
   * This is possible since we always cache each thread's currently executing 
   * task.
   *
   * \return Rank of the task being currently executed in its group.
   */
  unsigned int current_task_group_rank () {
    unsigned int grank;
    PFUNC_START_TRY_BLOCK()
    grank = task_cache[current_thread_id ()].get_rank ();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,current_task_group_rank)
    return grank;
  }

  /**
   * \brief Returns the size of the calling task's group.
   *
   * \details
   * We allow each task to get its rank and size from the running environment.
   * This is possible since we always cache each thread's currently executing 
   * task.
   *
   * \return Size of the task being currently executed in its group.
   */
  unsigned int current_task_group_size () {
    unsigned int gsize;
    PFUNC_START_TRY_BLOCK()
    gsize = task_cache[current_thread_id ()].get_size ();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,current_task_group_size)
    return gsize;
  }

  /**
   * \brief Executes a barrier accross the group of the currently executing 
   * task (and hence, thread). Most of the details regarding the barrier are 
   * stored with the task -- so we just refer back to the current task that
   * we cache.
   */
  void current_task_group_barrier () {
    PFUNC_START_TRY_BLOCK()
    task_cache[current_thread_id ()].barrier (*this);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,current_task_group_barrier)
  }

  /**
   * spawn_task
   *
   * This function is used to add a C++-style object to the queue.
   *
   * \param [in,out] new_task The new task to be added. new_task must have a
   * lifetime atleast till the wait on this task is complete. No copy is made
   * as it is expensive.
   * \param [in] new_attr The attributes that dictate execution of the task.
   * \param [in] new_group The group to be associated with the handle.
   * \param [in] new_work The function object that represents the code to be
   * executed.
   */ 
  void spawn_task (task& new_task, 
                   const attribute& new_attr, 
                   group& new_group,
                   functor& new_work)  {
    PFUNC_START_TRY_BLOCK()
    new_task.set_attr (new_attr);
    new_task.set_group (&new_group);
    new_task.set_func (&new_work);
    new_task.reset_completion (new_attr.get_num_waiters());
    unsigned int task_queue_number = new_attr.get_queue_number();

    if (QUEUE_CURRENT_THREAD == task_queue_number) /* current thread's queue*/
      task_queue_number=(thread_manager.tls_get())->get_task_queue_number();
    
    task_queue->put (&new_task, task_queue_number);
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,spawn_task)
  }

  /**
   * spawn_task
   *
   * This function is used to spawn a new task -- only from the virtual base
   * class. So, all the pointers are void* and we have to cast them back into
   * the right type. God save the typesafety in this case.
   *
   * \param [in,out] new_task The new task to be added. new_task must have a
   * lifetime atleast till the wait on this task is complete. No copy is made
   * as it is expensive.
   * \param [in] new_attr The attributes that dictate execution of the task.
   * \param [in] new_group The group to be associated with the handle.
   * \param [in] new_work The function object that represents the code to be
   * executed.
   */ 
  void spawn_task (void* new_task, 
                   void* new_attr, 
                   void* new_group,
                   void* new_work)  {
    PFUNC_START_TRY_BLOCK()
    spawn_task (*(static_cast<task*>(new_task)),
                *(static_cast<attribute*>(new_attr)),
                *(static_cast<group*>(new_group)),
                *(static_cast<functor*>(new_work)));
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,spawn_task)
  }

  /**
   * Constructor
   * \brief Create all the threads. Make them wait on the task queue
   *
   * \param [in] num_queues Number of queues to create.
   * \param [in] thds_per_queue Number of threads to create per queue.
   * \param [in] perf_data Performance metrics to be collected.
   * \param [in] affinity The affinity of the threads to the proceseors.
   */ 
  taskmgr (const unsigned int& num_queues, 
           const unsigned int* thds_per_queue,
#if PFUNC_USE_PAPI == 1
           const virtual_perf_data& perf_data,
#endif
           const unsigned int** affinity = NULL)  :
                      num_queues (num_queues),
                      num_threads (0),
                      threads_per_queue (NULL),
                      task_queue(NULL),
                      thread_handles (NULL),
                      thread_data (NULL),
                      thread_args (NULL),
                      thread_start_count (0),
#if PFUNC_USE_PAPI == 1
                      perf_event_values (NULL),
#endif
                      thread_state (NULL),
                      task_max_attempts (2000000)
                      PFUNC_EXCEPT_PTR_INIT() {
    PFUNC_START_TRY_BLOCK()
    /* Allocate memory for threads_per_queue */
    threads_per_queue = new unsigned int[num_queues];
    
    /* Copy-in threads_per_queue and calculate the number of threads */
    for (unsigned int i=0; i<num_queues; ++i) {
      threads_per_queue[i] = thds_per_queue[i];
      num_threads += threads_per_queue[i];
    }

    /* Set the ID of the main thread to be num_threads */
    main_thread_attr = new thread_attr (PFUNC_STACK_MAX,/* main thread stack */
                                        num_threads, /* main thread ID number */
                                        0,
                                        0), /* put main thread jobs on Q 0 */

#if PFUNC_HAVE_TLS == 1
    /* Allocate memory for the thread data */
    thread_manager.initialize (num_threads+1/*for the main thread*/);
    pfunc_thread_self_id = num_threads; /*for the main thread*/
#endif
    
    /* Allocate memory for the Queues */
    task_queue = new queue_type(num_queues);
    
    /* Allocate memory to hold the handles */
    thread_handles = new thread_handle_type [num_threads];
    
    /* Allocate memory to hold the thread_data */
    thread_data = new thread_attr*[num_threads];
    
    /* Allocate memory to hold the arguments to the thread function */
    thread_args = new reroute_function_arg*[num_threads];

    /* Allocate memory to hold the tasks. This is used for the cache */
    task_cache = new task[num_threads];
    
    /* Allocate memory for the thread_state */
    thread_state = new aligned_bool [num_threads];

    /* Add the main thread's attribute to TLS */
    thread_manager.tls_set (main_thread_attr);

    /* set the barrier */
    start_up_barrier.initialize (num_threads);

    /* If PERF has been defined, then so be it */
#if PFUNC_USE_PAPI == 1
    num_events = perf_data.get_num_events ();
    event_codes = perf_data.get_events ();
    perf_event_values = perf_data.get_event_storage ();

    if (!perf::initialize (event_codes, num_events))
      printf ("Could not initialize performance counting\n");
#endif

    /* Now to create threads */
    int index = -1;

    for (unsigned int i=0; i<num_queues; ++i) {
      for (unsigned int j=0; j<threads_per_queue[i]; ++j) {
        ++index;
        thread_data[index] = new thread_attr 
                                   (PFUNC_STACK_MAX, /* stack_size */
                                    index, /* Thread ID */
                                    (NULL==affinity)? PFUNC_NO_AFFINITY:
                                                      affinity [i][j], 
                                    i); /* Queue Number*/
     
        thread_args[index]=new reroute_function_arg(this, thread_data[index]);
        
        thread_manager.create_thread (thread_handles[index], /* handle */
                               thread_data[index], /* attributes */
                               reroute_function, /* trampoline function */
                               thread_args[index]); /* arg */
      }
    }

    /* Make sure that we do not exit this function until all the threads */
    while (thread_start_count != static_cast<unsigned int> (num_threads));
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,taskmgr)
  }

  /**
   * Destructor
   */
  virtual ~taskmgr ()  {
    PFUNC_START_TRY_BLOCK()

    /** Cancel all the threads */
    for (unsigned int i=0; i<num_threads; ++i) thread_state[i].cancel ();

    /** Wait for their completion */
    for (unsigned int i=0; i<num_threads; ++i) 
      thread_manager.join_thread (thread_handles[i]);

    for (unsigned int i=0; i<num_threads; ++i) {
      delete thread_data[i];
      delete thread_args[i];
    }

    delete task_queue;
    delete [] thread_handles;
    delete [] thread_data;
    delete [] task_cache;
    delete [] thread_args;
    delete [] threads_per_queue;
    delete [] thread_state;
    delete main_thread_attr;

    PFUNC_EXCEPT_PTR_CLEAR()
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,~taskmgr)
  }

  /**
   * Set the maximum number of attempts before backoff -- ~0x0 by default
   *
   * \param [in] max_attempts The new value of task_max_attempts.
   */
  void set_max_attempts (const unsigned int& max_attempts) {
    task_max_attempts = max_attempts;
  }

  /**
   * Get the maximum number of attempts before backoff -- ~0x0 by default
   *
   * \return The current value of task_max_attempts.
   */
  unsigned int get_max_attempts () const {
    return task_max_attempts;
  }

  /**
   * Function that retrieves a task from the task_queue (preferably from 
   * the thread's own) with some amount of regulation builtin. The regulation
   * is that if we cannot find a suitable task for a X number of attempts,
   * we relinquish control of the processor and try back with X/2 attempts.
   * Not quite an exponential backoff, but it does for now.
   * 
   * \param [in] completion_pred A boolean predicate that signals the completion
   *            of the waiting.
   * \param [in] max_attempts The maximum number of attempts to make.
   * \param [in] queue_number The primary queue number for the calling thread.
   * \param [in] task_pred The predicate based on which the task is selected.
   *
   * \return A pointer to the task that needs to be executed.
   */
  template <typename CompletionPredicate, typename TaskPredicate>
  task* get_task (const CompletionPredicate& completion_pred,
                  const unsigned int& max_attempts,
                  const unsigned int& queue_number,
                  const TaskPredicate& task_pred) {
    unsigned int num_attempts = max_attempts;
    task* return_value = NULL;

    PFUNC_START_TRY_BLOCK()

    while (!completion_pred() && (0 < num_attempts--)) {
      if (NULL != (return_value = 
                      task_queue->get (queue_number, task_pred))) 
                        goto end_of_get_task;
    }

    /** Decrease the number of attempts, yield and start back again */
    if (!completion_pred()) { 
      thread_manager.yield ();
      return_value =  get_task (completion_pred,
                                (0==(max_attempts/2)) ? 1 : (max_attempts/2),
                                queue_number,
                                task_pred);
    }

    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,get_task)

    end_of_get_task:
    return return_value;
  }

  /**
   * Endless loop that the worker threads execute
   *
   * \param [in] _my_attr thread_attr* cast as a void*.
   *
   * This function is invoked at the start of each thread. As such, 
   * each thread waits in an endless loop for work. The only piece 
   * of information that this thread needs is the JOBQUEUE which it 
   * is supposed to wait on. Also, an ID of some form would be nice.
   *
   * Upon completion of the job, it sets the status of the job to 
   * in a manner which can be checked. 
   */ 
  void operator()(void* _my_attr) {

    PFUNC_START_TRY_BLOCK()

    /* Get information about self */
    thread_attr* my_attr = 
                static_cast<thread_attr*> (_my_attr);

    /* Get the information pertaining to this thread */
    const unsigned int my_thread_id = my_attr->get_thread_id ();
    const unsigned int my_task_queue_number = my_attr->get_task_queue_number ();
    const unsigned int my_processor_affinity = my_attr->get_thread_affinity ();

    /* Set my id for thread_attr access from other places */
#if PFUNC_HAVE_TLS == 1
    pfunc_thread_self_id = my_thread_id;
#endif

    /* Save the attribute for later */
    thread_manager.tls_set (my_attr);

    /* Let us set the processor affinity now */
    if (PFUNC_NO_AFFINITY!=my_processor_affinity)
      thread_manager.set_affinity (my_processor_affinity);

    /* Wait for all the threads to get here */
    start_up_barrier();

    /* If performance has been defined, create events and start counting */
#if PFUNC_USE_PAPI == 1
    int my_event_set = perf::create_events ();
    if (!perf::start_events (my_event_set)) 
      printf ("%u: Could not start counting\n", my_thread_id);
#endif

    /* When everything is set up, signal the main thread */
    pfunc_fetch_and_add_32 (&thread_start_count, 1);

    /* WORK LOOP */
    task* my_task = NULL;
    while (NULL != (my_task = get_task ((thread_state[my_thread_id]),
                                        task_max_attempts,
                                        my_task_queue_number,
                                        regular_predicate()))) {
      task_cache [my_thread_id].shallow_copy(*my_task); /* Set the cache */
      my_task->run (); /* Now, lets run the job */
      my_task->notify (); /* signal whoever was waiting */
    }

    /** time to exit */
#if PFUNC_USE_PAPI == 1
    if (!perf::stop_events (my_event_set, perf_event_values[my_thread_id]))
      printf ("%u: Could not stop counting\n", my_thread_id);
#endif

    thread_manager.exit_thread ();

    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,op_paranthesis)
  }

  /**
   * Picks up and executes other tasks (non-exit) while waiting on another
   * task to complete. The other task's completion is signalled using the 
   * input parameter (testable event).
   *
   * \param [in] compl_event A testable event that signals completion of the 
   *  task that we are waiting on.
   */
  void progress_wait (event<testable_event>& compl_event) {

    PFUNC_START_TRY_BLOCK()

    thread_attr& my_data = *(thread_manager.tls_get ());

    const unsigned int my_thread_id = my_data.get_thread_id ();
    const unsigned int my_task_queue_number = my_data.get_task_queue_number ();

    /** Create the predicate from the event */
    task_completion_predicate completion_pred (compl_event);
     
    /** We need to handle a non-PFunc task trying to progress PFunc's tasks 
        right here because it does not fly! Typically, users are required to 
        just set "nested=false" in the task's attribute when using main thread.
        However, for people who do insist on committing this folly, we will,
        for now, simply keep yielding and testing till the task is complete.
        There seems to be no other good solution for now */
    if (num_threads == my_thread_id) {
      while (!completion_pred()) thread_manager.yield ();
    } else { /** PFunc's thread */
      task current_task;
     
      current_task.shallow_copy (task_cache[my_thread_id]);
     
      task* my_task = NULL;
      while (NULL != (my_task = get_task (completion_pred,
                                          task_max_attempts,
                                          my_task_queue_number,
                                          waiting_predicate
                                     (waiting_own_predicate(&current_task),
                                      waiting_steal_predicate(&current_task))))) {
     
        /* This task might steal again, set it to be in the cache */
        task_cache[my_thread_id].shallow_copy(*my_task);
       
        /* run the task */
        my_task->run (); 
       
        /* Notify the waiters on the second task */
        my_task->notify ();
       
        /* We have finished stealing, reset the cache again */
        task_cache[my_thread_id].shallow_copy(current_task);
      }
    }

    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,progress_wait)
  }

  /**
   * Picks up a task (non-exit) to execute while waiting on a barrier to
   * complete. 
   */
  void progress_barrier ()  {

    PFUNC_START_TRY_BLOCK()

    thread_attr& my_data = *(thread_manager.tls_get ());
    if (num_threads == my_data.get_thread_id ()) return;

    const unsigned int my_thread_id = my_data.get_thread_id ();
    const unsigned int my_task_queue_number = my_data.get_task_queue_number ();
    
    task current_task;

    current_task.shallow_copy (task_cache[my_thread_id]);

    task* my_task = task_queue->get (my_task_queue_number, 
                                     barrier_predicate
                                  (barrier_own_predicate(&current_task),
                                   barrier_steal_predicate(&current_task)));

    if (NULL == my_task) return;

     /* This task might steal again, set it to be in the cache */
    task_cache[my_thread_id].shallow_copy(*my_task);

    /* run the task */
    my_task->run (); 

    /* Notify the waiters on the second task */
    my_task->notify ();

    /* We have finished stealing, reset the cache again */
    task_cache[my_thread_id].shallow_copy(current_task);

    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(taskmgr,progress_wait)
  }
};
} /* namespace detail */ }  /* namespace pfunc */
#endif // PFUNC_TASKMGR_HPP
