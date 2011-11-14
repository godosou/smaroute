//-*-c++-*------------------------------------------------------------
//
// File name : patShortestPathAlgoDest.cc
// Author :    \URL[Michel Bierlaire]{http://roso.epfl.ch/mbi}
// Date :      Wed Apr 26 10:22:23 2006
//
//--------------------------------------------------------------------

#include "patDisplay.h"
#include "patShortestPathAlgoDest.h"

patShortestPathTreeDest::patShortestPathTreeDest(){}

patShortestPathAlgoDest::
patShortestPathAlgoDest(patULong root,
			vector< list <  pair<patReal,patULong> > >* adjList,
			patReal aMinimumLabel) :
  theDest(root),
  theTree(root,adjList->size()),
  minimumLabel(aMinimumLabel) {
  
}

patShortestPathAlgoDest::
patShortestPathAlgoDest(patULong root,
			vector< list <  pair<patArc*,patULong> > >* adjList,
			patReal aMinimumLabel) :
  theDest(root),
  adjacencyLists(adjList),
  theTree(root,adjList->size()),
  minimumLabel(aMinimumLabel) {

}

patBoolean patShortestPathAlgoDest::computeShortestPathTree(patError*& err) {
  listOfNodes.push_back(theDest) ;
  theTree.labels[theDest] = 0.0 ;

  while (!listOfNodes.empty()) {
    patULong nodeToProcess = *listOfNodes.begin() ;
    listOfNodes.pop_front() ;
    for (list < pair<patArc*,patULong> >::iterator upNode =
	   (*adjacencyLists)[nodeToProcess].begin() ;
	 upNode != (*adjacencyLists)[nodeToProcess].end() ;
	 ++ upNode) {
      patULong upNodeId = upNode->second ;
      patReal linkCost = upNode->first->getLength() ;
      patReal upNodeLabel = theTree.labels[upNodeId] ;
      if (upNodeLabel > theTree.labels[nodeToProcess] + 
	  linkCost) {
	theTree.labels[upNodeId] = 
	  theTree.labels[nodeToProcess] + linkCost ;
	if (theTree.labels[upNodeId] < minimumLabel) {
	  WARNING("NEGATIVE CYCLE DETECTED") ;
	  return patFALSE ;
	}
	theTree.successor[upNodeId] = nodeToProcess ;

	//	DEBUG_MESSAGE("Arc " << upNodeId << " to " << nodeToProcess << " in the shortest path") ;
	// Add the node following Bertsekas (1993)
	if (listOfNodes.empty()) {
	  listOfNodes.push_back(upNodeId) ;
	}
	else {
	  patReal topLabel = theTree.labels[*(listOfNodes.begin())] ;
	  if (upNodeLabel <= topLabel) {
	    listOfNodes.push_front(upNodeId) ;
	  }
	  else {
	    listOfNodes.push_back(upNodeId) ;
	  }
	}
      }
    }
  }
  return patTRUE ;
}
  
patShortestPathTreeDest patShortestPathAlgoDest::getTree() {
  return theTree ;
}
