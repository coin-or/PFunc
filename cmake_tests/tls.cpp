#ifndef __cplusplus
#warning "Please compile with a C++ compiler to be sure"
#endif
static __thread int i;
int main (int argc, char** argv) {
  i = argc;
}
