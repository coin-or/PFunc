cmake_minimum_required (VERSION 2.6)

# Add the pfunc source directory to be in the include directory path.
# Add the pfunc binary directory to be in the include directory path.
# This is done so that the generated header files can also be found.
include_directories (BEFORE ${PFUNC_SOURCE_DIR} ${PFUNC_BINARY_DIR})

# Make sure that we use the C++ compiler
SET_SOURCE_FILES_PROPERTIES(groups.c PROPERTIES LANGUAGE CXX)
SET_SOURCE_FILES_PROPERTIES(simple.c PROPERTIES LANGUAGE CXX)

add_executable (groups groups.c)
add_dependencies (groups pfunc)
target_link_libraries (groups pfunc)
if (NOT CMAKE_SYSTEM MATCHES "Windows")
  target_link_libraries (groups pthread)
endif (NOT CMAKE_SYSTEM MATCHES "Windows")

add_executable (simple simple.c)
add_dependencies (simple pfunc)
target_link_libraries (simple pfunc)
if (NOT CMAKE_SYSTEM MATCHES "Windows")
  target_link_libraries (simple pthread)
endif (NOT CMAKE_SYSTEM MATCHES "Windows")

add_custom_target (c_examples ALL)
add_dependencies (c_examples groups simple)
