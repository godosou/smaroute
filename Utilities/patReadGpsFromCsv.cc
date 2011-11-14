/*
 * patReadGpsFromCsv.cc
 *
 *  Created on: Nov 14, 2011
 *      Author: jchen
 */

#include "patReadGpsFromCsv.h"

#include<iostream>
#include<sstream>
#include<fstream>
patReadGpsFromCsv::patReadGpsFromCsv() {

}

patReadGpsFromCsv::~patReadGpsFromCsv() {
}

static void read(vector<patGpsPoint>& gps_sequence, string file_name,
		patError*& err) {
	ifstream file_stream_handler;

	file_stream_handler.open(file_name.c_str(), ios::in);
	if (!file_stream_handler) {
		stringstream str;
		str << "Error while parsing " << file_name;
		err = new patErrMiscError(str.str());
		WARNING(err->describe());
		return ;
	}
	DEBUG_MESSAGE("Read file:" << file_name);
	unsigned long curr_trip_id = 1, last_trip_id = -1; //current trip id
	unsigned long  pointNumber = 0;

	unsigned long currTime, currEndTime;
	patReal currLat, currLon, currSpeed, currHeading, currAccuracyH,
			currAccuracyV, currAccuracyS, currAccuracyHD;

	string line;
	getline(file_stream_handler, line);
	unsigned long userId = atol(line.c_str());
	DEBUG_MESSAGE("UserID: " << userId);

	while (getline(file_stream_handler, line)) {

		pointNumber++;
		istringstream linestream(line);

		string item;

		getline(linestream, item, ',');
		curr_trip_id = atol(item.c_str());

		getline(linestream, item, ','); //get  time
		currTime = atof(item.c_str());

		if (last_trip_id == -1) {

			last_trip_id==curr_trip_id;
		} else if (curr_trip_id != last_trip_id) { //getTripId

			stringstream str;
			str << "Invalid file with more than one trip id in line: " << pointNumber;
			err = new patErrMiscError(str.str());
			WARNING(err->describe());
			return;
		}

		getline(linestream, item, ','); //get longitude
		currLon = atof(item.c_str());
		getline(linestream, item, ','); //get latitude
		currLat = atof(item.c_str());
		getline(linestream, item, ','); //altitude
		getline(linestream, item, ','); //get speed
		currSpeed = atof(item.c_str());

		getline(linestream, item, ','); //get
		currHeading = atof(item.c_str());
		getline(linestream, item, ','); //get
		currAccuracyH = atof(item.c_str());
		getline(linestream, item, ','); //get
		currAccuracyV = atof(item.c_str());
		getline(linestream, item, ','); //get
		currAccuracyS = atof(item.c_str());
		getline(linestream, item, ','); //get
		currAccuracyHD = atof(item.c_str());
		patGpsPoint currPoint(userId, currTime, currLat, currLon, currSpeed,
				currAccuracyH, currHeading, currAccuracyV, currAccuracyS,
				currAccuracyHD, NULL);

		gps_sequence->insert(currPoint);
		currEndTime = currTime;
		last_trip_id = curr_trip_id;

	}
	DEBUG_MESSAGE("end time: " << currEndTime);

	DEBUG_MESSAGE(
			"Travler: " << userId << ",Trip:" << last_trip_id << ",GPS Points:"
					<< pointNumber);
}
