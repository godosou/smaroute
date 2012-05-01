
include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/MapMatching")
include_directories("${NewBioroute_SOURCE_DIR}/ShortestPath")
include_directories("${NewBioroute_SOURCE_DIR}/Math")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/MetropolishHastings")
include_directories("${NewBioroute_SOURCE_DIR}/MetropolisHasting")
add_executable(pmm pmm.cc)
target_link_libraries (pmm MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})

add_executable(pathGeneration pathGeneration.cc)
target_link_libraries (pathGeneration MHPath MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})