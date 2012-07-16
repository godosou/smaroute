/*
 * patChoiceSetWriter.cc
 *
 *  Created on: Apr 27, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "patChoiceSetWriter.h"
#include "patNBParameters.h"
#include <boost/lexical_cast.hpp>
using namespace boost;

patChoiceSetWriter::patChoiceSetWriter() :
		m_cost_function(NULL) {
	// TODO Auto-generated destructor stub
}
patChoiceSetWriter::~patChoiceSetWriter() {
	// TODO Auto-generated destructor stub
}
patChoiceSetWriter::patChoiceSetWriter(patPathWriter* pathWriter,
		const int sampleInterval, const patLinkAndPathCost* cost_function) :
		m_sample_interval(sampleInterval), m_path_writer(pathWriter), m_path_count(
				0), m_cost_function(cost_function) {
	if (pathWriter == NULL) {
		throw IllegalArgumentException("path writer is null");
	}
	if (sampleInterval <= 0) {
		throw IllegalArgumentException(
				"sample interval is not strictly positive");
	}

	m_warmup_iterations = patNBParameters::the()->WARMUP_ITERATIONS;
}

void patChoiceSetWriter::start() {
	m_path_count = 0;
}

void patChoiceSetWriter::processState(const patMultiModalPath& path,
		const double log_weight) {
	/*
	 * (1) check if this path should be written
	 */
	m_path_count++;

	string kml_file_name = m_path_writer->getFileName();
	int slash_position = kml_file_name.rfind("/");
	string folder = kml_file_name.substr(0, slash_position);
	string stop_file = folder + "/" + "stop";
	if (ifstream(stop_file.c_str())) {
		end();
		exit(0);
	}

	int sampled_count = m_path_count - m_warmup_iterations;
	if (sampled_count <= 0 || sampled_count % m_sample_interval != 0) {

		return;
	}
	/*
	 * (2) write out the link IDs that correspond to the inverted nodes
	 */
	map<patMultiModalPath, pair<int, double> >::iterator find_path =
			m_sampled_set.find(path);

	if (find_path == m_sampled_set.end()) {
		pair<int, double> d(1, log_weight);
		m_sampled_set.insert(
				pair<patMultiModalPath, pair<int, double> >(path, d));
	} else {
		find_path->second.first += 1;
		cout << (int) sampled_count / m_sample_interval << "paths, unique: "
				<< m_sampled_set.size() << endl;
	}

}

void patChoiceSetWriter::end() {
	int total_count = 0;
	for (map<patMultiModalPath, pair<int, double> >::iterator path_iter =
			m_sampled_set.begin(); path_iter != m_sampled_set.end();
			++path_iter) {
		total_count += path_iter->second.first;
		map<string, string> attrs;
		if (m_cost_function != NULL) {
			map<string, double> cf_attrs = m_cost_function->getAttributes(
					path_iter->first);

			for (map<string, double>::const_iterator attr_iter =
					cf_attrs.begin(); attr_iter != cf_attrs.end();
					++attr_iter) {
				attrs[attr_iter->first] = lexical_cast<string>(
						attr_iter->second);
			}
		}
//		double log_weight = m_path_cost->logWeightWithoutCorrection(path_iter->first);
//		DEBUG_MESSAGE("write a path "<<log_weight);
		attrs["logweight"] = lexical_cast<std::string>(
				path_iter->second.second);
		attrs["count"] = lexical_cast<std::string>(path_iter->second.first);
		m_path_writer->writePath(path_iter->first, attrs);

	}
	if (total_count != patNBParameters::the()->SAMPLE_COUNT) {
		WARNING(
				"sampled paths do not match: expected "<<patNBParameters::the()->SAMPLE_COUNT<<", return "<<total_count);
	}
}

