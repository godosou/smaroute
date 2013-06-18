/*
 * patReadObervations.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patReadObervations.h"

patReadObervations::patReadObervations() {

}

patReadObervations::~patReadObervations() {
}

void readFromFile::insertToDataVector(vector<patMeasurement*>& data_vector,
		vector<patMeasurement*>& inserted_vector) {
	vector<patMeasurement*>::iterator inclusive_iter = data_vector.begin();
	vector<patMeasurement*>::iterator inserted_iter = inserted_vector.begin();

	while (inserted_iter != inserted_vector.end()) {
		while ((*inserted_iter)->getTimeStamp()
				> (*inclusive_iter)->getTimeStamp()) {
			++inclusive_iter;
			if (inclusive_iter == data_vector.end()) {
				break;
			}
		}

		inclusive_iter = data_vector.insert(inclusive_iter, *inserted_iter);
		++inserted_iter;
	}
}
static void readFromFile(string file_name, vector<patMeasurement>& data_vector,
		patError*& err) {
	vector<patMeasurement*> ordered_all_data;
	string gps_file = patNBParameters::the()->dataDirectory + "/" + file_name
			+ "_gps.csv";
	string bt_file = patNBParameters::the()->dataDirectory + "/" + file_name
			+ "_bt.csv";
	string accel_file = patNBParameters::the()->dataDirectory + "/" + file_name
			+ "_bt.csv";
	patGpsSequence gps_sequence(gps_file, err);
	if (err != NULL) {
		insertToDataVector(ordered_all_data, gps_sequence->getPointerVector())
	}
	err = NULL;
	patAccelSequence accel_sequence(accel_file, err);
	if (err != NULL) {
		insertToDataVector(ordered_all_data, accel_sequence->getPointerVector())
	}
}
