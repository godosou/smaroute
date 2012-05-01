
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

double calAverageSpeed();
double calLengthCeil();
set<unsigned long> genEndNodes();
void appendSeg(patGpsPoint* gpsPoint,map<unsigned long,set<patPathJ > >* segSet,set<patArc*>* inheritDDR);

void unionTwoDomains(double* minLength,map<patPathJ,double >* newPathSet,
				map<patPathJ,double>* prevSet,set<patPathJ >* newSegSet,set<patArc*>* activeDomain);
void selectPaths(map<patPathJ,double>* newPathSet);
double sumProba(map<patPathJ,double>* newPathSet);
void selectPathsByPdf(list<patPathJ*>* preSelected,map<patPathJ,double>* newPathSet);
void selectPathsByCdf(list<patPathJ*>* preSelected,map<patPathJ,double>* newPathSet);

void selectPathsByNumber(list<patPathJ*>* preSelected,map<patPathJ,double>* newPathSet);
void selectPathsByShortest(map<patPathJ,double>* newPathSet,	
	set<patPathJ*>* tmpPathSet);
list<patPathJ*> selectPathByImportantDDR(map<patPathJ,double>* newPathSet);
set<patArc*>  selectImportantDDR();
void finalizeOneStage(map<patPathJ,double >* newPathSet);
set<patPathJ> getPaths();

double lastStage(patSample* theSample,patObservation* generatedObservation);
void init(patGpsPoint* firstGpsPoint);

void calPathDDR(map<patPathJ,double>* newPathSet);

patGpsPoint* lastGpsPoint();
void connectNodes(unsigned long node,list<unsigned long>* pathTemp,
		map<unsigned long,set<unsigned long> >* succ,
		map<patPathJ,double >*  newPathSet);
void newInitPath(list<unsigned long>* pathTemp,map<patPathJ,double >*  newPathSet);

void lowSpeedPoints(vector<patGpsPoint*>* lowSpeedGpsPoints,
	set<patArc*>* interArcs,	patNetwork* theNetwork );
protected:
	map<unsigned long, map<patPathJ,double> >  pathSet;
	map<unsigned long, map<patPathJ,double> >  oldPathSet;
	vector<patGpsPoint*> gpsSequence;
	patNetwork* baseNetwork; 
	set<patArcTransition>* arcTranSet;
	patUnixUniform ranNumUniform;

	vector<patGpsPoint>* originalGpsPoints;
};

#endif
