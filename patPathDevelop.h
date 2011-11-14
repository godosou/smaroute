
#ifndef patPathDevelop_h
#define patPathDevelop_h

#include <list>
#include <map>
#include <set>
#include "patType.h"
#include "patUniform.h" 
#include "patRandomNumberGenerator.h"
#include "patUnixUniform.h"

class patArc;
class patPathJ;
class patGpsPoint;
class patNetwork;
class patArcTransition;
class patObservation;
class patSample;
class patPathDevelop{
public:
 

patPathDevelop(patNetwork* theNetwork, set<patArcTransition>* theArcTranSet,
vector<patGpsPoint>* theOriginalGpsPoints);

patReal calAverageSpeed();
patReal calLengthCeil();
set<patULong> genEndNodes();
void appendSeg(patGpsPoint* gpsPoint,map<patULong,set<patPathJ > >* segSet,set<patArc*>* inheritDDR);

void unionTwoDomains(patReal* minLength,map<patPathJ,patReal >* newPathSet,
				map<patPathJ,patReal>* prevSet,set<patPathJ >* newSegSet,set<patArc*>* activeDomain);
void selectPaths(map<patPathJ,patReal>* newPathSet);
patReal sumProba(map<patPathJ,patReal>* newPathSet);
void selectPathsByPdf(list<patPathJ*>* preSelected,map<patPathJ,patReal>* newPathSet);
void selectPathsByCdf(list<patPathJ*>* preSelected,map<patPathJ,patReal>* newPathSet);

void selectPathsByNumber(list<patPathJ*>* preSelected,map<patPathJ,patReal>* newPathSet);
void selectPathsByShortest(map<patPathJ,patReal>* newPathSet,	
	set<patPathJ*>* tmpPathSet);
list<patPathJ*> selectPathByImportantDDR(map<patPathJ,patReal>* newPathSet);
set<patArc*>  selectImportantDDR();
void finalizeOneStage(map<patPathJ,patReal >* newPathSet);
set<patPathJ> getPaths();

patReal lastStage(patSample* theSample,patObservation* generatedObservation);
void init(patGpsPoint* firstGpsPoint);

void calPathDDR(map<patPathJ,patReal>* newPathSet);

patGpsPoint* lastGpsPoint();
void connectNodes(patULong node,list<patULong>* pathTemp,
		map<patULong,set<patULong> >* succ,
		map<patPathJ,patReal >*  newPathSet);
void newInitPath(list<patULong>* pathTemp,map<patPathJ,patReal >*  newPathSet);

void lowSpeedPoints(vector<patGpsPoint*>* lowSpeedGpsPoints,
	set<patArc*>* interArcs,	patNetwork* theNetwork );
protected:
	map<patULong, map<patPathJ,patReal> >  pathSet;
	map<patULong, map<patPathJ,patReal> >  oldPathSet;
	vector<patGpsPoint*> gpsSequence;
	patNetwork* baseNetwork; 
	set<patArcTransition>* arcTranSet;
	patUnixUniform ranNumUniform;

	vector<patGpsPoint>* originalGpsPoints;
};

#endif
