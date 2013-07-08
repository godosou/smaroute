
include_directories("${NewBioroute_SOURCE_DIR}/MetropolisHasting")
include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/Math")
include_directories("${NewBioroute_SOURCE_DIR}/Router")
include_directories("${NewBioroute_SOURCE_DIR}/RouteChoice")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/Utilities")
add_library(MHPath	
	MHEqualWeightFunction.cc
    MHPath.cc
    MHPoints.cc
    MHPathWriterWrapper.cc
    MHPathProposal.cc
    MHWeightFunction.cc
    MHSamplingWeightFunction.cc
    MHNodeInsertionProbability.cc
    MHNIPPG.cc
    MHNIPSP.cc
    MHPathSimWeight.cc
    MHPathGenerator.cc
    MHObservationWritterWrapper.cc
)
target_link_libraries (MHPath PGUtilities Math ${LIB_EXTRA})
