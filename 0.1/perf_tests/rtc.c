#ifndef WINDOWS
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifdef HPUX
#ifndef _STRUCT_TIMEVAL
#define _STRUCT_TIMEVAL
   /* Structure returned by gettimeofday(2) system call and others */
        struct timeval {
#if defined(_XPG4_EXTENDED) && !defined(_KERNEL)
                time_t   tv_sec;        /* seconds */
#else /* !__XPG4_EXTENDED || _KERNEL*/
                uint32_t tv_sec;   /* seconds */
#endif /* __XPG4_EXTENDED && !_KERNEL*/
                long     tv_usec;       /* and microseconds */
        };
#endif /* !__STRUCT_TIMEVAL */
#endif

double wsmprtc(void) {
struct timeval tp;
static long start=0, startu;

  if (!start) {
    gettimeofday(&tp, NULL);
    start = tp.tv_sec;
    startu = tp.tv_usec;
    return(0.0);
  }
  gettimeofday(&tp, NULL);
#ifdef IRIX
  return( 1.e-6*(((double) (tp.tv_sec - start)) + (tp.tv_usec-startu)*1.e-6) );
#else
  return( ((double) (tp.tv_sec - start)) + (tp.tv_usec-startu)*1.e-6 );
#endif
}

#else // DEFINED WINDOWS
#include <windows.h>
#include <time.h>

double wsmprtc (void) {
  SYSTEMTIME sys_time;
  FILETIME file_time;
  ULARGE_INTEGER uli;
  double ret_val = 0.0;
  static volatile BOOL has_been_initialized = FALSE;
  static volatile ULARGE_INTEGER start_time;

  if (!has_been_initialized) {
    GetSystemTime (&sys_time);
    SystemTimeToFileTime (&sys_time, &file_time);
    start_time.LowPart = file_time.dwLowDateTime;
    start_time.HighPart = file_time.dwHighDateTime;
    has_been_initialized = TRUE;
  } else {
    GetSystemTime (&sys_time);
    SystemTimeToFileTime (&sys_time, &file_time);
    uli.LowPart = file_time.dwLowDateTime;
    uli.HighPart = file_time.dwHighDateTime;

    uli.QuadPart -= start_time.QuadPart;
    ret_val = ((double) uli.QuadPart)/100.00;
  }
  return ret_val;
}
#endif
