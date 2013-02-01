//----------------------------------------------------------------
// File: patRandomWalk.h
// Author: Michel Bierlaire
// Creation: Tue May 19 17:09:55 2009
//----------------------------------------------------------------

#ifndef patRandomWalk_h
#define patRandomWalk_h

#include "patPathGenerationAlgo.h"
#include "patUnixUniform.h"

class patRandomWalk : public patPathGenerationAlgo {

 public:
  patRandomWalk() ;
  void generateNextPath(patNetwork* aNetwork, patOd aOd, patError*& err) ;
  patString getReport() ;
  void setKumaParameters(double a, double b) ;  
 private:

  patUnixUniform randomNumbersGenerator ;
  pair<unsigned long,double> getNextNodeInRandomWalk(patNetwork* aNetwork, 
						 unsigned long currentNode, 
						 unsigned long dest, 
						 patError*& err) ;
  double kumaA ;
  double kumaB ;
};

#endif
