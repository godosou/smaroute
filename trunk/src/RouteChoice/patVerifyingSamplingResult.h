/*
 * patVerifyingSamplingResult.h
 *
 *  Created on: Jun 7, 2012
 *      Author: jchen
 */

#ifndef PATVERIFYINGSAMPLINGRESULT_H_
#define PATVERIFYINGSAMPLINGRESULT_H_
#include "patMultiModalPath.h"
#include "patObservation.h"
#include <map>
using namespace std;

class patNetworkElements;
class patChoiceSet;
class patPathGenerator;
class patVerifyingSamplingResult {
public:

	patVerifyingSamplingResult(const vector<patObservation>& observations);
	double verifyProbability(const patChoiceSet& universal_set,
			const patPathGenerator* path_generator) const;
	virtual ~patVerifyingSamplingResult();
protected:
	map<const patMultiModalPath, pair<int, double> > m_paths;
	int m_total_nbr_of_paths;
	double m_total_probas;
}
;

#endif /* PATVERIFYINGSAMPLINGRESULT_H_ */
