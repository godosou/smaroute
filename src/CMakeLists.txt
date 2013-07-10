cmake_minimum_required(VERSION 2.8)
project (NewBioroute)
set (NewBioroute_VERSION_MAJOR 2)
set (NewBioroute_VERSION_MINOR 0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message(${CMAKE_SYSTEM_NAME})
	set(CMAKE_CXX_COMPILER g++)
	set(CMAKE_OSX_ARCHITECTURES x86_64 )
	set(CMAKE_CXX_FLAGS "-DMACOSX")
	set(CMAKE_C_FLAGS -DMACOSX)
	set(CMAKE_CXX_FLAGS "-g -Wall  ")
   # Mac OS X specific code
   SET(OperatingSystem "Mac OS X")

	set(BOOST_ROOT "/usr/local/boost_1_49_0") 
   include_directories("/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/usr/include/")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
    message("linux")
	set (CMAKE_INSTALL_PREFIX /data/jingmin/bin/)
	#set(BOOST_ROOT "/home/jchen/local/boost_1_50_0") 
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_CXX_FLAGS "-g -Wall -O2 ")# -I/users/jchen/libs/include
    SET(OperatingSystem "Linux")
    find_package(Biogeme)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#set(BIOGEME_FOLDER "/Users/jchen/Documents/Project/biogeme")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fPIC -fopenmp")
SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fopenmp")

#SET(XMLWRAPP_CXX_FLAGS "-g `xmlwrapp-config --cxxflags`")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("/opt/local/include")
include_directories("/usr/local/include")
include_directories("${PROJECT_BINARY_DIR}")
include_directories("/home/jchen/libs/include")

find_package(POSTGRES)
find_package(PQXX)
find_package(GSL)
find_package(SHP)
find_package(KML)
find_package(LibConfig) 
include_directories("${LIBCONFIG_INCLUDE_DIR}")

#SET(Boost_USE_STATIC_LIBS OFF)
#SET(Boost_USE_MULTITHREAD ON)


find_package(Boost )
find_library(LIB_XMLWRAPP xmlwrapp)
set(LIB_EXTRA  ${LIBCONFIGPP_LIBRARY} ${PQXX_LIBRARIES} ${SHP_LIBRARIES} ${KML_LIBRARIES} ${GSLCBLAS_LIBRARIES} ${GSL_CONFIG_LIBS}
     ${LIB_XMLWRAPP} ${LIB_UTILS} ${LIB_PARAMETERS} ${Boost_LIBRARIES} )

#ADD_DEFINITIONS( "-g `xmlwrapp-config --cxxflags`")
add_subdirectory(Utilities)
add_subdirectory(NetworkHandler)
add_subdirectory(Router)
add_subdirectory(MapMatching)
add_subdirectory(Math)
add_subdirectory(PathGeneration/MetropolishHastings)
add_subdirectory(PathGeneration/Enumeration)
add_subdirectory(PathGeneration/Utilities)
add_subdirectory(PathGeneration/RandomWalk)
#add_subdirectory(Unimodal)
add_subdirectory(bin)
#add_subdirectory(test)
add_subdirectory(RouteChoice)
#configure_file(params ${CMAKE_CURRENT_BINARY_DIR}/params COPYONLY)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/params ${CMAKE_BINARY_DIR}/params)


#target_link_libraries (CheckDatabase  ${LIB_EXTRA})
