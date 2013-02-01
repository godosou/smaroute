include  $(MAKEHOME)/general.mk

.PHONY: all clean veryclean



TARGET = routeChoice_Jingmin

SOURCE =	patCalculateProbaForPaths.cc\
			patReadPathFromKML.cc\
			patMapMatchingV2.cc\
			patMapMatchingRoute.cc\
			patStreetSegment.cc\
			patPathDevelop.cc\
			patMeasurementModel.cc\
			patTrafficModelComplex.cc\
			patMeasurementModelV2.cc\
			patTrafficModel.cc\
			patGpsPoint.cc\
			patTripParser.cc\
			patPathSampling.cc\
			patPathJ.cc\
			patOdJ.cc\
			patTraveler.cc\
			patSample.cc\
			patWriteBiogemeInput.cc\
			patArc.cc\
			patNode.cc\
			patNetwork.cc\
			patTrafficModelTriangular.cc\
			patReadNetworkFromXml.cc\
			patPathProbaAlgoV4.cc\
			patWriteKML.cc\
			patReadSample.cc\
			patShortestPathAlgoDest.cc\
			patShortestPathTreeDest.cc\
			patShortestPathAlgoRange.cc\
			patShortestPathTreeRange.cc\
			patObservation.cc\
			patDDR.cc\
			patArcDDR.cc\
			patPathDDR.cc\
			patGenPathFromArcTranSet.cc\
			patPath.cc\
			patOd.cc\
			patNBParameters.cc\
			patTripGraph.cc\
			patArcTransition.cc

			
	
			
			

LEXFLAGS += -Pyyspec

OBJ = $(SOURCE:%.cc=%.o) $(TARGET).o patDisplay.o
LIB = -lpatUtils -lpatParam -lpatRandomNumbers
#-lgsl -lgslcblas -lm

LIBFILE = $(LIB:-l%=lib%.a)

#IB_PATH+= -L/usr/local/lib
CCNOOPT += -g `xmlwrapp-config --cxxflags`


all:  $(TARGET)

bioroute.o: bioroute.cc
	$(CC) -c  $(CCNOOPT) $< -o $@

$(TARGET): $(OBJ) $(LIBFILE) 
	$(CC) $(LDFLAGS) -o $(TARGET) $(LIB_PATH)  $(OBJ) $(LIB) `xmlwrapp-config --libs` $(LIB_PATH) $(LIB) -lgsl -lgslcblas -lm
	echo $(TARGET) has been built

clean:
	rm -f $(OBJ)
	rm -f $(SOURCE:%.cc=%.u)

veryclean: clean
	rm -f $(TARGET) 

include $(SOURCE:%.cc=%.u)



