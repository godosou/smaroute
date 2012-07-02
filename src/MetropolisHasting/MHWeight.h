/*
 * MHWeight.h
 *
 *  Created on: Feb 29, 2012
 *      Author: jchen
 */

#ifndef MHWEIGHT_H_
#define MHWEIGHT_H_

template <class S>
class MHWeight {
public:
virtual double logWeight(const S& State) const=0;
virtual double logWeightWithoutCorrection(const S& State) const=0;

};

#endif /* MHWEIGHT_H_ */
