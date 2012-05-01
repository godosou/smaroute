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
void genOrigSet_Ids(vector<unsigned long> origIds);
set<patPathJ> genPaths(vector<patGpsPoint>* theGpsSequence,
map<patArc*,set<unsigned long> >* theAssocDomain);
void connectArcs(patArc* theArc,list<patArc*>* pathTemp,set<unsigned long> possibleDomain) ;
void recordPath(list<patArc*>* pathTemp);
	set<patPathJ> getPathSet();
void genOrigSet_All(vector<patGpsPoint>* theGpsSequence);
void genOrigSet_SortDDR(vector<patGpsPoint>* theGpsSequence,unsigned long k);
bool isVisited(unsigned long nodeUserId,list<patArc*>* pathTemp);
set<patPathJ> selectPaths(vector<patGpsPoint>* theGpsSequence,set<patArcTransition>* arcTranSet);
void writeGraph();
void genOrigSet_Bound(vector<patGpsPoint>* theGpsSequence,double lowerBound, double upperBound);
map<double, set<unsigned long> > sortDest(vector<patGpsPoint>* theGpsSequence);
map<double, set<unsigned long> > sortOrig(vector<patGpsPoint>* theGpsSequence);
double calLengthCeil();
double calAverageSpeed(); 

 protected:
	set<patNode*> origSet;
	set<patNode*> destSet;
	set<patPathJ> pathSet;
	double lengthCeil;
	patNetwork* baseNetwork;
	vector<patGpsPoint>* gpsSequence;
	map<patArc*,set<unsigned long> >* assocDomain;
	map<patArc*,set<patArc*> > successor;
};

#endif





