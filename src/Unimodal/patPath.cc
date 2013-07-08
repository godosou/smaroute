//----------------------------------------------------------------
// File: patPath.cc
// Author: Michel Bierlaire
// Creation: Mon Nov 17 22:27:29 2008
//----------------------------------------------------------------

#include "patPath.h"
#include "patConst.h"
#include "patDisplay.h"
#include "patArc.h"
#include "patLinkIterator.h"
#include "patErrMiscError.h"

patPath::patPath() : 
  name(patString("No name")) ,
  od(patBadId,patBadId) {

}

patBoolean patPath::checkOd() {
  if (listOfNodes.empty()) {
    WARNING("Empty list of nodes") ;
    return patFALSE ;
  }
  patULong firstNode = *(listOfNodes.begin()) ;
  patULong lastNode = *(listOfNodes.rbegin()) ;
  patBoolean ok = patTRUE ;
  if (firstNode != od.orig) {
    WARNING("Origin of the path is " << firstNode << " and not " << od.orig) ;
    ok = patFALSE ;
  }
  if (lastNode != od.dest) {
    //    WARNING("Destination of the path is " << lastNode << " and not " << od.dest) ;
    ok = patFALSE ;
  }
  return ok ;
}

ostream& operator<<(ostream& str, const patPath& x) {
  str << x.name << " (from " << x.od.orig << " to " << x.od.dest << "): [" ;
  for (list<patULong>::const_iterator i = x.listOfNodes.begin() ;
       i != x.listOfNodes.end() ;
       ++i) {
    if (i != x.listOfNodes.begin()) {
      str << "->" ;
    }
    str << *i ;
  }
  str << "]; " ;
  for (map<patString,patReal>::const_iterator i = x.attributes.begin() ;
       i != x.attributes.end() ;
       ++i) {
    if (i != x.attributes.begin()) {
      str << "; " ;
    }
    str << i->first << "=" << i->second ;
  }
  return str ;
}

patBoolean operator<(const patPath& p1, const patPath& p2) {
  if (p1.listOfNodes.size() < p2.listOfNodes.size()) {
    return patTRUE ;
  }
  if (p1.listOfNodes.size() > p2.listOfNodes.size()) {
    return patFALSE ;
  }
  list<patULong>::const_iterator i1 = p1.listOfNodes.begin() ;
  list<patULong>::const_iterator i2 = p2.listOfNodes.begin() ;
  while (i1 != p1.listOfNodes.end()) {
    if (*i1 < *i2) {
      return patTRUE ;
    }
    if (*i1 > *i2) {
      return patFALSE ;
    }
    ++i1 ;
    ++i2 ;
  }
  return patFALSE ;
}
