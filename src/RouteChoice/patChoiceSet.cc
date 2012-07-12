/*
 * patChoiceSet.cc
 *
 *  Created on: May 4, 2012
 *      Author: jchen
 */

#include "patChoiceSet.h"
#include "patException.h"
#include "patPathSizeComputer.h"
#include "patNBParameters.h"
#include "patRandomNumber.h"
#include "patSampleDiscreteDistribution.h"
#include <fstream>
#include <iomanip>
patChoiceSet::patChoiceSet() {
	// TODO Auto-generated constructor stub

}

patChoiceSet::~patChoiceSet() {
	// TODO Auto-generated destructor stub
}

void patChoiceSet::setOD(const patNode* origin, const patNode* destination) {
	m_od.setOrigin(origin);
	m_od.setDestination(destination);
}

void patChoiceSet::setOd(patOd& od) {
	m_od = od;
//	DEBUG_MESSAGE(m_od);
}

bool patChoiceSet::addPath(patMultiModalPath& path, double log_weight,
		int count) {
//	DEBUG_MESSAGE(m_od);
//
//	DEBUG_MESSAGE(path.generateOd());
//	if ((path.generateOd() == m_od) == false) {
//		throw RuntimeException("od not consistent");
//		return false;
//	}
	pair<set<patMultiModalPath>::const_iterator, bool> insert_result =
			m_paths.insert(path);

	if (insert_result.second == true) {
		//new path;
		if (m_count.find(path) != m_count.end()) {
			throw RuntimeException("path not in set but has count");
			//already in got it;
		}

		if (m_log_weight.find(path) != m_log_weight.end()) {
			throw RuntimeException("path not in set but has log weight");
			//already in got it;
		}

		m_count[path] = count;
		m_log_weight[path] = log_weight;
	} else {

		if (m_paths.find(path) == m_paths.end()) {
			throw RuntimeException("fail to insert path");
		}
		if (m_count.find(path) == m_count.end()) {
			throw RuntimeException("path  in set but no count");
			//already in got it;
		}

		if (m_log_weight.find(path) == m_log_weight.end()) {
			throw RuntimeException("path  in set but has no log weight");
			//already in got it;
		}
		if (m_log_weight[path] != log_weight) {
			throw RuntimeException("path  in set but different log weight");
		}
		m_count[path] += count;
	}
//	DEBUG_MESSAGE(count<<","<< log_weight);
}

int patChoiceSet::getSampledCount() const {
	int total = 0;
	for (map<const patMultiModalPath, int>::const_iterator path_iter =
			m_count.begin(); path_iter != m_count.end(); ++path_iter) {
		total += path_iter->second;
	}
	return total;
}

int patChoiceSet::getSampledCount(const patMultiModalPath& path) const {

	set<patMultiModalPath>::const_iterator find_path = m_paths.find(path);
	if (find_path == m_paths.end()) {
		return 0;
	}
	map<const patMultiModalPath, int>::const_iterator find_count = m_count.find(
			path);
	if (find_count == m_count.end()) {
		throw RuntimeException("path  in set but no count");
		return -1;
	} else {
		return find_count->second;
	}

}
bool patChoiceSet::isSampled(const patMultiModalPath& path) const {
	set<patMultiModalPath>::const_iterator find_path = m_paths.find(path);
	if (find_path != m_paths.end()) {
		return true;
	} else {
		return false;
	}
}
double patChoiceSet::getLogWeight(const patMultiModalPath& path) const {

	set<patMultiModalPath>::const_iterator find_path = m_paths.find(path);
	if (find_path == m_paths.end()) {
		return -1.0;
	}
	map<const patMultiModalPath, double>::const_iterator find_log_weight =
			m_log_weight.find(path);
	if (find_log_weight == m_log_weight.end()) {
		throw RuntimeException("path  in set but no m_log_weight");
		return -1.0;
	} else {
		return find_log_weight->second;
	}

}

