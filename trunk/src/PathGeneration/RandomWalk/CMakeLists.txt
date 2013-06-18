
include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/Math")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/Router")
add_library(RandomWalk
    RWPathGenerator.cc
)
#target_link_libraries (MHPath Utilities Math ${LIB_EXTRA})
