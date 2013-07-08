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
patShortestPathAlgoDest(unsigned long root,
			vector< list <  pair<double,unsigned long> > >* adjList,
			double aMinimumLabel) :
  theDest(root),
  theTree(root,adjList->size()),
  minimumLabel(aMinimumLabel) {
  
}

patShortestPathAlgoDest::
patShortestPathAlgoDest(unsigned long root,
			vector< list <  pair<patArc*,unsigned long> > >* adjList,
			double aMinimumLabel) :
  theDest(root),
  adjacencyLists(adjList),
  theTree(root,adjList->size()),
  minimumLabel(aMinimumLabel) {

}

bool patShortestPathAlgoDest::computeShortestPathTree(patError*& err) {
  listOfNodes.push_back(theDest) ;
  theTree.labels[theDest] = 0.0 ;

  while (!listOfNodes.empty()) {
    unsigned long nodeToProcess = *listOfNodes.begin() ;
    listOfNodes.pop_front() ;
    for (list < pair<patArc*,unsigned long> >::iterator upNode =
	   (*adjacencyLists)[nodeToProcess].begin() ;
	 upNode != (*adjacencyLists)[nodeToProcess].end() ;
	 ++ upNode) {
      unsigned long upNodeId = upNode->second ;
      double linkCost = upNode->first->getLength() ;
      double upNodeLabel = theTree.labels[upNodeId] ;
      if (upNodeLabel > theTree.labels[nodeToProcess] + 
	  linkCost) {
	theTree.labels[upNodeId] = 
	  theTree.labels[nodeToProcess] + linkCost ;
	if (theTree.labels[upNodeId] < minimumLabel) {
	  WARNING("NEGATIVE CYCLE DETECTED") ;
	  return false ;
	}
	theTree.successor[upNodeId] = nodeToProcess ;

	//	DEBUG_MESSAGE("Arc " << upNodeId << " to " << nodeToProcess << " in the shortest path") ;
	// Add the node following Bertsekas (1993)
	if (listOfNodes.empty()) {
	  listOfNodes.push_back(upNodeId) ;
	}
	else {
	  double topLabel = theTree.labels[*(listOfNodes.begin())] ;
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
  return true ;
}
  
patShortestPathTreeDest patShortestPathAlgoDest::getTree() {
  return theTree ;
}
