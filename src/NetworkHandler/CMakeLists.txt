include_directories("${NewBioroute_SOURCE_DIR}/MapMatching")

include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
include_directories("${LIBCONFIG_INCLUDE_DIR}")


add_library(NetworkHandler
                patNetworkElements.cc
                patNetworkTrain.cc
				patSpatialSearch.cc
                patWriteNetworkToDB.cc
                patCompressedArc.cc
		patGetShortestPathFromDB.cc
                patNetworkCompresser.cc
                patReadNetworkFromOSM.cc
                patPathWriter.cc
                patKMLPathWriter.cc
                patRoadBase.cc
                patArc.cc
                patNode.cc
                patArcSequence.cc
                patWay.cc
                patOd.cc
                patMultiModalPath.cc
                patPublicTransportSegment.cc
                patGeoBoundingBox.cc
                patNetworkBase.cc
                patNetworkUnimodal.cc
                patNetworkPublicTransport.cc
                patNetworkBike.cc
                patNetworkBus.cc
                patNetworkMetro.cc
                patNetworkCar.cc
                patNetworkWalk.cc
                patNetworkEnvironment.cc
                patReadPathFromShp.cc)
#target_link_libraries (NetworkHandler Utilities  ${LIB_EXTRA})
