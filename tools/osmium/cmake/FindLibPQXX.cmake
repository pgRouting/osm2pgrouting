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
find_path(LIBPQXX_INCLUDE_DIR NAMES /include/pqxx/connection.hxx PATH_SUFFIXES ${SUFFIX_FOR_INCLUDE_PATH} PATHS

    # Look in other places.
    ${LIBPQXX_DIR_SEARCH}

    # Help the user find it if we cannot.
    DOC "The ${LIBPQXX_INCLUDE_DIR_MESSAGE}"
    )

# Now try to get the include and library path.
if(LIBPQXX_INCLUDE_DIR)

    if(EXISTS "${LIBPQXX_INCLUDE_DIR}")
        set(LIBPQXX_INCLUDE_DIRS
            ${LIBPQXX_INCLUDE_DIR}/include
            )
    endif()
endif()

find_library (LIBPQXX_LIBRARIES
    NAMES
    pqxx
    )

FIND_PACKAGE_HANDLE_STANDARD_ARGS("PQXX"
    "libpqxx couldn't be found"
    LIBPQXX_LIBRARIES
    LIBPQXX_INCLUDE_DIRS
    )

mark_as_advanced (PQXX_INCLUDE_DIR PQXX_LIBRARY)
