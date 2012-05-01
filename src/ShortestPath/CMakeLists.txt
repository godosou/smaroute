
include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${NewBioroute_SOURCE_DIR}/NetworkHandler")
include_directories("${NewBioroute_SOURCE_DIR}/MapMatching")
add_library(ShortestPath
                patNetworkReducer.cc
                patShortestPathGeneral.cc
                patShortestPathTreeGeneral.cc
                patLinkCost.cc
                patLinkCostExcludingNodes.cc
                patRouter.cc)
#target_link_libraries  (ShortestPath NetworkHandler Utilities  ${LIB_EXTRA})
