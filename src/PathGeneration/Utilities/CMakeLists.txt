include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
add_library(PGUtilities
patWeightFunction.cc
    patChoiceSetWriter.cc
    patObservationWritter.cc
)
target_link_libraries (PGUtilities NetworkHandler Utilities Math ${LIB_EXTRA})
