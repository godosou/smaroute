include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
include_directories("${NewBioroute_SOURCE_DIR}/MapMatching")
include_directories("${NewBioroute_SOURCE_DIR}/MetropolisHasting")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/MetropolishHastings")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/RandomWalk")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/Router")
include_directories("${NewBioroute_SOURCE_DIR}/Math")
add_library(RouteChoice
				patSimulateProbabilisticObs.cc
				patExperimentBed.cc
				patSimulateProbabilisticPaths.cc
patGetPathProbasFromObservations.cc
    patChoiceSet.cc
    patVerifyingSamplingResult.cc
    patReadChoiceSetFromKML.cc
    patSampleChoiceSetForObservations.cc
    patPathSetSimilarity.cc
  #              patSampleChoiceSetWithObservations.cc
                patSimulation.cc
                patWriteBiogemeData.cc
                patReadPathsFromKML.cc
                #patComputePathSize.cc
                patObservation.cc
                patUtilityFunction.cc
                patReadObservationFromKML.cc
       #         patSampleChoiceSetWithRandomWalk.cc
)
target_link_libraries (RouteChoice  ShortestPath MHPath  RandomWalk ${LIB_EXTRA})
