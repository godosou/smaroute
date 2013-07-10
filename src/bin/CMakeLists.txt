

include_directories("${NewBioroute_SOURCE_DIR}/headers")
include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/MapMatching")
include_directories("${NewBioroute_SOURCE_DIR}/Router")
include_directories("${NewBioroute_SOURCE_DIR}/Math")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/MetropolishHastings")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/RandomWalk")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/Enumeration")
include_directories("${NewBioroute_SOURCE_DIR}/MetropolisHasting")
include_directories("${NewBioroute_SOURCE_DIR}/RouteChoice")

#add_executable(pmm pmm.cc)
#target_link_libraries (pmm MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})
#add_executable(genSP genSP.cc)
#target_link_libraries (genSP MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})

#add_executable(bioroute bioroute.cc)
#target_link_libraries (bioroute RandomWalk RouteChoice MHPath MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})

#add_executable(mr.osm mr.osm.cc)
#target_link_libraries (mr.osm   NetworkHandler  Utilities  Math ${LIB_EXTRA})

add_executable(testTrainNetwork testTrainNetwork.cc)
target_link_libraries (testTrainNetwork   NetworkHandler  Utilities  ${LIB_EXTRA})

INSTALL(PROGRAMS  ${CMAKE_CURRENT_BINARY_DIR}/bioroute ${CMAKE_CURRENT_BINARY_DIR}/pmm
  DESTINATION .) 