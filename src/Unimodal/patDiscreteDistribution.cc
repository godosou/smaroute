//-*-c++-*------------------------------------------------------------
//
// File name : patDiscreteDistribution.cc
// Author :    \URL[Michel Bierlaire]{http://rosowww.epfl.ch/mbi}
// Date :      Thu Mar  6 16:37:30 2003
//
//--------------------------------------------------------------------

#include <numeric>
#include "patDisplay.h"
#include "patConst.h"
#include "patDiscreteDistribution.h"
#include "patUniform.h"

patDiscreteDistribution::patDiscreteDistribution(const vector<patReal>* d,
						 patUniform* rng) : weights(d), randomNumberGenerator(rng) {

}

vector<patReal>::size_type patDiscreteDistribution::operator()() {
  if (randomNumberGenerator == NULL || weights == NULL) {
    return patBadId ;
  }

  patReal sum = accumulate(weights->begin(),weights->end(),0.0) ;

  patError* err(NULL) ;
  patReal randomNbr = randomNumberGenerator->getUniform(err) ;
  patReal cumul = 0.0 ;
  for (vector<patReal>::size_type i = 0 ;
       i < weights->size() ;
       ++i) {
    cumul += (*weights)[i]/sum ;
    if (randomNbr < cumul) {
      return i ;
    }
  }


  return(weights->size()) ;

  
}
