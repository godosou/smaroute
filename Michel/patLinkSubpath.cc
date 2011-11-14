//----------------------------------------------------------------
// File: patLinkSubpath.cc
// Author: Michel Bierlaire
// Creation: Sat May 30 19:24:41 2009
//----------------------------------------------------------------

#include "patLinkSubpath.h"
#include "patErrNullPointer.h"
#include "patDisplay.h"
#include "patArc.h"

patLinkSubpath::patLinkSubpath(patArc* anArc) :
  patSubpath((anArc==NULL)?patBadId:anArc->userId),theLink(anArc) {
  
}

list<patULong> patLinkSubpath::getListOfNodes(patError*& err) {
  list<patULong> theList ;
  theList.push_back(theLink->upNodeId) ;
  theList.push_back(theLink->downNodeId) ;
  return theList ;
}

list<patULong> patLinkSubpath::getListOfArcs(patError*& err) {
  if (theLink == NULL) {
    err = new patErrNullPointer("patArc") ;
    WARNING(err->describe()) ;
    return list<patULong>() ;
  }
  list<patULong> theList ;
  theList.push_back(theLink->userId) ;
  return theList ;
}

patULong patLinkSubpath::getOrig(patError*& err) {
  if (theLink == NULL) {
    err = new patErrNullPointer("patArc") ;
    WARNING(err->describe()) ;
    return patULong() ;
  }
  return theLink->upNodeId ;
}

patULong patLinkSubpath::getDest(patError*& err) {
  if (theLink == NULL) {
    err = new patErrNullPointer("patArc") ;
    WARNING(err->describe()) ;
    return patULong() ;
  }
  return theLink->downNodeId ;
}
