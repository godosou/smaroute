/*
 * patChoiceSet.h
 *
 *  Created on: May 4, 2012
 *      Author: jchen
 */

#ifndef PATCHOICESET_H_
#define PATCHOICESET_H_
#include "patMultiModalPath.h"
#include "patOd.h"
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <set>
#include "patPathGenerator.h"
#include "patUtilityFunction.h"
using namespace std;
using namespace std::tr1;
class patRandomNumber;
class patChoiceSet {
public:
	patChoiceSet();
	void setOD(const patNode* origin, const patNode* destination);
	void setOd(patOd& od);
	bool addPath(patMultiModalPath& path, double log_weight, int count);
	virtual ~patChoiceSet();

	bool isSampled(const patMultiModalPath& path) const;
	int size() const;
	int getSampledCount() const;
	int getSampledCount(const patMultiModalPath& path) const;
	double getLogWeight(const patMultiModalPath& path) const;
	set<patMultiModalPath> getChoiceSet() const;

	unordered_map<string, double> genAttributes(
			const patMultiModalPath& chosen_path,
			const patUtilityFunction* utility_function,
			const patPathGenerator* path_generator,
			const patChoiceSet* universal_choiceset,
			const unsigned& choice_set_size) const;
	unordered_map<string, double> genAttributes(
			const patMultiModalPath& chosen_path,
			const patUtilityFunction* utility_function,
			const patPathGenerator* path_generator,
			const patChoiceSet* universal_choiceset = NULL) const;
	map<const patMultiModalPath, double> computeSC(
			const patMultiModalPath& chosen_alternative,
			const patPathGenerator* path_generator) const;
	void exportCadytsVisData(const patMultiModalPath& chosen_path,
			string file_path) const;
	int getUniquePaths() const;

	patChoiceSet sampleSubSet(const unsigned choice_set_size) const;
	const map<const patMultiModalPath, int>& getCount() const {
		return m_count;
	}
	const map<const patMultiModalPath, double>& getLogWeight() const {
		return m_log_weight;
	}
	void exportSHP(const patMultiModalPath& chosen_path,
			string file_path) const;

/**
 * Compute the similarity of the choice set.
 */
	double computeSimilarity(const patMultiModalPath& chosen_path,
			const unsigned& choice_set_size) const;
	bool empty() const;
protected:
	patOd m_od;
	vector<patMultiModalPath> m_paths;
//	set<patMultiModalPath> m_paths;
	map<const patMultiModalPath, int> m_count;
	map<const patMultiModalPath, double> m_log_weight;
};

#endif /* PATCHOICESET_H_ */
