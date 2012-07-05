#include "patObservation.h"
#include "patDisplay.h"
#include "patException.h"
#include "patKMLPathWriter.h"
#include "patNBParameters.h"
#include <sys/stat.h>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <iomanip>
#include <iostream>
#include "patRandomNumber.h"
patObservation::patObservation():m_id("") {
	m_path_probas.clear();
//	make_pair(patMultiModalPath(), 1.0);
}

/*
 * "equal" operator for observations
 *@param[in] observation1 observations2 observations to be compared
 *@return patTRUE, if the observations have the same id and the same traveler;
 */
bool operator==(const patObservation& observation1,
		const patObservation& observation2) {

	if (observation1.getId() == observation2.getId()) {
		return true;
	} else {
		return false;
	}

}
bool operator<(const patObservation& observation1,
		const patObservation& observation2) {
	return (observation1.getId() < observation2.getId());

}
void patObservation::setStartTime(double& theStartTime) {
	m_start_time = theStartTime;
}

void patObservation::setEndTime(double& theEndTime) {
	m_end_time = theEndTime;
}

void patObservation::setMapBounds(double minLat, double maxLat, double minLon,
		double maxLon) {

	minLatitude = minLat;
	maxLatitude = maxLat;
	minLongitude = minLon;
	maxLongitude = maxLon;
}

const map<const patMultiModalPath, double>& patObservation::getPathProbas() const {
	return m_path_probas;
}
map<const patMultiModalPath, double> patObservation::getNormalizedPathProbas() const {
	map<const patMultiModalPath, double> normalized_proba;
	double sum = 0.0;
	for (map<const patMultiModalPath, double>::const_iterator path_iter =
			m_path_probas.begin(); path_iter != m_path_probas.end();
			++path_iter) {
		sum += path_iter->second;
	}
	if (sum == 0.0) {
		throw RuntimeException("Proba sum to 0");
	}
	for (map<const patMultiModalPath, double>::const_iterator path_iter =
			m_path_probas.begin(); path_iter != m_path_probas.end();
			++path_iter) {
		normalized_proba[path_iter->first] = path_iter->second / sum;
	}
	return normalized_proba;
}

string patObservation::getId() const {
	return m_id;
}

