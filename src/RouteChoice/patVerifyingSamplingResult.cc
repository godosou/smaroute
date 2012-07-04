/*
 * patVerifyingSamplingResult.cc
 *
 *  Created on: Jun 7, 2012
 *      Author: jchen
 */

#include "patVerifyingSamplingResult.h"
#include <sstream>
#include <dirent.h>
#include "patNetworkElements.h"
#include "patReadChoiceSetFromKML.h"
#include "patOd.h"
#include "patChoiceSet.h"
#include "patDisplay.h"
#include "patPathGenerator.h"
#include "patRandomNumber.h"
patVerifyingSamplingResult::patVerifyingSamplingResult(string folder_name,
		const patNetworkElements* network, const patRandomNumber& rnd) :
		m_folder_name(folder_name), m_network(network), m_rnd(rnd) {
	getpaths();
}

list<string> patVerifyingSamplingResult::getFiles() const {
	DIR * dip;
	struct dirent *dit;
	const char* dirName = m_folder_name.c_str();

	DEBUG_MESSAGE("Try to open directory " << dirName);
	if ((dip = opendir(dirName)) == NULL) {
		stringstream str;
		str << "Directory " << dirName
				<< " doesn't exist or no permission to read.";
		throw RuntimeException(str.str().c_str());
		return list<string>();
	}

	list<string> dirContent;
	unsigned char isFile = 0x8;

	while ((dit = readdir(dip)) != NULL) {
		if (dit->d_type == isFile) {
			string fileName(dit->d_name);
			if (fileName.find(".kml") != string::npos) {
//				DEBUG_MESSAGE("Found file:" << fileName);
				dirContent.push_back(fileName);
			}

		}
	}

	closedir(dip);
	return dirContent;
}

void patVerifyingSamplingResult::getpaths() {
	list<string> files = getFiles();
	m_total_nbr_of_paths = 0;
	m_total_probas = 0.0;
	for (list<string>::const_iterator file_iter = files.begin();
			file_iter != files.end(); ++file_iter) {
		string file_name = m_folder_name + "/" + *file_iter;
//		DEBUG_MESSAGE(file_name);
		patReadChoiceSetFromKML rc(m_network);
		if (!ifstream((file_name).c_str())) {

			continue;
		}
		try {
			map<patOd, patChoiceSet> css = rc.read(file_name, m_rnd);
			for (map<patOd, patChoiceSet>::const_iterator od_iter = css.begin();
					od_iter != css.end(); ++od_iter) {

				const map<const patMultiModalPath, int>& count =
						od_iter->second.getCount();
				const map<const patMultiModalPath, double>& log_weight =
						od_iter->second.getLogWeight();
				if (count.size() != log_weight.size()) {
					throw RuntimeException(
							"count and logweight not consistent");
				}
				for (map<const patMultiModalPath, int>::const_iterator path_iter =
						count.begin(); path_iter != count.end(); ++path_iter) {

					map<patMultiModalPath, pair<int, double> >::iterator find_path =
							m_paths.find(path_iter->first);
					m_total_nbr_of_paths += path_iter->second;
					if (find_path == m_paths.end()) {
						m_paths[path_iter->first] = pair<int, double>(
								path_iter->second, 0.0);
					} else {
						find_path->second.first += path_iter->second;
					}
				}

				for (map<const patMultiModalPath, double>::const_iterator path_iter =
						log_weight.begin(); path_iter != log_weight.end();
						++path_iter) {

					map<patMultiModalPath, pair<int, double> >::iterator find_path =
							m_paths.find(path_iter->first);
					if (find_path == m_paths.end()) {
						throw RuntimeException(
								"path in log weight not in count");
					} else {
						if (find_path->second.second == 0.0) {
							m_total_probas += exp(path_iter->second);
							find_path->second.second = path_iter->second;
						} else {
							if (find_path->second.second != path_iter->second) {
								throw RuntimeException(
										"log weights for same path are different");
							}
						}

					}
				}

			}
		} catch (...) {
			WARNING("fail to open"<<file_name);
			continue;
		}
	}

}
double patVerifyingSamplingResult::verifyProbability(
		const patChoiceSet& universal_set,
		const patPathGenerator* path_generator) const {
	set<patMultiModalPath> path_set = universal_set.getChoiceSet();
	double chi2 = 0.0;
	vector<double> theo_probas;
	vector<double> empirical_probas;
	double total_theo_probas = 0.0;
	DEBUG_MESSAGE(
			"total sample count: "<<m_total_nbr_of_paths<<", total paths: "<<m_paths.size()<<", universal choice set: "<< path_set.size());
	for (set<patMultiModalPath>::const_iterator path_iter = path_set.begin();
			path_iter != path_set.end(); ++path_iter) {
		map<patMultiModalPath, pair<int, double> >::const_iterator find_path =
				m_paths.find(*path_iter);
		double log_weight_file = 0.0;
		double empirical_proba = 0.0;
		double theo_proba_file = 0.0;
		double theo_proba = path_generator->calculatePathLogWeight(*path_iter);
		total_theo_probas += exp(theo_proba);
		theo_probas.push_back(exp(theo_proba));
		if (find_path != m_paths.end()) {
			log_weight_file = find_path->second.second;
			empirical_proba = (double) find_path->second.first
					/ m_total_nbr_of_paths;
			theo_proba_file = exp(find_path->second.second) / m_total_probas;
		}
		empirical_probas.push_back(empirical_proba);
//		DEBUG_MESSAGE(
//				"probas:"<< empirical_proba<<"/"<<theo_proba_file<<", log weight file/theo: "<<log_weight_file<<"/"<< theo_proba);
		if (log_weight_file != 0.0
				&& abs(log_weight_file - theo_proba) > 0.0001) {
			WARNING(
					"log weight in file ("<<log_weight_file<<") and theo ("<<theo_proba<<") not the same.");
		}

	}
	for (unsigned i = 0; i < theo_probas.size(); ++i) {
		theo_probas[i] /= total_theo_probas;
	}
	for (unsigned i = 0; i < theo_probas.size(); ++i) {
		DEBUG_MESSAGE(theo_probas[i] <<"-"<< empirical_probas[i]);
		chi2 += m_total_nbr_of_paths * (theo_probas[i] - empirical_probas[i])
				* (theo_probas[i] - empirical_probas[i]) / theo_probas[i];

	}DEBUG_MESSAGE("CHI2: "<<chi2);
	DEBUG_MESSAGE("done verifying");
	return chi2;
//	return true;
}

patVerifyingSamplingResult::~patVerifyingSamplingResult() {
	// TODO Auto-generated destructor stub
}

