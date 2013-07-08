
add_executable(pum routeChoice_Jingmin.cc
patCalculateProbaForPaths.cc
			patReadPathFromKML.cc
			patMapMatchingV2.cc
			patMapMatchingRoute.cc
			patStreetSegment.cc
			patPathDevelop.cc
			patMeasurementModel.cc
			patTrafficModelComplex.cc
			patMeasurementModelV2.cc
			patTrafficModel.cc
			patGpsPoint.cc
			patDisplay.cc
			patTripParser.cc
			patPathSampling.cc
			patPathJ.cc
			patOdJ.cc
			patTraveler.cc
			patSample.cc
			patWriteBiogemeInput.cc
			patArc.cc
			patNode.cc
			patNetwork.cc
			patTrafficModelTriangular.cc
			patReadNetworkFromXml.cc
			patPathProbaAlgoV4.cc
			patWriteKML.cc
			patReadSample.cc
			patShortestPathAlgoDest.cc
			patShortestPathTreeDest.cc
			patShortestPathAlgoRange.cc
			patShortestPathTreeRange.cc
			patObservation.cc
			patDDR.cc
			patArcDDR.cc
			patPathDDR.cc
			patGenPathFromArcTranSet.cc

			patPath.cc
			patOd.cc
			patNBParameters.cc
			patTripGraph.cc
			patArcTransition.cc
			patDiscreteDistribution.cc
			patGeoCoordinates.cc
			patUnixUniform.cc
)
target_link_libraries (pum ${LIB_EXTRA})


INSTALL(PROGRAMS pum
  DESTINATION .)
#target_link_libraries (CheckDatabase  ${LIB_EXTRA})
