//-*-c++-*------------------------------------------------------------
//
// File name : patShortestPathAlgoDest.h
// Author :    \URL[Michel Bierlaire]{http://roso.epfl.ch/mbi}
// Date :      Tue May 16 15:19:24 2006
//
//--------------------------------------------------------------------

#ifndef patShortestPathAlgoDest_h
#define patShortestPathAlgoDest_h

#include <vector>
#include <list>
#include <deque>
#include "patType.h"
#include "patError.h"
#include "patShortestPathTreeDest.h"
#include "patArc.h"
class patShortestPathAlgoDest {
 
 public:

patShortestPathAlgoDest(patULong root,
			vector< list <  pair<patReal,patULong> > >* adjList,
			patReal aMinimumLabel);
  patShortestPathAlgoDest(patULong root,
		     vector< list <  pair<patArc*,patULong> > >* adjList,
			  patReal aMinimumLabel) ;
  // Return patFALSE if a negative cycle has been detected. patTRUE otherwise.
  patBoolean computeShortestPathTree(patError*& err) ;
  
  patShortestPathTreeDest getTree() ;
  
private:
  /**
     Each element of the vector corresponds to a node
     and contains the list of successor nodes. The pair contains first the generalized cost and second the internal id, 
  */
  vector< list <  pair<patArc*,patULong> > >* adjacencyLists ;
  patULong theDest ;
  patShortestPathTreeDest theTree ;
  deque<patULong> listOfNodes ;
  patReal minimumLabel ;
};



#endif
