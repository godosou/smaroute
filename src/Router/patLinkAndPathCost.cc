/*
 * patPathCost.cc
 *
 *  Created on: Jun 1, 2012
 *      Author: jchen
 */

#include "patLinkAndPathCost.h"
#include "patRoadBase.h"
#include "patMultiModalPath.h"
#include "patException.h"
#include "patNetworkElements.h"
patLinkAndPathCost::~patLinkAndPathCost() {
	if (m_ps_computer != NULL) {
		delete m_ps_computer;
		m_ps_computer = NULL;
	}

}

patLinkAndPathCost* patLinkAndPathCost::clone() const {
	return new patLinkAndPathCost(*this);
}
patLinkAndPathCost::patLinkAndPathCost(
		const map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
		const double &link_scale, const double &ps_scale) :
		m_link_cost_scale(link_scale),
		m_pathsize_coefficient(link_scale * ps_scale), m_link_coefficients(
				link_coef), m_ps_computer(NULL) {



}

const map<ARC_ATTRIBUTES_TYPES, double>& patLinkAndPathCost::getLinkCoefficients() const{
	return  m_link_coefficients;
}
double patLinkAndPathCost::getCost(const patRoadBase* road) const {
	return m_link_cost_scale * road->getGeneralizedCost();

}

double patLinkAndPathCost::getCost(const patMultiModalPath& path) const {

	double pathCost = 0.0;
	vector<const patArc*> arcs = path.getArcList();
	for (vector<const patArc*>::const_iterator arc_iter = arcs.begin();
			arc_iter != arcs.end(); ++arc_iter) {
		pathCost += getCost(*arc_iter);
	}
	if (m_ps_computer != NULL && m_pathsize_coefficient > 0.0) {
		double ps_value = m_ps_computer->getPS(path);
		pathCost +=  m_pathsize_coefficient * log(ps_value);
	}
	return pathCost;
}

double patLinkAndPathCost::getCostWithPathSize(const patMultiModalPath& path,
		double path_size) const {
	double path_cost = getCost(path) + m_pathsize_coefficient * log(path_size);
	return path_cost;
}
double patLinkAndPathCost::getPathSizeCoefficient() const {
	return m_pathsize_coefficient;
}

void patLinkAndPathCost::setPathSizeComputer(patPathSizeComputer* pathsize) {
	m_ps_computer = pathsize->clone();
}
/*
 * Get attributes of a path for the utility function.
 * @param path: the path.
 * @return: hashed map with key and value.
 */
map<string, double> patLinkAndPathCost::getAttributes(
		const patMultiModalPath& path) const {
	map<string, double> attributes;
	for (map<ARC_ATTRIBUTES_TYPES, double>::const_iterator a_iter =
			m_link_coefficients.begin(); a_iter != m_link_coefficients.end();
			++a_iter) {
		//DEBUG_MESSAGE(a_iter->second<<"*"<<arc->getAttribute(a_iter->first));
		double cost = 0.0;
		vector<const patArc*> arcs = path.getArcList();
		for (vector<const patArc*>::const_iterator arc_iter = arcs.begin();
				arc_iter != arcs.end(); ++arc_iter) {
			cost += ((double) (*arc_iter)->getAttribute(a_iter->first));
		}
		attributes[patArc::getAttributeTypeString(a_iter->first)] = cost; //FIXME scale parameter
	}
	if (m_ps_computer != NULL) {
		double ps_value = m_ps_computer->getPS(path);
		attributes["path_size"] = m_ps_computer->getPS(path);
	}
	return attributes;

}
