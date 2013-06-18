//----------------------------------------------------------------
// File: patListSubpath.cc
// Author: Michel Bierlaire
// Creation: Sat May 30 21:07:51 2009
//----------------------------------------------------------------

#include "patListSubpath.h"
#include "patErrNullPointer.h"
#include "patDisplay.h"

patListSubpath::patListSubpath(unsigned long aId) :
  patSubpath(aId) {

}

list<unsigned long> patListSubpath::getListOfNodes(patError*& err) {

  list<unsigned long> theNodesList ;
  for (list<patSubpath*>::iterator i = theList.begin() ;
       i != theList.end() ;
       ++i) {
    if ((*i) == NULL) {
      err = new patErrNullPointer("patSubpath") ;
      WARNING(err->describe());
      return list<unsigned long>() ;      
    }
    list<unsigned long> nodes = (*i)->getListOfNodes(err) ;
    if (err != NULL) {
      WARNING(err->describe()) ;
      return list<unsigned long>() ;
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


list<unsigned long> patListSubpath::getListOfArcs(patError*& err) {
  list<unsigned long> theArcsList ;
  for (list<patSubpath*>::iterator i = theList.begin() ;
       i != theList.end() ;
       ++i) {
    if ((*i) == NULL) {
      err = new patErrNullPointer("patSubpath") ;
      WARNING(err->describe());
      return list<unsigned long>() ;      
    }
    list<unsigned long> arcs = (*i)->getListOfArcs(err) ;
    if (err != NULL) {
      WARNING(err->describe()) ;
      return list<unsigned long>() ;
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

unsigned long patListSubpath::getOrig(patError*& err) {
  patSubpath* first = *(theList.begin()) ;
  return first->getOrig(err) ;
}

unsigned long patListSubpath::getDest(patError*& err) {
  patSubpath* last =  *(theList.rbegin()) ;
  return last->getDest(err) ;
}

