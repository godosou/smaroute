//----------------------------------------------------------------
// File: patListSubpath.cc
// Author: Michel Bierlaire
// Creation: Sat May 30 21:07:51 2009
//----------------------------------------------------------------

#include "patListSubpath.h"
#include "patErrNullPointer.h"
#include "patDisplay.h"

patListSubpath::patListSubpath(patULong aId) :
  patSubpath(aId) {

}

list<patULong> patListSubpath::getListOfNodes(patError*& err) {

  list<patULong> theNodesList ;
  for (list<patSubpath*>::iterator i = theList.begin() ;
       i != theList.end() ;
       ++i) {
    if ((*i) == NULL) {
      err = new patErrNullPointer("patSubpath") ;
      WARNING(err->describe());
      return list<patULong>() ;      
    }
    list<patULong> nodes = (*i)->getListOfNodes(err) ;
    if (err != NULL) {
      WARNING(err->describe()) ;
      return list<patULong>() ;
    }
    if (i != theList.begin()) {
      // The first node must be removed to avoid duplication. It is
      // indeed the last onew of the previous list.
      nodes.pop_front() ;
    }

    theNodesList.splice(theNodesList.end(),nodes) ;
  }
  return theNodesList ;
}


list<patULong> patListSubpath::getListOfArcs(patError*& err) {
  list<patULong> theArcsList ;
  for (list<patSubpath*>::iterator i = theList.begin() ;
       i != theList.end() ;
       ++i) {
    if ((*i) == NULL) {
      err = new patErrNullPointer("patSubpath") ;
      WARNING(err->describe());
      return list<patULong>() ;      
    }
    list<patULong> arcs = (*i)->getListOfArcs(err) ;
    if (err != NULL) {
      WARNING(err->describe()) ;
      return list<patULong>() ;
    }
    theArcsList.splice(theArcsList.end(),arcs) ;
  }
  return theArcsList ;

}

void patListSubpath::pushFrontSubpath(patSubpath* aSubpath) {
  theList.push_front(aSubpath) ;
}

void patListSubpath::pushBackSubpath(patSubpath* aSubpath) {
  theList.push_back(aSubpath) ;
}

patULong patListSubpath::getOrig(patError*& err) {
  patSubpath* first = *(theList.begin()) ;
  return first->getOrig(err) ;
}

patULong patListSubpath::getDest(patError*& err) {
  patSubpath* last =  *(theList.rbegin()) ;
  return last->getDest(err) ;
}

