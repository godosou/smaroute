
#ifndef patOdJ_h
#define patOdJ_h

#include "patType.h"
#include "patShortestPathTreeDest.h"
#include "patNode.h"
#include "patArc.h"
#include <iostream>
#include <set>
//#include "patPathJ.h"//Jingmin
#include "patNetwork.h"
#include "patShortestPathTreeRange.h"


class patPathJ;
class patOdJ {

	public:
		patOdJ();
		/**
		* Constructor
		* @param o origin node
		* @param d destination node
		*/
		patOdJ(patNode* o, patNode* d) ;
	patPathJ* findPath( patPathJ aPath);
	patPathJ* addPath(patPathJ&  thePath);	
	set<patPathJ>* patOdJ::getAllPaths();
	patULong getPathNumber();
		friend ostream& operator<<(ostream& str, const patOdJ& x) ;
		friend patBoolean operator<(const patOdJ& od1, const patOdJ& od2) ;
		friend patBoolean operator==(const patOdJ& od1, const patOdJ& od2);
		patNode* getOrigin();
		patNode* getDestination();
void setPathSetByDDRs(map<patPathJ*,vector<patReal> >* pathDDRs);
  protected:
  patNetwork* theNetwork;
  patNode* orig ;
  patNode* dest ;
set<patPathJ> pathSet;
};
#endif
