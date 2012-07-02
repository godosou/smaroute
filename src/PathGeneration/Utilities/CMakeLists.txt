include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
add_library(PGUtilities
patWeightFunction.cc
    patChoiceSetWriter.cc
)
#target_link_libraries (MHPath Utilities Math ${LIB_EXTRA})
