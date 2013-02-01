//----------------------------------------------------------------
// File: patOd.cc
// Author: Michel Bierlaire
// Creation: Sun Nov  2 17:35:46 2008
//----------------------------------------------------------------

#include <iostream>
#include "patConst.h"

#include "patOd.h"

patOd::patOd(patULong o, patULong d) : orig(o), dest(d) {

}

ostream& operator<<(ostream& str, const patOd& x) {
  str << "[" << x.orig << "-" << x.dest << "]" ;
  return str ;
}

patBoolean operator<(const patOd& od1, const patOd& od2) {
  if (od1.orig < od2.orig) {
    return patTRUE ;
  }
  if (od1.orig > od2.orig) {
    return patFALSE ;
  }  
  return (od1.dest < od2.dest) ;
}
