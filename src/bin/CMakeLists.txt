
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
#
#add_executable(testPathGeneration testPathGeneration.cc)
#target_link_libraries (testPathGeneration MHPath MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})
add_executable(pathGeneration pathGeneration.cc)
target_link_libraries (pathGeneration RandomWalk RouteChoice MHPath MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})
#
add_executable(enumeratPaths pathEnumeration.cc)
target_link_libraries (enumeratPaths  PGUtilities PathEnumeration  NetworkHandler  Utilities   ${LIB_EXTRA})

add_executable(synSimulatePath synSimulatePath.cc)
target_link_libraries (synSimulatePath PGUtilities MHPath ShortestPath RouteChoice NetworkHandler  Utilities Math  ${LIB_EXTRA})

add_executable(synPathGeneration synPathGeneration.cc)
target_link_libraries (synPathGeneration RandomWalk MHPath PGUtilities ShortestPath RouteChoice NetworkHandler  Utilities Math  ${LIB_EXTRA})
#
add_executable(synWriteBiogeme synWriteBiogeme.cc)
target_link_libraries (synWriteBiogeme RandomWalk PGUtilities RouteChoice MHPath MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})

add_executable(writeBiogeme writeBiogeme.cc)
target_link_libraries (writeBiogeme RandomWalk RouteChoice MHPath MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})

#add_executable(countSampledChosenPath countSampledChosenPath.cc)
#target_link_libraries (countSampledChosenPath RouteChoice MHPath MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})
