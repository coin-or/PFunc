#ifndef PFUNC_UTILITY_H
#define PFUNC_UTILITY_H

#include <pfunc/config.h>
#include <pfunc/environ.hpp>
#include <stdlib.h>

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

/**
 * Returns a random double in the range (0.0,1.0]. Notice that the random 
 * number generator is never initialized --- we don't really care about :).
 * @return random double in the range (0.0,1.0].
 */
static const double NORMALIZER = static_cast<double>(RAND_MAX);
static inline double get_next_rand () {
  return (static_cast<double>(rand())/NORMALIZER);
}

/**
 * Returns the next highest power of two for the given number.
 * @param[in] n The number for which we want to find the next closest power.
 * @return The power of 2 closest to 'n'. E.g., for 3, 4 is returned.
 */
static inline int get_closest_power_of_2 (const int n) {
  int closest_power_of_2 = 0x1;
  while (n > closest_power_of_2) closest_power_of_2 = closest_power_of_2 << 1;
  return closest_power_of_2;
}

/**
 * Returns the time in seconds (as a double) to micro-second accuracy.
 * This function can be used to measure the time elapsed. Eg.,
 * start = micro_time ();
 * do_stuff ();
 * end = micro_time () - start;
 */
static PFUNC_INLINE double micro_time () {
#if PFUNC_WINDOWS == 1

  /** Use GetSystemTime to get microsecond time resolution */
  SYSTEMTIME sys_time;
  FILETIME file_time;
  ULARGE_INTEGER uli;

  GetSystemTime (&sys_time);
  SystemTimeToFileTime (&sys_time, &file_time);
  uli.LowPart = file_time.dwLowDateTime;
  uli.HighPart = file_time.dwHighDateTime;

  /** Filetime represents the number of 100 nanosecond intervals since
   * January 1 1601 (UTC). To convert this into seconds, we need to 
   * multiply by 1.e-7.
   */
  return ((double) uli.QuadPart)*1e-7;

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

#endif /* PFUNC_UTILITY_H */
