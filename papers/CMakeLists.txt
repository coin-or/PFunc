# This just adds a custom target which includes all the other examples

cmake_minimum_required (VERSION 2.6)

# Add the SC paper
add_subdirectory (sc)

# Add the HiPC paper
add_subdirectory (hipc)

# Add the ParCO paper
add_subdirectory (parco)

add_custom_target (papers ALL)
add_dependencies (papers sc parco hipc)
