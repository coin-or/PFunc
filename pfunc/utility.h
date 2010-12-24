#ifndef PFUNC_UTILITY_H
#define PFUNC_UTILITY_H

#include <pfunc/config.h>
#include <pfunc/environ.hpp>

#if PFUNC_WINDOWS == 1
#include <Windows.h>
#endif

#if PFUNC_HAVE_SYS_TIME_H == 1
#include <sys/time.h>
#endif

/**
 * All the utility functions are in the C domain on purpose. Its a lot 
 * simpler to use this way and we do not need a wrapper from C++.
 */
#if defined (c_plusplus) || defined (__cplusplus)
extern "C" {
#endif

static PFUNC_INLINE double micro_time () {
#if PFUNC_WINDOWS == 1

  /** Use GetSystemTime to get millisecond time resolution */
  SYSTEMTIME sys_time;
  FILETIME file_time;
  ULARGE_INTEGER uli;

  GetSystemTime (&sys_time);
  SystemTimeToFileTime (&sys_time, &file_time);
  uli.LowPart = file_time.dwLowDateTime;
  uli.HighPart = file_time.dwHighDateTime;

  return ((double) uli.QuadPart)/100.00;

#elif PFUNC_HAVE_SYS_TIME_H == 1

  /** Use gettimeofday() to return the time in milliseconds */
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return (double)tp.tv_sec + tp.tv_usec*1.e-6;

#else 
#warn "No accurate implementation found --- returning 0.0 always"
  return 0.0;
#endif
}

#if defined (c_plusplus) || defined (__cplusplus)
}
#endif

#endif // PFUNC_UTILITY_H
