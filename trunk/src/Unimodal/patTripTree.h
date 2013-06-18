#ifndef patTripTree_h
#define patTripTree_h


#include <vector>
#include <list>
#include "patType.h"
#include "patError.h"

class patTripTree {

  friend class patTrip;

public:
  patTripTree(patULong theOrig, patULong nNodes) ;

protected:
  patULong root ;
  vector<patReal> DDRValues ;
  vector<patULong> upNodes ;
  vector<patULong> downNodes ;
  vector<list<patULong>> internalNodes;

};


#endif
