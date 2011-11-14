#ifndef patMapMatchingRoute_h
#define patMapMatchingRoute_h 

#include <map>
#include <list>
#include <set>
#include <vector>
#include "patError.h"
#include "patType.h"
#include "patString.h"
#include "patStreetSegment.h"
#include "patNetwork.h"
#include "patNode.h"
#include "patArc.h"
#include "patGpsPoint.h"
#include "patPathJ.h"

class patMapMatchingRoute{
	public:
	patMapMatchingRoute();
  friend bool operator==(const patMapMatchingRoute& route1, const patMapMatchingRoute& route2) ;
friend bool operator!=(const patMapMatchingRoute& route1, const patMapMatchingRoute& route2) ;

friend bool operator<(const patMapMatchingRoute& route1, const patMapMatchingRoute& route2) ;
	void addArc(patArc* aArc,patNetwork* theNetwork,patError* err);
        void  addStreet(patStreetSegment currStreet, patNetwork* theNetwork, patError*& err);
        void addGpsStreetAssignment(patStreetSegment currStreet, patGpsPoint* currGps,patNetwork* theNetwork, patError*& err);
                    void addGpsArcAssignment(patArc* aArc,
	patGpsPoint* aGps,patNetwork* theNetwork,patError* err);
	patArc* getLastArc();
	
patGpsPoint* getLastGps();
void calScore(patNetwork* theNetwork);
void reCalScore(patArc* theArc, patGpsPoint* theGpsPoint,patBoolean newLink,patNetwork* theNetwork);


map<patArc*,patReal> getMinArcScores();
patULong getNbrOfGpsInRoute();
patNode* getRealStartNode();
patULong getRouteSize();
patReal getScore();
map<patArc*,patReal> getotalArcScores();
void setRealStartNode(patNode* n);
vector<patGpsPoint*>* getGpsAssignedToArc(patArc* arc);

vector<patGpsPoint*>* getGpsAssignedToStreet(patStreetSegment* aSegment);

patPathJ getPath();
patULong  getPath(patPathJ* newPath, patNode* startNode, patNode* endNode);
list<patArc*> getAllArcs();
patNode* getStartNode(patNetwork* theNetwork, patError*& err);
patNode* getEndNode(patNetwork* theNetwork,patError*& err);
patArc* getFirstArc();
patGpsPoint* getFirstGps();
patReal	getArcScore(patArc* arc);
patBoolean searchFromBegin(patArc* aArc);
patBoolean searchFromEnd(patArc* aArc);
patBoolean searchFromBegin(patNode* aNode);
patBoolean searchFromEnd(patNode* aNode);

patBoolean searchFromBegin(patULong nodeUserId);

patBoolean searchFromEnd(patNode* aNode, patNode* stopNode);
patStreetSegment* getLastStreet();
		patULong getRouteStreetSize();
patArc* getArc(patULong j);
	protected:
		list<pair<patArc*, vector<patGpsPoint*> > > route;
		patReal score;
		map<patArc*, patReal> totalArcScores;
		map<patArc*, patReal> minArcScores;
		patNode* realStartNode;
		patULong nbrOfGpsInRoute;
		vector<patGpsPoint*> gpsSequence;
                list<pair<patStreetSegment, vector<patGpsPoint*> > > StreetRoute;
                patGpsPoint* lastGpsPoint;
};

#endif
