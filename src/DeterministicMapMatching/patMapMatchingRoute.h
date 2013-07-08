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
void reCalScore(patArc* theArc, patGpsPoint* theGpsPoint,bool newLink,patNetwork* theNetwork);


map<patArc*,double> getMinArcScores();
unsigned long getNbrOfGpsInRoute();
patNode* getRealStartNode();
unsigned long getRouteSize();
double getScore();
map<patArc*,double> getotalArcScores();
void setRealStartNode(patNode* n);
vector<patGpsPoint*>* getGpsAssignedToArc(patArc* arc);

vector<patGpsPoint*>* getGpsAssignedToStreet(patStreetSegment* aSegment);

patPathJ getPath();
unsigned long  getPath(patPathJ* newPath, patNode* startNode, patNode* endNode);
list<patArc*> getAllArcs();
patNode* getStartNode(patNetwork* theNetwork, patError*& err);
patNode* getEndNode(patNetwork* theNetwork,patError*& err);
patArc* getFirstArc();
patGpsPoint* getFirstGps();
double	getArcScore(patArc* arc);
bool searchFromBegin(patArc* aArc);
bool searchFromEnd(patArc* aArc);
bool searchFromBegin(patNode* aNode);
bool searchFromEnd(patNode* aNode);

bool searchFromBegin(unsigned long nodeUserId);

bool searchFromEnd(patNode* aNode, patNode* stopNode);
patStreetSegment* getLastStreet();
		unsigned long getRouteStreetSize();
patArc* getArc(unsigned long j);
	protected:
		list<pair<patArc*, vector<patGpsPoint*> > > route;
		double score;
		map<patArc*, double> totalArcScores;
		map<patArc*, double> minArcScores;
		patNode* realStartNode;
		unsigned long nbrOfGpsInRoute;
		vector<patGpsPoint*> gpsSequence;
                list<pair<patStreetSegment, vector<patGpsPoint*> > > StreetRoute;
                patGpsPoint* lastGpsPoint;
};

#endif
