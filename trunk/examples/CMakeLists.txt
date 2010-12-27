# This just adds a custom target which includes all the other examples

cmake_minimum_required (VERSION 2.6)

# Add the C examples
add_subdirectory (c)

# Add the C++ examples
add_subdirectory (cxx)

add_custom_target (examples ALL)
add_dependencies (examples c_examples cxx_examples)
