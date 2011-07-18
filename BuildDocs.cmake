#
# Author: Prabhanjan Kambadur
# Check if all things required to build documentation are found. If so, just 
# go ahead and add a target that builds the documentation.
#

include (FindDoxygen)
include (FindLATEX)

# Check if we have doxygen
if (DOXYGEN_FOUND)
  # Check if we have the right tools for compiling the PDF
  if (LATEX_COMPILER AND DVIPS_CONVERTER AND 
      PS2PDF_CONVERTER AND MAKEINDEX_COMPILER)
    message (STATUS "Found the tools required to build documentation")

    # Now, configure the project from the .in file and place in bindir
    if (EXISTS "${PFUNC_SOURCE_DIR}/doc/doxygen.config.in")
      configure_file (${PFUNC_SOURCE_DIR}/doc/doxygen.config.in
                      ${PFUNC_BINARY_DIR}/doc/doxygen.config
                      @ONLY)
      add_custom_target (doc 
                         ${DOXYGEN_EXECUTABLE}
                         ${PFUNC_BINARY_DIR}/doc/doxygen.config)
    else (EXISTS "${PFUNC_SOURCE_DIR}/doc/doxygen.config.in")
      message (STATUS "Configuration file not found. Cannot build docs")
    endif (EXISTS "${PFUNC_SOURCE_DIR}/doc/doxygen.config.in")
  else (LATEX_COMPILER AND DVIPS_CONVERTER AND 
      PS2PDF_CONVERTER AND MAKEINDEX_COMPILER)
    message (STATUS "Required tools for building the PDF not found")
  endif (LATEX_COMPILER AND DVIPS_CONVERTER AND 
      PS2PDF_CONVERTER AND MAKEINDEX_COMPILER)
else (DOXYGEN_FOUND)
  message (STATUS "Error: Doxygen not found, cannot build documentation")
endif (DOXYGEN_FOUND)
