#ifndef PFUNC_ENVIRON_HPP
#define PFUNC_ENVIRON_HPP

/**
 * \file environ.hpp
 * \author Prabhanjan Kambadur
 * This file contains all the environ specific defines that we need.
 * In PFunc, we are concerned about the OS-type, Architecture and Compiler.
 * In this file, we check that the environment we are running on is something
 * we have seen and tested before. For now, we will only honor the following:
 *
 * OS: Linux, Darwin, AIX and Windows.
 * Compiler: GNU, Intel, XLC and VS.
 * Architecture: X86-based, PPC.
 */

#include <pfunc/config.h>

/** Define alignment policy for different OS types */
#if PFUNC_WINDOWS == 1
#define ALIGN16 __declspec(align(16))
#define ALIGN32 __declspec(align(32))
#define ALIGN64 __declspec(align(64))
#define ALIGN128 __declspec(align(128))
#elif PFUNC_LINUX == 1 || PFUNC_AIX == 1 || PFUNC_DARWIN == 1
#define ALIGN16 __attribute__((aligned(16)))
#define ALIGN32 __attribute__((aligned(32)))
#define ALIGN64 __attribute__((aligned(64)))
#define ALIGN128 __attribute__((aligned(128)))
#else
#define ALIGN16 
#define ALIGN32 
#define ALIGN64 
#define ALIGN128
#endif

/** 
 * Get a definition of inline for the C compilers.
 */
#if defined (c_plusplus) || defined (__cplusplus)
#define PFUNC_INLINE inline
#else 
#if PFUNC_HAVE_INLINE == 1
#define PFUNC_INLINE inline
#elif PFUNC_HAVE_UNDERSCORE_INLINE == 1
#define PFUNC_INLINE __inline
#elif PFUNC_HAVE_UNDERSCORE_INLINE_UNDERSCORE == 1
#define PFUNC_INLINE __inline__
#else
#define PFUNC_INLINE /*nothing*/
#endif
#endif

#endif /* PFUNC_ENVIRON_HPP */
