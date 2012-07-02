
#ifndef patTripParser_h
#define patTripParser_h

#include <vector>
#include <map>
#include <list>
#include "patType.h"
#include "patGeoCoordinates.h"

#include "patNetwork.h"

#include "patGpsPoint.h"

#include "patNode.h"

#include "patArc.h"
#include "patObservation.h"
#include "patTripGraph.h"

class patOd;
class patSample;
class patPathJ;
class patArcTransition;
class patPathDevelop;
class patTripParser {

friend class patSample;

public:
	patTripParser();

	void readGPSFromFile( patString& fileName,patError*& err);
	patTripParser( patTraveler* theTraveler,
		unsigned long& theTipId,
		unsigned long& theStartTime) ;

	void setEnviroment(patSample* sample, patNetwork* network);
	void setMapBounds(double minLat,
		double maxLat,
		double minLon,
		double maxLon) ;

	void inputGpsTrack(vector<patGpsPoint>* gpsTrack);
	bool addPoint( patGpsPoint theGpsPoint) ;
	unsigned long firstValidGps();

	void genDDR(vector< list < pair<patArc*,unsigned long> > >* adjList,
		set<patArcTransition>* arcTranSet);
	bool addRealPath(list<unsigned long> realPath);
	void setGpsType();

void doMapMatching(vector< list < pair<patArc*, unsigned long> > >* adjList);
void doProbabilisticMapMatching(vector< list < pair<patArc*, unsigned long> > >* adjList);
void doSensitivityAnalysis();
void cleanGpsData();
void setGpsSpeedType();
void presentSAResults(patString SAType,map<patString, vector<double> >);

void setPMMAlgoParam(patString paramName, double paramValue);
vector<double> prepareParamsForSA(patString paramName);
void genMapMatchingResult(vector< list < pair<patArc*, unsigned long> > >* adjList);
	void endOfTrip(vector< list < pair<patArc*,unsigned long> > >* adjList,
		unsigned long theEndTime);
	void setOd(patOd* theOd);

	void writeToKML(patString fileName);

	void genOd();
	void genOdDDR( ) ;


	void genDDR();
	patObservation rtnObservation();

vector<patGpsPoint> increaseSamplingInterval(double newInterval);
	void genAssocDomain(map<patArc*,set<unsigned long> >* associateDomain);
vector<patGpsPoint>* getGpsSequence();
 protected:
  vector<patGpsPoint> gpsSequence ;
  patOd* od;
  double ddrAll;
  double ddrRawAll;
  double avgSpeed;
  map<patNode*, double> originDDR ;
  map<patNode*, double> destinationDDR ;
  patObservation generatedObservation;
  patSample* theSample;
  patPathJ realPath;
  patNetwork* theNetwork;
  patTripGraph* tripGraph;
  patPathDevelop* pathDevelop;
} ;

#endif
