#ifndef PFUNC_ATTRIBUTE_HPP 
#define PFUNC_ATTRIBUTE_HPP

/**
 * \file attribute.hpp
 * \brief Implementation of PFUNC -- A Task Parallel API
 * \author Prabhanjan Kambadur
 */
#include <limits>
#include <pfunc/trampolines.hpp>

namespace pfunc { namespace detail {

/**
 * Constant that specifies that the task is to be put on current thread.
 */
const static unsigned int QUEUE_CURRENT_THREAD=0xFFFF;

/**
 * Default level of a spawned task -- set it to minimum so that it can 
 * steal any task it wants when in progress_wait ().
 */
const static unsigned int PFUNC_DEFAULT_TASK_LEVEL = 0;
 
/**
 * \brief An implementation of the ``attributes'' that a task can have.
 * 
 * \param Priority The type of the priority, defaults to int.
 * \param Compare The comparison function that orders on Priority.
 * \param Alloc The allocator that needs to be used to allocate attribute.
 *
 * \details
 * Each ``task'' that is added has certain attributes. These include the
 * queue_number, priority of the task, etc. This structure implements the
 * functionality described above. Note that a single attribute can be reused
 * with multiple tasks if they are all going to have the same attributes.
 */
template <typename Priority,
          typename Compare>
struct attribute {
  public:
  typedef Compare compare_type; /**< Type of the ordering function */
  typedef Priority priority_type; /**< Type of the priority */
  typedef unsigned int qnum_type; /**< Type of the qnumber */
  typedef unsigned int num_waiters_type; /**< Type of the num waiters */
  typedef bool nested_type; /**< Type of is_nested */
  typedef bool grouped_type; /**< Type of join_group */
  typedef unsigned int level_type; /**< Type of the level variable */

  private:
  qnum_type queue_number; /**< The queue that this task should be put into */
  priority_type priority; /**< Priority of this task */
  num_waiters_type num_waiters; /**< Number of parents of this task */
  nested_type is_nested; /**< Is the task nested */
  grouped_type join_group; /**< Should we join the group or not */
  level_type level; /**< Denotes the level of the task in the spawn tree */

  public:
  /**
   * \return Queue number to which this task belongs 
   */
  const qnum_type& get_queue_number () const  { return queue_number; }

  /**
   * \return Priority of the task
   */
  const priority_type& get_priority () const  { return priority; }

  /**
   * \return Number of waiters receiving completion notices
   */
  const num_waiters_type& get_num_waiters () const  { 
    return num_waiters; 
  }

  /**
   * \return True if the task is a nested task
   * \return False otherwise
   */
  const nested_type& get_nested () const  { return is_nested; } 

  /**
   * \return True is the task should join the group
   * \return False otherwise
   */
  const grouped_type get_grouped () const  { return join_group; }

  /**
   * \return Level of the current task in the spawn tree.
   */
  const level_type& get_level () const  { return level; }

  /**
   * \param qnum Queue number that this particular task should be put
   * into
   */
  void set_queue_number (const qnum_type& qnum)  {queue_number = qnum;}

  /**
   * \param prio Priority of this task.
   */
  void set_priority (const priority_type& prio)  { priority = prio; }

  /**
   * \param nwait Number of waiters receiving completion notices.
   */
  void set_num_waiters (const num_waiters_type& nwait)  { 
    num_waiters = nwait; 
  }

  /**
   * \param nest Nest (unnest) the task.
   */
  void set_nested (const nested_type& nest)  { is_nested = nest; } 

  /**
   * \param grouped Group the task.
   */
  void set_grouped (const grouped_type& grouped)  { 
    join_group = grouped; 
  } 

  /**
   * \param nwait Number of waiters receiving completion notices.
   */
  void set_level (const level_type& new_level)  { level = new_level; }

  /**
   * Constructor
   */
  attribute (const nested_type& is_nested = true,
             const grouped_type& join_group = false) :
                  queue_number (QUEUE_CURRENT_THREAD),
                  priority ((std::numeric_limits<Priority>::min)()),
                  num_waiters (1),
                  is_nested (is_nested),
                  join_group (join_group),
                  level (PFUNC_DEFAULT_TASK_LEVEL) {}

  /**
   * operator<
   */
  friend bool operator<(const attribute& one, const attribute& two)  {
    compare_type comp;
    return comp (one.priority, two.priority);
  }
}; /* struct attribute */

} /* detail */ } /* namespace pfunc */

#endif // PFUNC_ATTRIBUTE_HPP
