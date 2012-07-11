/*
 * getObservations.h
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#ifndef GETOBSERVATIONS_H_
#define GETOBSERVATIONS_H_



#include "patChoiceSet.h"
#include <boost/lexical_cast.hpp>
#include <dirent.h>
#include "patDisplay.h"
#include "patObservation.h"
#include "patNetworkElements.h"
#include "patReadObservationFromKML.h"
#include "patReadPathsFromKML.h"
#include "patMultiModalPath.h"

#include <vector>
using namespace std;
vector<string> getObservationFiles(string folder) {

	DIR * dip;
	struct dirent *dit;
	const char* dir_name_char = folder.c_str();

	DEBUG_MESSAGE("Try to open directory " << dir_name_char);
	if ((dip = opendir(dir_name_char)) == NULL) {
		stringstream str;
		str << "Directory " << dir_name_char
				<< " doesn't exist or no permission to read.";
		WARNING(str.str());
		exit(-1);
	}
//	DEBUG_MESSAGE("Direcotry " << dir_name_char << " is now open");
	vector<string> observation_files;
	unsigned char isFile = 0x8;
	string esp("~");
	string kml("kml");

	while ((dit = readdir(dip)) != NULL) {
		if (dit->d_type == isFile) {
			//DEBUG_MESSAGE(dit->d_name);
			string fileName(dit->d_name);
			if (fileName.find(esp) == string::npos
					&& fileName.find(kml) != string::npos) {
//				DEBUG_MESSAGE("Found file:" << fileName);
				observation_files.push_back(fileName);
			}

		}
	}

	closedir(dip);

	if (observation_files.empty()) {
		exit(0);
	}

	DEBUG_MESSAGE(observation_files.size()<<" kml files are read.");
	return observation_files;
}

vector<patObservation> readSyntheticObservations(string& folder,
		vector<string>& observation_files,
		const patNetworkElements& network_elements) {

	vector<patObservation> observations;
	for (unsigned i = 0; i < observation_files.size(); ++i) {
		string file_name = folder + observation_files[i];
		patObservation new_observation;
		new_observation.setId(boost::lexical_cast<string>(i));
		if (!ifstream(file_name.c_str())) {
			WARNING(file_name);
			throw RuntimeException("no valid observation file");
		}
		patReadPathsFromKML rp;

		vector<patMultiModalPath> obs_paths = rp.read(&network_elements,
				file_name);
		if (obs_paths.size() != 1) {
			WARNING("WRONG PATH NUMBER"<<obs_paths.size());
		}
		new_observation.addPath(obs_paths.front(), 1.0);
		observations.push_back(new_observation);

	}
	DEBUG_MESSAGE(observations.size()<<" observations are read.");
	return observations;
}
vector<patObservation> readRealObservations(string& folder,
		vector<string>& observation_files,
		const patNetworkElements& network_elements) {

	vector<patObservation> observations;
	for (unsigned i = 0; i < observation_files.size(); ++i) {
		string file_name = folder + observation_files[i];
		patObservation new_observation;
		patReadObservationFromKML ro(&network_elements);
		ro.parseFile(file_name, &new_observation);
		observations.push_back(new_observation);
	}
	DEBUG_MESSAGE(observations.size()<<" observations are read.");
	return observations;
}
#endif /* GETOBSERVATIONS_H_ */
