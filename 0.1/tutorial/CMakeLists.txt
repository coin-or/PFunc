# We need at least CMake 2.6 to compile
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# Check for the following programs that are needed to compile the tutorial
include (FindLATEX)
include (FindPerl)

# Set the source files for the tutorial
set (TUTORIAL_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/tutorial.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/tutorial-defs.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/introduction.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/generate.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/initialize.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/spawn.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/fibonacci.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/pack.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/group.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/exception.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/perf.tex)

set (TUTORIAL_CLEAN
    ${CMAKE_CURRENT_BINARY_DIR}/error
    ${CMAKE_CURRENT_BINARY_DIR}/tutorial-defs.aux
    ${CMAKE_CURRENT_BINARY_DIR}/tutorial.aux
    ${CMAKE_CURRENT_BINARY_DIR}/tutorial.log
    ${CMAKE_CURRENT_BINARY_DIR}/tutorial.dvi
    ${CMAKE_CURRENT_BINARY_DIR}/tutorial.ps
    ${CMAKE_CURRENT_BINARY_DIR}/tutorial.pdf)

# Set the fig files for the tutorial 
set (TUTORIAL_FIGS
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/attributes.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/functors.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/group.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/life-cycle.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/organization.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/pfunc.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/overview.eps)

# Print out all the things that were found
if (LATEX_COMPILER AND DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)
  message (STATUS "Found LATEX, DVIPS and PS2PDF compilers")

  # Compiler for tex to dvi
  set (TEX_TO_DVI ${CMAKE_CURRENT_SOURCE_DIR}/../CMake/tex_to_dvi.pl)
  set (COPY_TEX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../CMake/copy_tex_sources.pl)
  
  # Now, we define a bunch of custom commands that will result in the 
  # tutorial getting built 
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tutorial.pdf
    DEPENDS ${TUTORIAL_SOURCES} ${TUTORIAL_FIGS}
    COMMAND ${PERL_EXECUTABLE}
    ARGS ${COPY_TEX_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR} 
                             ${CMAKE_CURRENT_BINARY_DIR}
                             figs
    COMMAND ${PERL_EXECUTABLE}
    ARGS ${TEX_TO_DVI} ${LATEX_COMPILER} ${CMAKE_CURRENT_BINARY_DIR}/tutorial
    COMMAND ${DVIPS_CONVERTER} 
    ARGS ${CMAKE_CURRENT_BINARY_DIR}/tutorial.dvi 
         -o ${CMAKE_CURRENT_BINARY_DIR}/tutorial.ps
    COMMAND ${PS2PDF_CONVERTER} 
    ARGS ${CMAKE_CURRENT_BINARY_DIR}/tutorial.ps)

  # Now, add a custom target
  add_custom_target (tutorial ALL 
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tutorial.pdf)

  # Now, add the clean files
  add_custom_target (tutorial_clean
    ${CMAKE_COMMAND} -E remove -f ${TUTORIAL_CLEAN})

else (LATEX_COMPILER AND DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)
  message (STATUS "Cannot compile tutorial -- required compilers NOT found")
endif (LATEX_COMPILER AND DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)

# Install the file in the right place
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/tutorial.pdf
         DESTINATION ${CMAKE_INSTALL_PREFIX}/tutorial
         CONFIGURATIONS Release Debug RelWithDebugInfo
         OPTIONAL)
