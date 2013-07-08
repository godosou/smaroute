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
  patSubpath((anArc==NULL)?patBadId:anArc->m_user_id),theLink(anArc) {
  
}

list<unsigned long> patLinkSubpath::getListOfNodes(patError*& err) {
  list<unsigned long> theList ;
  theList.push_back(theLink->m_up_node_id) ;
  theList.push_back(theLink->m_down_node_id) ;
  return theList ;
}

list<unsigned long> patLinkSubpath::getListOfArcs(patError*& err) {
  if (theLink == NULL) {
    err = new patErrNullPointer("patArc") ;
    WARNING(err->describe()) ;
    return list<unsigned long>() ;
  }
  list<unsigned long> theList ;
  theList.push_back(theLink->m_user_id) ;
  return theList ;
}

unsigned long patLinkSubpath::getOrig(patError*& err) {
  if (theLink == NULL) {
    err = new patErrNullPointer("patArc") ;
    WARNING(err->describe()) ;
    return unsigned long() ;
  }
  return theLink->m_up_node_id ;
}

unsigned long patLinkSubpath::getDest(patError*& err) {
  if (theLink == NULL) {
    err = new patErrNullPointer("patArc") ;
    WARNING(err->describe()) ;
    return unsigned long() ;
  }
  return theLink->m_down_node_id ;
}
