#ifndef patGenPathFromArcTranSet_h
#define patGenPathFromArcTranSet_h

#include <set>
#include <list>
#include <map>
#include "patType.h"
class patArcTransition;
class patPathJ;
class patGpsPoint;

class patGenPathFromArcTranSet{
public:
patGenPathFromArcTranSet(set<patArcTransition>* theArcTranSet,patGpsPoint* aG,
	patGpsPoint* bG);

void genPath();

patBoolean recordPath(list<patPathJ> pathTemp);
void connectTrans(patArcTransition* currTran,
		list<patPathJ>* pathTemp,
		map<patGpsPoint*, set<patArcTransition*> >* beginTranSet);

set<patPathJ> getPathSet();
protected:
set<patPathJ> pathSet;
set<patArcTransition>* arcTranSet;
patGpsPoint* startGpsPoint;
patGpsPoint* endGpsPoint;
};

#endif

