# We need at least CMake 2.6 to compile
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# Check for the following programs that are needed to compile the pfunc
include (FindLATEX)
include (FindPerl)

# Set the source files for the pfunc
set (DAG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/refs.bib 
    ${CMAKE_CURRENT_SOURCE_DIR}/pfunc.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/pfunc-defs.tex)

set (DAG_CLEAN
    ${CMAKE_CURRENT_BINARY_DIR}/error
    ${CMAKE_CURRENT_BINARY_DIR}/pfunc-defs.aux
    ${CMAKE_CURRENT_BINARY_DIR}/pfunc.aux
    ${CMAKE_CURRENT_BINARY_DIR}/pfunc.bbl
    ${CMAKE_CURRENT_BINARY_DIR}/pfunc.blg
    ${CMAKE_CURRENT_BINARY_DIR}/pfunc.log
    ${CMAKE_CURRENT_BINARY_DIR}/pfunc.dvi
    ${CMAKE_CURRENT_BINARY_DIR}/pfunc.ps
    ${CMAKE_CURRENT_BINARY_DIR}/pfunc.pdf)

# Set the fig files for the pfunc 
set (DAG_FIGS
    ${CMAKE_CURRENT_SOURCE_DIR}/dagresults/dag_memory.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/dagresults/dag_speedup.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/cgresults/cg_8.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/fimresults/fim_8.eps)

# Print out all the things that were found
if (LATEX_COMPILER AND BIBTEX_COMPILER AND 
      DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)
  message (STATUS "Found LATEX, BIBTEX, DVIPS and PS2PDF compilers")

  # Set the tex to dvi perl script
  set (TEX_TO_DVI ${CMAKE_CURRENT_SOURCE_DIR}/../CMake/tex_to_dvi.pl)
  set (COPY_TEX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../CMake/copy_tex_sources.pl)
  
  # Now, we define a bunch of custom commands that will result in the 
  # pfunc getting built 
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pfunc.pdf
    DEPENDS ${DAG_SOURCES} ${DAG_FIGS}
    COMMAND ${PERL_EXECUTABLE}
    ARGS ${COPY_TEX_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR} 
                             ${CMAKE_CURRENT_BINARY_DIR}
                             cgresults dagresults fimresults figs
    COMMAND ${PERL_EXECUTABLE}
    ARGS ${TEX_TO_DVI}
            ${LATEX_COMPILER} ${BIBTEX_COMPILER} 
              ${CMAKE_CURRENT_BINARY_DIR}/pfunc
    COMMAND ${DVIPS_CONVERTER} 
    ARGS ${CMAKE_CURRENT_BINARY_DIR}/pfunc.dvi
         -o ${CMAKE_CURRENT_BINARY_DIR}/pfunc.ps
    COMMAND ${PS2PDF_CONVERTER} 
    ARGS ${CMAKE_CURRENT_BINARY_DIR}/pfunc.ps)

  # Now, add a custom target
  add_custom_target (sc ALL 
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pfunc.pdf)

  # Now, add the clean files
  add_custom_target (sc_clean
    ${CMAKE_COMMAND} -E remove -f ${DAG_CLEAN})

else (LATEX_COMPILER AND BIBTEX_COMPILER AND 
        DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)
  message (STATUS "Cannot compile pfunc -- required compilers NOT found")
endif (LATEX_COMPILER AND BIBTEX_COMPILER AND 
        DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)

# Install the file in the right place
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/pfunc.pdf
         DESTINATION ${CMAKE_INSTALL_PREFIX}/papers
         CONFIGURATIONS Release Debug RelWithDebugInfo
         OPTIONAL)
