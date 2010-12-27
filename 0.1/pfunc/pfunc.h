#ifndef PFUNC_H
#define PFUNC_H

#include <pfunc/pfunc_common.h>

#ifndef PFUNC_PACK_CODE /** we only need decls of pack and unpack */

/** Type of the work function */
typedef void (*pfunc_c_work_func_t)(void*);

#ifdef PFUNC_LIBRARY_CODE /** include this part if compiling C-interface */

#include <pfunc/generator.hpp>

/** The C Interface 
    Author: Prabhanjan Kambadur   **/

namespace pfunc { namespace detail {
/** Type of the functor itself */
struct internal_work_func_t {
  /** 
   * Set the work function pointer in our function object.
   *
   * \param[in] ptr The work function to be used.
   */
  void set_func (const pfunc_c_work_func_t& ptr) { func_ptr = ptr;}

  /** 
   * Get the work function pointer from our function object.
   *
   * \return The work function to be used.
   */
  pfunc_c_work_func_t get_func ( ) const { return func_ptr;}

  /** 
   * Set the argument to the work function. 
   *
   * \param[in] arg The work function argument.
   */
  void set_arg (void* arg) { func_arg = arg;}

  /** 
   * Get the argument to the work function. 
   *
   * \return The work function argument.
   */
  void* get_arg ( ) const { return func_arg;}

  /** 
   * The right constructor to use 
   *
   * \param[in] func The work function.
   * \param[in] arg The work function argument.
   */
  internal_work_func_t (pfunc_c_work_func_t func, void* arg) :
                  func_ptr (func), func_arg (arg) {}

  /** The default constructor */
  internal_work_func_t () : func_ptr (NULL), func_arg (NULL) {}

  /** 
   * The copy constructor i
   *
   * \param[in] other The work function to copy from.
   */
  internal_work_func_t (const internal_work_func_t& other) : 
                  func_ptr (other.get_func()), func_arg (other.get_arg()) {}

  /** Call the back end function */
  void operator() (void) { func_ptr (func_arg); }

  private:
  pfunc_c_work_func_t func_ptr; /**< The stored work function pointer */
  void* func_arg; /**< The stored work function argument */
};

/** 
 * \def Generate the typedefs for a type. We currently offer four types --
 * lifo, fifo, cilk and prio. This macro is used to generate the required
 * nested types (attribute, group, task and taskmgr) for each of the types.
 */
#define PFUNC_GEN_TYPES(sched) \
typedef generator <sched##S, \
                   use_default,  \
                   internal_work_func_t> pfunc_##sched##_type_t; \
typedef pfunc_##sched##_type_t::attribute pfunc_##sched##_attr_t; \
typedef pfunc_##sched##_type_t::task pfunc_##sched##_task_t; \
typedef pfunc_##sched##_type_t::taskmgr pfunc_##sched##_taskmgr_t; \
typedef pfunc_##sched##_type_t::group pfunc_##sched##_group_t; 

/** Generate the typedef's for cilk */
PFUNC_GEN_TYPES(cilk)

/** Generate the typedef's for fifo */
PFUNC_GEN_TYPES(fifo)

/** Generate the typedef's for lifo */
PFUNC_GEN_TYPES(lifo)

/** Generate the typedef's for prio */
PFUNC_GEN_TYPES(prio)
} /* namespace detail */ } /* namespace pfunc */

#else /** if we are not in the library code, include atomic operations */

#include "pfunc_atomics.h"

#endif /** PFUNC_LIBRARY_CODE */

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * \def This macro declares dummy structures (note, we do not define them 
 * for additional type safety) that we use as spohisticated void*. Basically,
 * we are using pointers to go between C and C++. At least, by having different
 * type of pointers for each (attribute, group, task, taskmgr), we ensure that
 * users do not inadvertently mix and match PFunc's library instances.
 */
#define PFUNC_GEN_C_TYPES(sched) \
struct pfunc_##sched##_dummy_attr_t; \
typedef struct pfunc_##sched##_dummy_attr_t* pfunc_##sched##_attr_t; \
struct pfunc_##sched##_dummy_task_t; \
typedef struct pfunc_##sched##_dummy_task_t* pfunc_##sched##_task_t; \
struct pfunc_##sched##_dummy_taskmgr_t; \
typedef struct pfunc_##sched##_dummy_taskmgr_t* pfunc_##sched##_taskmgr_t; \
struct pfunc_##sched##_dummy_group_t; \
typedef struct pfunc_##sched##_dummy_group_t* pfunc_##sched##_group_t; \

