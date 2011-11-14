/*
 * patm_gps_vector.cc
 *
 *  Created on: Nov 14, 2011
 *      Author: jchen
 */

#include "patGpsSequence.h"
#include "patNBParameters.h"
patGpsSequence::patGpsSequence(string gps_file_name, patError*& err) {
	patReadGpsFromCsv::read(m_gps_vector, gps_file_name, patError*& err);
}

patGpsSequence::~patGpsSequence() {
}


void patGpsSequence::recalculateSpeedHeading() {
	patReal prevHeading = m_gps_vector.front().getHeading();
	patReal prevSpeed = m_gps_vector.front().getSpeed();
	patULong nbrOfStrangeHeading = 0;
	patULong nbr = 0;
	patReal speedSum = 0.0;
	for (patULong i = 1; i < m_gps_vector.size(); ++i) {
		if (prevHeading == m_gps_vector[i].getHeading()
				&& prevSpeed == m_gps_vector[i].getSpeed()) {
			nbrOfStrangeHeading += 1;
			if (nbrOfStrangeHeading
					> patNBParameters::the()->maxStrangeHeading) {
				//DEBUG_MESSAGE("strange heading data&heading at ("<<i+1<<"), value: "<<m_gps_vector[i].getHeading());
				patReal newHeading = -10.0;
				patReal newSpeed;
				if (i == 0) {
					newHeading = m_gps_vector[0].calHeading(&m_gps_vector[1]);
					newSpeed = 3.6
							* patNBParameters::the()->calStrangeSpeedRatio
							* m_gps_vector[0].distanceTo(&m_gps_vector[1])
							/ (m_gps_vector[1].getTimeStamp()
									- m_gps_vector[0].getTimeStamp());

					m_gps_vector[0].setHeading(newHeading);
					m_gps_vector[0].setSpeed(newSpeed);
					//m_gps_vector[0].setSpeedAccuracy(newSpeed * patNBParameters::the()->calStrangeSpeedVarianceA + patNBParameters::the()->calStrangeSpeedVarianceB);

				} else if (i == (m_gps_vector.size() - 1) || i == 1) {
					newHeading = m_gps_vector[i - 1].calHeading(&m_gps_vector[i]);
					newSpeed = 3.6
							* patNBParameters::the()->calStrangeSpeedRatio
							* m_gps_vector[i - 1].distanceTo(&m_gps_vector[i])
							/ (m_gps_vector[i].getTimeStamp()
									- m_gps_vector[i - 1].getTimeStamp());
					m_gps_vector[i].setSpeed(newSpeed);

					if (i == 1) {
						m_gps_vector[0].setHeading(newHeading);
						m_gps_vector[0].setSpeed(newSpeed);
						//m_gps_vector[0].setSpeedAccuracy(newSpeed * patNBParameters::the()->calStrangeSpeedVarianceA + patNBParameters::the()->calStrangeSpeedVarianceB);

					}

				} else {

					newSpeed = 3.6
							* patNBParameters::the()->calStrangeSpeedRatio
							* m_gps_vector[i].distanceTo(&m_gps_vector[i + 1])
							/ (m_gps_vector[i + 1].getTimeStamp()
									- m_gps_vector[i].getTimeStamp());
					m_gps_vector[i].setSpeed(newSpeed);
					newHeading = m_gps_vector[i].calHeading(&m_gps_vector[i - 1],
							&m_gps_vector[i + 1]);

				}
				m_gps_vector[i].setSpeed(newSpeed);
				//m_gps_vector[i].setSpeedAccuracy(newSpeed * 0.3 );

				m_gps_vector[i].setHeading(newHeading);

				//DEBUG_MESSAGE("after recalculate heading: "<<m_gps_vector[i].getHeading());
				//DEBUG_MESSAGE("after recalculate speed: "<<m_gps_vector[i].getSpeed());
			}
		} else {
			nbrOfStrangeHeading = 0;
			prevSpeed = m_gps_vector[i].getSpeed();
			prevHeading = m_gps_vector[i].getHeading();
		}
	}

}

void patGpsSequence::setGpsSpeedType() {

	for (int i = 0; i < m_gps_vector.size(); ++i) {
		m_gps_vector[i].setSpeedType();
	}
}

int patGpsSequence::size() const{
	return m_gps_vector.size();
}
