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

add_executable (reduce reduce.cpp)
add_dependencies (reduce pfunc)
if (NOT CMAKE_SYSTEM MATCHES "Windows")
  target_link_libraries (reduce pthread)
endif (NOT CMAKE_SYSTEM MATCHES "Windows")

##############################################################################
# For parallel_while loop demonstration
include(FindBISON)
include(FindFLEX)

find_package(BISON)
find_package(FLEX)

if (FLEX_FOUND AND BISON_FOUND)
  BISON_TARGET (MyParser 
                dot_reader/dot.y 
                ${CMAKE_CURRENT_BINARY_DIR}/dot_parser.cpp)
  FLEX_TARGET (MyScanner 
               dot_reader/dot.l 
               ${CMAKE_CURRENT_BINARY_DIR}/dot_lexer.cpp)
  ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

  include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  add_executable(while while.cpp 
                 ${BISON_MyParser_OUTPUTS} 
                 ${FLEX_MyScanner_OUTPUTS})
else (FLEX_FOUND AND BISON_FOUND)
  message (STATUS "Did not find FLEX and YACC -- skipping while")
endif (FLEX_FOUND AND BISON_FOUND)
##############################################################################

add_custom_target (cxx_examples ALL)
add_dependencies (cxx_examples fibonacci matmult for while)
