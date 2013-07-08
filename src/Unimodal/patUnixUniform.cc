//-*-c++-*------------------------------------------------------------
//
// File name : patUnixUniform.cc
// Author :    \URL[Michel Bierlaire]{http://roso.epfl.ch/mbi}
// Date :      Thu Mar  6 16:21:25 2003
//
//--------------------------------------------------------------------

#include <stdlib.h>
#include "patUnixUniform.h"


patUnixUniform::patUnixUniform(unsigned long s) : seed(s) {
  srand(seed) ;
}
void patUnixUniform::setSeed(unsigned long s) {
  seed = s ;
  srand(seed) ;
}
unsigned long patUnixUniform::getSeed() {
  return seed ;
}
patReal patUnixUniform::getUniform(patError*& err) {
  return rand() / (double(RAND_MAX+1.0)) ;
}

