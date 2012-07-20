/*
 * MHLinkAndPathCost.cc
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#include "MHWeightFunction.h"
#include "patNBParameters.h"
#include <boost/algorithm/string.hpp>
#include "patNetworkBase.h"
#include "patRouter.h"
#include "patNode.h"
#include "patMultiModalPath.h"
#include  <math.h>
using namespace boost::algorithm;


MHWeightFunction::MHWeightFunction(const map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
		const double &link_scale, const double &ps_scale, const double obs_scale) :
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

double MHWeightFunction::computeObsWeight(const patMultiModalPath& path) const{

	double obs_weight=0.0;
	if (m_path_probas != NULL) {

		map<patMultiModalPath, double>::const_iterator find_path =
				m_path_probas->find(path);
		if (find_path != m_path_probas->end()) {
//			obs_weight = log(exp(pathCost) + m_obs_scale * find_path->second);
//			cout << m_obs_scale<<"*"<<find_path->second<<endl;
			obs_weight = log(1.0 + m_obs_scale * find_path->second);
//			obs_weight *= log(1.0 + m_obs_scale * find_path->second);
		}
	}
	return obs_weight;
}
double MHWeightFunction::logWeigthOriginal(
		const patMultiModalPath& path) const {

	double pathCost = getCost(path);

	pathCost+=computeObsWeight(path);
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

double MHWeightFunction::calculateObsScale(const patMultiModalPath& sp_path) {

	if (m_path_probas != NULL) {

		cout<<"path proba size"<<m_path_probas->size()<<endl;
		double sp_proba = 0.0;
		map<patMultiModalPath, double>::const_iterator find_path =
				m_path_probas->find(sp_path);
		if (find_path != m_path_probas->end()) {
			sp_proba = find_path->second;
		}

		double highest_proba = 0.0;
		patMultiModalPath highest_path;
		for (map<patMultiModalPath, double>::const_iterator path_iter =
				m_path_probas->begin();
				path_iter != m_path_probas->end();
				++path_iter) {
//			cout<<"path proba: "<<path_iter->second<<endl;
			if(path_iter->second> highest_proba){//FIXME check oD
				highest_proba = path_iter->second;
				highest_path = path_iter->first;
			}
		}

		double sp_utility = getCost(sp_path);
		double highest_utility = getCost(highest_path);

//		m_obs_scale = exp(sp_utility/highest_utility)/(highest_proba-sp_proba);
		m_obs_scale = patNBParameters::the()->mh_obs_scale* (exp(sp_utility-highest_utility)-1.0)/highest_proba;
		cout<<sp_utility<<","<<highest_utility<<","<<highest_proba<<","<<sp_proba<<endl;
		cout<<"obs scale:"<<m_obs_scale<<endl;
		if(!isfinite(m_obs_scale)){
			throw RuntimeException("inifinit obs scale");
		}
		if(m_obs_scale<0.0){
			m_obs_scale=0.0;
		}
	}

}
