/*
 * patSampleDiscreteDistribution.h
 *
 *  Created on: Jun 1, 2012
 *      Author: jchen
 */
#include <vector>
#include "patRandomNumber.h"
using namespace std;

#ifndef PATSAMPLEDISCRETEDISTRIBUTION_H_
#define PATSAMPLEDISCRETEDISTRIBUTION_H_

class patSampleDiscreteDistribution {
public:
	patSampleDiscreteDistribution();
	unsigned int operator()(const vector<double> probas, const patRandomNumber& rnd);

	static vector<unsigned int> sampleWithOutReplaceMent(const vector<double> probas,
			const unsigned int count, const patRandomNumber& rnd);
	virtual ~patSampleDiscreteDistribution();
};

#endif /* PATSAMPLEDISCRETEDISTRIBUTION_H_ */
