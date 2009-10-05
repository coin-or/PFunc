#include <cstdio>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>
#include <tbb/scalable_allocator.h>

#define HASH #
#define BOOBOO HASH ## ifdef BOO

#define ANJU() \
BOOBOO \
  #error \
\#endif


struct my_exception {
  const char* what (void) const {
    return "What the hell";
  }
};

struct anju {
  void kill () throw (my_exception) {
#if 0
    try {
#endif
      ;int* i = new int; 
#if 0
    } catch (...) {
      throw my_exception();     
    }
#endif
  }
};

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
  return( ((double) (tp.tv_sec - start)) + (tp.tv_usec-startu)*1.e-6 );
}

struct boom {
  virtual void allocate_one_int (int*&) = 0;
  virtual void delete_one_int (int*&) = 0;
};

struct baz 
#ifdef VIRTUAL
: virtual boom
#endif
{
#ifdef EXCEPTION
  void allocate_one_int (int*& ptr) throw (my_exception) {
    try {
      ptr = new int;
    } catch (...) {
      throw my_exception();
    }
  }

  void delete_one_int (int*& ptr) throw (my_exception) {
    try {
      delete ptr;
    } catch (...) {
      throw my_exception();
    }
  }
#else
  void allocate_one_int (int*& ptr) {
    ptr = (int*) scalable_malloc (sizeof(int));
  }

  void delete_one_int (int*& ptr) {
    scalable_free (ptr);
  }
#endif
};


struct foo {
  anju an;
  int** a;
#ifdef VIRTUAL
  boom* baz_ptr;
#else
  baz* baz_ptr;
#endif

#ifdef VIRTUAL
  foo (boom* ptr) : baz_ptr (ptr) {}
  virtual ~foo () {}
#else
  foo (baz* ptr): baz_ptr (ptr) {}
#endif

#ifdef EXCEPTION
  void do_one_thing () throw (my_exception) {
    try {
      a = new int* [25000000];
      for (int i=0; i<25000000; ++i) {
        baz_ptr->allocate_one_int(a[i]); 
      }
    } catch (const my_exception& error) {
      printf ("%s\n", error.what ());
    }
  }

  void do_one_more_thing () throw (my_exception) {
    try {
      for (int i=0; i<25000000; ++i) {
        baz_ptr->delete_one_int (a[i]);
      }
      delete [] a;
    } catch (const my_exception& error) {
      printf ("%s\n", error.what ());
    }
  }

#else

  void do_one_thing () {
    a = new int* [25000000];
    for (int i=0; i<25000000; ++i) {
      baz_ptr->allocate_one_int (a[i]);
    }
  }

  void do_one_more_thing () throw (my_exception) {
    for (int i=0; i<25000000; ++i) {
      baz_ptr->delete_one_int (a[i]);
    }
    delete [] a;
  }

#endif
};

int main () {
#ifdef VIRTUAL
  boom* baz_ptr = new baz;
#else
  baz* baz_ptr = new baz;
#endif

  foo bar (baz_ptr);

  double time = wsmprtc ();
  bar.do_one_thing ();
  bar.do_one_more_thing ();
  printf ("%lf\n", wsmprtc() - time);
  return 0;
}
