# - Find libpqxx
#   Find the libpqxx includes and client library
# This module defines
#  PQXX_INCLUDE_DIRS
#  PQXX_LIBRARIES
#  PQXX_FOUND

include (FindPackageHandleStandardArgs)

#
# Look for an installation.
#
find_path(
  PQXX_INCLUDE_DIR
  NAMES pqxx/pqxx
  PATHS
    ${_PQXX_DIR}/include
    ${_PQXX_DIR}
    ${CMAKE_INSTALL_PREFIX}/include
    /usr/local/pgsql/include
    /usr/local/include
    /usr/include
  DOC "pqxx include directories"
  )
mark_as_advanced (PQXX_INCLUDE_DIR)

find_library (PQXX_LIBRARIES
  NAMES pqxx
  DOC "pqxx library"
  )
mark_as_advanced (PQXX_LIBRARY)


if (PQXX_INCLUDE_DIR)
  if (EXISTS "${PQXX_INCLUDE_DIR}/pqxx/version.hxx")
    set (PQXX_VERSION_FILE "${PQXX_INCLUDE_DIR}/pqxx/version.hxx")
    message(STATUS "PQXX_VERSION_FILE=${PQXX_VERSION_FILE}")
    file(READ "${PQXX_VERSION_FILE}" PQXX_FILE)
    string(REGEX MATCH "PQXX_VERSION \"([0-9]*).([0-9]*).([0-9]*)" PQXX_VERSION_LINE ${PQXX_FILE})
    string(REGEX REPLACE "PQXX_VERSION \"([0-9]*).([0-9]*).([0-9]*)" "\\1" PQXX_VERSION_MAYOR ${PQXX_VERSION_LINE})
    string(REGEX REPLACE "PQXX_VERSION \"([0-9]*).([0-9]*).([0-9]*)" "\\2" PQXX_VERSION_MINOR ${PQXX_VERSION_LINE})
    string(REGEX REPLACE "PQXX_VERSION \"([0-9]*).([0-9]*).([0-9]*)" "\\3" PQXX_VERSION_MICRO ${PQXX_VERSION_LINE})
    set(PQXX_VERSION "${PQXX_VERSION_MAYOR}.${PQXX_VERSION_MINOR}.${PQXX_VERSION_MICRO}")
    message(STATUS "PQXX_VERSION=${PQXX_VERSION}")
    message(STATUS "PQXX_VERSION_MAYOR=${PQXX_VERSION_MAYOR}")
    message(STATUS "PQXX_VERSION_MINOR=${PQXX_VERSION_MINOR}")
    message(STATUS "PQXX_VERSION_MICRO=${PQXX_VERSION_MICRO}")
    unset(PQXX_VERSION_MAYOR)
    unset(PQXX_VERSION_MINOR)
    unset(PQXX_VERSION_MICRO)
    unset(PQXX_FILE)
    unset(PQXX_VERSION_LINE)
  endif()
endif()



FIND_PACKAGE_HANDLE_STANDARD_ARGS("PQXX"
  VERSION_VAR PQXX_VERSION
  REQUIRED_VARS PQXX_LIBRARIES PQXX_INCLUDE_DIR
  FAIL_MESSAGE "libpqxx couldn't be found"
  )


