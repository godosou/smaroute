
#ifndef patShortestPathAlgoRange_h
#define patShortestPathAlgoRange_h

#include <vector>
#include <deque>
#include <list>
#include "patNetwork.h"	
#include "patGpsPoint.h"
#include "patShortestPathTreeRange.h"

class patShortestPathAlgoRange {
 
 public:
  // The minimumLabel is used to detect negative cycles. See Bertsekas, p. 61
  patShortestPathAlgoRange(patGpsPoint* gpsPoint, 
  						 vector< list <  pair<patArc*,patULong> > >* adjList,patNetwork* theNetwork,
  						 patReal aMinimumLabel
  						) ;
  // Return patFALSE if a negative cycle has been detected. patTRUE otherwise.
  patBoolean computeShortestPathTree(set<patULong> theRoot,
  patReal ceil,set<patArc*>* ddrArcs) ;
  void buildAdjacencyLists();
  patShortestPathTreeRange getTree() ;
  
 private:

  patNetwork* baseNetwork;
  patGpsPoint* theGpsPoint;
  vector< list <  pair<patArc*,patULong> > >* adjacencyLists;
  set<patULong> theRoot ;
  patShortestPathTreeRange theTree ;
  deque<patULong> listOfNodes ;
  patReal minimumLabel ;

};




#endif
