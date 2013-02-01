
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
  						 vector< list <  pair<patArc*,unsigned long> > >* adjList,patNetwork* theNetwork,
  						 double aMinimumLabel
  						) ;
  // Return patFALSE if a negative cycle has been detected. patTRUE otherwise.
  bool computeShortestPathTree(set<unsigned long> theRoot,
  double ceil,set<patArc*>* ddrArcs) ;
  void buildAdjacencyLists();
  patShortestPathTreeRange getTree() ;
  
 private:

  patNetwork* baseNetwork;
  patGpsPoint* theGpsPoint;
  vector< list <  pair<patArc*,unsigned long> > >* adjacencyLists;
  set<unsigned long> theRoot ;
  patShortestPathTreeRange theTree ;
  deque<unsigned long> listOfNodes ;
  double minimumLabel ;

};




#endif
