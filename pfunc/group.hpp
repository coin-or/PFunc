#ifndef PFUNC_GROUP_HPP
#define PFUNC_GROUP_HPP

  
/**
 * \file group.hpp
 * \brief Implementation of groups for PFUNC -- A Task Parallel API
 * \author Prabhanjan Kambadur
 */
#include <pfunc/pfunc_common.h>
#include <pfunc/exception.hpp>
#include <pfunc/mutex.hpp>
#include <pfunc/pfunc_atomics.h>

namespace pfunc { namespace detail {

/**
 * \brief Implements a group structure across which barriers can be executed.
 */
struct group {
  private:
  ALIGN128 volatile bool barrier_phase; /**< Toggle of phases */
  ALIGN128 volatile unsigned int barrier_count; /**< # tasks active in barrier*/
  volatile unsigned int rank_token; /**< Gives out ranks to tasks */
  unsigned int group_id; /**< For debugging purposes */
  unsigned int group_size; /**< Number of tasks in this group */
  mutex group_lock; /**< Lock for implementing the barrier */
  unsigned int type_of_barrier; /**< Type of the barrier to be used */
  PFUNC_DEFINE_EXCEPT_PTR() /**< The exception holder */

  public:
  /** 
   * Implements the spinning barrier.
   */
  void barrier_spin ()  {
    PFUNC_START_TRY_BLOCK()
    while (!group_lock.trylock()); /* spin until lock acquire */
    volatile bool my_phase = barrier_phase;
    ++barrier_count;
    if (barrier_count == group_size) {
      barrier_count = 0;
      barrier_phase = !barrier_phase;
      group_lock.unlock();
    } else {
      group_lock.unlock();
      while (my_phase == barrier_phase); /* spin until  different phase */
    }
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(group,barrier_spin)
  }


  /**
   * \brief Implements the work-stealing barrier.
   *
   * \param [in,out] taskmgr The instance of the library that we steal work from.
   */
  template <typename TaskManager>
  void barrier_steal (TaskManager& taskmgr) {
    PFUNC_START_TRY_BLOCK()
    group_lock.lock();
    volatile bool my_phase = barrier_phase;
    ++barrier_count;
    if (barrier_count == group_size) {
      barrier_count = 0;
      barrier_phase = !barrier_phase;
      group_lock.unlock();
    } else {
      group_lock.unlock();
      while (my_phase == barrier_phase) taskmgr.progress_barrier ();
    }
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(group,barrier_steal)
  }

  /**
   * \return Join this group and return a new rank.
   */
  unsigned int join_group ()  {
    return pfunc_fetch_and_add_32 
              (reinterpret_cast<volatile int32_t*>(&rank_token), 1);
  }

  /**
   * Decrements the rank_token by one.
   */
  void leave_group (void)  {
    pfunc_fetch_and_add_32 
      (reinterpret_cast<volatile int32_t*>(&rank_token), -1);
  }

  /**
   * \return Id of the group
   */
  unsigned int get_id () const  { return group_id; }

  /**
   * get_group_size
   * \return Number of tasks in the group
   */
  unsigned int get_size ()  const  { return group_size; }

  /**
   * \return Type of the barrier 
   */
  unsigned int get_barrier() const  { return type_of_barrier; }

  /**
   * \param gid Id of the group
   */
  void set_id (const unsigned int& gid)  { group_id = gid; }

  /**
   * \param gsize Number of tasks in the group
   */
  void set_size (const unsigned int& gsize)  { group_size = gsize; }

  /**
   * \param barr Type of the barrier 
   */
  void set_barrier (const unsigned int& barr) {type_of_barrier = barr;}

  /**
   * \param [in,out] taskmgr The instance that does the steal.
   */
  template <typename TaskManager>
  void barrier (TaskManager& taskmgr)  {
    PFUNC_START_TRY_BLOCK()
    if (group_size > 1) {
      if (BARRIER_SPIN == type_of_barrier) return barrier_spin();
      else if (BARRIER_STEAL == type_of_barrier) return barrier_steal(taskmgr);
    } else {
      /* If the group count is 0 or 1, no use with the barrier */
    }
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(group,barrier)
  }

  /**
   * Default constructor
   */
  group ()  : barrier_phase (false),
              barrier_count (0),
              rank_token (0),
              group_id (0), 
              group_size (0), 
              type_of_barrier (BARRIER_SPIN)
              PFUNC_EXCEPT_PTR_INIT() {}

  /**
   * Parameterized Constructor
   * 
   * \param [in] group_id ID of the group.
   * \param [in] group_size Size of the group.
   */
  group (const unsigned int& group_id,
         const unsigned int& group_size) : barrier_phase (false),
                                           barrier_count (0),
                                           rank_token (0),
                                           group_id (group_id), 
                                           group_size (group_size),
                                           type_of_barrier (BARRIER_SPIN)
                                           PFUNC_EXCEPT_PTR_INIT() {}

  /**
   * Parameterized Constructor
   * 
   * \param [in] group_id ID of the group.
   * \param [in] group_size Size of the group.
   * \param [in] barrier Type of the barrier.
   */
  group (const unsigned int& group_id,
         const unsigned int& group_size,
         const unsigned int& barrier) : barrier_phase (false),
                                        barrier_count (0),
                                        rank_token (0),
                                        group_id (group_id), 
                                        group_size (group_size),
                                        type_of_barrier (barrier)
                                        PFUNC_EXCEPT_PTR_INIT() {}

  /**
   * Destructor
   */
  ~group ()  { PFUNC_EXCEPT_PTR_CLEAR() }


  /**
   * \param [in] one The group to be compared
   * \param [in] two The other group to be compared
   * \return true If the groups are same
   * \return false otherwise
   */
  friend bool operator== (const group& one, const group& two)  {
    return (one.group_size == two.group_size && one.group_id == two.group_id);
  }
};

} /* namespace detail */ } /* namespace pfunc */
#endif // PFUNC_GROUP_HPP
