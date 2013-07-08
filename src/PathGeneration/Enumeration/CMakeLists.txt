
include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/Math")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/PathGeneration")
add_library(PathEnumeration
    patEnumeratePaths.cc
)
#target_link_libraries (MHPath Utilities Math ${LIB_EXTRA})
