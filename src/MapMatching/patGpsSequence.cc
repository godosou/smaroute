/*
 * patm_gps_vector.cc
 *
 *  Created on: Nov 14, 2011
 *      Author: jchen
 */

#include "patGpsSequence.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patException.h"
#include<iostream>
#include<sstream>
#include<fstream>
patGpsSequence::patGpsSequence(string gps_file_name) {

	readFromFile(gps_file_name);

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
		double temp_current_speed = m_gps_vector[i]->getSpeed();
		double temp_current_heading = m_gps_vector[i]->getHeading();
		if ((prevHeading == m_gps_vector[i]->getHeading()
				&& prevSpeed == m_gps_vector[i]->getSpeed())
				|| i < m_gps_vector.size() - 1
						&& m_gps_vector[i]->getHeading()
								== m_gps_vector[i + 1]->getHeading()
						&& m_gps_vector[i]->getSpeed()
								== m_gps_vector[i + 1]->getSpeed()) {
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

					m_gps_vector[i - 1]->setHeading(
							m_gps_vector[i - 1]->calHeading(m_gps_vector[i]));

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

				} else {

					if (m_gps_vector[i]->getTimeStamp()
							- m_gps_vector[i - 1]->getTimeStamp() > 30.0
							&& m_gps_vector[i + 1]->getTimeStamp()
									- m_gps_vector[i]->getTimeStamp() > 30.0) {
						m_gps_vector[i]->setType("low_speed");
					}
				}
//				DEBUG_MESSAGE(
//						"after recalculate heading: "<<m_gps_vector[i]->getHeading());
//				_MESSAGE(
//						"after recalculate speed: "<<m_gps_vector[i]->getSpeed());
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

void patGpsSequence::readFromFile(string file_name) {
	ifstream file_stream_handler;

	file_stream_handler.open(file_name.c_str(), ios::in);
	if (!file_stream_handler) {
		stringstream str;
		str << "patGpsSequence: Error while parsing " << file_name;
		WARNING(str.str());
		return;
//		throw RuntimeException(str.str().c_str());
	}

	cout << "Read file:" << file_name << endl;
	unsigned long curr_trip_id = 1, last_trip_id = -1; //current trip id
	unsigned long pointNumber = 0;

	unsigned long currTime, currEndTime = 0.0;
	double currLat, currLon, currSpeed, currHeading, currAccuracyH,
			currAccuracyV, currAccuracyS, currAccuracyHD;

	string line;
	getline(file_stream_handler, line);
	unsigned long userId = atol(line.c_str());

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
			throw RuntimeException(str.str().c_str());
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
//	DEBUG_MESSAGE("end time: " << currEndTime);
//
//	DEBUG_MESSAGE(
//			"Travler: " << userId << ",Trip:" << last_trip_id << ",GPS Points:" << pointNumber);
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

double patGpsSequence::computeLength() const {
	double length = 0.0;
	if (m_gps_vector.size() <= 1) {
		return length;
	}
	for (unsigned i = 1; i < m_gps_vector.size(); ++i) {
		length += m_gps_vector.at(i)->distanceTo(m_gps_vector.at(i - 1));
	}
	return length;
}
vector<double> patGpsSequence::distanceTo(const patRoadBase& road) const {
	vector<double> p_distances;
	vector<const patArc*> arc_list = road.getArcList();
	for (vector<patGpsPoint*>::const_iterator p_iter = m_gps_vector.begin();
			p_iter != m_gps_vector.end(); ++p_iter) {
		double min_p_dist = DBL_MAX;
		for (vector<const patArc*>::const_iterator a_iter = arc_list.begin();
				a_iter != arc_list.end(); ++a_iter) {
			double a_dist = (*p_iter)->distanceTo(*a_iter)["link"];
			min_p_dist = a_dist < min_p_dist ? a_dist : min_p_dist;
		}
		p_distances.push_back(min_p_dist);
	}
	return p_distances;
}

patGeoBoundingBox patGpsSequence::computeBoundingBox(
		const double& margin) const {
	double max_lat = -DBL_MAX;
	double min_lon = DBL_MAX;
	double min_lat = DBL_MAX;
	double max_lon = -DBL_MAX;
	for (vector<patGpsPoint*>::const_iterator p_iter = m_gps_vector.begin();
			p_iter != m_gps_vector.end(); ++p_iter) {
		double lat = (*p_iter)->getLatitude();
		double lon = (*p_iter)->getLongitude();
//		cout <<"lat: "<<lat<<", lon: "<<lon<<endl;
		max_lat = lat > max_lat ? lat : max_lat;
		min_lat = lat < min_lat ? lat : min_lat;

		max_lon = lon > max_lon ? lon : max_lon;
		min_lon = lon < min_lon ? lon : min_lon;
	}
	max_lat += margin;
	max_lon += margin;

	min_lat -= margin;
	min_lon -= margin;
	return patGeoBoundingBox(min_lon, max_lat, max_lon, min_lat);

}

const patGpsPoint* patGpsSequence::getFirstGps() const {
	if (m_gps_vector.empty()) {
		throw RuntimeException("patGpsSequence: No gps in the sequence.");
	}
	return m_gps_vector.front();
}
const patGpsPoint* patGpsSequence::getLastGps() const {
	if (m_gps_vector.empty()) {
		throw RuntimeException("patGpsSequence: No gps in the sequence.");
	}
	return m_gps_vector.back();
}
