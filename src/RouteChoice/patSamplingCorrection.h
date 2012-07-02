/*
 * patSamplingCorrection.h
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#ifndef PATSAMPLINGCORRECTION_H_
#define PATSAMPLINGCORRECTION_H_
#include "patMultiModalPath.h"
#include "patChoiceSet.h"
#include <tr1/unordered_map>
#include "MHLinkAndPathCost.h"
#include <map>
using namespace std;
using namespace std::tr1;
class patSamplingCorrection {
public:
	patSamplingCorrection(const MHWeightFunction* link_cost);
	unordered_map<const patMultiModalPath*, double> computeSC(
			const patMultiModalPath& chosen_alternative,
			const patChoiceSet& sampled_choice_set) const;
	virtual ~patSamplingCorrection();

protected:
	const MHWeightFunction* m_link_cost;
};

#endif /* PATSAMPLINGCORRECTION_H_ */
