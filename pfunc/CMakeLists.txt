
# Author: Prabhanjan Kambadur
# 
# Here are a list of things that we need to check for
# 
# 1. Threading package: pthreads or windows threads.
# 2. Operating system: Windows, Linux, Apple, AIX.
# 3. Futex.
# 4. If the compiler accepts "__thread" option.
#

# Generate header file
include (CheckIncludeFileCXX)

check_include_file_cxx (errno.h PFUNC_HAVE_ERRNO_H)
check_include_file_cxx (limits.h PFUNC_HAVE_LIMITS_H)
check_include_file_cxx ("sys/sysctl.h" PFUNC_HAVE_SYSCTL_H)
check_include_file_cxx ("sys/resource.h" PFUNC_HAVE_SYS_RESOURCE_H)
check_include_file_cxx ("sys/syscall.h" PFUNC_HAVE_SYSCALL_H)
check_include_file_cxx ("unistd.h" PFUNC_HAVE_UNISTD_H)
check_include_file_cxx ("time.h" PFUNC_HAVE_TIME_H)
check_include_file_cxx ("sys/time.h" PFUNC_HAVE_SYS_TIME_H)
check_include_file_cxx ("sched.h" PFUNC_HAVE_SCHED_H)
check_include_file_cxx ("stdint.h" PFUNC_HAVE_STDINT_H)
check_include_file_cxx ("ext/hash_map" PFUNC_HAVE_HASH_MAP_H)

# Try to see if we can get the processor information
if (CMAKE_SYSTEM_PROCESSOR MATCHES "Intel*" OR
    CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86" OR
    CMAKE_SYSTEM_PROCESSOR MATCHES "x86_*")
  set (PFUNC_X86 1)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc" OR
        CMAKE_SYSTEM_PROCESSOR MATCHES "Power*" OR
        CMAKE_SYSTEM_PROCESSOR MATCHES "ppc*")
  set (PFUNC_PPC 1)
else (CMAKE_SYSTEM_PROCESSOR MATCHES "Intel*" OR
      CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86" OR
      CMAKE_SYSTEM_PROCESSOR MATCHES "x86_*")
  message (FATAL_ERROR "PFunc is currently only supported on Intel and PPC")
endif (CMAKE_SYSTEM_PROCESSOR MATCHES "Intel*" OR
       CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86" OR
       CMAKE_SYSTEM_PROCESSOR MATCHES "x86_*")

# Get the operating system that is in use
if (CMAKE_SYSTEM MATCHES "Linux")
  set (PFUNC_LINUX 1)
elseif (CMAKE_SYSTEM MATCHES "Windows")
  message (STATUS "We only support 32-bit Windows as of now")
  set (PFUNC_WINDOWS 1)
elseif (CMAKE_SYSTEM MATCHES "Darwin")
  set (PFUNC_DARWIN 1)
elseif (CMAKE_SYSTEM MATCHES "AIX")
  set (PFUNC_AIX 1)
