/*
 * patSamplingCorrection.cc
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#include "patSamplingCorrection.h"
#include <math.h>
patSamplingCorrection::patSamplingCorrection(const MHWeightFunction* link_cost) :
		m_link_cost(link_cost) {

}

patSamplingCorrection::~patSamplingCorrection() {
	// TODO Auto-generated destructor stub
}

unordered_map<const patMultiModalPath*, double> patSamplingCorrection::computeSC(
		const patMultiModalPath& chosen_alternative,
		const patChoiceSet& sampled_choice_set) const {

	unordered_map<const patMultiModalPath*, double> sc;

	set<patMultiModalPath> choice_set = sampled_choice_set.getChoiceSet();

	for (set<patMultiModalPath>::const_iterator path_iter = choice_set.begin();
			path_iter != choice_set.end(); ++path_iter) {
		const patMultiModalPath* path = &*path_iter;
		int rep = sampled_choice_set.getSampledCount(path);
		int log_proba = sampled_choice_set.getLogWeight(path);
		DEBUG_MESSAGE(rep<<","<<log_proba);
		sc[path] = log(rep) - log_proba;

	}
	const patMultiModalPath* chosen_sampled = sampled_choice_set.isSampled(chosen_alternative);
	if (chosen_sampled!=NULL) {
		int chosen_replicates = sampled_choice_set.getSampledCount(
				chosen_sampled) + 1;
		int log_proba = sampled_choice_set.getLogWeight(chosen_sampled);
		sc[&chosen_alternative] = log(chosen_replicates) - log_proba;
	}
	else{
		int chosen_replicates =  1;
		int log_proba = m_link_cost->logWeigthOriginal(
				chosen_alternative);
		sc[&chosen_alternative] = log(chosen_replicates) - log_proba;

	}
}
