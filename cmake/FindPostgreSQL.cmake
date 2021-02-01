# - Find PostgreSQL
# Find the PostgreSQL includes and client library
# This module defines
#  POSTGRESQL_INCLUDE_DIR, where to find POSTGRESQL.h
#  POSTGRESQL_LIBRARIES, the libraries needed to use POSTGRESQL.
#  POSTGRESQL_FOUND, If false, do not try to use PostgreSQL.

# Copyright (c) 2006, Jaroslaw Staniek, <js@iidea.pl>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIBRARIES)
    set(POSTGRESQL_FOUND TRUE)

else(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIBRARIES)

    find_program(POSTGRESQL_PG_CONFIG NAMES pg_config
        PATHS
        /usr/lib/postgresql/*/bin/
        )
    message(STATUS "POSTGRESQL_PG_CONFIG is " ${POSTGRESQL_PG_CONFIG})

    if(POSTGRESQL_PG_CONFIG)
        execute_process(
            COMMAND ${POSTGRESQL_PG_CONFIG} --includedir
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE T_POSTGRESQL_INCLUDE_DIR)
    endif(POSTGRESQL_PG_CONFIG)

    find_path(POSTGRESQL_INCLUDE_DIR libpq-fe.h
        ${T_POSTGRESQL_INCLUDE_DIR}
		/usr/pgsql-*/include
        /usr/include
        /usr/include/pgsql
        /usr/local/include/pgsql
        /usr/include/postgresql
        /usr/include/postgresql/*
        /usr/local/include/postgresql
        /usr/local/include/postgresql/*
        $ENV{ProgramFiles}/PostgreSQL/*/include
        $ENV{SystemDrive}/PostgreSQL/*/include
        )

    if(POSTGRESQL_PG_CONFIG)
        execute_process(
            COMMAND ${POSTGRESQL_PG_CONFIG} --libdir
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE T_POSTGRESQL_LIB_DIR)
    endif(POSTGRESQL_PG_CONFIG)

    find_library(POSTGRESQL_LIBRARIES NAMES pq libpq
        ${T_POSTGRESQL_LIB_DIR}
        PATHS
		/usr/pgsql-*/lib
        /usr/lib
        /usr/local/lib
        /usr/lib/postgresql
        $ENV{ProgramFiles}/PostgreSQL/*/lib
        $ENV{SystemDrive}/PostgreSQL/*/lib
        )

    if(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIBRARIES)
        set(POSTGRESQL_FOUND TRUE)
        message(STATUS "POSTGRESQL_INCLUDE_DIR: ${POSTGRESQL_INCLUDE_DIR}")
        message(STATUS "POSTGRESQL_LIBRARIES: ${POSTGRESQL_LIBRARIES}")
        include_directories(${POSTGRESQL_INCLUDE_DIR})
    else(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIBRARIES)
        set(POSTGRESQL_FOUND FALSE)
        message(STATUS "PostgreSQL not found.")
    endif(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIBRARIES)

    mark_as_advanced(POSTGRESQL_INCLUDE_DIR POSTGRESQL_LIBRARIES)

endif(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIBRARIES)
