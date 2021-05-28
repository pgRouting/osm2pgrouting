cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

PROJECT(osm2pgrouting)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
SET(SHARE_DIR "${CMAKE_INSTALL_PREFIX}/share/osm2pgrouting")

find_package(PostgreSQL REQUIRED)
find_package(PQXX REQUIRED)
include_directories(${PQXX_INCLUDE_DIR})
find_package(EXPAT REQUIRED)


FILE(GLOB osm2pgrouting_lib_SOURCES "${CMAKE_SOURCE_DIR}/src/*/*.cpp")

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
message(STATUS "PQXX_INCLUDE_DIR: ${PQXX_INCLUDE_DIR}")
message(STATUS "POSTGRESQL_INCLUDE_DIR: ${POSTGRESQL_INCLUDE_DIR}")
message(STATUS "EXPAT_INCLUDE_DIRS: ${EXPAT_INCLUDE_DIRS}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "POSTGRESQL_LIBRARIES: ${POSTGRESQL_LIBRARIES}")
message(STATUS "Boost_LIBRARIES: ${boost_LIBRARIES}")
message(STATUS "PQXX_LIBRARIES: ${PQXX_LIBRARIES}")

INCLUDE_DIRECTORIES(src
    ${POSTGRESQL_INCLUDE_DIR}
    ${EXPAT_INCLUDE_DIRS}
    ${OSM2PGROUTING_INCLUDE_DIRS}
    )

ADD_EXECUTABLE(osm2pgrouting ${osm2pgrouting_lib_SOURCES})

TARGET_LINK_LIBRARIES(osm2pgrouting
    ${PQXX_LIBRARIES}
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
