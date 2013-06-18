
//----------------------------------------------------------------
// File: patOd.h
// Author: Michel Bierlaire
// Creation: Sun Nov  2 17:33:44 2008
//----------------------------------------------------------------

#ifndef patOd_h
#define patOd_h

#include "patType.h"

class patOd {
  friend ostream& operator<<(ostream& str, const patOd& x) ;
  friend patBoolean operator<(const patOd& od1, const patOd& od2) ;
 public:
  patOd(patULong o, patULong d) ;
  // User id
  patULong orig ;
  // User id
  patULong dest ;
};
#endif
