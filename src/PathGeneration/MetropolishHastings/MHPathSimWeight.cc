/*
 * MHPathSimWeight.cc
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#include "MHPathSimWeight.h"
#include "patMultiModalPath.h"
MHPathSimWeight::MHPathSimWeight(const double& path_distance_scale) :
		m_path_distance_scale(path_distance_scale),m_original_path(NULL) {

}

MHPathSimWeight::~MHPathSimWeight() {
}

double MHPathSimWeight::logWeigthOriginal(const patMultiModalPath& path) const {
	double distance = m_original_path->distanceFrom(&path);

	return -m_path_distance_scale * distance;

}
