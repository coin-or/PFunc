#ifndef PFUNC_FUTEX_H
#define PFUNC_FUTEX_H

#if PFUNC_HAVE_SYSCALL_H == 1
#include <sys/syscall.h>
#endif

#if PFUNC_HAVE_UNISTD_H == 1
#include <unistd.h>
#endif

#if PFUNC_HAVE_ERRNO_H == 1
#include <errno.h>
#endif

#ifndef SYS_futex
#warning "SYS_futex not defined"
#define SYS_futex	202
#endif

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Atomically checks if the value at the given address is what is 
 * expected. If so, the the calling thread is put to sleep waiting on a 
 * futex_wake call to occur. If the value at the address has changed, the 
 * calling thread returns without entering the kernel. 
 *
 * \param[in] addr Address at which the value needs to be checked.
 * \param[in] val The value if found at the given address, the sleep should 
 *                be entering. 
 */
static inline void futex_wait (int* addr, int val) {
  syscall (SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
}

/**
 * \brief Wakes up the specified number of threads which are sleeping on
 * using futex_wait at the given address.
 *
 * \param[in] addr Address at which the threads are waiting.
 * \param[in] nthreads The number of threads to wake up.
 */
static inline void futex_wake (int *addr, int nthreads) {
  syscall (SYS_futex, addr, FUTEX_WAKE, nthreads, NULL, NULL, 0);
}

/**
 * \brief Performs a nop -- basically a way to ensure we are not using memory.
 */
static inline void cpu_relax (void) {
#if PFUNC_X86 == 1
  __asm__ __volatile__ ("rep; nop" : : : "memory");
#else
  /** Do nothing as of now */
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* PFUNC_FUTEX_H */
