PROJECT(getrestirction)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
SET(SHARE_DIR "/usr/share/getrestrictions")

FIND_PACKAGE(PostgreSQL REQUIRED)
find_package(LibPQXX REQUIRED)
FIND_PACKAGE(EXPAT REQUIRED)


find_package(Osmium REQUIRED COMPONENTS io pbf xml)
if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})






FIND_PACKAGE(Boost)
if(Boost_INCLUDE_DIRS)
    message(STATUS "Boost headers were found here: ${Boost_INCLUDE_DIRS}")
else(Boost_INCLUDE_DIRS)
    message(FATAL_ERROR " Please check your Boost installation ") 
endif(Boost_INCLUDE_DIRS)

FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)

FILE(GLOB SRC "${CMAKE_SOURCE_DIR}/src/*.cpp"  "${CMAKE_SOURCE_DIR}/src/*/*.cpp")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -std=c++0x")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -Wconversion -pedantic -Wextra  -frounding-math -Wno-deprecated -fmax-errors=10")


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

ADD_EXECUTABLE(getrestrictions ${SRC})

TARGET_LINK_LIBRARIES(getrestrictions
    ${LIBPQXX_LIBRARIES}
    ${POSTGRESQL_LIBRARIES}
    ${EXPAT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${OSMIUM_LIBRARIES}
    )

INSTALL(TARGETS getrestrictions
    RUNTIME DESTINATION "/usr/bin"
    )

