#ifndef PFUNC_GENERATOR_S_HPP
#define PFUNC_GENERATOR_S_HPP

/***************************************************************************
 * \file generator.hpp Generate the library instance description.
 * \author Prabhanjan Kambadur
 */
 
#include <pfunc/attribute.hpp>
#include <pfunc/group.hpp>
#include <pfunc/task.hpp>
#include <pfunc/taskmgr.hpp>
#include <pfunc/scheduler.hpp>
#include <pfunc/trampolines.hpp>
#include <functional>


namespace pfunc {

  /** Base structure for tag based partial specializations */
  struct default_tag {};

  /** Using default value for the scheduler template parameter */
  struct sched_tag : public default_tag {};

  /** Using default value for the compare template parameter */
  struct comp_tag : public default_tag {};

  /** Using default value for the functor template parameter */
  struct func_tag : public default_tag {};
  
  /**
   * Template version of the default type for each of the template
   * parameters in the Library Instance Generator 
   */
  template <typename T> struct default_type {};

  /** Specialization for scheduler default */
  template <> struct default_type <sched_tag> { 
    typedef cilkS type; /** Default scheduler type */
  };

  /** Specialization for compare default */
  template <> struct default_type <comp_tag> { 
    typedef std::less<int> type; /** Default compare type */
  };

  /** Specialization for functor default */
  template <> struct default_type <func_tag> { 
    typedef detail::virtual_functor type; /** Default functor type */
  };

/**
 * \def Generate the typedefs for the related types of the given 
 * library instance description. 
 */
#define GENERATE_PFUNC_TYPES() \
    typedef detail::attribute<priority_type, compare_type>  attribute; \
    typedef detail::task <attribute, functor> task; \
    typedef detail::taskmgr <scheduler, task> taskmgr; \
    typedef detail::group group;

  /**
   * Generator structure that is specialized to produce the required 
   * library instance description. There are three explicit template 
   * parameters:
   * 1. SchedPolicy: The scheduling policy to be used.
   * 2. Compare: The comparison function to use in case the scheduling policy
   *              requires ordering of tasks.
   * 3. Functor: The function object that will be executed.
   *
   * There is a fourth, implicit template parameter, "Priority" that denotes
   * the type of the priority associated with each task. This is extracted 
   * as a nested type from the "Compare" type.
   */
  template <typename SchedPolicy,
            typename Compare,
            typename Functor>
  struct generator {
    typedef SchedPolicy scheduler; /** typedef for the scheduler */
    typedef Compare compare_type; /** typedef for the compare_type */
    typedef typename compare_type::first_argument_type priority_type; /* typedef for the priority_type */
    typedef Functor functor; /** typedef for the functor */

    GENERATE_PFUNC_TYPES()
  };

  /**
   * This structure can be used by developers if they wish the default types
   * to be used for instantiating the library. Depending on the position of 
   * this type, we will use the appropriate default value for that param.
   */
  struct use_default {};

  /** Specialization for using all default values */
  template <> 
  struct generator <use_default, use_default, use_default> {
    typedef default_type<sched_tag>::type scheduler;/** typedef for the scheduler */
    typedef default_type<comp_tag>::type compare_type;/** typedef for the compare_type */
    typedef compare_type::first_argument_type priority_type;/* typedef for the priority_type */
    typedef default_type<func_tag>::type functor;/** typedef for the functor */

    GENERATE_PFUNC_TYPES()
  };

  /** Specialization for using user picked scheduling policy */
  template <typename SchedPolicy>
  struct generator <SchedPolicy, use_default, use_default> {
    typedef SchedPolicy scheduler;/** typedef for the scheduler */
    typedef typename default_type<comp_tag>::type compare_type;/** typedef for the compare_type */
    typedef typename compare_type::first_argument_type priority_type;/* typedef for the priority_type */
    typedef typename default_type<func_tag>::type functor;/** typedef for the functor */

    GENERATE_PFUNC_TYPES()
  };


  /** Specialization for using user picked comparison operator */
  template <typename Compare>
  struct generator <use_default, Compare, use_default> {
    typedef default_type<sched_tag>::type scheduler;/** typedef for the scheduler */
    typedef Compare compare_type;/** typedef for the compare_type */
    typedef typename compare_type::first_argument_type priority_type;/* typedef for the priority_type */
    typedef default_type<func_tag>::type functor;/** typedef for the functor */

    GENERATE_PFUNC_TYPES()
  };


  /** Specialization for using user picked functor */
  template <typename Functor>
  struct generator <use_default, use_default, Functor> {
    typedef default_type<sched_tag>::type scheduler;/** typedef for the scheduler */
    typedef default_type<comp_tag>::type compare_type;/** typedef for the compare_type */
    typedef compare_type::first_argument_type priority_type;/* typedef for the priority_type */
    typedef Functor functor;/** typedef for the functor */

    GENERATE_PFUNC_TYPES()
  };


  /** Specialization for using user picked scheduling policy and compare */
  template <typename SchedPolicy, typename Compare>
  struct generator <SchedPolicy, Compare, use_default> {
    typedef SchedPolicy scheduler;/** typedef for the scheduler */
    typedef Compare compare_type;/** typedef for the compare_type */
    typedef typename compare_type::first_argument_type priority_type;/* typedef for the priority_type */
    typedef default_type<func_tag>::type functor;/** typedef for the functor */

    GENERATE_PFUNC_TYPES()
  };


  /** Specialization for using user picked scheduling policy and functor */
  template <typename SchedPolicy, typename Functor>
  struct generator <SchedPolicy, use_default, Functor> {
    typedef SchedPolicy scheduler;/** typedef for the scheduler */
    typedef default_type<comp_tag>::type compare_type;/** typedef for the compare_type */
    typedef compare_type::first_argument_type priority_type;/* typedef for the priority_type */
    typedef Functor functor;/** typedef for the functor */

    GENERATE_PFUNC_TYPES()
  };


  /** Specialization for using user picked compare op and functor */
  template <typename Compare, typename Functor>
  struct generator <use_default, Compare, Functor> {
    typedef default_type<sched_tag>::type scheduler;/** typedef for the scheduler */
    typedef Compare compare_type;/** typedef for the compare_type */
    typedef typename compare_type::first_argument_type priority_type;/* typedef for the priority_type */
    typedef Functor functor;/** typedef for the functor */

    GENERATE_PFUNC_TYPES()
  };

} /* namespace pfunc */

#endif // PFUNC_GENERATOR_S_HPP