else (CMAKE_SYSTEM MATCHES "Linux")
  message (FATAL_ERROR "PFunc is currently only supported on Linux, Windows,
  AIX and Darwin")
endif (CMAKE_SYSTEM MATCHES "Linux")

# Ensure that we have a threading package
include (FindThreads)
if (CMAKE_USE_WIN32_THREADS_INIT)
  set (PFUNC_HAVE_WINDOWS_THREADS 1)
elseif (CMAKE_USE_PTHREADS_INIT)
  set (PFUNC_HAVE_PTHREADS 1)
else (CMAKE_USE_WIN32_THREADS_INIT)
  message (FATAL_ERROR "PFunc could not locate a compatible threading
                        package")
endif (CMAKE_USE_WIN32_THREADS_INIT)

# Check if futexes are supported
if (PFUNC_LINUX AND PFUNC_HAVE_TIME_H AND 
    PFUNC_HAVE_SYSCALL_H AND PFUNC_HAVE_UNISTD_H)
  try_run (PFUNC_FUTEX_EXIT_CODE PFUNC_FUTEX_COMPILED
           ${CMAKE_CURRENT_BINARY_DIR}
           ${CMAKE_CURRENT_SOURCE_DIR}/cmake_tests/futex.cpp)
  if (PFUNC_FUTEX_EXIT_CODE EQUAL 0)
    message (STATUS "Checking if we have futex support - found")
    # Unsetting Futex support on PPC for now. There seems to be a bug in my locks.
    if (PFUNC_PPC EQUAL 1)
      message (STATUS "NOT enabling futex support - awaiting bug fix on PPC")
    else (PFUNC_PPC EQUAL 1)
      set (PFUNC_HAVE_FUTEX 1)
    endif (PFUNC_PPC EQUAL 1)
  else (PFUNC_FUTEX_EXIT_CODE EQUAL 0)
    message (STATUS "Checking if we have futex support - not found")
  endif (PFUNC_FUTEX_EXIT_CODE EQUAL 0)
endif (PFUNC_LINUX AND PFUNC_HAVE_TIME_H AND 
       PFUNC_HAVE_SYSCALL_H AND PFUNC_HAVE_UNISTD_H)

# Check if thread level storage is supported
try_compile (PFUNC_TLS_COMPILED
             ${CMAKE_CURRENT_BINARY_DIR}
             ${CMAKE_CURRENT_SOURCE_DIR}/cmake_tests/tls.cpp)
if (PFUNC_TLS_COMPILED)
  message (STATUS "Checking if thread local storage is supported - found")
  set (PFUNC_HAVE_TLS 1)
else (PFUNC_TLS_COMPILED)
  message (STATUS "Checking if thread local storage is supported - not found")
endif (PFUNC_TLS_COMPILED)

# Check if sched_setaffinity is supported
if (PFUNC_LINUX AND PFUNC_HAVE_SCHED_H)
  try_run (PFUNC_SCHED_EXIT_CODE PFUNC_SCHED_COMPILED
           ${CMAKE_CURRENT_BINARY_DIR}
           ${CMAKE_CURRENT_SOURCE_DIR}/cmake_tests/sched_affinity.cpp)
  if (PFUNC_SCHED_EXIT_CODE EQUAL 0)
    message (STATUS "Checking if we have scheduler affinity support - found")
    set (PFUNC_HAVE_SCHED_AFFINITY 1)
  else (PFUNC_SCHED_EXIT_CODE EQUAL 0)
    message (STATUS "Checking if we have scheduler affinity support - not found")
  endif (PFUNC_SCHED_EXIT_CODE EQUAL 0)
endif (PFUNC_LINUX AND PFUNC_HAVE_SCHED_H)

# Check if thread level storage is supported
if (PFUNC_WINDOWS AND MSVC)
  set (INLINE_COMPILE_FLAGS "/DINLINE_STRING=inline")
  set (UNDERSCORE_INLINE_COMPILE_FLAGS "/DINLINE_STRING=__inline")
  set (UNDERSCORE_INLINE_UNDERSCORE_COMPILE_FLAGS "/DINLINE_STRING=__inline__")
else (PFUNC_WINDOWS AND MSVC)
  set (INLINE_COMPILE_FLAGS "-DINLINE_STRING=inline")
  set (UNDERSCORE_INLINE_COMPILE_FLAGS "-DINLINE_STRING=__inline")
  set (UNDERSCORE_INLINE_UNDERSCORE_COMPILE_FLAGS "-DINLINE_STRING=__inline__")
endif (PFUNC_WINDOWS AND MSVC)

try_compile (PFUNC_INLINE_COMPILE_CODE
             ${CMAKE_CURRENT_BINARY_DIR}
             ${CMAKE_CURRENT_SOURCE_DIR}/cmake_tests/inline.c
             COMPILE_DEFINITIONS ${INLINE_COMPILE_FLAGS})
if (PFUNC_INLINE_COMPILE_CODE)
  message (STATUS "Checking if inline is accepted by compiler - yes")
  set (PFUNC_HAVE_INLINE 1)
else (PFUNC_INLINE_COMPILE_CODE)
  message (STATUS "Checking if inline is accepted by compiler - no")

  try_compile (PFUNC_UNDERSCORE_INLINE_COMPILE_CODE
               ${CMAKE_CURRENT_BINARY_DIR}
               ${CMAKE_CURRENT_SOURCE_DIR}/cmake_tests/inline.c
               COMPILE_DEFINITIONS ${UNDERSCORE_INLINE_COMPILE_FLAGS})
  if (PFUNC_UNDERSCORE_INLINE_COMPILE_CODE)
    message (STATUS "Checking if __inline is accepted by compiler - yes")
    set (PFUNC_HAVE_UNDERSCORE_INLINE 1)
  else (PFUNC_UNDERSCORE_INLINE_COMPILE_CODE)
    message (STATUS "Checking if __inline is accepted by compiler - no")
    message (STATUS ${UNDERSCORE_INLINE_COMPILE_FLAGS})

    try_compile (PFUNC_UNDERSCORE_INLINE_UNDERSCORE_COMPILE_CODE
                 ${CMAKE_CURRENT_BINARY_DIR}
                 ${CMAKE_CURRENT_SOURCE_DIR}/cmake_tests/inline.c
                 COMPILE_DEFINITIONS 
                  ${UNDERSCORE_INLINE_UNDERSCORE_COMPILE_FLAGS})
    if (PFUNC_UNDERSCORE_INLINE_UNDERSCORE_COMPILE_CODE)
      message (STATUS "Checking if __inline__ is accepted by compiler - yes")
      set (PFUNC_HAVE_UNDERSCORE_INLINE_UNDERSCORE 1)
    else (PFUNC_UNDERSCORE_INLINE_UNDERSCORE_COMPILE_CODE)
      message (STATUS "Checking if __inline__ is accepted by compiler - no")
      message (WARNING "Inline support was not found in the C compiler")
    endif (PFUNC_UNDERSCORE_INLINE_UNDERSCORE_COMPILE_CODE)

  endif (PFUNC_UNDERSCORE_INLINE_COMPILE_CODE)

endif (PFUNC_INLINE_COMPILE_CODE)

# Option to enable exceptions
option (USE_EXCEPTIONS "Whether to enable exceptions" OFF)
if (USE_EXCEPTIONS)
  set (PFUNC_USE_EXCEPTIONS 
       1
       CACHE
       STRING
       "Enables exceptions to be used"
       FORCE)
endif (USE_EXCEPTIONS)

# Option to enable PAPI profiling
option (USE_PAPI "Whether to enable PAPI support" OFF)
if (USE_PAPI)
  check_include_file_cxx (papi.h PFUNC_HAVE_PAPI_H)
  if (PFUNC_HAVE_PAPI_H)
    set (PFUNC_USE_PAPI
         1
         CACHE
         STRING
         "Enables PAPI-based profiling to be used"
         FORCE)
  else (PFUNC_HAVE_PAPI_H)
    message (STATUS "PAPI support disabled as papi.h was not found")
  endif (PFUNC_HAVE_PAPI_H)
endif (USE_PAPI)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Now to set the install files
install (DIRECTORY .
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/pfunc
         PATTERN "CMake*" EXCLUDE
         PATTERN "cmake_tests" EXCLUDE
         PATTERN "config.h.in" EXCLUDE
         PATTERN "Makefile" EXCLUDE
         PATTERN ".svn" EXCLUDE)

# Make sure you install config.h also. This is not in the source directory.
install (FILES
         ${CMAKE_CURRENT_BINARY_DIR}/config.h
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/pfunc)
         
