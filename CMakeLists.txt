# We need at least CMake 2.6 to compile
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# Check for the following programs that are needed to compile the itemset
include (FindLATEX)
include (FindPerl)

# Set the source files for the itemset
set (DAG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/refs.bib 
    ${CMAKE_CURRENT_SOURCE_DIR}/itemset.tex 
    ${CMAKE_CURRENT_SOURCE_DIR}/itemset-defs.tex)

set (DAG_CLEAN
    ${CMAKE_CURRENT_BINARY_DIR}/error
    ${CMAKE_CURRENT_BINARY_DIR}/itemset-defs.aux
    ${CMAKE_CURRENT_BINARY_DIR}/itemset.aux
    ${CMAKE_CURRENT_BINARY_DIR}/itemset.bbl
    ${CMAKE_CURRENT_BINARY_DIR}/itemset.blg
    ${CMAKE_CURRENT_BINARY_DIR}/itemset.log
    ${CMAKE_CURRENT_BINARY_DIR}/itemset.dvi
    ${CMAKE_CURRENT_BINARY_DIR}/itemset.ps
    ${CMAKE_CURRENT_BINARY_DIR}/itemset.pdf)

# Set the fig files for the itemset 
set (DAG_FIGS
    ${CMAKE_CURRENT_SOURCE_DIR}/figs/fim_8.eps)

# Print out all the things that were found
if (LATEX_COMPILER AND BIBTEX_COMPILER AND 
      DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)
  message (STATUS "Found LATEX, BIBTEX, DVIPS and PS2PDF compilers")
  
  # Set the tex to dvi perl script
  set (TEX_TO_DVI ${CMAKE_CURRENT_SOURCE_DIR}/../CMake/tex_to_dvi.pl)
  set (COPY_TEX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../CMake/copy_tex_sources.pl)

  # Now, we define a bunch of custom commands that will result in the 
  # itemset getting built 
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/itemset.pdf
    DEPENDS ${DAG_SOURCES} ${DAG_FIGS}
    COMMAND ${PERL_EXECUTABLE}
    ARGS ${COPY_TEX_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR} 
                             ${CMAKE_CURRENT_BINARY_DIR}
                             figs
    COMMAND ${PERL_EXECUTABLE}
    ARGS ${TEX_TO_DVI}
            ${LATEX_COMPILER} ${BIBTEX_COMPILER} 
              ${CMAKE_CURRENT_BINARY_DIR}/itemset
    COMMAND ${DVIPS_CONVERTER} 
    ARGS ${CMAKE_CURRENT_BINARY_DIR}/itemset.dvi
         -o ${CMAKE_CURRENT_BINARY_DIR}/itemset.ps
    COMMAND ${PS2PDF_CONVERTER} 
    ARGS ${CMAKE_CURRENT_BINARY_DIR}/itemset.ps)

  # Now, add a custom target
  add_custom_target (parco ALL 
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/itemset.pdf)

  # Now, add the clean files
  add_custom_target (parco_clean
    ${CMAKE_COMMAND} -E remove -f ${DAG_CLEAN})

else (LATEX_COMPILER AND BIBTEX_COMPILER AND 
        DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)
  message (STATUS "Cannot compile itemset -- required compilers NOT found")
endif (LATEX_COMPILER AND BIBTEX_COMPILER AND 
        DVIPS_CONVERTER AND PS2PDF_CONVERTER AND PERL_FOUND)

# Install the file in the right place
install (FILES $CMAKE_CURRENT_BINARY_DIR}/itemset.pdf
         DESTINATION ${CMAKE_INSTALL_PREFIX}/papers
         CONFIGURATIONS Release Debug RelWithDebugInfo
         OPTIONAL)
