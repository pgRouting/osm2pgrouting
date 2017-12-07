PROJECT(osm2pgrouting)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set (SHARE_DIR "/usr/share/osm2pgrouting")

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

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -std=c++14")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wconversion -pedantic -Wextra  -frounding-math -Wno-deprecated -fmax-errors=10")


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
    RUNTIME DESTINATION "/usr/bin"
    )

if(WIN32)
    target_link_libraries(osm2pgrouting wsock32 ws2_32)
endif()

INSTALL(FILES
    "${CMAKE_SOURCE_DIR}/COPYING"
    "${CMAKE_SOURCE_DIR}/Readme.md"
    "${CMAKE_SOURCE_DIR}/mapconfig.xml"
    "${CMAKE_SOURCE_DIR}/mapconfig_for_cars.xml"
    "${CMAKE_SOURCE_DIR}/mapconfig_for_bicycles.xml"
    "${CMAKE_SOURCE_DIR}/mapconfig_for_pedestrian.xml"
    DESTINATION "${SHARE_DIR}")


#INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
#TARGET_LINK_LIBRARIES( osm2pgrouting ${Boost_LIBRARIES} )
