/*
 * patSampleDiscreteDistribution.cc
 *
 *  Created on: Jun 1, 2012
 *      Author: jchen
 */

#include "patSampleDiscreteDistribution.h"
#include "patDisplay.h"
#include "patException.h"
#include <numeric>
using namespace std;
patSampleDiscreteDistribution::patSampleDiscreteDistribution() {
	// TODO Auto-generated constructor stub

}

patSampleDiscreteDistribution::~patSampleDiscreteDistribution() {
	// TODO Auto-generated destructor stub
}

unsigned int patSampleDiscreteDistribution::operator()(
		const vector<double> probas, const patRandomNumber& rnd) {
	double sum = 0.0;

	for (vector<double>::const_iterator p_iter = probas.begin();
			p_iter != probas.end(); ++p_iter) {
		sum += *p_iter;
	}
	if (sum == 0.0) {
		WARNING("all probas are zero");
	}
	double r_nbr = rnd.nextDouble();
	double cumul = 0.0;
	for (vector<double>::size_type i = 0; i < probas.size(); ++i) {
		cumul += (probas.at(i)) / sum;
		if (r_nbr < cumul) {
			return i;
		}
	}
	return probas.size() - 1;

}

vector<unsigned int> patSampleDiscreteDistribution::sampleWithOutReplaceMent(
		const vector<double> probas, const unsigned int count,
		const patRandomNumber& rnd) {

	if (count > probas.size()) {
		throw RuntimeException("Sample more than the population");
	}
	vector<unsigned int> sampled;
	vector<double> copy_proba = probas;
	for (unsigned i = 0; i < count; ++i) {

		unsigned int new_sample=patSampleDiscreteDistribution()(copy_proba,rnd);
//		DEBUG_MESSAGE(new_sample);
		sampled.push_back(new_sample);
		copy_proba[new_sample]=0.0;
	}
	return sampled;
}
