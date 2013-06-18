#ifndef patObservation_h
#define patObservation_h

#include <map>
#include "patMultiModalPath.h"
#include "patOd.h"
#include <iostream>
#include <tr1/unordered_map>
#include <set>
#include "patPathGenerator.h"
#include "patUtilityFunction.h"
#include "patChoiceSet.h"
using namespace std;
using namespace std::tr1;
#include "patPathGenerator.h"
class patRandomNumber;
class patObservation {
public:
	friend bool operator==(const patObservation& observation1,
			const patObservation& observation2);
	friend bool operator<(const patObservation& observation1,
			const patObservation& observation2);
	patObservation();

	void setId(string);
	void setStartTime(double& theStartTime);

	void setEndTime(double& theEndTime);

	void setMapBounds(double minLat, double maxLat, double minLon,
			double maxLon);

	string getId() const;
	const map<const patMultiModalPath, double>& getPathProbas() const;
	map<const patMultiModalPath, double> getNormalizedPathProbas() const;

	void orderPathsByOD();
	pair<int, int> countChosenPathsSampled();
	void addPath(const patMultiModalPath& new_path, double proba, bool replace=true);
	void sampleChoiceSet(patPathGenerator* path_generator, const string folder);
//	void putODChoiceSet(patOd od, patChoiceSet& od_choice_set);

	list<unordered_map<string, string> > genAttributes(const unsigned& choice_set_size, const patRandomNumber& rnd,
			const patUtilityFunction* utility_function,
			const patPathGenerator* path_generator,
			const patChoiceSet* universal_choiceset = NULL) const;
	void exportCadytsVisData() const;
	vector<int> getUniquePathsPerOD() const;
	unsigned getNbrOfCandidates() const;
	unsigned getNbOfOds() const;
	void setChoiceSet(map<patOd, patChoiceSet>& choice_set);

	void writeChoiceSetSHP(const string& choice_set_folder) const;
	const map<patOd, patChoiceSet>& getChoiceSet() const{
		return m_choice_set;
	}

/**
 * Compute similarity indicator for paths
 */
	double computeChoiceSetSimilarity(const unsigned& choice_set_size);
 void writeKML(const string& folder) const;
protected:
	string m_id;
	double m_start_time;
	double m_end_time;
	double minLatitude;
	double maxLatitude;
	double minLongitude;
	double maxLongitude;

	set<patMultiModalPath> m_paths;
	map<const patMultiModalPath, double> m_path_probas;

	map<patOd, unordered_set<const patMultiModalPath*> > m_paths_by_od;
	map<patOd, patChoiceSet> m_choice_set;
}
;
#endif
