/*
 * patBTSequence.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patBTSequence.h"

#include "patBTMeasurement.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patErrMiscError.h"
#include<iostream>
#include<sstream>
#include<fstream>
patBTSequence::patBTSequence(string bt_file_name, patError*& err) {
	readFromFile(bt_file_name,err);

}

patBTSequence::~patBTSequence() {
}

void patBTSequence::readFromFile(string file_name, patError*& err) {
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
	unsigned long bt_time, bt_amount;

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
		bt_time = atof(item.c_str());
		getline(linestream, item, ','); //get bt amount
		bt_amount = atof(item.c_str());

		//DEBUG_MESSAGE(bt_time<<","<<bt_amount);
		patMeasurement* currPoint = new patBTMeasurement(bt_time, bt_amount);

		m_measurement_vector.push_back(currPoint);
	}
	DEBUG_MESSAGE("number of bt observations"<<m_measurement_vector.size());
}
