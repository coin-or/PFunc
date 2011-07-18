# Build the library
# Author: Prabhanjan Kambadur

# Add the pfunc source directory to be in the include directory path.
# Add the pfunc binary directory to be in the include directory path.
# This is done so that the generated header files can also be found.
include_directories (BEFORE ${PFUNC_SOURCE_DIR} ${PFUNC_BINARY_DIR})

# Add the library
add_library (pfunc STATIC pfunc_c.cpp pfunc_pack.cpp)

# Ensure that config.h was built before this
set_source_files_properties (pfunc_c.cpp pfunc_pack.cpp
                             OBJECT_DEPENDS 
                              ${CMAKE_CURRENT_BINARY_DIR}/../pfunc/config.h)

# Install the library
install (TARGETS pfunc 
         ARCHIVE 
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
