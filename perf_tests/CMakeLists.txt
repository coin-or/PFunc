cmake_minimum_required (VERSION 2.6)

# Add the pfunc source directory to be in the include directory path.
# Add the pfunc binary directory to be in the include directory path.
# This is done so that the generated header files can also be found.
include_directories (BEFORE ${PFUNC_SOURCE_DIR} ${PFUNC_BINARY_DIR})

#Figure out what the C++ library is called and add it to the required libs.
find_library (STDCXX NAMES C stdc++ REQUIRED)

# Add this test only if we have pthreads and unistd
if (PFUNC_HAVE_PTHREAD_H AND PFUNC_HAVE_UNISTD_H)
  add_executable (cancel_threads cancel_threads.c)
  target_link_libraries (cancel_threads pthread)
endif (PFUNC_HAVE_PTHREAD_H AND PFUNC_HAVE_UNISTD_H)

# Add this test only if we have pthreads and resource
if (PFUNC_HAVE_PTHREAD_H AND PFUNC_HAVE_SYS_RESOURCE_H)
add_executable (mutex_test mutex_test.cpp)
target_link_libraries (mutex_test pthread)
endif (PFUNC_HAVE_PTHREAD_H AND PFUNC_HAVE_SYS_RESOURCE_H)

add_executable (pfunc_barrier_time pfunc_barrier_time.cpp)
add_dependencies (pfunc_barrier_time pfunc)
if (NOT CMAKE_SYSTEM MATCHES "Windows")
  target_link_libraries (pfunc_barrier_time pthread)
endif (NOT CMAKE_SYSTEM MATCHES "Windows")

add_executable (thrdperf thrdperf.c)
add_dependencies (thrdperf pfunc)
target_link_libraries (thrdperf pfunc)
if (NOT CMAKE_SYSTEM MATCHES "Windows")
  target_link_libraries (thrdperf ${STDCXX} pthread)
endif (NOT CMAKE_SYSTEM MATCHES "Windows")

add_custom_target (perf_tests ALL)
add_dependencies (perf_tests pfunc_barrier_time mutex_test)
