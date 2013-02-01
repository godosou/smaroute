#ifndef patTripGraph_h
#define patTripGraph_h
#include <set>
#include <map>
#include <list>
#include "patType.h"

class patNetwork;
class patArc;
class patPathJ;
class patGpsPoint;
class patNode;
class patArcTransition;
class patTripGraph{

 public:
  patTripGraph(); 

patTripGraph(patNetwork* baseNetwork);
	void setOrigin(set<patNode*> origNodeSet);
void setSuccessor(patArc* upArc,patArc* downArc);
void genOrigSet_Ids(vector<patULong> origIds);
set<patPathJ> genPaths(vector<patGpsPoint>* theGpsSequence,
map<patArc*,set<patULong> >* theAssocDomain);
void connectArcs(patArc* theArc,list<patArc*>* pathTemp,set<patULong> possibleDomain) ;
void recordPath(list<patArc*>* pathTemp);
	set<patPathJ> getPathSet();
void genOrigSet_All(vector<patGpsPoint>* theGpsSequence);
void genOrigSet_SortDDR(vector<patGpsPoint>* theGpsSequence,patULong k);
patBoolean isVisited(patULong nodeUserId,list<patArc*>* pathTemp);
set<patPathJ> selectPaths(vector<patGpsPoint>* theGpsSequence,set<patArcTransition>* arcTranSet);
void writeGraph();
void genOrigSet_Bound(vector<patGpsPoint>* theGpsSequence,patReal lowerBound, patReal upperBound);
map<patReal, set<patULong> > sortDest(vector<patGpsPoint>* theGpsSequence);
map<patReal, set<patULong> > sortOrig(vector<patGpsPoint>* theGpsSequence);
patReal calLengthCeil();
patReal calAverageSpeed(); 

 protected:
	set<patNode*> origSet;
	set<patNode*> destSet;
	set<patPathJ> pathSet;
	patReal lengthCeil;
	patNetwork* baseNetwork;
	vector<patGpsPoint>* gpsSequence;
	map<patArc*,set<patULong> >* assocDomain;
	map<patArc*,set<patArc*> > successor;
};

#endif





