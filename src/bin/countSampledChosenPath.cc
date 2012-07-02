/*
 * writeBiogeme.cc
 *
 *  Created on: May 4, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patReadObservationFromKML.h"
#include "patObservation.h"
#include <dirent.h>
#include "patWay.h"
#include "patObservation.h"
#include "patReadChoiceSetFromKML.h"
#include <fstream>
#include <iomanip>
int main(int argc, char *argv[]) {

	try {
		DEBUG_MESSAGE("STARTED..")

		/*
		 * (1) Extract single command line parameter.
		 */
		patError* err(NULL);

		//Read parameters
		if (argc == 1) {
			patNBParameters::the()->readFile(
					"/Users/jchen/Documents/Project/newbioroute/src/params/config.xml",
					err);

		} else {
			patNBParameters::the()->readFile(argv[1], err);

		}
		patNBParameters::the()->init(err);
		if (err != NULL) {
			exit(-1);
		}
		string dir_name = patNBParameters::the()->observationDirectory
				+ "observations/";

		DIR * dip;
		struct dirent *dit;
		const char* dir_name_char = dir_name.c_str();

		DEBUG_MESSAGE("Try to open directory " << dir_name_char);
		if ((dip = opendir(dir_name_char)) == NULL) {
			stringstream str;
			str << "Directory " << dir_name_char
					<< " doesn't exist or no permission to read.";
			WARNING(str.str());
			exit(-1);
		}DEBUG_MESSAGE("Direcotry " << dir_name_char << " is now open");
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
					DEBUG_MESSAGE("Found file:" << fileName);
					observation_files.push_back(fileName);
				}

			}
		}

		closedir(dip);

		DEBUG_MESSAGE("nbr of observations" << observation_files.size());
		if (observation_files.empty()) {
			exit(0);
		}

		patWay::initiateNetworkTypeRules();
		patGeoBoundingBox bb = patGeoBoundingBox(
				patNBParameters::the()->boundingBoxLeftUpLongitude,
				patNBParameters::the()->boundingBoxLeftUpLatitude,
				patNBParameters::the()->boundingBoxRightBottumLongitude,
				patNBParameters::the()->boundingBoxRightBottumLatitude);

		//		patGeoBoundingBox bb = patGeoBoundingBox(6.49909428385,
		//				46.550856996900002, 6.5700872475999997, 46.505016372300001);

		patNetworkEnvironment network_environment(bb, err);
		//Load networks
		if (err != NULL) {
			exit(-1);
		}

		DEBUG_MESSAGE("network loaded");

		int sampled = 0;
		int not_sampled = 0;
		for (int i = 0; i < observation_files.size(); ++i) {
			string file_name = dir_name + observation_files[i];

//			DEBUG_MESSAGE("===BEGIN " << file_name << " ===");
			patObservation new_observation;
			patReadObservationFromKML ro(
					&network_environment.getNetworkElements());
			ro.parseFile(file_name, new_observation);
			patReadChoiceSetFromKML rc(
					&network_environment.getNetworkElements());
			map<patOd,patChoiceSet> css = rc.read(
					patNBParameters::the()->observationDirectory
							+ patNBParameters::the()->choiceSetFolder + "/"
							+ new_observation.getId() + "_sample.kml"
					);
			new_observation.setChoiceSet(css);
			pair<int, int> ccps = new_observation.countChosenPathsSampled();
			DEBUG_MESSAGE(new_observation.getId()<<":"<<ccps.first<<","<<ccps.second<<" ("<<(double) ccps.first/(ccps.first+ccps.second)<<")")
			sampled += ccps.first;
			not_sampled += ccps.second;
//			DEBUG_MESSAGE("===End " << file_name << " ===");

			//boost::thread workerThread(workerFunc,file_path,&path_generator,&network_environment.getNetworkElements());
//		    boost::thread workerThread = testThread();
			//	    workerThread.join();
		}
		string scp_txt_fn = patNBParameters::the()->observationDirectory
				+ patNBParameters::the()->choiceSetFolder
				+ "/sampledChosenPath.txt";
		ofstream sampleFile(scp_txt_fn.c_str(), ios::app);
		sampleFile << sampled << "," << not_sampled;
		sampleFile.close();

	} catch (exception& e) {
		DEBUG_MESSAGE(e.what());
	}

}
