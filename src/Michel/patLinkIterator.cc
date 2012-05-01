//----------------------------------------------------------------
// File: patLinkIterator.cc
// Author: Michel Bierlaire
// Creation: Wed May 20 16:54:20 2009
//----------------------------------------------------------------

#include "patDisplay.h"
#include "patLinkIterator.h"
#include "patNetwork.h"

patLinkIterator::patLinkIterator(list<unsigned long>* lon,
				 patNetwork* n) : listOfNodes(lon),
						  theNetwork(n) {
  
}
void patLinkIterator::first() {
  upNode = listOfNodes->begin() ;
  downNode = listOfNodes->begin() ;
  if (downNode != listOfNodes->end()) {
    ++downNode ;
  }
}

void patLinkIterator::next() {
  ++upNode ;
  ++downNode ;
}
bool patLinkIterator::isDone() {
  return (downNode == listOfNodes->end()) ;
}
patArc* patLinkIterator::currentItem() {
  return theNetwork->getArcFromNodesUserId(*upNode,*downNode) ;
}