void patObservation::setId(string id) {
	m_id = id;
}
void patObservation::orderPathsByOD() {
	m_paths_by_od.clear();
	for (set<patMultiModalPath>::const_iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {
		patOd od((*path_iter).getUpNode(), (*path_iter).getDownNode());
		m_paths_by_od[od];
		m_paths_by_od[od].insert(&*path_iter);
	}
}

void patObservation::addPath(const patMultiModalPath& new_path, double proba) {

	m_paths.insert(new_path);
	m_path_probas.insert(make_pair(new_path,proba));
	}

void patObservation::sampleChoiceSet(patPathGenerator* path_generator,
		const string obs_folder) {
	orderPathsByOD();
	m_choice_set.clear();
	cout<< "paths: "<<m_paths.size()<<"; ods: "<<m_paths_by_od.size()<<endl;
	string observation_folder = obs_folder;
	if (observation_folder == "") {
		observation_folder = patNBParameters::the()->observationDirectory
				+ patNBParameters::the()->choiceSetFolder;
	}
	string file_name = observation_folder  + getId() + "_sample.kml";
//	DEBUG_MESSAGE(file_name);
	if (patNBParameters::the()->overwriteSampleFile==0 && ifstream((file_name).c_str())) {
		cout<<"SKIPPED"<<endl;
		return;
	}

	patKMLPathWriter path_writer(file_name);
	vector<patOd> ods;
	for (map<patOd, unordered_set<const patMultiModalPath*> >::const_iterator od_iter =
			m_paths_by_od.begin(); od_iter != m_paths_by_od.end(); ++od_iter) {
		ods.push_back(od_iter->first);
	}
//#pragma omp parallel num_threads(2)
	{
//#pragma omp for
		for (unsigned int i = 0; i < ods.size(); ++i) {
			cout<<"Start an od"<<endl;
			path_generator->setPathWriter(&path_writer);
			path_generator->run(ods[i].getOrigin(), ods[i].getDestination());
			cout<<"An od is dealt with"<<endl;
		}
	}
	//
	//

	path_writer.close();
	cout<<"KML written"<<endl;
}

//void patObservation::putODChoiceSet(patOd od, patChoiceSet& od_choice_set) {
//
//	unsigned int total_sampled = od_choice_set.getSampledCount();
//	if (total_sampled != patNBParameters::the()->SAMPLE_COUNT) {
//		stringstream ss;
//		ss << "wrong sampled count";
//		ss << total_sampled << "," << patNBParameters::the()->SAMPLE_COUNT;
//		throw RuntimeException(ss.str().c_str());
//	}
//	m_choice_set[od] = od_choice_set;
//}
pair<int, int> patObservation::countChosenPathsSampled() {
	int sampled = 0;
	int not_sampled = 0;
	for (set<patMultiModalPath>::const_iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {
		patOd od((*path_iter).getUpNode(), (*path_iter).getDownNode());
		map<patOd, patChoiceSet>::const_iterator find_choice_set =
				m_choice_set.find(od);
		if (find_choice_set == m_choice_set.end()) {
			WARNING(" no choice set is given"<<od);
			throw RuntimeException("no choice set is given");
		}
		if (find_choice_set->second.isSampled(*path_iter) == true) {
			sampled++;
		} else {
			not_sampled++;
		}
	}
	return pair<int, int>(sampled, not_sampled);
}
list<unordered_map<string, string> > patObservation::genAttributes(
		const unsigned& choice_set_size, const patRandomNumber& rnd,
		const patUtilityFunction* utility_function,
		const patPathGenerator* path_generator,
		const patChoiceSet* universal_choiceset) const {
	map<const patMultiModalPath, double> normalized_probas =
			getNormalizedPathProbas();
	list < unordered_map<string, string> > attributes;
	int i = 1;
//	DEBUG_MESSAGE(m_paths.size()<<","<<m_choice_set.size());
	for (set<patMultiModalPath>::const_iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {
		patOd od((*path_iter).getUpNode(), (*path_iter).getDownNode());
		map<patOd, patChoiceSet>::const_iterator find_choice_set =
				m_choice_set.find(od);
		if (find_choice_set == m_choice_set.end()) {
			WARNING(" no choice set is given");
			throw RuntimeException("no choice set is given");
		}

		unordered_map<string, double> row_attributes =
				find_choice_set->second.genAttributes(*path_iter,
						utility_function, path_generator, universal_choiceset,choice_set_size,rnd);
		unordered_map < string, string > path_attributes;

		row_attributes["AggWeight"] = m_path_probas.find(*path_iter)->second;
		row_attributes["AggWeightNormalized"] = normalized_probas[*path_iter];
		row_attributes["Orig"] = (double) od.getOrigin()->getUserId();
		row_attributes["Dest"] = (double) od.getDestination()->getUserId();
		if (i == m_paths.size()) {
			row_attributes["AggLast"] = 1;
		} else {

			row_attributes["AggLast"] = 0;
		}
		for (unordered_map<string, double>::const_iterator attr_iter =
				row_attributes.begin(); attr_iter != row_attributes.end();
				++attr_iter) {
			stringstream ss;
			ss << setprecision(7); //<<setiosflags(ios::scientific);
			ss << attr_iter->second;
			if (attr_iter->first == "choiceRP") {
//				DEBUG_MESSAGE(attr_iter->second<<","<<ss.str());
			}
			path_attributes[attr_iter->first] = ss.str();
		}
		path_attributes["tripId"] = getId(); //.substr(3, 4);
		attributes.push_back(path_attributes);
		i++;
	}
	return attributes;
}

void patObservation::exportCadytsVisData() const {
	string choice_set_folder = patNBParameters::the()->observationDirectory
			+ patNBParameters::the()->choiceSetFolder + "/";

	int i = 1;
	for (set<patMultiModalPath>::const_iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {
		patOd od((*path_iter).getUpNode(), (*path_iter).getDownNode());
		map<patOd, patChoiceSet>::const_iterator find_choice_set =
				m_choice_set.find(od);
		if (find_choice_set == m_choice_set.end()) {
			throw RuntimeException("no choice set is given");
		}
		stringstream ss;
		ss << i;

		if (find_choice_set->second.isSampled(*path_iter) == true) {
			ss << "_sampled";
		}
		string file_path = choice_set_folder + getId() + "/" + ss.str();
		system(("mkdir -p \"" + file_path + "\"").c_str());
		find_choice_set->second.exportCadytsVisData(*path_iter, file_path);
		++i;
	}
}
vector<int> patObservation::getUniquePathsPerOD() const {
	vector<int> uppod;
	for (map<patOd, patChoiceSet>::const_iterator od_iter =
			m_choice_set.begin(); od_iter != m_choice_set.end(); ++od_iter) {
		uppod.push_back(od_iter->second.getUniquePaths());
	}
	return uppod;
}
unsigned patObservation::getNbrOfCandidates() const {
	return m_paths.size();
}
unsigned patObservation::getNbOfOds() const {
	return m_choice_set.size();
}

void patObservation::setChoiceSet(map<patOd, patChoiceSet>& choice_set) {
	m_choice_set = choice_set;
//	DEBUG_MESSAGE(m_choice_set.size());
}
