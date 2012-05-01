//----------------------------------------------------------------
// File: patTripObServ.h

// Creation: 17 March 2009
//----------------------------------------------------------------

#ifndef patTrip_h
#define patTrip_h

#include <vector>
#include <map>
#include <list>
#include "patType.h"
#include "patGeoCoordinates.h"
#include "patGpsPoint.h"
#include "patNetwork.h"
class patTrip {
	friend class patSample;

public:
	patTrip(patNetwork* network,
			vector<list<pair<patArc*, unsigned long> > >* theAdjList);

	void newTrip(unsigned long theUserId, unsigned long theTripId, unsigned long theStartTime);
	bool addPoint(const patGpsPoint theGpsPoint);
	void clean();
	void endofTrip(unsigned long theEndTime);
	void genDDR();
	void genOdDDR();
	void genPath();
	void writeToFile();
	vector<double> getPathDDR(list<patArc*> path);
	void recordPath(list<list<unsigned long> >* pathTemp);
	void connectPoints(unsigned long endNodeId, unsigned long hierarchy,
			list<list<unsigned long> >* pathTemp);
	void writeToKML(patString fileName, patError*& err);
	void setMapBounds(double minLat, double maxLat, double minLon,
			double maxLon);
protected:
	unsigned long userId; //owner of the trip
	unsigned long tripId; //trip id
	unsigned long startTime; //timestamp of first gps record
	unsigned long endTime; //timestamp of last gps record
	double minLatitude;
	double maxLatitude;
	double minLongitude;
	double maxLongitude;
	unsigned long originNode;
	unsigned long destinationNode;
	vector<patGpsPoint> gpsSequence;
	map<patNode*, double> originDDR;
	map<patNode*, double> destinationDDR;
	vector<patPath> listOfPath;

	map<pair<patNode*, patNode*> ,vector<list<patArc*> > > pathList;
	map<pair<patNode*, patNode*> ,vector<vector<double> > > pathDDRs;
	patNetwork* theNetwork;
	vector<list<pair<patArc*, unsigned long> > >* adjList;
};

#endif
