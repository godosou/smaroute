//----------------------------------------------------------------
// File: patSubpath.cc
// Author: Michel Bierlaire
// Creation: Sat May 30 19:17:59 2009
//----------------------------------------------------------------

#include "patSubpath.h"
#include "patDisplay.h"
#include "patErrNullPointer.h"
#include "patNetwork.h"

patSubpath::patSubpath(patULong aId) :
  id(aId) {

}

patPath patSubpath::getPath(patString aName,patError*& err) {
  patPath thePath ;
  thePath.name = aName ;
  thePath.od.orig = getOrig(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return patPath() ;
  }
  thePath.od.dest = getDest(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return patPath() ;
  }
  thePath.listOfNodes = getListOfNodes(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return patPath() ;
  }
  return thePath ;
  
}

patPath patSubpath::getPath(patError*& err) {
  stringstream str ;
  str << "Subpath " << id << " from " << getOrig(err) << " to " << getDest(err) ;
  return getPath(patString(str.str()),err) ;

}

patBoolean patSubpath::checkConnectivity(patNetwork* theNetwork, 
					 patError*& err) {

  if (theNetwork == NULL) {
    err = new patErrNullPointer("patNetwork") ;
    WARNING(err->describe()) ;
    return patFALSE ;
  }
   list<patULong> theArcs = getListOfArcs(err) ;
   if (err != NULL) {
    WARNING(err->describe()) ;
    return patFALSE ;
   }
   list<patULong>::iterator first = theArcs.begin() ;
   list<patULong>::iterator second = theArcs.begin() ;
   ++second ;
   while (second != theArcs.end()) {
     patBoolean ok = theNetwork->consecutiveArcs(*first,*second,err) ;
     if (err != NULL) {
       WARNING(err->describe()) ;
       return patFALSE ;
     }
     if (!ok) {
       WARNING("Arcs " << *first << " and " << *second << " are not consecutive") ;
       return patFALSE ;
     }
     ++first ;
     ++second ;
   }
   return patTRUE ;
}
patULong patSubpath::getId() const {
  return id ;
}
