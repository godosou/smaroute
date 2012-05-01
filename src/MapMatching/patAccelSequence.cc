/*
 * patAccelSequence.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patAccelSequence.h"
#include "patAccelMeasurement.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patErrMiscError.h"
#include<iostream>
#include<sstream>
#include<fstream>
patAccelSequence::patAccelSequence(string file_name, patError*& err) {
	readFromFile(file_name,err);



}

patAccelSequence::~patAccelSequence() {
}



void patAccelSequence::readFromFile(string file_name, patError*& err) {
	ifstream file_stream_handler;

	file_stream_handler.open(file_name.c_str(), ios::in);
	if (!file_stream_handler) {
		stringstream str;
		str << "Error while parsing " << file_name;
		err = new patErrMiscError(str.str());
		WARNING(err->describe());
		return;
	}

	DEBUG_MESSAGE("Read file:" << file_name);
	unsigned long accel_time, accel;

	string line;
	/*
	getline(file_stream_handler, line);
	unsigned long userId = atol(line.c_str());
	DEBUG_MESSAGE("UserID: " << userId);
*/
	while (getline(file_stream_handler, line)) {

		istringstream linestream(line);

		string item;

		getline(linestream, item, ','); //get  time
		accel_time = atof(item.c_str());

		getline(linestream, item, ','); //get accel
		accel = atof(item.c_str());

		patMeasurement* currPoint = new patAccelMeasurement(accel_time, accel);

		m_measurement_vector.push_back(currPoint);
	}
	DEBUG_MESSAGE("number of accel observations"<<m_measurement_vector.size());
}
