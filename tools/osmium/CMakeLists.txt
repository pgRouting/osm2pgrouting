cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

PROJECT(getrestrictions VERSION 3.0.0
  LANGUAGES C CXX)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
SET(SHARE_DIR "/usr/share/getrestrictions")

find_package(PostgreSQL REQUIRED)
find_package(PQXX REQUIRED)
include_directories(${PQXX_INCLUDE_DIR})
find_package(EXPAT REQUIRED)


find_package(Osmium REQUIRED COMPONENTS io pbf xml)
if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})


FILE(GLOB SRC "${CMAKE_SOURCE_DIR}/src/*.cpp"  "${CMAKE_SOURCE_DIR}/src/*/*.cpp")

#---------------------------------------------
# C++ Compiler requirements
#---------------------------------------------
#---------------------------------------------

#---------------------------------------------
# Boost
#---------------------------------------------
#---------------------------------------------
find_package(Boost ${BOOST_MINIMUM_VERSION} REQUIRED COMPONENTS program_options)
if (NOT Boost_VERSION_MACRO)
  set(Boost_VERSION_MACRO ${Boost_VERSION})
endif()
add_definitions(-DBoost_VERSION_MACRO=${Boost_VERSION_MACRO})
add_definitions(-DBOOST_ALLOW_DEPRECATED_HEADERS)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})


message(STATUS "PQXX_VERSION=${PQXX_VERSION}")
if (PQXX_VERSION VERSION_GREATER_EQUAL "7.0.0")
  set(CMAKE_CXX_STANDARD 17)
else()
  add_definitions(-DPQXX_DISCONNECT)
  set(CMAKE_CXX_STANDARD 14)
endif()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -Wconversion -pedantic -Wextra  -frounding-math -Wno-deprecated")

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
message(STATUS "PQXX_INCLUDE_DIR: ${PQXX_INCLUDE_DIR}")
message(STATUS "POSTGRESQL_INCLUDE_DIR: ${POSTGRESQL_INCLUDE_DIR}")
message(STATUS "EXPAT_INCLUDE_DIRS: ${EXPAT_INCLUDE_DIRS}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "POSTGRESQL_LIBRARIES: ${POSTGRESQL_LIBRARIES}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "PQXX_LIBRARIES: ${PQXX_LIBRARIES}")

INCLUDE_DIRECTORIES(src
    ${POSTGRESQL_INCLUDE_DIR}
    ${EXPAT_INCLUDE_DIRS}
    ${OSM2PGROUTING_INCLUDE_DIRS}
    )

ADD_EXECUTABLE(getrestrictions ${SRC})

TARGET_LINK_LIBRARIES(getrestrictions
    ${PQXX_LIBRARIES}
    ${POSTGRESQL_LIBRARIES}
    ${EXPAT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${OSMIUM_LIBRARIES}
    )

INSTALL(TARGETS getrestrictions
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
    )

if(WIN32)
    target_link_libraries(getrestrictions wsock32 ws2_32)
endif()
