SRC_DIR=../pfunc/
LIB_DIR=../lib/
CILK_CC=/home/pkambadu/cilk-install/bin/cilkc
TBB_INC=-I/home/pkambadu/tbb-install/include/
TBB_LDFLAGS=-L/home/pkambadu/tbb-install/lib/ -ltbb
CFLAGS=-O3 -funroll-loops -fomit-frame-pointer -I$(SRC_DIR) -lm -Wall -Wextra -pedantic-errors -Wold-style-cast -m64 -march=opteron
CFLAGS_DEBUG=-O0 -g -I$(SRC_DIR) -lm -DDEBUG
CFLAGS_ASM=-S -I$(SRC_DIR) -fverbose-asm -DDEBUG -O4 -fomit-frame-pointer -funroll-loops -m64 -march=opteron
C_LDFLAGS_DEBUG=-L${LIB_DIR} -lpfunc_dbg -lstdc++ -lpthread 
C_LDFLAGS=-L${LIB_DIR} -lpfunc -lstdc++ -lpthread 
CXX_LDFLAGS_DEBUG=-lpthread 
CXX_LDFLAGS=-lpthread 

RELEASE_OBJS=simple groups fibonacci cilk_fibonacci serial_fibonacci tbb_fibonacci
DEBUG_OBJS=simple_dbg groups_dbg fibonacci_dbg

all: $(RELEASE_OBJS) $(DEBUG_OBJS)

fibonacci_dbg: fibonacci.cpp
	$(CXX) $(CFLAGS_DEBUG) $< -o $@ $(CXX_LDFLAGS_DEBUG)

fibonacci_asm: fibonacci.cpp
	$(CXX) $(CFLAGS_ASM) $<

fibonacci: fibonacci.cpp 
	$(CXX) $(CFLAGS) $< -o $@ $(CXX_LDFLAGS)

serial_fibonacci: serial_fibonacci.c
	$(CC) $(CFLAGS) $< -o $@ 

cilk_fibonacci: fibonacci.cilk
	$(CILK_CC) $(CFLAGS) $< -o $@

tbb_fibonacci: tbb_fibonacci.cpp
	$(CXX) $(CFLAGS_DEBUG) $(TBB_INC) $< -o $@ $(TBB_LDFLAGS)

simple_dbg: simple.c $(HEADERS) debug
	$(CC) $(CFLAGS_DEBUG) $< -o $@ $(C_LDFLAGS_DEBUG)

groups_dbg: groups.c $(HEADERS) debug 
	$(CC) $(CFLAGS_DEBUG) $< -o $@ $(C_LDFLAGS_DEBUG)

simple: simple.c $(HEADERS) release 
	$(CC) $(CFLAGS) $< -o $@ $(C_LDFLAGS)

groups: groups.c $(HEADERS) release
	$(CC) $(CFLAGS) $< -o $@ $(C_LDFLAGS)

debug: 
	cd ${LIB_DIR}; make debug

release:
	cd ${LIB_DIR}; make release

clean:
	cd $(LIB_DIR); make clean 
	rm -rf *.o *.obj $(RELEASE_OBJS) $(DEBUG_OBJS)
	rm -rf core.* *.pdb *.ini *.ilk *.log *.exe recode
	rm -rf *.pfy *.suo err* *.dSYM 