set<patMultiModalPath> patChoiceSet::getChoiceSet() const {
	return m_paths;
}
int patChoiceSet::size() const {
	return m_paths.size();
}
unordered_map<string, double> patChoiceSet::genAttributes(
		const patMultiModalPath& chosen_path,
		const patUtilityFunction* utility_function,
		const patPathGenerator* path_generator,
		const patChoiceSet* universal_choiceset,
		const unsigned& choice_set_size, const patRandomNumber& rnd) const {
	patChoiceSet new_choice_set = sampleSubSet(choice_set_size, rnd);
	return new_choice_set.genAttributes(chosen_path, utility_function,
			path_generator, universal_choiceset);
}
unordered_map<string, double> patChoiceSet::genAttributes(
		const patMultiModalPath& chosen_path,
		const patUtilityFunction* utility_function,
		const patPathGenerator* path_generator,
		const patChoiceSet* universal_choiceset) const {
	//bool chosen_sampled = isSampled(chosen_path);
	unordered_map<string, double> attributes;

	patPathSizeComputer* ps_computer = new patPathSizeComputer(getChoiceSet(),
			chosen_path);
	patPathSizeComputer* ps_computer_u = NULL;

	if (universal_choiceset == NULL) {
		ps_computer_u = ps_computer->clone();
	} else {
		ps_computer_u = new patPathSizeComputer(universal_choiceset->getChoiceSet());
	}
	map<const patMultiModalPath, double> sc = computeSC(chosen_path,
			path_generator);
//	DEBUG_MESSAGE(ps.size()<<","<<sc.size());
	set<patMultiModalPath>::const_iterator find_chosen = m_paths.find(
			chosen_path);

	map<ARC_ATTRIBUTES_TYPES, double> chosen_cost;
	int i = 0;
//	DEBUG_MESSAGE(m_paths.size());
	if (find_chosen == m_paths.end()) {
//		DEBUG_MESSAGE("chosen not smapled");
		map<ARC_ATTRIBUTES_TYPES, double> path_cost =
				utility_function->getAttributes(chosen_path);
		chosen_cost = path_cost;
		map<const patMultiModalPath, double>::const_iterator find_sc = sc.find(
				chosen_path);
		if (find_sc == sc.end()) {
			throw RuntimeException("no sc found");
		}
		for (map<ARC_ATTRIBUTES_TYPES, double>::const_iterator coef_iter =
				path_cost.begin(); coef_iter != path_cost.end(); ++coef_iter) {
			stringstream ss;
			ss << patArc::getAttributeTypeString(coef_iter->first) << i;
			attributes[ss.str()] = coef_iter->second;
		}
		{

			stringstream ss;
			ss << "avail" << i;
			attributes[ss.str()] = 1;
		}
		{

			stringstream ss;
			ss << "ps" << i;
			attributes[ss.str()] = ps_computer->getPS(chosen_path);
		}

		{

			stringstream ss;
			ss << "ps_u" << i;
			attributes[ss.str()] = ps_computer_u->getPS(chosen_path);

		}

		{

			stringstream ss;
			ss << "sc" << i;
			attributes[ss.str()] = find_sc->second;
		}

		attributes["choiceRP"] = 0;
		++i;
	} else {
//		DEBUG_MESSAGE("chosen sampled");
	}
	for (set<patMultiModalPath>::const_iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {
//		DEBUG_MESSAGE(i);
		map<ARC_ATTRIBUTES_TYPES, double> path_cost =
				utility_function->getAttributes(*path_iter);
		chosen_cost = path_cost;
		map<const patMultiModalPath, double>::const_iterator find_sc = sc.find(
				*path_iter);
		if (find_sc == sc.end()) {
			throw RuntimeException("no sc found");
		}

		{

			stringstream ss;
			ss << "ps" << i;
			attributes[ss.str()] = ps_computer->getPS(*path_iter);
		}

		{

			stringstream ss;
			ss << "ps_u" << i;
			attributes[ss.str()] = ps_computer_u->getPS(*path_iter);
		}
		{

			stringstream ss;
			ss << "sc" << i;
			attributes[ss.str()] = find_sc->second;
		}
		if (*path_iter == chosen_path) {
			attributes["choiceRP"] = i;
		}

		for (map<ARC_ATTRIBUTES_TYPES, double>::const_iterator coef_iter =
				path_cost.begin(); coef_iter != path_cost.end(); ++coef_iter) {
			stringstream ss;
			ss << patArc::getAttributeTypeString(coef_iter->first) << i;
			attributes[ss.str()] = coef_iter->second;
		}
		{

			stringstream ss;
			ss << "avail" << i;
			attributes[ss.str()] = 1;
		}
		i++;
	}
	for (; i <= patNBParameters::the()->SAMPLE_COUNT; ++i) {

//		DEBUG_MESSAGE(i);
		{

			stringstream ss;
			ss << "avail" << i;
			attributes[ss.str()] = 0;
		}
		{

			stringstream ss;
			ss << "ps_u" << i;
			attributes[ss.str()] = 9999;
		}

		{

			stringstream ss;
			ss << "ps" << i;
			attributes[ss.str()] = 9999;
		}

		{

			stringstream ss;
			ss << "sc" << i;
			attributes[ss.str()] = 9999;
		}

		for (map<ARC_ATTRIBUTES_TYPES, double>::const_iterator coef_iter =
				chosen_cost.begin(); coef_iter != chosen_cost.end();
				++coef_iter) {
			stringstream ss;
			ss << patArc::getAttributeTypeString(coef_iter->first) << i;
			attributes[ss.str()] = 9999;
		}
	}
	delete ps_computer;
	ps_computer = NULL;
	delete ps_computer_u;
	ps_computer_u = NULL;

	return attributes;
}
map<const patMultiModalPath, double> patChoiceSet::computeSC(
		const patMultiModalPath& chosen_alternative,
		const patPathGenerator* path_generator) const {

	map<const patMultiModalPath, double> sc;

	for (set<patMultiModalPath>::const_iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {

		int rep = getSampledCount(*path_iter);
		double log_proba = getLogWeight(*path_iter);
		sc[*path_iter] = log((double) rep) - log_proba;

	}
	bool chosen_sampled = isSampled(chosen_alternative);
	if (chosen_sampled == true) {
		int chosen_replicates = getSampledCount(chosen_alternative) + 1;
		double log_proba = getLogWeight(chosen_alternative);
		sc[chosen_alternative] = log((double) chosen_replicates) - log_proba;
	} else {
		int chosen_replicates = 1;
		double log_proba = path_generator->calculatePathLogWeight(
				chosen_alternative);
		sc[chosen_alternative] = log((double) chosen_replicates) - log_proba;
	}

	return sc;
}
int patChoiceSet::getUniquePaths() const {
	return m_paths.size();
}
void patChoiceSet::exportCadytsVisData(const patMultiModalPath& chosen_path,
		string file_path) const {
	unordered_map<const patArc*, double> empty_matrix;
	unordered_map<const patArc*, vector<double> > final_matrix;
	for (set<patMultiModalPath>::const_iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {
		vector<const patArc*> path_arcs = (*path_iter).getArcList();
		for (vector<const patArc*>::const_iterator arc_iter = path_arcs.begin();
				arc_iter != path_arcs.end(); ++arc_iter) {
			empty_matrix[*arc_iter] = 0.0;
		}
	}
	{
		vector<const patArc*> path_arcs = chosen_path.getArcList();
		for (vector<const patArc*>::const_iterator arc_iter = path_arcs.begin();
				arc_iter != path_arcs.end(); ++arc_iter) {
			empty_matrix[*arc_iter] = 0.0;
		}

	}
	{
		unordered_map<const patArc*, double> new_matrix = empty_matrix;
		vector<const patArc*> path_arcs = chosen_path.getArcList();
		for (vector<const patArc*>::const_iterator arc_iter = path_arcs.begin();
				arc_iter != path_arcs.end(); ++arc_iter) {
			new_matrix[*arc_iter] = 1.0;
		}
		for (unordered_map<const patArc*, double>::const_iterator arc_iter =
				new_matrix.begin(); arc_iter != new_matrix.end(); ++arc_iter) {
			final_matrix[arc_iter->first];
			final_matrix[arc_iter->first].push_back(arc_iter->second);
		}
	}
	for (set<patMultiModalPath>::const_iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {
		unordered_map<const patArc*, double> new_matrix = empty_matrix;
		vector<const patArc*> path_arcs = (*path_iter).getArcList();

		double v = 2.0;
		if (chosen_path == (*path_iter)) {
			v = 1.0;
		}
		for (vector<const patArc*>::const_iterator arc_iter = path_arcs.begin();
				arc_iter != path_arcs.end(); ++arc_iter) {
			new_matrix[*arc_iter] = v;
		}
		for (unordered_map<const patArc*, double>::const_iterator arc_iter =
				new_matrix.begin(); arc_iter != new_matrix.end(); ++arc_iter) {
			final_matrix[arc_iter->first];
			final_matrix[arc_iter->first].push_back(arc_iter->second);
		}
	}
	set<patMultiModalPath> all_paths = m_paths;
	all_paths.insert(chosen_path);
	ofstream cadyts_paths((file_path + "/paths.xml").c_str());

	cadyts_paths << "<paths>" << endl;
	cadyts_paths << "\t<odpair from=\""
			<< chosen_path.front()->getUpNode()->getUserId() << "_"
			<< chosen_path.front()->getDownNode()->getUserId() << "\" to = \""
			<< chosen_path.back()->getUpNode()->getUserId() << "_"
			<< chosen_path.back()->getDownNode()->getUserId() << "\" >" << endl;

	for (set<patMultiModalPath>::const_iterator path_iter = all_paths.begin();
			path_iter != all_paths.end(); ++path_iter) {
		vector<const patArc*> path_arcs = (*path_iter).getArcList();
		cadyts_paths << "\t\t<path links=\"";

		for (vector<const patArc*>::const_iterator arc_iter = path_arcs.begin();
				arc_iter != path_arcs.end(); ++arc_iter) {
			cadyts_paths << (*arc_iter)->getUpNode()->getUserId() << "_"
					<< (*arc_iter)->getDownNode()->getUserId() << " ";
		}
		cadyts_paths << "\" logweight=\"";
		cadyts_paths << -0.0;
		cadyts_paths << "\" />" << endl;
	}
	cadyts_paths << "\t</odpair>" << endl;
	cadyts_paths << "</paths>" << endl;
	ofstream cadyts_vis_data((file_path + "/vis-data.xml").c_str());
	cadyts_vis_data << "<dynamicdata starttime=\"0\" binsize=\"1\" bincount=\""
			<< m_paths.size() + 1
			<< "\" subclass=\"bioroute.analysis.VisualAnalyzer\" >" << endl;
	for (unordered_map<const patArc*, vector<double> >::const_iterator arc_iter =
			final_matrix.begin(); arc_iter != final_matrix.end(); ++arc_iter) {
		cadyts_vis_data << "<entry key=\""
				<< arc_iter->first->getUpNode()->getUserId() << "_"
				<< arc_iter->first->getDownNode()->getUserId() << "\" value=\"";
		for (vector<double>::const_iterator b_iter = arc_iter->second.begin();
				b_iter != arc_iter->second.end(); ++b_iter) {
			cadyts_vis_data << *b_iter << " ";
		}
		cadyts_vis_data << "\" />" << endl;
	}
	cadyts_vis_data << "</dynamicdata>" << endl;

	cadyts_vis_data.close();
}

patChoiceSet patChoiceSet::sampleSubSet(const unsigned choice_set_size,
		const patRandomNumber& rnd) const {
	if (m_paths.size() > choice_set_size) {
		vector<patMultiModalPath> all_paths;
		vector<double> all_logweights;
		for (set<patMultiModalPath>::const_iterator path_iter = m_paths.begin();
				path_iter != m_paths.end(); ++path_iter) {
			map<const patMultiModalPath, int>::const_iterator find_count =
					m_count.find(*path_iter);
			map<const patMultiModalPath, double>::const_iterator find_log_weight =
					m_log_weight.find(*path_iter);
			if (find_count == m_count.end()) {
				WARNING("path not in count");
				throw RuntimeException("path not in count");
			}
			if (find_log_weight == m_log_weight.end()) {
				WARNING("path not in count");
				throw RuntimeException("path not in count");
			}
			int count = find_count->second;
			for (unsigned count_iter = 0; count_iter < count; ++count_iter) {
				all_paths.push_back(*path_iter);
				all_logweights.push_back(find_log_weight->second);
			}
		}
		patChoiceSet new_choice_set;
		patOd new_od = m_od;
		new_choice_set.setOd(new_od);

		vector<double> probas(all_paths.size(), 1.0);
		vector<unsigned int> sampled_path_indics =
				patSampleDiscreteDistribution::sampleWithOutReplaceMent(probas,
						choice_set_size, rnd);

		for (vector<unsigned int>::iterator sample_iter =
				sampled_path_indics.begin();
				sample_iter != sampled_path_indics.end(); ++sample_iter) {

			new_choice_set.addPath(all_paths[*sample_iter],
					all_logweights[*sample_iter], 1);
		}
		return new_choice_set;
//					DEBUG_MESSAGE("OK");
	} else {
		return *this;
	}
}
