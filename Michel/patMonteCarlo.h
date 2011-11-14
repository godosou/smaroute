//----------------------------------------------------------------
// File: patMonteCarlo.h
// Author: Michel Bierlaire
// Creation: Wed May 20 11:37:57 2009
//----------------------------------------------------------------

#ifndef patMonteCarlo_h
#define patMonteCarlo_h

#include "patPathGenerationAlgo.h"
#include "patNormalWichura.h"
#include "patUnixUniform.h"

class patMonteCarlo : public patPathGenerationAlgo {

 public:
  patMonteCarlo() ;
  void generateNextPath(patNetwork* aNetwork, patOd aOd, patError*& err) ;
  patString getReport() ;
  void setStandardError(patReal s) ;
 private:
  patNormalWichura theNormal ;
  patUnixUniform arng ;
  patReal stderr ;
};

#endif
