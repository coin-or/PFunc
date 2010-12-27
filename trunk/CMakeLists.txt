###############################################################################

# Author: Prabhanjan Kambadur

###############################################################################

# We need at least CMake 2.6.0 to compile
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

###############################################################################

# Name of the project
project(PFUNC)

###############################################################################

# Version
set(PFUNC_VERSION 1.0)

###############################################################################

# Warn if the source and the binary directories are the same
if (CMAKE_SOURCE_DIR MATCHES CMAKE_BINARY_DIR)
  message (STATUS "Warning! In source builds clutter up source dirs")
endif (CMAKE_SOURCE_DIR MATCHES CMAKE_BINARY_DIR)

###############################################################################

# Add CMake to the module path
list (APPEND CMAKE_MODULE_PATH ${PFUNC_SOURCE_DIR}/CMake)

###############################################################################

# Set the install prefix
if (NOT CMAKE_INSTALL_PREFIX)
  message (STATUS "Warning! Install path not set")
  message (STATUS "Setting the install path to /usr/local")
  set (CMAKE_INSTALL_PREFIX "/usr/local/")
endif (NOT CMAKE_INSTALL_PREFIX)

###############################################################################

# Check for the presence of a C and C++ compilers
if (NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
  message (FATAL "C or C++ compilers could not be found!")
endif (NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)

###############################################################################

# Make sure that the users know that there are choices to build
if (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "None")
  # Release version is being built -- exceptions are disabled 
  message (STATUS "Building the Release version")
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
  # Debug version is being built -- exceptions are enabled 
  message (STATUS "Building the Debug version")
elseif (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
  # RelWithDebInfo version is being built -- exceptions are enabled 
  message (STATUS "Building the Release version with Debug information")
else (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "None")
  # User has not choosen anything -- so we build Release by default 
  message (STATUS "Building the Release version")
  set (CMAKE_BUILD_TYPE 
       Release 
       CACHE 
       STRING
       "Options are: None, Debug, Release, RelWithDebInfo."
       FORCE)
endif (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "None")

###############################################################################

# Add the subdirectories
add_subdirectory (pfunc)
add_subdirectory (lib)

###############################################################################

# Tutorial: do we need to build this
option (BUILD_TUTORIAL "Whether to build the PFunc tutorial" OFF)
if (BUILD_TUTORIAL)
  add_subdirectory (tutorial)
endif (BUILD_TUTORIAL)

###############################################################################

# Do we build documentation
option (BUILD_DOCS "Whether to build the PFunc documentation" OFF)
if (BUILD_DOCS)
  include (BuildDocs)
endif (BUILD_DOCS)

###############################################################################

# Examples
option (BUILD_EXAMPLES "Whether to build examples" ON)
if (BUILD_EXAMPLES)
  add_subdirectory (examples)
endif (BUILD_EXAMPLES)

###############################################################################

# Perf tests
option (BUILD_PERF_TESTS "Whether to build examples" OFF)
if (BUILD_PERF_TESTS)
  add_subdirectory (perf_tests)
endif (BUILD_PERF_TESTS)

###############################################################################

# Uninstall
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

###############################################################################
