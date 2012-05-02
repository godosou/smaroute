
include_directories("${NewBioroute_SOURCE_DIR}/MetropolisHasting")
include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/Math")
include_directories("${NewBioroute_SOURCE_DIR}/ShortestPath")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
add_library(MHPath
    MHPath.cc
    MHPoints.cc
    MHPathProposal.cc
    MHLinkAndPathCost.cc
    MHPathWriterWrapper.cc
  MHPathGenerator.cc
)
#target_link_libraries (MHPath Utilities Math ${LIB_EXTRA})
