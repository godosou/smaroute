//-*-c++-*------------------------------------------------------------
//
// File name : patDiscreteDistribution.h
// Author :    \URL[Michel Bierlaire]{http://rosowww.epfl.ch/mbi}
// Date :      Thu Mar  6 16:32:31 2003
//
//--------------------------------------------------------------------

#ifndef patDiscreteDistribution_h
#define patDiscreteDistribution_h

#include <vector>
#include "patType.h"

class patUniform ;

/**
   @doc This objects generates draw from a discrete
   distribution. Based on a table of real values representing relative
   weights, it returns a random index, such that the probability of
   drawing j is the weight of j divided by the sum of all weights in
   the table.
 */

class patDiscreteDistribution {

 public:
  patDiscreteDistribution(const vector<patReal>* d,patUniform* rng) ;

  vector<patReal>::size_type operator()() ;

 private:

  const vector<patReal>* weights ;
  patUniform* randomNumberGenerator ;
  
};
#endif
