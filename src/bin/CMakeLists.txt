

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

add_executable(pmm pmm.cc)
target_link_libraries (pmm MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})
add_executable(genSP genSP.cc)
target_link_libraries (genSP MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})

add_executable(bioroute bioroute.cc)
target_link_libraries (bioroute RandomWalk RouteChoice MHPath MapMatching NetworkHandler ShortestPath Utilities  Math ${LIB_EXTRA})

INSTALL(TARGETS pmm genSP bioroute
  RUNTIME DESTINATION ../bin
)
