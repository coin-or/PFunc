cmake_minimum_required (VERSION 2.6)

# Add the pfunc source directory to be in the include directory path.
# Add the pfunc binary directory to be in the include directory path.
# This is done so that the generated header files can also be found.
include_directories (BEFORE ${PFUNC_SOURCE_DIR} ${PFUNC_BINARY_DIR})

add_executable (fibonacci fibonacci.cpp)
add_dependencies (fibonacci pfunc)
if (NOT CMAKE_SYSTEM MATCHES "Windows")
  target_link_libraries (fibonacci pthread)
endif (NOT CMAKE_SYSTEM MATCHES "Windows")

add_executable (matmult matmult.cpp)
add_dependencies (matmult pfunc)
if (NOT CMAKE_SYSTEM MATCHES "Windows")
  target_link_libraries (matmult pthread)
endif (NOT CMAKE_SYSTEM MATCHES "Windows")

add_executable (for for.cpp)
add_dependencies (for pfunc)
if (NOT CMAKE_SYSTEM MATCHES "Windows")
  target_link_libraries (for pthread)
endif (NOT CMAKE_SYSTEM MATCHES "Windows")

add_custom_target (cxx_examples ALL)
add_dependencies (cxx_examples fibonacci matmult for)
