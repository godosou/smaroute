/*
 * patm_gps_vector.cc
 *
 *  Created on: Nov 14, 2011
 *      Author: jchen
 */

#include "patGpsSequence.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patErrMiscError.h"
#include<iostream>
#include<sstream>
#include<fstream>
patGpsSequence::patGpsSequence(string gps_file_name, patError*& err) {

	readFromFile(gps_file_name, err);

	recalculateSpeedHeading();
	setGpsSpeedType();
}

void patGpsSequence::recalculateSpeedHeading() {
	double prevHeading = m_gps_vector.front()->getHeading();
	double prevSpeed = m_gps_vector.front()->getSpeed();
	unsigned long nbrOfStrangeHeading = 0;
	unsigned long nbr = 0;
	double speedSum = 0.0;
	for (unsigned long i = 1; i < m_gps_vector.size(); ++i) {
		double temp_current_speed = m_gps_vector[i]->getSpeed() ;
		double temp_current_heading = m_gps_vector[i]->getHeading() ;
		if ((prevHeading == m_gps_vector[i]->getHeading()
				&& prevSpeed == m_gps_vector[i]->getSpeed())||
				i< m_gps_vector.size()-1 &&
				 m_gps_vector[i]->getHeading() == m_gps_vector[i+1]->getHeading()
				&& m_gps_vector[i]->getSpeed() == m_gps_vector[i+1]->getSpeed()
		) {
			nbrOfStrangeHeading += 1;
			if (nbrOfStrangeHeading
					> patNBParameters::the()->maxStrangeHeading) {
				//DEBUG_MESSAGE("strange heading data&heading at ("<<i+1<<"), value: "<<m_gps_vector[i]->getHeading());
				double newHeading = -10.0;
				double newSpeed;
				if (i == 0) {
					newHeading = m_gps_vector[i]->calHeading(
							m_gps_vector[i + 1]);
					newSpeed = m_gps_vector[i]->calSpeed(m_gps_vector[i + 1]);
					//m_gps_vector[0]->setSpeedAccuracy(newSpeed * patNBParameters::the()->calStrangeSpeedVarianceA + patNBParameters::the()->calStrangeSpeedVarianceB);

				} else if (i == 1) {
					double new_speed_for_first = m_gps_vector[i]->calSpeed(
							m_gps_vector[i - 1]);
					m_gps_vector[i - 1]->setSpeed(new_speed_for_first);

					m_gps_vector[i - 1]->setHeading(m_gps_vector[i-1]->calHeading(
							m_gps_vector[i]));

					newHeading = m_gps_vector[i]->calHeading(
							m_gps_vector[i - 1], m_gps_vector[i + 1]);

					newSpeed = m_gps_vector[i]->calSpeed(m_gps_vector[i - 1],
							m_gps_vector[i + 1]);
				}

				else if (i == (m_gps_vector.size() - 1)) {
					newHeading = m_gps_vector[i - 1]->calHeading(
							m_gps_vector[i]);
					newSpeed = m_gps_vector[i]->calSpeed(m_gps_vector[i - 1]);

				} else {
					newSpeed = m_gps_vector[i]->calSpeed(m_gps_vector[i - 1],
							m_gps_vector[i + 1]);
					;
					newHeading = m_gps_vector[i]->calHeading(
							m_gps_vector[i - 1], m_gps_vector[i + 1]);
				}

				m_gps_vector[i]->setSpeed(newSpeed);
				m_gps_vector[i]->setHeading(newHeading);

				if (i == (m_gps_vector.size() - 1)) {

					if (m_gps_vector[i]->getTimeStamp()
							- m_gps_vector[i - 1]->getTimeStamp() > 30.0) {
						m_gps_vector[i]->setType("low_speed");
					}

				}
				else{

					if (m_gps_vector[i]->getTimeStamp()
							- m_gps_vector[i - 1]->getTimeStamp() > 30.0 &&
							m_gps_vector[i+1]->getTimeStamp()
														- m_gps_vector[i]->getTimeStamp() > 30.0) {
						m_gps_vector[i]->setType("low_speed");
					}
				}
				DEBUG_MESSAGE("after recalculate heading: "<<m_gps_vector[i]->getHeading());
				DEBUG_MESSAGE("after recalculate speed: "<<m_gps_vector[i]->getSpeed());
			}
		} else {
			nbrOfStrangeHeading = 0;
		}
		prevSpeed = temp_current_speed;
		prevHeading = temp_current_heading;
	}

}

void patGpsSequence::setGpsSpeedType() {

	for (int i = 0; i < m_gps_vector.size(); ++i) {
		m_gps_vector[i]->setSpeedType();
	}
}

int patGpsSequence::size() const {
	return m_gps_vector.size();
}

void patGpsSequence::readFromFile(string file_name, patError*& err) {
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
	unsigned long curr_trip_id = 1, last_trip_id = -1; //current trip id
	unsigned long pointNumber = 0;

	unsigned long currTime, currEndTime=0.0;
	double currLat, currLon, currSpeed, currHeading, currAccuracyH,
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

			last_trip_id == curr_trip_id;
		} else if (curr_trip_id != last_trip_id) { //getTripId

			stringstream str;
			str << "Invalid file with more than one trip id in line: "
					<< pointNumber;
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
		patGpsPoint* currPoint = new patGpsPoint(userId, currTime, currLat,
				currLon, currSpeed, currAccuracyH, currHeading, currAccuracyV,
				currAccuracyS, currAccuracyHD, NULL);

		m_gps_vector.push_back(currPoint);
		currEndTime = currTime;
		last_trip_id = curr_trip_id;

	}
	DEBUG_MESSAGE("end time: " << currEndTime);

	DEBUG_MESSAGE(
			"Travler: " << userId << ",Trip:" << last_trip_id << ",GPS Points:"
					<< pointNumber);
}

patGpsSequence::~patGpsSequence() {

	for (vector<patGpsPoint*>::iterator iter = m_gps_vector.begin();
			iter != m_gps_vector.end(); ++iter) {
		delete *iter;
		*iter = NULL;
	}
}
patGpsPoint* patGpsSequence::at(int i) {
	if (i >= m_gps_vector.size()) {
		return NULL;
	} else {
		return m_gps_vector[i];
	}
}
