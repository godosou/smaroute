//----------------------------------------------------------------
// File: patPath.h
// Author: Michel Bierlaire
// Creation: Mon Nov 17 22:25:11 2008
//----------------------------------------------------------------

#ifndef patPath_h
#define patPath_h

#include <list>
#include <map>
#include "patType.h"
#include "patOd.h"
#include "patError.h"
#include "patIterator.h"

class patArc ;

class patPath {
  friend class patNetwork ;
  friend ostream& operator<<(ostream& str, const patPath& x) ;
  friend bool operator<(const patPath& p1, const patPath& p2) ;

 public:
  patPath() ;
  patString name ;
  patOd od ;

  /**
     List of the internal id of the nodes
   */
  list<unsigned long> listOfNodes ;

  bool checkOd() ;

 protected:
  map<patString,double> attributes ;

};
#endif 
