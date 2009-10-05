#ifndef __cplusplus
#warning "Please compile with a C++ compiler to be sure"
#endif
#include <sys/time.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
int main(int argc, char** argv) {
#ifndef SYS_futex
#define SYS_futex 202
#endif
  int foo = 1;
  if(ENOSYS==syscall(SYS_futex, &foo, 1/*wake*/, 1, 0, 0, 0))
    return -1;
  else
    return 0;
}
