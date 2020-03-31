#
# CMakeLists.txt -- Generate Core Adjust documentation with doxygen.
#

find_package(Doxygen QUIET)
if(NOT DEFINED BUILD_DOC)
  set(BUILD_DOC OFF)
endif()
set(BUILD_DOC ${BUILD_DOC} CACHE BOOL "Build documentation with doxygen?" FORCE)
if(BUILD_DOC)
  if(DOXYGEN_FOUND)
    configure_file(Doxyfile.in Doxyfile @ONLY)
    configure_file(custom.css.in custom.css @ONLY)
    configure_file(custom_dark_theme.css.in custom_dark_theme.css @ONLY)
    set(DOXYGEN_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    add_custom_target(doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating documentation with Doxygen"
      VERBATIM)
  else()
    message(STATUS "Generating documentation requires doxygen.")
  endif()
endif()

