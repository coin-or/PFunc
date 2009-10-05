# We need at least CMake 2.6 to compile
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# Check for the following programs that are needed to compile the dag
include (FindLATEX)
include (FindPerl)

# Set the source files for the dag
set (DAG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/refs.bib 
    ${CMAKE_CURRENT_SOURCE_DIR}/dag.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/dag-defs.tex)

set (DAG_CLEAN
    ${CMAKE_CURRENT_BINARY_DIR}/error
    ${CMAKE_CURRENT_BINARY_DIR}/dag-defs.aux
    ${CMAKE_CURRENT_BINARY_DIR}/dag.aux
    ${CMAKE_CURRENT_BINARY_DIR}/dag.bbl
    ${CMAKE_CURRENT_BINARY_DIR}/dag.blg
    ${CMAKE_CURRENT_BINARY_DIR}/dag.log
    ${CMAKE_CURRENT_BINARY_DIR}/dag.dvi
    ${CMAKE_CURRENT_BINARY_DIR}/dag.ps
    ${CMAKE_CURRENT_BINARY_DIR}/dag.pdf)

# Set the fig files for the dag 
set (DAG_FIGS
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/dag.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/dag_memory.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/dag_speedup.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/dataflow.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/deadlock.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/fib.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/load_balance.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/lu.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/memory_g7jac200.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/memory_twotone.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/openmp.eps
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/stealing.eps)

# Print out all the things that were found
if (LATEX_COMPILER AND BIBTEX_COMPILER AND 
      DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)
  message (STATUS "Found LATEX, BIBTEX, DVIPS and PS2PDF compilers")
  
  # Set the tex to dvi perl script
  set (TEX_TO_DVI ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/tex_to_dvi.pl)
  set (COPY_TEX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/copy_tex_sources.pl)

  # Now, we define a bunch of custom commands that will result in the 
  # dag getting built 
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dag.pdf
    DEPENDS ${DAG_SOURCES} ${DAG_FIGS}
    COMMAND ${PERL_EXECUTABLE}
    ARGS ${COPY_TEX_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR} 
                             ${CMAKE_CURRENT_BINARY_DIR}
                             figs
    COMMAND ${PERL_EXECUTABLE}
    ARGS ${TEX_TO_DVI}
            ${LATEX_COMPILER} ${BIBTEX_COMPILER} 
              ${CMAKE_CURRENT_BINARY_DIR}/dag
    COMMAND ${DVIPS_CONVERTER} 
    ARGS ${CMAKE_CURRENT_BINARY_DIR}/dag.dvi
         -o ${CMAKE_CURRENT_BINARY_DIR}/dag.ps
    COMMAND ${PS2PDF_CONVERTER} 
    ARGS ${CMAKE_CURRENT_BINARY_DIR}/dag.ps)

  # Now, add a custom target
  add_custom_target (hipc ALL 
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dag.pdf)

  # Now, add the clean files
  add_custom_target (hipc_clean
    ${CMAKE_COMMAND} -E remove -f ${DAG_CLEAN})

else (LATEX_COMPILER AND BIBTEX_COMPILER AND 
        DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)
  message (STATUS "Cannot compile dag -- required compilers NOT found")
endif (LATEX_COMPILER AND BIBTEX_COMPILER AND 
        DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)

# Install the file in the right place
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/dag.pdf
         DESTINATION ${CMAKE_INSTALL_PREFIX}/papers
         CONFIGURATIONS Release Debug RelWithDebugInfo
         OPTIONAL)
