SRC_DIR=../pfunc/
LIB_DIR=../lib/

HEADERS=$(SRC_DIR)pfunc.hpp \
        $(SRC_DIR)pfunc.h \
        $(SRC_DIR)lib_t.hpp \
        $(SRC_DIR)attr_t.hpp \
        $(SRC_DIR)group_t.hpp \
        $(SRC_DIR)handle_t.hpp \
        $(SRC_DIR)detail/pfunc_mutex_t.hpp \
        $(SRC_DIR)detail/pfunc_cond_t.hpp \
        $(SRC_DIR)detail/pfunc_event_t.hpp \
        $(SRC_DIR)detail/pfunc_thread_t.hpp \
        $(SRC_DIR)detail/pfunc_exception_t.hpp \
        $(SRC_DIR)detail/pfunc_miscellany.hpp 


all: thrdperf

debug: 
	cd ${LIB_DIR}; make debug

release:
	cd ${LIB_DIR}; make release


ifeq ($(OS), Windows_NT)
  CC=icl
  CFLAGS=/I$(SRC_DIR) /Zi /DWINDOWS /D_WIN32_WINNT=0x0502 /EHsc /D_CRT_SECURE_NO_DEPRECATE
  #CFLAGS=/I$(SRC_DIR) /O3 /DWINDOWS /D_WIN32_WINNT=0x0502 /EHsc /D_CRT_SECURE_NO_DEPRECATE
  LDFLAGS=/L${LIB_DIR} /lpfunc

else

  OS=$(shell uname -s)

  ifeq ($(OS), Linux)
  	CC=icc
		CXX=icpc
  	CFLAGS=-O0 -g -I$(SRC_DIR) -no-multibyte-chars
  	#CFLAGS=-O3 -funroll-loops -fomit-frame-pointer -I$(SRC_DIR)
		CXXFLAGS=$(CFLAGS)
  	LDFLAGS=-lpthread -L${LIB_DIR} -lpfunc -lstdc++
  endif
  ifeq ($(OS), AIX)
  	CC=xlc_r
  	#CFLAGS=-O0 -g -I$(SRC_DIR)
  	CFLAGS=-O3 -funroll-loops -fomit-frame-pointer -Wall -I$(SRC_DIR)
		CXXFLAGS=$(CFLAGS)
  	LDFLAGS=-lpthread -L${LIB_DIR} -lpfunc -lstdc++
  endif
  ifeq ($(OS), Darwin)
  	CC=gcc
  	#CFLAGS=-O0 -g -I$(SRC_DIR)
  	CFLAGS=-O3 -funroll-loops -fomit-frame-pointer -Wall -I$(SRC_DIR)
		CXXFLAGS=$(CFLAGS)
  	LDFLAGS=-lpthread -L${LIB_DIR} -lpfunc -lstdc++
  endif

endif

pfunc_barrier_time: pfunc_barrier_time.cpp rtc.c $(HEADERS) release
	$(CXX) $(CXXFLAGS) $< rtc.c -o $@ $(LDFLAGS)

pfunc_item_set_mining: pfunc_item_set_mining.cpp rtc.c $(HEADERS) release
	$(CXX) $(CXXFLAGS) $< rtc.c -o $@ $(LDFLAGS)

pfunc_delta_stepping: pfunc_delta_stepping.cpp edge_property_iterator.hpp rtc.c $(HEADERS) debug
	$(CXX) $(CXXFLAGS) $< rtc.c -o $@ $(LDFLAGS) -lboost_graph

thrdperf: thrdperf.c rtc.c $(HEADERS) release
	$(CC) $(CFLAGS) thrdperf.c rtc.c -o $@ $(LDFLAGS)

fib: omp_fibonacci.c
	@echo "Using Intel Compiler Directly -- Uses Tasks"
	@echo "Run this like so \"#./fib <number>\""
	icc -openmp $< -o $@ -no-multibyte-chars

clean:
	cd $(LIB_DIR); make clean
	rm -rf *.o *.obj thrdperf  
	rm -rf core.* *.pdb *.ini *.ilk *.log *.exe
	rm -rf *.pfy *.suo out* err* *.dSYM pfunc_dag pfunc_delta_stepping
	rm -rf pfunc_item_set_mining pfunc_barrier_time fib
