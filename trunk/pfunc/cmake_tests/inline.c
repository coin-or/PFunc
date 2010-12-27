#ifndef INLINE_STRING
#error "Inline string was not supplied"
#else
static INLINE_STRING void foo (int bar) {}
#endif

int main () { foo (10); }
