
#ifndef patTripParser_h
#define patTripParser_h

#include <vector>
#include <map>
#include <list>
#include "patType.h"
#include "patGeoCoordinates.h"

#include "patNetwork.h"



#include "patNode.h"

#include "patArc.h"
#include "patObservation.h"
#include "patTripGraph.h"

class patOdJ;
class patSample;
class patPathJ;
class patGpsPoint;
class patArcTransition;
class patPathDevelop;
class patTripParser {

friend class patSample;

public:
	patTripParser();
	patTripParser( patTraveler* theTraveler,
		patULong& theTipId,
		patULong& theStartTime) ;

	void setEnviroment(patSample* sample, patNetwork* network);
	void setMapBounds(patReal minLat,
		patReal maxLat,
		patReal minLon,
		patReal maxLon) ;

	void inputGpsTrack(vector<patGpsPoint>* gpsTrack);
	patBoolean addPoint( patGpsPoint theGpsPoint) ;
	patULong firstValidGps();

	void genDDR(vector< list < pair<patArc*,patULong> > >* adjList,
		set<patArcTransition>* arcTranSet);
	patBoolean addRealPath(list<patULong> realPath);
	void setGpsType();

void doMapMatching(vector< list < pair<patArc*, patULong> > >* adjList);
void doProbabilisticMapMatching(vector< list < pair<patArc*, patULong> > >* adjList);
void doSensitivityAnalysis();
void cleanGpsData();
void setGpsSpeedType();
void presentSAResults(patString SAType,map<patString, vector<patReal> >);

void setPMMAlgoParam(patString paramName, patReal paramValue);
vector<patReal> prepareParamsForSA(patString paramName);
void genMapMatchingResult(vector< list < pair<patArc*, patULong> > >* adjList);
	void endOfTrip(vector< list < pair<patArc*,patULong> > >* adjList,
		patULong theEndTime);
	void setOd(patOdJ* theOd);

	void writeToKML(patString fileName);

	void genOd();
	void genOdDDR( ) ;


	void genDDR();
	patObservation rtnObservation();

vector<patGpsPoint> increaseSamplingInterval(patReal newInterval);
	void genAssocDomain(map<patArc*,set<patULong> >* associateDomain);

 protected:
  vector<patGpsPoint> gpsSequence ;
  patOdJ* od;
  patReal ddrAll;
  patReal ddrRawAll;
  patReal avgSpeed;
  map<patNode*, patReal> originDDR ;
  map<patNode*, patReal> destinationDDR ;
  patObservation generatedObservation;
  patSample* theSample;
  patPathJ realPath;
  patNetwork* theNetwork;
  patTripGraph* tripGraph;
  patPathDevelop* pathDevelop;
} ;

#endif
