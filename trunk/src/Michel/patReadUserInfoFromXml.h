//----------------------------------------------------------------
// File: patReadUserInfoFromXml.h
// Author: Michel Bierlaire
// Creation: Sun Jun  7 10:25:58 2009
//----------------------------------------------------------------

#ifndef patReadUserInfoFromXml_h
#define patReadUserInfoFromXml_h

#include "patString.h"
#include "patType.h"
#include "patError.h"
#include "patNetwork.h"
#include "patReadNetworkFromXml.h"

class patReadUserInfoFromXml {

 public:
  patReadUserInfoFromXml(patString fName);
  bool readFile(patError*& err) ;
  patNetwork getNetwork() ;
 private:
  patString fileName ;
  patNetwork theNetwork ;
  bool cleanTheNetwork ;

};

#endif
