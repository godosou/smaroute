
include_directories("${NewBioroute_SOURCE_DIR}/MetropolisHasting")
include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/Math")
include_directories("${NewBioroute_SOURCE_DIR}/Router")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/Utilities")
add_library(MHPath
    MHPath.cc
    MHPoints.cc
    MHPathWriterWrapper.cc
    MHPathProposal.cc
    MHWeightFunction.cc
  MHPathGenerator.cc
)
target_link_libraries (MHPath PGUtilities Math ${LIB_EXTRA})
