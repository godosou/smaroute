/*
 * MHLinkAndPathCost.cc
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#include "MHWeightFunction.h"
#include "patNBParameters.h"
#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;

MHWeightFunction::MHWeightFunction(double link_scale, double length_coef,
		double ps_coef, double obs_scale, double sb_coef) :
		patLinkAndPathCost::patLinkAndPathCost(link_scale, length_coef, ps_coef,
				sb_coef), m_obs_scale(obs_scale), m_path_probas(NULL) {

}

MHWeightFunction::MHWeightFunction(map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
		double &link_scale, double &ps_scale, double obs_scale) :
		patLinkAndPathCost::patLinkAndPathCost(link_coef, link_scale, ps_scale), m_obs_scale(
				obs_scale), m_path_probas(NULL) {

}

MHWeightFunction* MHWeightFunction::clone() const {
	return new MHWeightFunction(*this);
}
MHWeightFunction::MHWeightFunction(const MHWeightFunction& another) :
		patLinkAndPathCost::patLinkAndPathCost(another) {
	m_path_probas = another.m_path_probas;
	m_obs_scale = another.m_obs_scale;
}
MHWeightFunction::~MHWeightFunction() {
	// TODO Auto-generated destructor stub

}

double MHWeightFunction::logWeigthOriginal(
		const patMultiModalPath& path) const {

	double pathCost = getCost(path);

	if (m_path_probas != NULL) {

		map<patMultiModalPath, double>::const_iterator find_path =
				m_path_probas->find(path);
		if (find_path != m_path_probas->end()) {
//		DEBUG_MESSAGE(m_obs_scale<<"*"<<find_path->second);
//			pathCost = log(exp(pathCost) + m_obs_scale * find_path->second);
			pathCost += log(1.0 + m_obs_scale * find_path->second);
		}
	}
	return pathCost; // - this.nodeLoopScale * nodeLoopCnt);
}

double MHWeightFunction::logWeight(const MHPath& path) const {
//    DEBUG_MESSAGE(logWeigthOriginal(path)<<","<<path.pointCombinationSize()<<","<<log(path.pointCombinationSize()));
	return (logWeigthOriginal(path) - log(path.pointCombinationSize()));
}

void MHWeightFunction::setPathProbas(
		const map<const patMultiModalPath, double>* path_probas) {
	m_path_probas = path_probas;
}

double MHWeightFunction::getObsWeightScale() const {
	return m_obs_scale;
}
