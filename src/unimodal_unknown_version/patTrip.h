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
			vector<list<pair<patArc*, patULong> > >* theAdjList);

	void newTrip(patULong theUserId, patULong theTripId, patULong theStartTime);
	patBoolean addPoint(const patGpsPoint theGpsPoint);
	void clean();
	void endofTrip(patULong theEndTime);
	void genDDR();
	void genOdDDR();
	void genPath();
	void writeToFile();
	vector<patReal> getPathDDR(list<patArc*> path);
	void recordPath(list<list<patULong> >* pathTemp);
	void connectPoints(patULong endNodeId, patULong hierarchy,
			list<list<patULong> >* pathTemp);
	void writeToKML(patString fileName, patError*& err);
	void setMapBounds(patReal minLat, patReal maxLat, patReal minLon,
			patReal maxLon);
protected:
	patULong userId; //owner of the trip
	patULong tripId; //trip id
	patULong startTime; //timestamp of first gps record
	patULong endTime; //timestamp of last gps record
	patReal minLatitude;
	patReal maxLatitude;
	patReal minLongitude;
	patReal maxLongitude;
	patULong originNode;
	patULong destinationNode;
	vector<patGpsPoint> gpsSequence;
	map<patNode*, patReal> originDDR;
	map<patNode*, patReal> destinationDDR;
	vector<patPath> listOfPath;

	map<pair<patNode*, patNode*> ,vector<list<patArc*> > > pathList;
	map<pair<patNode*, patNode*> ,vector<vector<patReal> > > pathDDRs;
	patNetwork* theNetwork;
	vector<list<pair<patArc*, patULong> > >* adjList;
};

#endif
