#ifndef PFUNC_TASK_HPP
#define PFUNC_TASK_HPP

/**
 * \file task.hpp
 * \brief Implementation of task For PFUNC -- A Task Parallel API
 * \author Prabhanjan Kambadur
 *
 */
#include <cstdlib>

#include <pfunc/no_copy.hpp>
#include <pfunc/exception.hpp>
#include <pfunc/event.hpp>
#include <pfunc/trampolines.hpp>
#include <pfunc/attribute.hpp>
#include <pfunc/group.hpp>

namespace pfunc { namespace detail {

/**
 * \brief An implementation of a task structure.
 *
 * \param Attribute The type of the Attribute. 
 * \param Functor The type of the function object to be used.
 *
 * Implements the task structure. Main intention is to enable 
 * communication of not only the task that a thread has to perform
 * but also to provide the executing thread with attributes that are
 * related to a particular task. 
 */
template <typename Attribute,
          typename Functor>
struct task : public no_copy {
  typedef Attribute attribute; /* Type of the attribute. */
  typedef Functor functor; /* Type of the functor. */

  private:
  attribute attr; /**< Attribute that describes the task */
  group* grp; /**< The group for the task */
  unsigned int gsize; /**< Size of the group */
  unsigned int grank; /**< Rank of the task in the group */
  functor* func; /**< Function object that represents the task */
  event<testable_event> testing_compl; /**< testable event */
  event<waitable_event> waiting_compl; /**< waitable event */
  PFUNC_DEFINE_EXCEPT_PTR()

  public:
  /**
   * \return Attribute that governs the execution of the function.
   */
  const attribute& get_attr() const  { return attr;}

  /**
   * \return Group that the task is associated with.
   */
  group* get_group() const  { return grp;}

  /**
   * \return Size of the group associated with the task.
   */
  unsigned int get_size () const  { return gsize; }

  /**
   * \return Rank of the querying task in this grp
   */
  unsigned int get_rank () const  { return grank; }

  /**
   * \param [in] at Attribute that governs execution of the task
   */
  void set_attr (const attribute& at)  { attr = at; }

  /**
   * \param [in] gp The group that is associated with the task.
   */
  void set_group (group* gp)  {
    grp = gp;
    if (!attr.get_grouped()) return;
    else {
      gsize = grp->get_size();
      grank = grp->join_group();
    }
  }

  /**
   * \param [in] fn Work function to be executed by the executing thread.
   */
  void set_func (functor* fn)  { func = fn; }

  /**
   * \param [in] nwait Number of waiters to receive notification
   */
  void reset_completion (const unsigned int& nwait = 1) {
    if (attr.get_nested ()) testing_compl.reset (nwait);
    else waiting_compl.reset (nwait);
    PFUNC_EXCEPT_PTR_CLEAR()
  }

  /**
   * Run the work function 
   */
  void run ()  { 
    PFUNC_START_TRY_BLOCK()
    (*func)(); 
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_STORE(task,run)
  }

  /**
   * Wait for the current job to complete.
   * 
   * \param [in] taskmgr The task manager that is controlling the execution.
   */
  template <typename TaskManager>
  void wait (TaskManager& taskmgr)  { 
    PFUNC_CHECK_AND_RETHROW()

    PFUNC_START_TRY_BLOCK()
    if (attr.get_nested ()) {
      taskmgr.progress_wait (testing_compl);
    } else {
      waiting_compl.wait ();
    }
    if (attr.get_grouped ()) grp->leave_group ();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(task,run)
  }

  /**
   * Test for the current job to complete.
   * 
   * \param [in] taskmgr The task manager that is controlling the execution.
   */
  template <typename TaskManager>
  bool test (TaskManager& taskmgr)  { 
    PFUNC_CHECK_AND_RETHROW()

    bool return_value = false;
    PFUNC_START_TRY_BLOCK()
    if (attr.get_nested ()) return_value = testing_compl.test ();
    else return_value = waiting_compl.test ();

    if (return_value && attr.get_grouped ()) grp->leave_group();

    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(task,run)

    return return_value;
  }

  /**
   * Notify a job's completion. If there are more than one waiters on this 
   * particular task, then broadcast. Else, a simple signal will do.
   */
  void notify ()  { 
    PFUNC_CHECK_AND_RETHROW()

    PFUNC_START_TRY_BLOCK()
    if (attr.get_nested()) testing_compl.notify ();
    else waiting_compl.notify();
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(task,run)
  }

  /**
   * Execute a barrier across all the tasks in this task's group.
   *
   * \param [in] taskmgr The task manager that is controlling the execution.
   */
  template <typename TaskManager>
  void barrier (TaskManager& taskmgr)   { 
    PFUNC_START_TRY_BLOCK()
    grp->barrier(taskmgr); 
    PFUNC_END_TRY_BLOCK()
    PFUNC_CATCH_AND_RETHROW(task,run)
  }

  /**
   * Default constructor.
   */
  task ()  : grp (NULL),
             gsize (0),
             grank (0),
             func (NULL)
             PFUNC_EXCEPT_PTR_INIT() {}

  /**
   * Parameterized constructor.
   *
   * \param [in] attr The attribute for this task.
   * \param [in] grp The group for this task.
   * \param [in] func The work function for this task.
   */
  task (const attribute& attr, const group*& grp, functor*& func) :
                 attr (attr),
                 grp (grp),
                 gsize (0),
                 grank (0),
                 func (NULL)
                 PFUNC_EXCEPT_PTR_INIT() {}

  /** 
   * Destructor.
   */
  ~task ()  { PFUNC_EXCEPT_PTR_CLEAR() }

  /**
   * operator< to compare two tasks.
   *
   * \param [in] one The first task.
   * \param [in] two The second task.
   *
   * \return true If the first task has a lower priority than the second.
   */
  friend bool operator<(const task& one, const task& two)  {
    return (one.attr < two.attr);
  }

  /*
   * Shallow copy of a task. This is used explicitly as the structure itself
   * is not copyable.
   *
   * \param [in] other The other task from which to shallow copy.
   */
  void shallow_copy(const task& other)  {
    attr = other.attr;
    grp = other.grp;
    gsize = other.gsize;
    grank = other.grank;
  }
}; /* task */
  
} /* namespace detail */ } /* namespace pfunc */

#endif // PFUNC_TASK_HPP
