/*
 * patSampleFromDiscreteUntilCdf.h
 *
 *  Created on: Nov 16, 2011
 *      Author: jchen
 */

#ifndef PATSAMPLEFROMDISCRETEUNTILCDF_H_
#define PATSAMPLEFROMDISCRETEUNTILCDF_H_
#include <set>
#include <vector>
using namespace std;
class patSampleFromDiscreteUntilCdf {
public:
	patSampleFromDiscreteUntilCdf(vector<double> probabilities);

	/**
	 * Normalize the probablity vector such that it sums up to one.
	 * @return false if all are zero; true otherwise.
	 */
	bool normalizeProbabilities(vector<double>& probas);
	virtual ~patSampleFromDiscreteUntilCdf();
	void setAsLogLike();
	/**
	 * Sample a set of elements until the sum of their pdf >= a pre-defined value
	 * @param cdf the pre-defined cdf
	 * @return a set of indices for the elements;
	 */
	set<int> sample(double cdf);
	set<int> sampleByCount(unsigned long count);

protected:

	vector<double> m_probabilities;
};

#endif /* PATSAMPLEFROMDISCRETEUNTILCDF_H_ */
