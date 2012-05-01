/*
 * patSampleFromDiscreteUntilCdf.cc
 *
 *  Created on: Nov 16, 2011
 *      Author: jchen
 */

#include "patSampleFromDiscreteUntilCdf.h"
#include <time.h>
#include "patDisplay.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <numeric>

boost::mt19937 gen;
patSampleFromDiscreteUntilCdf::patSampleFromDiscreteUntilCdf(
		vector<double> probabilities) :
		m_probabilities(probabilities) {

}
bool patSampleFromDiscreteUntilCdf::normalizeProbabilities(
		vector<double>& probas) {
	double sum = 0.0;
	for (unsigned int i = 0; i < probas.size(); ++i) {
		//DEBUG_MESSAGE(probas[i]);
		sum += probas[i];
	}

	if (sum == 0.0) {
		//WARNING("Invalid proba, all zero");
		return false;
	}

	for (unsigned int i = 0; i < probas.size(); ++i) {
		probas[i] /= sum;
	}
	return true;

}
patSampleFromDiscreteUntilCdf::~patSampleFromDiscreteUntilCdf() {
}

set<int> patSampleFromDiscreteUntilCdf::sample(double cdf) {
	set<int> sampled;
	if (!normalizeProbabilities(m_probabilities)) {
		return sampled;
	}

	vector<double> sampling = m_probabilities;
	double sampled_cdf = 0.0;
	while (sampled_cdf < cdf) {
		if (!normalizeProbabilities(sampling)) {
			return sampled;
		}
		/*
		 patDiscreteDistribution discreteDraw(&sampling, &m_ran_uniform);
		 int new_sample = discreteDraw();
		 sampling[new_sample] = 0.0;
		 sampled_cdf += m_probabilities[new_sample];
		 sampled.insert(new_sample);
		 */
	}
	return sampled;
}

void patSampleFromDiscreteUntilCdf::setAsLogLike() {

	double largest = -DBL_MAX;
	for (unsigned int i = 0; i < m_probabilities.size(); ++i) {
		if (m_probabilities[i] > largest) {
			largest = m_probabilities[i];
		}
	}
	for (unsigned int i = 0; i < m_probabilities.size(); ++i) {
		m_probabilities[i]= exp(m_probabilities[i]-largest);
	}

}
set<int> patSampleFromDiscreteUntilCdf::sampleByCount(unsigned long count) {
	set<int> sampled;
	if (!normalizeProbabilities(m_probabilities)) {
		return sampled;
	}

	vector<double> sampling = m_probabilities;
	unsigned long sampled_count = 0;
	while (sampled_count < count) {
		if (!normalizeProbabilities(sampling)) {
			return sampled;
		}

		vector<double> cumulative;
		std::partial_sum(sampling.begin(), sampling.end(),
				std::back_inserter(cumulative));
		boost::uniform_real<> dist(0, cumulative.back());
		boost::variate_generator<boost::mt19937&, boost::uniform_real<> > die(
				gen, dist);

		//patDiscreteDistribution discreteDraw(&sampling, &m_ran_uniform);
		int new_sample = std::lower_bound(cumulative.begin(), cumulative.end(),
				die()) - cumulative.begin();
		sampling[new_sample] = 0.0;
		sampled_count += 1;
		sampled.insert(new_sample);
	}
	return sampled;
}
