cmake_minimum_required(VERSION 2.8)
project (NewBioroute)
set (NewBioroute_VERSION_MAJOR 2)
set (NewBioroute_VERSION_MINOR 0)
set(CMAKE_CXX_COMPILER g++-4.2)
set(CMAKE_OSX_ARCHITECTURES x86_64 )
set(CMAKE_CXX_FLAGS "-DMACOSX")
set(CMAKE_C_FLAGS -DMACOSX)
set(CMAKE_CXX_FLAGS "-g -Wall")
#set(BIOGEME_FOLDER "/Users/jchen/Documents/Project/biogeme")

#SET(XMLWRAPP_CXX_FLAGS "-g `xmlwrapp-config --cxxflags`")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/../libs/biogeme/include")
include_directories("${PROJECT_SOURCE_DIR}/../libs/biogeme/randomNumbers")
include_directories("${PROJECT_SOURCE_DIR}/../libs/biogeme/parameters")
include_directories("/opt/local/include")
include_directories("/usr/local/include")
include_directories("${PROJECT_BINARY_DIR}")
include_directories("/usr/local/boost_1_49_0")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(POSTGRES)
find_package(PQXX)
find_package(GSL)
find_package(SHP)
find_package(KML)
find_package(Boost)

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREAD ON)


find_library(LIB_UTILS patUtils "${PROJECT_SOURCE_DIR}/../libs/biogeme/utils/")
find_library(LIB_RANDOMNUMBER patRandomNumbers "${PROJECT_SOURCE_DIR}/../libs/biogeme/randomNumbers")
find_library(LIB_PARAMETERS patParam "${PROJECT_SOURCE_DIR}/../libs/biogeme/parameters")
find_library(LIB_XMLWRAPP xmlwrapp)

set(LIB_EXTRA ${PQXX_LIBRARIES} ${SHP_LIBRARIES} ${KML_LIBRARIES} ${GSLCBLAS_LIBRARIES} ${GSL_CONFIG_LIBS}
     ${LIB_XMLWRAPP} ${LIB_UTILS} ${LIB_PARAMETERS})

#ADD_DEFINITIONS( "-g `xmlwrapp-config --cxxflags`")
add_subdirectory(Utilities)
add_subdirectory(NetworkHandler)
add_subdirectory(ShortestPath)
add_subdirectory(MapMatching)
add_subdirectory(Math)
add_subdirectory(PathGeneration/MetropolishHastings)
add_subdirectory(bin)
add_subdirectory(test)
configure_file(params ${CMAKE_CURRENT_BINARY_DIR}/params COPYONLY)


#target_link_libraries (CheckDatabase  ${LIB_EXTRA})
