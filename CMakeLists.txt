cmake_minimum_required(VERSION 2.8)
project (NewBioroute)
set (NewBioroute_VERSION_MAJOR 2)
set (NewBioroute_VERSION_MINOR 0)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_OSX_ARCHITECTURES x86_64 )
set(CMAKE_CXX_FLAGS "-DMACOSX")
set(CMAKE_C_FLAGS -DMACOSX)
set(CMAKE_CXX_FLAGS "-g -Wall")

#SET(XMLWRAPP_CXX_FLAGS "-g `xmlwrapp-config --cxxflags`")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/../include")
include_directories("${PROJECT_SOURCE_DIR}/../randomNumbers")
include_directories("${PROJECT_SOURCE_DIR}/../parameters")
include_directories("/opt/local/include")
include_directories("/usr/local/include")
include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(POSTGRES)
find_package(PQXX)
find_package(GSL)
find_package(SHP)

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREAD OFF)
include_directories("/usr/local/boost_1_46_1")
find_package(Boost)


find_library(LIB_UTILS patUtils "${PROJECT_SOURCE_DIR}/../utils/")
find_library(LIB_RANDOMNUMBER patRandomNumbers "${PROJECT_SOURCE_DIR}/../randomNumbers")
find_library(LIB_PARAMETERS patParam "${PROJECT_SOURCE_DIR}/../parameters")
find_library(LIB_XMLWRAPP xmlwrapp)
set(LIB_EXTRA ${PQXX_LIBRARIES} ${SHP_LIBRARIES} ${GSLCBLAS_LIBRARIES} ${GSL_CONFIG_LIBS}
     ${LIB_XMLWRAPP} ${LIB_UTILS} ${LIB_RANDOMNUMBER} ${LIB_PARAMETERS})

#ADD_DEFINITIONS( "-g `xmlwrapp-config --cxxflags`") 

add_executable(CheckDatabase CheckDatabase.cc
    patCalculateProbaForPaths.cc 
    patDisplay.cc
			patReadPathFromShp.cc 
			patNetworkElements.cc 
			patGeoBoundingBox.cc 
			patNetworkUnimodal.cc 
			patNetworkCar.cc 
			patWay.cc 
			patPostGISDataType.cc 
			patPostGreSQLConnector.cc 
			patReadPathFromKML.cc 
			patMapMatchingV2.cc 
			patMapMatching.cc 
			patMapMatchingRoute.cc 
			patStreetSegment.cc 
			patPathDevelop.cc 
			patMeasurementModel.cc 
			patTrafficModelComplex.cc 
			patMeasurementModelV2.cc 
			patTrafficModel.cc 
			patGpsPoint.cc 
			patTripParser.cc 
			patPathSampling.cc 
			patPathJ.cc 
			patOdJ.cc 
			patTraveler.cc 
			patSample.cc 
			patWriteBiogemeInput.cc 
			patArc.cc 
			patNode.cc 
			patNetwork.cc 
			patTrafficModelTriangular.cc 
			patReadNetworkFromXml.cc 
			patPathProbaAlgoV4.cc 
			patWriteKML.cc 
			patReadSample.cc 
			patShortestPathAlgoDest.cc 
			patShortestPathTreeDest.cc 
			patShortestPathAlgoRange.cc 
			patShortestPathTreeRange.cc 
			patObservation.cc 
			patDDR.cc 
			patArcDDR.cc 
			patPathDDR.cc 
			patGenPathFromArcTranSet.cc 
			patPath.cc 
			patOd.cc 
			patNBParameters.cc 
			patTripGraph.cc 
			patArcTransition.cc
)
target_link_libraries (CheckDatabase  ${LIB_EXTRA})