/** Priority (when using "prio" is always of type "int" */ 
typedef int pfunc_attr_priority_t;

/** Type to be used for specifying the queue numbers */
typedef unsigned int pfunc_attr_qnum_t;

/** Type to be used for specifying the number of waiters for a task */
typedef unsigned int pfunc_attr_num_waiters_t;

/** Type to be used for specifying if a task is nested or not */
typedef unsigned int pfunc_attr_nested_t;

/** Type to be used for specifying if a task belongs to a group or not */
typedef unsigned int pfunc_attr_grouped_t;

/** Type to be used for specifying the level of a task */
typedef unsigned int pfunc_attr_level_t;

/** Type to be used for getting and setting the group size */
typedef unsigned int pfunc_group_size_t;

/** Type to be used for getting and setting the group ID */
typedef unsigned int pfunc_group_id_t;

/** Type to be used for getting and setting the type of barrier to use */
typedef unsigned int pfunc_group_barrier_t;

/** 
 * \def Generates the declarations for functions to initialize and clear 
 * attribute structures.
 */
#define PFUNC_GEN_ATTR_INITCLEAR_DECLS(sched) \
int pfunc_##sched##_attr_init (pfunc_##sched##_attr_t*); \
int pfunc_##sched##_attr_clear (pfunc_##sched##_attr_t*); \

/** 
 * \def Generates the declarations for functions to set the different
 * attributes of a task store in an attribute structure.
 */
