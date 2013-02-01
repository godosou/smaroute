
#ifndef patShortestPathAlgoRangeSimpleOrig_h
#define patShortestPathAlgoRangeSimpleOrig_h

#include <vector>
#include <deque>
#include <list>
#include "patNetwork.h"	
#include "patGpsPoint.h"
#include "patShortestPathTreeRange.h"

class patShortestPathAlgoRangeSimpleOrig {
 
 public:
  // The minimumLabel is used to detect negative cycles. See Bertsekas, p. 61
  patShortestPathAlgoRange( vector< list <  pair<patArc*,patULong> > >* adjList,patNetwork* theNetwork,
  						 patReal aMinimumLabel
  						) ;
  // Return patFALSE if a negative cycle has been detected. patTRUE otherwise.
  patBoolean computeShortestPathTree(set<patULong> theRoot,  patReal ceil) ;
  void buildAdjacencyLists();
  patShortestPathTreeRange getTree() ;
  
 private:

  patNetwork* baseNetwork;
  vector< list <  pair<patArc*,patULong> > >* adjacencyLists;
  set<patULong> theRoot ;
  patShortestPathTreeRange theTree ;
  deque<patULong> listOfNodes ;
  patReal minimumLabel ;

};




#endif
