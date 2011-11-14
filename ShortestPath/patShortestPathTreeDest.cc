//-*-c++-*------------------------------------------------------------
//
// File name : patShortestPathTreeDest.cc
// Author :    \URL[Michel Bierlaire]{http://roso.epfl.ch/mbi}
// Date :      Tue May 16 15:08:02 2006
//
//--------------------------------------------------------------------

#include "patShortestPathTreeDest.h"
#include "patErrOutOfRange.h"
#include "patArc.h"
patShortestPathTreeDest::patShortestPathTreeDest(patULong theRoot, patULong nNodes) :
    theDest(theRoot),
    labels(nNodes,patMaxReal),
    successor(nNodes,patBadId) {
  
} ;
  
list<patULong> patShortestPathTreeDest::getShortestPathFrom(patULong fromNode,
							    patError*& err) {

//   DEBUG_MESSAGE("From node" << fromNode) ;
//   DEBUG_MESSAGE("List of successors") ;
//   for (int i = 0 ; i < successor.size() ; ++i) {
//     DEBUG_MESSAGE(i << "->" << successor[i]) ;
//   }

  list<patULong> theList ;
  if (fromNode >= successor.size()) {
    err = new patErrOutOfRange<patULong>(fromNode,0,successor.size()-1) ;
    WARNING(err->describe()) ;
    return list<patULong>() ;
  }

  for (patULong currentNode = fromNode ;
       currentNode != patBadId ;
       currentNode = successor[currentNode]) {
    theList.push_back(currentNode) ;
  }

  return theList ;
}
