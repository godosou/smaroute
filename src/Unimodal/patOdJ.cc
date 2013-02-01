
#include <iostream>
#include "patConst.h"
#include "patNetwork.h"
#include "patDisplay.h"
#include "patError.h"
#include "patOdJ.h"//Jingmin
#include "patErrOutOfRange.h"
#include "patErrMiscError.h"
#include "patErrNullPointer.h"
#include "patPathJ.h"
#include "patShortestPathAlgoDest.h"
#include "patShortestPathTreeRange.h"
#include "patShortestPathAlgoRange.h"
#include "patShortestPathTreeDest.h"
 
#include "patNBParameters.h"
#include "patPower.h"
#include "patPathJ.h"
patOdJ::patOdJ()
{

}
patOdJ::patOdJ(patNode* o, patNode* d) : 
	orig(o), 
	dest(d) 
	{
	
}

ostream& operator<<(ostream& str, const patOdJ& x) {
  str << "[" << *(x.orig) << "-" << *(x.dest) << "]" ;
  return str ;
}

/*
 operator for comparing paths
 */
patBoolean operator<(const patOdJ& od1, const patOdJ& od2) {
  if (od1.orig->userId < od2.orig->userId) {
    return patTRUE ;
  }
  if (od1.orig->userId > od2.orig->userId) {
    return patFALSE ;
  }  
  return (od1.dest->userId < od2.dest->userId) ;
}

/*
 operation "=" for comparing paths
 */
patBoolean operator==(const patOdJ& od1, const patOdJ& od2) {
	if(od1.orig == od2.orig && od2.dest == od2.dest){
		return patTRUE;
	}
	return patFALSE;
}



/*
 find a path 
 */
patPathJ* patOdJ::findPath( patPathJ aPath){

	set<patPathJ>::iterator pathFound = pathSet.find(aPath);
	
	if(pathFound == pathSet.end()){
		return NULL;
	}
	
	return const_cast<patPathJ*>(&(*pathFound));
}

patPathJ* patOdJ::addPath(patPathJ&  thePath){
	patPathJ* pathFound =	findPath(thePath);
	
	if (pathFound != NULL){
		return pathFound;	
	}
	pathSet.insert(pathSet.end(),thePath);
	
	return findPath(thePath);

}


set<patPathJ>* patOdJ::getAllPaths(){

	return &pathSet;
}

patULong patOdJ::getPathNumber(){
	return pathSet.size();
}

patNode* patOdJ::getOrigin(){
return orig;
}
patNode* patOdJ::getDestination(){
return dest;
}
/*
void patOdJ::setPathSetByDDRs(map<patPathJ*,vector<patReal> >* pathDDRs){
	//DEBUG_MESSAGE("path size: "<<pathSet.size()<<", ddr size: "<<pathDDRs->size());
	for (set<patPathJ>::iterator pathIter=pathSet.begin();
		pathIter!=pathSet.end();
		){
		
		if(pathDDRs->empty()||
			pathDDRs==NULL||
			(pathDDRs->find(const_cast<patPathJ*>(&(*pathIter)))==pathDDRs->end()&&
			ddrsPathSet.find(const_cast<patPathJ*>(&(*pathIter)))==ddrsPathSet.end())
			
			){
		
			pathSet.erase(pathIter++);
			continue;
					
		}
		else{
			ddrsPathSet.insert(const_cast<patPathJ*>(&(*pathIter)));
			pathIter++;
		}
	}
//	DEBUG_MESSAGE("pathSet:"<<pathSet.size()<<", ddrsPathSet:"<<ddrsPathSet.size());
}
*/