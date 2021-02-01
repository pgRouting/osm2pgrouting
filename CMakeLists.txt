PROJECT(osm2pgrouting)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
SET(SHARE_DIR "${CMAKE_INSTALL_PREFIX}/share/osm2pgrouting")

FIND_PACKAGE(PostgreSQL REQUIRED)
find_package(LibPQXX REQUIRED)
FIND_PACKAGE(EXPAT REQUIRED)


FIND_PACKAGE(Boost)
if(Boost_INCLUDE_DIRS)
    message(STATUS "Boost headers were found here: ${Boost_INCLUDE_DIRS}")
else(Boost_INCLUDE_DIRS)
    message(FATAL_ERROR " Please check your Boost installation ")
endif(Boost_INCLUDE_DIRS)

FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)

FILE(GLOB osm2pgrouting_lib_SOURCES "${CMAKE_SOURCE_DIR}/src/*/*.cpp")

#---------------------------------------------
# C++ Compiler requirements
#---------------------------------------------
set(GNU_CXX_MINIMUM_VERSION "4.6")
set(MSVC_CXX_MINIMUM_VERSION "18.0")

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS GNU_CXX_MINIMUM_VERSION)
        message(FATAL_ERROR "GCC version must be at least ${GNU_CXX_MINIMUM_VERSION}!
        Found version ${CMAKE_CXX_COMPILER_VERSION}")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS MSVC_CXX_MINIMUM_VERSION)
        message(FATAL_ERROR "MSVC version must be at least ${MSVC_CXX_MINIMUM_VERSION}!")
    endif()
else()
    message(STATUS "Unknown compiler minimum version for ${CMAKE_CXX_COMPILER_ID}")
endif()

if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    message(STATUS "Using  -std=c++14.")
elseif(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    message(STATUS "Using  -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    message(STATUS "Using  -std=c++0x.")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++0x or C++11 or C++14 flags support.")
endif()



set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wconversion -pedantic -Wextra  -frounding-math -Wno-deprecated -fmax-errors=10")

if(WIN32 AND MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_DEPRECATE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_SCL_SECURE_NO_DEPRECATE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_NONSTDC_NO_DEPRECATE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -EHsc")
endif()

#--------------------------------------------------------

set (OSM2PGROUTING_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include")
message(STATUS "LIBPQXX_INCLUDE_DIRS: ${LIBPQXX_INCLUDE_DIRS}")
message(STATUS "POSTGRESQL_INCLUDE_DIR: ${POSTGRESQL_INCLUDE_DIR}")
message(STATUS "EXPAT_INCLUDE_DIRS: ${EXPAT_INCLUDE_DIRS}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "POSTGRESQL_LIBRARIES: ${POSTGRESQL_LIBRARIES}")
message(STATUS "Boost_LIBRARIES: ${boost_LIBRARIES}")
message(STATUS "LIBPQXX_LIBRARIES: ${LIBPQXX_LIBRARIES}")

INCLUDE_DIRECTORIES(src
    ${LIBPQXX_INCLUDE_DIRS}
    ${POSTGRESQL_INCLUDE_DIR}
    ${EXPAT_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${OSM2PGROUTING_INCLUDE_DIRS}
    )

ADD_EXECUTABLE(osm2pgrouting ${osm2pgrouting_lib_SOURCES})

TARGET_LINK_LIBRARIES(osm2pgrouting
    ${LIBPQXX_LIBRARIES}
    ${POSTGRESQL_LIBRARIES}
    ${EXPAT_LIBRARIES}
    ${Boost_LIBRARIES}
    )

INSTALL(TARGETS osm2pgrouting
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
    )

if(WIN32)
    target_link_libraries(osm2pgrouting wsock32 ws2_32)
endif()

INSTALL(FILES
    "${CMAKE_SOURCE_DIR}/COPYING"
    "${CMAKE_SOURCE_DIR}/README.md"
    "${CMAKE_SOURCE_DIR}/mapconfig.xml"
    "${CMAKE_SOURCE_DIR}/mapconfig_for_cars.xml"
    "${CMAKE_SOURCE_DIR}/mapconfig_for_bicycles.xml"
    "${CMAKE_SOURCE_DIR}/mapconfig_for_pedestrian.xml"
    DESTINATION "${SHARE_DIR}")


#INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
#TARGET_LINK_LIBRARIES( osm2pgrouting ${Boost_LIBRARIES} )