#define PFUNC_GEN_ATTR_SET_DECLS(sched) \
int pfunc_##sched##_attr_priority_set \
    (pfunc_##sched##_attr_t, pfunc_attr_priority_t);\
int pfunc_##sched##_attr_qnum_set \
    (pfunc_##sched##_attr_t, pfunc_attr_qnum_t);\
int pfunc_##sched##_attr_num_waiters_set \
    (pfunc_##sched##_attr_t, pfunc_attr_num_waiters_t);\
int pfunc_##sched##_attr_nested_set \
    (pfunc_##sched##_attr_t, pfunc_attr_nested_t);\
int pfunc_##sched##_attr_grouped_set \
    (pfunc_##sched##_attr_t, pfunc_attr_grouped_t);\
int pfunc_##sched##_attr_level_set \
    (pfunc_##sched##_attr_t, pfunc_attr_level_t);

/** 
 * \def Generates the declarations for functions to get the different
 * attributes of a task store in an attribute structure.
 */
#define PFUNC_GEN_ATTR_GET_DECLS(sched) \
int pfunc_##sched##_attr_priority_get \
    (pfunc_##sched##_attr_t, pfunc_attr_priority_t*);\
int pfunc_##sched##_attr_qnum_get \
    (pfunc_##sched##_attr_t, pfunc_attr_qnum_t*);\
int pfunc_##sched##_attr_num_waiters_get \
    (pfunc_##sched##_attr_t, pfunc_attr_num_waiters_t*);\
int pfunc_##sched##_attr_nested_get \
    (pfunc_##sched##_attr_t, pfunc_attr_nested_t*);\
int pfunc_##sched##_attr_grouped_get \
    (pfunc_##sched##_attr_t, pfunc_attr_grouped_t*); \
int pfunc_##sched##_attr_level_get \
    (pfunc_##sched##_attr_t, pfunc_attr_level_t*);

/**
 * \def Generates all the declarations related to attributes by calling 
 * each of PFUNC_GEN_ATTR_INITCLEAR_DECLS, PFUNC_GEN_ATTR_SET_DECLS and 
 * PFUNC_GEN_ATTR_GET_DECLS respectively.
 */
#define PFUNC_GEN_ATTR_DECLS(sched) \
  PFUNC_GEN_ATTR_INITCLEAR_DECLS(sched)\
  PFUNC_GEN_ATTR_SET_DECLS(sched) \
  PFUNC_GEN_ATTR_GET_DECLS(sched)

/** 
 * \def Generates the declarations for functions to initialize and clear 
 * group structures.
 */
#define PFUNC_GEN_GROUP_INITCLEAR_DECLS(sched) \
int pfunc_##sched##_group_init (pfunc_##sched##_group_t*); \
int pfunc_##sched##_group_clear (pfunc_##sched##_group_t*); \

/** 
 * \def Generates the declarations for functions that set the different
 * attributes of a group structure.
 */
#define PFUNC_GEN_GROUP_SET_DECLS(sched) \
int pfunc_##sched##_group_size_set \
    (pfunc_##sched##_group_t, pfunc_group_size_t);\
int pfunc_##sched##_group_id_set \
    (pfunc_##sched##_group_t, pfunc_group_id_t);\
int pfunc_##sched##_group_barrier_set \
    (pfunc_##sched##_group_t, pfunc_group_barrier_t);\

/** 
 * \def Generates the declarations for functions that get the different
 * attributes of a group structure.
 */
#define PFUNC_GEN_GROUP_GET_DECLS(sched) \
int pfunc_##sched##_group_size_get \
    (pfunc_##sched##_group_t, pfunc_group_size_t*);\
int pfunc_##sched##_group_id_get \
    (pfunc_##sched##_group_t, pfunc_group_id_t*);\
int pfunc_##sched##_group_barrier_get \
    (pfunc_##sched##_group_t, pfunc_group_barrier_t*);\

/**
 * \def Generates all the declarations related to groups by calling 
 * each of PFUNC_GEN_GROUP_INITCLEAR_DECLS, PFUNC_GEN_GROUP_SET_DECLS and 
 * PFUNC_GEN_GROUP_GET_DECLS respectively.
 */
#define PFUNC_GEN_GROUP_DECLS(sched) \
  PFUNC_GEN_GROUP_INITCLEAR_DECLS(sched)\
  PFUNC_GEN_GROUP_SET_DECLS(sched) \
  PFUNC_GEN_GROUP_GET_DECLS(sched)

/** 
 * \def Generates the declarations for functions to initialize and clear 
 * task structures.
 */
#define PFUNC_GEN_TASK_INITCLEAR_DECLS(sched) \
int pfunc_##sched##_task_init (pfunc_##sched##_task_t*); \
int pfunc_##sched##_task_clear (pfunc_##sched##_task_t*); 

/**
 * \def Generates all the declarations related to tasks by calling 
 * PFUNC_GEN_TASK_INITCLEAR_DECLS. Note that this is mostly vanity and has 
 * been included for consistency reasons.
 */
#define PFUNC_GEN_TASK_DECLS(sched) \
  PFUNC_GEN_TASK_INITCLEAR_DECLS(sched)

/** 
 * \def Generates the declarations for functions to initialize and clear 
 * taskmgr structures.
 */
#define PFUNC_GEN_TASKMGR_INITCLEAR_DECLS(sched) \
int pfunc_##sched##_taskmgr_init (pfunc_##sched##_taskmgr_t*, \
                                  const unsigned int, \
                                  const unsigned int*, \
                                  const unsigned int**); \
int pfunc_##sched##_taskmgr_clear (pfunc_##sched##_taskmgr_t*); 

/**
 * \def Generates all the declarations related to setting and getting the 
 * maximum number of attempts made by each thread before yielding and doing
 * the exponential backoff
 */
#define PFUNC_GEN_TASKMGR_SET_AND_GET_MAX_ATTEMPTS_DECLS(sched) \
int pfunc_##sched##_taskmgr_max_attempts_set (pfunc_##sched##_taskmgr_t, \
                                              const unsigned int); \
int pfunc_##sched##_taskmgr_max_attempts_get (pfunc_##sched##_taskmgr_t, \
                                              unsigned int*);

/**
 * \def Generates all the declarations related to taskmgr by calling 
 * PFUNC_GEN_TASKMGR_INITCLEAR_DECLS. Note that this is mostly vanity and has 
 * been included for consistency reasons.
 */
#define PFUNC_GEN_TASKMGR_DECLS(sched) \
  PFUNC_GEN_TASKMGR_INITCLEAR_DECLS(sched) \
  PFUNC_GEN_TASKMGR_SET_AND_GET_MAX_ATTEMPTS_DECLS(sched)

/** 
 * \def The C users need an option of reusing the work func structures. For
 * this, we provide a means of creating an abstract container through which
 * they can populate their work.  The good part about doing it this way is that
 * the users get to reuse the memory 
 */
#define PFUNC_GEN_WORK_TYPES_DECLS(sched) \
struct pfunc_##sched##_dummy_work_t; \
typedef struct pfunc_##sched##_dummy_work_t* pfunc_##sched##_work_t; \
int pfunc_##sched##_work_init (pfunc_##sched##_work_t*); \
int pfunc_##sched##_work_clear (pfunc_##sched##_work_t*); \
int pfunc_##sched##_work_func_set (pfunc_##sched##_work_t, pfunc_c_work_func_t); \
int pfunc_##sched##_work_arg_set (pfunc_##sched##_work_t, void*); \
int pfunc_##sched##_work_func_get (pfunc_##sched##_work_t, pfunc_c_work_func_t*); \
int pfunc_##sched##_work_arg_get (pfunc_##sched##_work_t, void**); \

/**
 * \def Generates declarations related to test and wait for each of the 
 * PFunc library instance descriptions (cilk, fifo, lifo, prio).
 */
#define PFUNC_GEN_TESTS_AND_WAITS_DECLS(sched) \
int pfunc_##sched##_wait (pfunc_##sched##_taskmgr_t, pfunc_##sched##_task_t); \
int pfunc_##sched##_wait_all (pfunc_##sched##_taskmgr_t, pfunc_##sched##_task_t*, int); \
int pfunc_##sched##_wait_any (pfunc_##sched##_taskmgr_t, pfunc_##sched##_task_t*, int, int*); \
int pfunc_##sched##_test (pfunc_##sched##_taskmgr_t, pfunc_##sched##_task_t); \
int pfunc_##sched##_test_all (pfunc_##sched##_taskmgr_t, pfunc_##sched##_task_t*, int, int*); \

/**
 * \def Generates declarations related to spawning task for each of the 
 * PFunc library instance descriptions (cilk, fifo, lifo, prio). Note that 
 * C users can either directly spawn a C function as a task or create a 
 * C++ work function abstractly and then reuse that. Wrapping the C work 
 * function is highly recommended.
 */
#define PFUNC_GEN_RUN_DECLS(sched) \
int pfunc_##sched##_spawn_c (pfunc_##sched##_taskmgr_t, \
                             pfunc_##sched##_task_t, \
                             pfunc_##sched##_attr_t, \
                             pfunc_##sched##_group_t, \
                             pfunc_c_work_func_t, \
                             void*); \
int pfunc_##sched##_spawn_cxx (pfunc_##sched##_taskmgr_t, \
                               pfunc_##sched##_task_t, \
                               pfunc_##sched##_attr_t, \
                               pfunc_##sched##_group_t, \
                               pfunc_##sched##_work_t); 

/** 
 * \def Generates the declarations for getting and setting the thread_id,
 * group_rank, group_size and finally for issuing a barrier from within 
 * a task. Also adding getting and setting the maximum number of attempts
 * to be made at retrieving a task before backing off.
 */
#define PFUNC_GEN_RANK_AND_SIZE_DECLS(sched) \
int pfunc_##sched##_thread_id (pfunc_##sched##_taskmgr_t, unsigned int*); \
int pfunc_##sched##_group_rank (pfunc_##sched##_taskmgr_t, unsigned int*); \
int pfunc_##sched##_group_size (pfunc_##sched##_taskmgr_t, unsigned int*); \
int pfunc_##sched##_barrier (pfunc_##sched##_taskmgr_t); 


/***************************************************************************
 * Adding the generators for declarations of global versions of the following
 * functions: init, clear, thread_id, group_rank, group_size, barrier, wait,
 * wait_all, wait_any, test, test_all, spawn_c and spawn_cxx. These take in an 
 * implicit argument (taskmgr) that has to have been initialized before hand.
 * Note: The functions are similar to their local counterparts, but differ in
 * two respects.
 * 1. "taskmgr" argument is missing.
 * 2. Name of the function now includes the suffix "_gbl" -- this is required
 * as there is no function overloading in C.
 */

/**
 * \def Generate the declarations for the init and clear of the taskmgrs.
 */
#define PFUNC_GEN_GLOBAL_TASKMGR_INIT_CLEAR(sched) \
int pfunc_##sched##_init (pfunc_##sched##_taskmgr_t*); \
int pfunc_##sched##_clear ();

/**
 * \def Generates all the global declarations related to setting and getting
 * the maximum number of attempts made by each thread before yielding and doing
 * the exponential backoff.
 */
#define PFUNC_GEN_TASKMGR_GLOBAL_SET_AND_GET_MAX_ATTEMPTS_DECLS(sched) \
int pfunc_##sched##_taskmgr_max_attempts_set_gbl (const unsigned int); \
int pfunc_##sched##_taskmgr_max_attempts_get_gbl (unsigned int*);

/**
 * \def For vanity
 */
#define PFUNC_GEN_GLOBAL_TASKMGR_DECLS(sched) \
        PFUNC_GEN_GLOBAL_TASKMGR_INIT_CLEAR(sched)  \
        PFUNC_GEN_TASKMGR_GLOBAL_SET_AND_GET_MAX_ATTEMPTS_DECLS(sched)

/**
 * \def Generates declarations related to the global versions of test and wait
 * for each of the PFunc library instance descriptions (cilk, fifo, lifo,
 * prio).
 */
#define PFUNC_GEN_GLOBAL_TESTS_AND_WAITS_DECLS(sched) \
int pfunc_##sched##_wait_gbl (pfunc_##sched##_task_t); \
int pfunc_##sched##_wait_all_gbl (pfunc_##sched##_task_t*, int); \
int pfunc_##sched##_wait_any_gbl (pfunc_##sched##_task_t*, int, int*); \
int pfunc_##sched##_test_gbl (pfunc_##sched##_task_t); \
int pfunc_##sched##_test_all_gbl (pfunc_##sched##_task_t*, int, int*); \

/**
 * \def Generates declarations related to the global versions of spawning task
 * for each of the PFunc library instance descriptions (cilk, fifo, lifo,
 * prio). Note that C users can either directly spawn a C function as a task or
 * create a C++ work function abstractly and then reuse that. Wrapping the C
 * work function is highly recommended.
 */
#define PFUNC_GEN_GLOBAL_RUN_DECLS(sched) \
int pfunc_##sched##_spawn_c_gbl (pfunc_##sched##_task_t, \
                                 pfunc_##sched##_attr_t, \
                                 pfunc_##sched##_group_t, \
                                 pfunc_c_work_func_t, \
                                 void*); \
int pfunc_##sched##_spawn_cxx_gbl (pfunc_##sched##_task_t, \
                                   pfunc_##sched##_attr_t, \
                                   pfunc_##sched##_group_t, \
                                   pfunc_##sched##_work_t); 

/** 
 * \def Generates the declarations for the global versions of the functions for
 * getting and setting the thread_id, group_rank, group_size and finally for
 * issuing a barrier from within a task.
 */
#define PFUNC_GEN_GLOBAL_RANK_AND_SIZE_DECLS(sched) \
int pfunc_##sched##_thread_id_gbl (unsigned int*); \
int pfunc_##sched##_group_rank_gbl (unsigned int*); \
int pfunc_##sched##_group_size_gbl (unsigned int*); \
int pfunc_##sched##_barrier_gbl (); 

/***************************************************************************/

/** 
 * \def This is an umbrella macro that can be used to define all the required
 * functions for one particular type. To add C support for a new type of
 * schedule, all that is needed is to define this macro and we are all set.
 */
#define PFUNC_GEN_ALL_TYPES_DECLS(sched) \
  PFUNC_GEN_C_TYPES(sched) \
  PFUNC_GEN_ATTR_DECLS(sched) \
  PFUNC_GEN_GROUP_DECLS(sched) \
  PFUNC_GEN_TASK_DECLS(sched) \
  PFUNC_GEN_TASKMGR_DECLS(sched) \
  PFUNC_GEN_WORK_TYPES_DECLS(sched) \
  PFUNC_GEN_TESTS_AND_WAITS_DECLS(sched) \
  PFUNC_GEN_RUN_DECLS(sched) \
  PFUNC_GEN_RANK_AND_SIZE_DECLS(sched) \
  PFUNC_GEN_GLOBAL_TASKMGR_DECLS(sched) \
  PFUNC_GEN_GLOBAL_TESTS_AND_WAITS_DECLS(sched) \
  PFUNC_GEN_GLOBAL_RUN_DECLS(sched) \
  PFUNC_GEN_GLOBAL_RANK_AND_SIZE_DECLS(sched)

/** Generate all declarations for cilk */
PFUNC_GEN_ALL_TYPES_DECLS(cilk)

/** Generate all declarations for fifo */
PFUNC_GEN_ALL_TYPES_DECLS(fifo)

/** Generate all declarations for lifo */
PFUNC_GEN_ALL_TYPES_DECLS(lifo)

/** Generate all declarations for prio */
PFUNC_GEN_ALL_TYPES_DECLS(prio)

/** Declare the global cilk task manager handle */
extern pfunc_cilk_taskmgr_t pfunc_cilk_global_tmanager;

/** Declare the global fifo task manager handle */
extern pfunc_fifo_taskmgr_t pfunc_fifo_global_tmanager;

/** Declare the global lifo task manager handle */
extern pfunc_lifo_taskmgr_t pfunc_lifo_global_tmanager;

/** Declare the global prio task manager handle */
extern pfunc_prio_taskmgr_t pfunc_prio_global_tmanager;

#ifdef __cplusplus
}
#endif

#endif /** PFUNC_PACK_CODE */

#ifdef __cplusplus
extern "C" {
#endif

/** Utility functions to pack and unpack the arguements into a single buffer. */
int pfunc_pack (char**, const char* format, ...);
int pfunc_unpack (char*, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif /* PFUNC_H */
