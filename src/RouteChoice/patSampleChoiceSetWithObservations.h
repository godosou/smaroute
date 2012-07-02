/*
 * patSampleChoiceSetWithObservations.h
 *
 *  Created on: May 26, 2012
 *      Author: jchen
 */

#ifndef PATSAMPLECHOICESETWITHOBSERVATIONS_H_
#define PATSAMPLECHOICESETWITHOBSERVATIONS_H_

#include "patObservation.h"
#include "patOd.h"
#include "patMultiModalPath.h"
#include <map>
#include <vector>
using namespace std;
class MHPathGenerator;
class patSampleChoiceSetWithObservations {
public:
	patSampleChoiceSetWithObservations();

	void sampleChoiceSet(MHPathGenerator* path_generator, string folder);
	void averagePathProbas();
	const map<const patMultiModalPath, double>& getPathProbas() const {
		return m_path_obs_proba;
	}
//	void addObservation(patObservation& obs);
	/**
	 * Add an observation, update m_od_obs and m_path_obs_proba.
	 */
	void addObservation( patObservation& obs, const bool& sample = true);
	virtual ~patSampleChoiceSetWithObservations();

	const vector<patObservation>& getObs() const {
		return m_observations;
	}
protected:
	vector<patObservation> m_observations;
	map<const patOd, int> m_od_count;
	map<const patMultiModalPath, double> m_path_obs_proba;
	map<const patOd, map<const patMultiModalPath, double> > m_od_path_probas;
}
;

#endif /* PATSAMPLECHOICESETWITHOBSERVATIONS_H_ */
