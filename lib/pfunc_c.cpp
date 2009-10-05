#define PFUNC_LIBRARY_CODE /** enable taskmgrrary definitions */
#include <pfunc/config.h>
#include <pfunc/pfunc.h>
#include <pfunc/exception.hpp>


#define PFUNC_GEN_ATTR_INITCLEAR_DEFS(sched) \
int pfunc_##sched##_attr_init (pfunc_##sched##_attr_t* attr) { \
  PFUNC_START_TRY_BLOCK() \
  *attr = reinterpret_cast<pfunc_##sched##_attr_t> \
            (new (std::nothrow) pfunc::detail::pfunc_##sched##_attr_t); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_attr_clear (pfunc_##sched##_attr_t* attr) { \
  PFUNC_START_TRY_BLOCK() \
  delete reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*> (*attr); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} 

#define PFUNC_GEN_ATTR_SET_DEFS(sched) \
int pfunc_##sched##_attr_priority_set \
    (pfunc_##sched##_attr_t attr, pfunc_attr_priority_t priority) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  cpp_attr.set_priority (priority); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_attr_qnum_set \
    (pfunc_##sched##_attr_t attr, pfunc_attr_qnum_t queue_num) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  cpp_attr.set_queue_number (queue_num); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
}\
\
int pfunc_##sched##_attr_num_waiters_set \
    (pfunc_##sched##_attr_t attr, pfunc_attr_num_waiters_t num_waiters) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  cpp_attr.set_num_waiters (num_waiters); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_attr_nested_set \
    (pfunc_##sched##_attr_t attr, pfunc_attr_nested_t nested) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  cpp_attr.set_nested (nested); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_attr_grouped_set \
    (pfunc_##sched##_attr_t attr, pfunc_attr_grouped_t grouped) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  cpp_attr.set_grouped (grouped); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_attr_level_set \
    (pfunc_##sched##_attr_t attr, pfunc_attr_level_t level) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  cpp_attr.set_level (level); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\

#define PFUNC_GEN_ATTR_GET_DEFS(sched) \
int pfunc_##sched##_attr_priority_get \
    (pfunc_##sched##_attr_t attr, pfunc_attr_priority_t* priority) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  *priority = cpp_attr.get_priority (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_attr_qnum_get \
    (pfunc_##sched##_attr_t attr, pfunc_attr_qnum_t* queue_num) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  *queue_num = cpp_attr.get_queue_number (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
}\
\
int pfunc_##sched##_attr_num_waiters_get \
    (pfunc_##sched##_attr_t attr, pfunc_attr_num_waiters_t* num_waiters) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  *num_waiters = cpp_attr.get_num_waiters (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_attr_nested_get \
    (pfunc_##sched##_attr_t attr, pfunc_attr_nested_t* nested) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  *nested = cpp_attr.get_nested (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_attr_grouped_get \
    (pfunc_##sched##_attr_t attr, pfunc_attr_grouped_t* grouped) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  *grouped = cpp_attr.get_grouped (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_attr_level_get \
    (pfunc_##sched##_attr_t attr, pfunc_attr_level_t* level) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  *level = cpp_attr.get_level (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\

#define PFUNC_GEN_ATTR_DEFS(sched) \
  PFUNC_GEN_ATTR_INITCLEAR_DEFS(sched)\
  PFUNC_GEN_ATTR_SET_DEFS(sched) \
  PFUNC_GEN_ATTR_GET_DEFS(sched)

#define PFUNC_GEN_GROUP_INITCLEAR_DEFS(sched) \
int pfunc_##sched##_group_init (pfunc_##sched##_group_t* group) { \
  PFUNC_START_TRY_BLOCK() \
  *group = reinterpret_cast<pfunc_##sched##_group_t> \
            (new (std::nothrow) pfunc::detail::pfunc_##sched##_group_t); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_group_clear (pfunc_##sched##_group_t* group) { \
  PFUNC_START_TRY_BLOCK() \
  delete reinterpret_cast<pfunc::detail::pfunc_##sched##_group_t*> (*group); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} 

#define PFUNC_GEN_GROUP_SET_DEFS(sched) \
int pfunc_##sched##_group_size_set \
    (pfunc_##sched##_group_t group, pfunc_group_size_t size) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_group_t& cpp_group = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_group_t*>(group)); \
  cpp_group.set_size (size); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_group_id_set \
    (pfunc_##sched##_group_t group, pfunc_group_id_t id) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_group_t& cpp_group = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_group_t*>(group)); \
  cpp_group.set_id (id); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_group_barrier_set \
    (pfunc_##sched##_group_t group, pfunc_group_barrier_t barrier) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_group_t& cpp_group = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_group_t*>(group)); \
  cpp_group.set_barrier (barrier); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \

#define PFUNC_GEN_GROUP_GET_DEFS(sched) \
int pfunc_##sched##_group_size_get \
    (pfunc_##sched##_group_t group, pfunc_group_size_t* size) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_group_t& cpp_group = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_group_t*>(group)); \
  *size = cpp_group.get_size (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_group_id_get \
    (pfunc_##sched##_group_t group, pfunc_group_id_t* id) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_group_t& cpp_group = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_group_t*>(group)); \
  *id = cpp_group.get_id (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_group_barrier_get \
    (pfunc_##sched##_group_t group, pfunc_group_barrier_t* barrier) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_group_t& cpp_group = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_group_t*>(group)); \
  *barrier = static_cast <pfunc_group_barrier_t> (cpp_group.get_barrier ()); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \

#define PFUNC_GEN_GROUP_DEFS(sched) \
  PFUNC_GEN_GROUP_INITCLEAR_DEFS(sched)\
  PFUNC_GEN_GROUP_SET_DEFS(sched) \
  PFUNC_GEN_GROUP_GET_DEFS(sched)

#define PFUNC_GEN_TASK_INITCLEAR_DEFS(sched) \
int pfunc_##sched##_task_init (pfunc_##sched##_task_t* task) { \
  PFUNC_START_TRY_BLOCK() \
  *task = reinterpret_cast<pfunc_##sched##_task_t> \
    (new (std::nothrow) pfunc::detail::pfunc_##sched##_task_t ()); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_task_clear (pfunc_##sched##_task_t* task) { \
  PFUNC_START_TRY_BLOCK() \
  delete reinterpret_cast<pfunc::detail::pfunc_##sched##_task_t*> (*task); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} 

#define PFUNC_GEN_TASK_DEFS(sched) \
  PFUNC_GEN_TASK_INITCLEAR_DEFS(sched)

#define PFUNC_GEN_TASKMGR_INITCLEAR_DEFS(sched) \
int pfunc_##sched##_taskmgr_init (pfunc_##sched##_taskmgr_t* taskmgr, \
                              const unsigned int num_queues, \
                              const unsigned int* num_threads_per_queue, \
                              const unsigned int** thread_affinities) { \
  PFUNC_START_TRY_BLOCK() \
  *taskmgr = reinterpret_cast<pfunc_##sched##_taskmgr_t> \
    (new (std::nothrow) pfunc::detail::pfunc_##sched##_taskmgr_t (num_queues, \
                                                      num_threads_per_queue, \
                                                      thread_affinities)); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_taskmgr_clear (pfunc_##sched##_taskmgr_t* taskmgr) { \
  PFUNC_START_TRY_BLOCK() \
  delete reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*> (*taskmgr); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} 

/**
 * \def Generate the definitions for setting and getting the maximum number 
 * of attempts to get a task made before each thread employs exponential 
 * backoff.
 */
#define PFUNC_GEN_TASKMGR_SET_AND_GET_MAX_ATTEMPTS_DEFS(sched) \
int pfunc_##sched##_taskmgr_max_attempts_set \
                        (pfunc_##sched##_taskmgr_t taskmgr, \
                         const unsigned int max_attempts) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  cpp_taskmgr.set_max_attempts (max_attempts); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_taskmgr_max_attempts_get \
                        (pfunc_##sched##_taskmgr_t taskmgr, \
                         unsigned int* max_attempts) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  *max_attempts = cpp_taskmgr.get_max_attempts (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \

#define PFUNC_GEN_TASKMGR_DEFS(sched) \
  PFUNC_GEN_TASKMGR_INITCLEAR_DEFS(sched) \
  PFUNC_GEN_TASKMGR_SET_AND_GET_MAX_ATTEMPTS_DEFS(sched)

/** functions for waits and tests */
#define PFUNC_GEN_TESTS_AND_WAITS_DEFS(sched) \
int pfunc_##sched##_wait (pfunc_##sched##_taskmgr_t taskmgr, \
                          pfunc_##sched##_task_t task) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  pfunc::detail::pfunc_##sched##_task_t& cpp_task = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_task_t*>(task)); \
  cpp_task.wait (cpp_taskmgr); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_wait_all (pfunc_##sched##_taskmgr_t taskmgr, \
                              pfunc_##sched##_task_t* tasks, \
                              int count) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  for (int i=0; i<count; ++i) { \
    pfunc::detail::pfunc_##sched##_task_t& cpp_task = \
        *(reinterpret_cast<pfunc::detail::pfunc_##sched##_task_t*>(tasks[i])); \
    cpp_task.wait (cpp_taskmgr); \
  } \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_wait_any (pfunc_##sched##_taskmgr_t taskmgr, \
                              pfunc_##sched##_task_t* tasks, \
                              int count, \
                              int* completed) { \
  PFUNC_START_TRY_BLOCK() \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
\
} \
\
int pfunc_##sched##_test (pfunc_##sched##_taskmgr_t taskmgr, \
                          pfunc_##sched##_task_t task) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  pfunc::detail::pfunc_##sched##_task_t& cpp_task = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_task_t*>(task)); \
  return (cpp_task.test (cpp_taskmgr)) ? \
      PFUNC_TASK_COMPLETE: PFUNC_TASK_INCOMPLETE;\
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_test_all (pfunc_##sched##_taskmgr_t taskmgr, \
                              pfunc_##sched##_task_t* tasks, \
                              int count, \
                              int* completed) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  for (int i=0; i<count; ++i) { \
    pfunc::detail::pfunc_##sched##_task_t& cpp_task = \
        *(reinterpret_cast<pfunc::detail::pfunc_##sched##_task_t*>(tasks[i])); \
    completed[i] = cpp_task.test (cpp_taskmgr); \
  } \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} 

/** functions for spawn */
#define PFUNC_GEN_RUN_DEFS(sched) \
int pfunc_##sched##_spawn_c (pfunc_##sched##_taskmgr_t taskmgr, \
                             pfunc_##sched##_task_t task, \
                             pfunc_##sched##_attr_t attr, \
                             pfunc_##sched##_group_t group, \
                             pfunc_c_work_func_t work, \
                             void* arg) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  pfunc::detail::pfunc_##sched##_task_t& cpp_task = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_task_t*>(task)); \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  pfunc::detail::pfunc_##sched##_group_t& cpp_group = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_group_t*>(group)); \
  pfunc::detail::internal_work_func_t& cpp_work_func = \
      *(new (std::nothrow) pfunc::detail::internal_work_func_t (work, arg)); \
  cpp_taskmgr.spawn_task (cpp_task, cpp_attr, cpp_group, cpp_work_func); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_spawn_cxx (pfunc_##sched##_taskmgr_t taskmgr, \
                               pfunc_##sched##_task_t task, \
                               pfunc_##sched##_attr_t attr, \
                               pfunc_##sched##_group_t group, \
                               pfunc_##sched##_work_t work) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  pfunc::detail::pfunc_##sched##_task_t& cpp_task = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_task_t*>(task)); \
  pfunc::detail::pfunc_##sched##_attr_t& cpp_attr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_attr_t*>(attr)); \
  pfunc::detail::pfunc_##sched##_group_t& cpp_group = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_group_t*>(group)); \
  pfunc::detail::internal_work_func_t& cpp_work_func = \
      *(reinterpret_cast<pfunc::detail::internal_work_func_t*>(work)); \
  cpp_taskmgr.spawn_task (cpp_task, cpp_attr, cpp_group, cpp_work_func); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \

/** functions for getting rank and size */
#define PFUNC_GEN_RANK_AND_SIZE_DEFS(sched) \
int pfunc_##sched##_thread_id (pfunc_##sched##_taskmgr_t taskmgr, unsigned int* id) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  *id = cpp_taskmgr.current_thread_id (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_group_rank (pfunc_##sched##_taskmgr_t taskmgr, unsigned int* rank) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  const pfunc::detail::pfunc_##sched##_task_t& cpp_task = \
      *(cpp_taskmgr.current_task_information ()); \
  *rank = cpp_task.get_rank (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_group_size (pfunc_##sched##_taskmgr_t taskmgr, unsigned int* size) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  const pfunc::detail::pfunc_##sched##_task_t& cpp_task = \
      *(cpp_taskmgr.current_task_information ()); \
  *size = cpp_task.get_size (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_barrier (pfunc_##sched##_taskmgr_t taskmgr) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::pfunc_##sched##_taskmgr_t& cpp_taskmgr = \
      *(reinterpret_cast<pfunc::detail::pfunc_##sched##_taskmgr_t*>(taskmgr)); \
  pfunc::detail::pfunc_##sched##_task_t& cpp_task = \
      *(cpp_taskmgr.current_task_information ()); \
  cpp_task.barrier (cpp_taskmgr); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} 

#define PFUNC_GEN_WORK_TYPES_DEFS(sched) \
int pfunc_##sched##_work_init (pfunc_##sched##_work_t* work) { \
  PFUNC_START_TRY_BLOCK() \
  *work = reinterpret_cast<pfunc_##sched##_work_t> \
    (new (std::nothrow) pfunc::detail::internal_work_func_t ()); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_work_clear (pfunc_##sched##_work_t* work) { \
  PFUNC_START_TRY_BLOCK() \
  delete reinterpret_cast<pfunc::detail::internal_work_func_t*> (*work); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_work_func_set (pfunc_##sched##_work_t work, \
                                   pfunc_c_work_func_t func) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::internal_work_func_t& cpp_work = \
        *(reinterpret_cast<pfunc::detail::internal_work_func_t*>(work)); \
  cpp_work.set_func (func); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_work_arg_set (pfunc_##sched##_work_t work, \
                                   void* arg) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::internal_work_func_t& cpp_work = \
        *(reinterpret_cast<pfunc::detail::internal_work_func_t*>(work)); \
  cpp_work.set_arg (arg); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
\
int pfunc_##sched##_work_func_get (pfunc_##sched##_work_t work, \
                                   pfunc_c_work_func_t* func) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::internal_work_func_t& cpp_work = \
        *(reinterpret_cast<pfunc::detail::internal_work_func_t*>(work)); \
  *func = cpp_work.get_func (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} \
int pfunc_##sched##_work_arg_get (pfunc_##sched##_work_t work, \
                                  void** arg) { \
  PFUNC_START_TRY_BLOCK() \
  pfunc::detail::internal_work_func_t& cpp_work = \
        *(reinterpret_cast<pfunc::detail::internal_work_func_t*>(work)); \
  *arg = cpp_work.get_arg (); \
  return PFUNC_SUCCESS; \
  PFUNC_END_TRY_BLOCK() \
  PFUNC_C_CATCH_AND_RETURN_EXCEPTION_CODE() \
} 

/**************************************************************************
 * Global versions of the following functions are here:
 * 1. init
 * 2. clear
 * 3. wait
 * 4. wait_all
 * 5. wait_any
 * 6. test
 * 7. test_all
 * 9. thread_id
 * 10. group_rank
 * 11. group_size
 * 12. barrier
 *
 * Basically, the implementation of these functions will turn around and call
 * the local counterparts with the global taskmgr objects. For purposes of 
 * performance, we have choosen to not check if the library has been 
 * initialized. That is completely upto the users. We may add more debugging 
 * support later.
 **************************************************************************/

/** definition of the cilk global taskmgr object */
pfunc_cilk_taskmgr_t pfunc_cilk_global_tmanager = NULL;

/** definition of the fifo global taskmgr object */
pfunc_fifo_taskmgr_t pfunc_fifo_global_tmanager = NULL;

/** definition of the lifo global taskmgr object */
pfunc_lifo_taskmgr_t pfunc_lifo_global_tmanager = NULL;

/** definition of the prio global taskmgr object */
pfunc_prio_taskmgr_t pfunc_prio_global_tmanager = NULL;

/**
 * \def Generate the definitions of the functions that initialize and clear 
 * the global taskmgr objects.
 */
#define PFUNC_GEN_GLOBAL_TASKMGR_INIT_CLEAR_DEFS(sched) \
int pfunc_##sched##_init (pfunc_##sched##_taskmgr_t* local_tmanager) { \
  int return_value;  \
  if (NULL == local_tmanager || NULL == *local_tmanager) { \
    return_value = PFUNC_INVALID_ARGUMENTS; \
  } else { \
    pfunc_##sched##_global_tmanager = *local_tmanager; \
    return_value = PFUNC_SUCCESS; \
  } \
  return return_value; \
} \
\
int pfunc_##sched##_clear () { \
  int return_value; \
  if (NULL == pfunc_##sched##_global_tmanager) { \
    return_value = PFUNC_INVALID_ARGUMENTS; \
  } else { \
    pfunc_##sched##_global_tmanager = NULL; \
    return_value = PFUNC_SUCCESS; \
  } \
  return return_value; \
}

/**
 * \def Generate the definitions for setting and getting the maximum number 
 * of attempts to get a task made before each thread employs exponential 
 * backoff.
 */
#define PFUNC_GEN_TASKMGR_GLOBAL_SET_AND_GET_MAX_ATTEMPTS_DEFS(sched) \
int pfunc_##sched##_taskmgr_max_attempts_set_gbl \
                         (const unsigned int max_attempts) { \
  return pfunc_##sched##_taskmgr_max_attempts_set \
                                          (pfunc_##sched##_global_tmanager, \
                                           max_attempts); \
} \
\
int pfunc_##sched##_taskmgr_max_attempts_get_gbl \
                         (unsigned int* max_attempts) { \
  return pfunc_##sched##_taskmgr_max_attempts_get \
                                          (pfunc_##sched##_global_tmanager, \
                                           max_attempts); \
} \

/**
 * \def For purposes of vanity only
 */
#define PFUNC_GEN_GLOBAL_TASKMGR_DEFS(sched) \
  PFUNC_GEN_GLOBAL_TASKMGR_INIT_CLEAR_DEFS(sched) \
  PFUNC_GEN_TASKMGR_GLOBAL_SET_AND_GET_MAX_ATTEMPTS_DEFS(sched)

/** 
 * \def Generate definitions for global versions of waits and tests 
 */
#define PFUNC_GEN_GLOBAL_TESTS_AND_WAITS_DEFS(sched) \
int pfunc_##sched##_wait_gbl (pfunc_##sched##_task_t task) { \
  return pfunc_##sched##_wait (pfunc_##sched##_global_tmanager, task); \
} \
\
int pfunc_##sched##_wait_all_gbl (pfunc_##sched##_task_t* tasks, \
                                  int count) { \
  return pfunc_##sched##_wait_all (pfunc_##sched##_global_tmanager, \
                                   tasks, \
                                   count); \
} \
\
int pfunc_##sched##_wait_any_gbl (pfunc_##sched##_task_t* tasks, \
                                  int count, \
                                  int* completed) { \
  return pfunc_##sched##_wait_any (pfunc_##sched##_global_tmanager, \
                                   tasks, \
                                   count, \
                                   completed); \
} \
\
int pfunc_##sched##_test_gbl (pfunc_##sched##_task_t task) { \
  return pfunc_##sched##_test (pfunc_##sched##_global_tmanager, task); \
} \
\
int pfunc_##sched##_test_all_gbl (pfunc_##sched##_task_t* tasks, \
                                  int count, \
                                  int* completed) { \
  return pfunc_##sched##_test_all (pfunc_##sched##_global_tmanager, \
                                   tasks, \
                                   count, \
                                   completed); \
} 

/**
 * \def Generate the definitions for the global versions of spawn_c and spawn_cxx
 */
#define PFUNC_GEN_GLOBAL_RUN_DEFS(sched) \
int pfunc_##sched##_spawn_c_gbl (pfunc_##sched##_task_t task, \
                                 pfunc_##sched##_attr_t attr, \
                                 pfunc_##sched##_group_t group, \
                                 pfunc_c_work_func_t work, \
                                 void* arg) { \
  return pfunc_##sched##_spawn_c (pfunc_##sched##_global_tmanager, \
                                task, \
                                attr, \
                                group, \
                                work, \
                                arg); \
} \
\
int pfunc_##sched##_spawn_cxx_gbl (pfunc_##sched##_task_t task, \
                                   pfunc_##sched##_attr_t attr, \
                                   pfunc_##sched##_group_t group, \
                                   pfunc_##sched##_work_t work) { \
  return pfunc_##sched##_spawn_cxx (pfunc_##sched##_global_tmanager, \
                                    task, \
                                    attr, \
                                    group, \
                                    work); \
} \

/** 
 * \def Generate definitions for global versions of functions used for 
 * thread_id, rank, size and barrier
 */
#define PFUNC_GEN_GLOBAL_RANK_AND_SIZE_DEFS(sched) \
int pfunc_##sched##_thread_id_gbl (unsigned int* id) { \
  return pfunc_##sched##_thread_id (pfunc_##sched##_global_tmanager, id); \
} \
\
int pfunc_##sched##_group_rank_gbl (unsigned int* rank) { \
  return pfunc_##sched##_group_rank (pfunc_##sched##_global_tmanager, rank); \
} \
\
int pfunc_##sched##_group_size_gbl (unsigned int* size) { \
  return pfunc_##sched##_group_size (pfunc_##sched##_global_tmanager, size); \
} \
\
int pfunc_##sched##_barrier_gbl ( ) { \
  return pfunc_##sched##_barrier (pfunc_##sched##_global_tmanager); \
} 

/***************************************************************************/

/**
 * \def Generate all the type declarations for the C interface.
 */
#define PFUNC_GEN_ALL_TYPES_DEFS(sched) \
  PFUNC_GEN_ATTR_DEFS(sched) \
  PFUNC_GEN_GROUP_DEFS(sched) \
  PFUNC_GEN_TASK_DEFS(sched) \
  PFUNC_GEN_TASKMGR_DEFS(sched) \
  PFUNC_GEN_WORK_TYPES_DEFS(sched) \
  PFUNC_GEN_TESTS_AND_WAITS_DEFS(sched) \
  PFUNC_GEN_RUN_DEFS(sched) \
  PFUNC_GEN_RANK_AND_SIZE_DEFS(sched) \
  PFUNC_GEN_GLOBAL_TASKMGR_DEFS(sched) \
  PFUNC_GEN_GLOBAL_TESTS_AND_WAITS_DEFS(sched) \
  PFUNC_GEN_GLOBAL_RUN_DEFS(sched) \
  PFUNC_GEN_GLOBAL_RANK_AND_SIZE_DEFS(sched)

/** Generate the type decls for cilk library instance description */
PFUNC_GEN_ALL_TYPES_DEFS(cilk)

/** Generate the type decls for fifo library instance description */
PFUNC_GEN_ALL_TYPES_DEFS(fifo)

/** Generate the type decls for lifo library instance description */
PFUNC_GEN_ALL_TYPES_DEFS(lifo)

/** Generate the type decls for prio library instance description */
PFUNC_GEN_ALL_TYPES_DEFS(prio)
