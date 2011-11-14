#ifndef patShortestPathTreeRange_h
#define patShortestPathTreeRange_h


#include <vector> 
#include <list>
#include <set>
#include "patType.h"
#include "patError.h"
#include "patArc.h"
#include "patNetwork.h"
class patGpsPoint;
class patShortestPathTreeRange {

  friend class patShortestPathAlgoRange ;
  friend class patOdJ;

public:
  patShortestPathTreeRange( patULong nNodes) ;
  patShortestPathTreeRange( );
  list<patULong> getShortestPathTo(patULong dest,
							patNetwork* theNetwork,map<patArc*,patReal>* theLinkDDR) ;
	list<patArc* > 	 getShortestPathToV2(patULong dest);
		patReal getShortestPathCost(patULong dest);
protected:
  set<patULong> root ;
  vector<patReal> labels ;
	vector<pair<patULong,patArc* > > predecessor;
  vector<list<pair<patULong,patArc*> > > successor ;

};


#endif
