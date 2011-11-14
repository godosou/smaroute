#include "patDiscretizeDomain.h"
#include "patGpsPoint.h"
#include "patArc.h"
#include "patDisplay.h"

patDiscretizeDomain::patDiscretizeDomain(patGpsPoint* theGpsPoint):
gps(theGpsPoint){

}
void patDiscretizeDomain::run(){

}

set<patArc*> patDiscretizeDomain::getArcs(){
	set<patArc*> rtn;
	for(map<patArc*,set<patReal> >::iterator arcIter=positions.begin();
		arcIter!=positions.end();
		arcIter++){
		rtn.insert(arcIter->first);
	
	}
	return rtn.
}
set<patReal> patDiscretizeDomain::getPositions(patArc* theArc){
	set<patReal> rtn;
	if (positions.find(theArc)==positions.end()){
		return rtn;
	}
	else{
		return positions[theArc];
	}

}
map<patArc*,set<patReal> > patDiscretizeDomain::getAll(){
	return positions;
}