#include "patGenPathFromArcTranSet.h"
#include "patType.h"
#include "patGpsPoint.h"
#include "patArcTransition.h"
#include "patPathJ.h"
#include "patDisplay.h"
patGenPathFromArcTranSet::patGenPathFromArcTranSet(set<patArcTransition>* theArcTranSet,
	patGpsPoint* aG,
	patGpsPoint* bG):
	arcTranSet(theArcTranSet),
	startGpsPoint(aG),
	endGpsPoint (bG){

}

void patGenPathFromArcTranSet::genPath(){
	DEBUG_MESSAGE("arc tran number "<< arcTranSet->size());
	map<patGpsPoint*, set<patArcTransition*> > beginTranSet;
	
	for(set<patArcTransition>::iterator arcTranIter = arcTranSet->begin();
					arcTranIter!=arcTranSet->end();
					++arcTranIter){
		patArcTransition* currTran = const_cast<patArcTransition*>(&(*arcTranIter));
		patGpsPoint* aGps = currTran->getPrevGpsPoint();
		if(beginTranSet.find(aGps)==beginTranSet.end()){
			beginTranSet[aGps] = set<patArcTransition*>();
		}
		beginTranSet[aGps].insert(currTran);
		
	}
	
	
	if(beginTranSet.find(startGpsPoint) == beginTranSet.end()||beginTranSet[startGpsPoint].empty()){
		DEBUG_MESSAGE("no domain at first gps point");
		return;
	}
	DEBUG_MESSAGE("trans from frist gps point "<<beginTranSet[startGpsPoint].size());
	for(set<patArcTransition*>::iterator aIter = beginTranSet[startGpsPoint].begin();
				aIter!=beginTranSet[startGpsPoint].end();
				++aIter){
		list<patPathJ> pathTemp;
		connectTrans(*aIter,&pathTemp,&beginTranSet);
	}
}

patBoolean patGenPathFromArcTranSet::recordPath(list<patPathJ> pathTemp){
	//DEBUG_MESSAGE("recrod a path");
	list<patPathJ>::iterator pIter = pathTemp.begin();
	patPathJ aPath = *pIter;
	pIter++;
	for (;pIter!=pathTemp.end();++pIter){
		if (aPath.join(*pIter)==patFALSE){
			DEBUG_MESSAGE("can't connect between segements");
			return patFALSE;
		}
	}
	pathSet.insert(aPath);
	//DEBUG_MESSAGE("a path recorded");
	return patTRUE;
}

void patGenPathFromArcTranSet::connectTrans(patArcTransition* currTran,
		list<patPathJ>* pathTemp,
		map<patGpsPoint*, set<patArcTransition*> >* beginTranSet){
	pathTemp->push_back(currTran->getPath());
	patGpsPoint* nextGpsPoint = currTran->getCurrGpsPoint();
	if(beginTranSet->find(nextGpsPoint) == beginTranSet->end()||
		(*beginTranSet)[nextGpsPoint].empty()){
		if(nextGpsPoint == endGpsPoint){
			recordPath(*pathTemp);
		}
		return ;
	}
	patArc* lastArc = currTran->getLastArc();
	for(set<patArcTransition*>::iterator tranIter = (*beginTranSet)[nextGpsPoint].begin();
		tranIter!= (*beginTranSet)[nextGpsPoint].end();
		++tranIter){
		patArcTransition* aTran = *tranIter;
		if (aTran->getFirstArc() == lastArc){
			connectTrans(aTran,
				pathTemp,
				beginTranSet);
			pathTemp->pop_back();
		}
	}
}

set<patPathJ> patGenPathFromArcTranSet::getPathSet(){
	return pathSet;
}

