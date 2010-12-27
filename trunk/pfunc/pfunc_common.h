#ifndef PFUNC_COMMON_H
#define PFUNC_COMMON_H

/**
 * \file pfunc_common.h
 * \brief Implementation of PFUNC -- Contains the common codes
 * \author Prabhanjan Kambadur
 *
 */

/** Types of barriers */
enum {
  BARRIER_SPIN = 0, /**< Spin until barrier is satisfied */
  BARRIER_WAIT, /**< Sleep until barrier is satisfied */
  BARRIER_STEAL /**< Keep working on another job until barrier is staisfied */
};

/* Declaring some enumarations */
enum { PFUNC_ERROR = -0xFFFF, /**< An error has occured */
       PFUNC_UNINITIALIZED, /**< Library has not been initialized */
       PFUNC_NOMEM, /**< Malloc failed */
       PFUNC_INVALID_ARGUMENTS, /**< The arguments are mismatched */
       PFUNC_INVALID_ATTR, /**< The attribute is not present */
       PFUNC_INVALID_ATTR_NAME, /**< Attribute is not one of the predefined ones */
       PFUNC_INVALID_ATTR_VALUE, /**< The attribute specified is not present */
       PFUNC_INVALID_GROUP, /**< The group is not valid */
       PFUNC_INVALID_GROUP_ATTR, /**< The group attribute is not valid */
       PFUNC_INVALID_HANDLE, /**< Handle specified is not valid */
       PFUNC_TASK_INCOMPLETE, /**< Incomplete task */
       PFUNC_SUCCESS = 0, /**< The function succeeded */
       PFUNC_INITIALIZED, /**< PFUNC library has been initialized */
       PFUNC_TASK_COMPLETE /**< The added ``task'' is complete without errors */
};

#endif /* PFUNC_COMMON_H */
