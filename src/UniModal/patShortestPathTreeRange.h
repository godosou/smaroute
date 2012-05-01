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
  patShortestPathTreeRange( unsigned long nNodes) ;
  patShortestPathTreeRange( );
  list<unsigned long> getShortestPathTo(unsigned long dest,
							patNetwork* theNetwork,map<patArc*,double>* theLinkDDR) ;
	list<patArc* > 	 getShortestPathToV2(unsigned long dest);
		double getShortestPathCost(unsigned long dest);
protected:
  set<unsigned long> root ;
  vector<double> labels ;
	vector<pair<unsigned long,patArc* > > predecessor;
  vector<list<pair<unsigned long,patArc*> > > successor ;

};


#endif
