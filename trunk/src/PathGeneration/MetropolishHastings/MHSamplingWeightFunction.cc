/*
 * MHSamplingWeightFunction.cpp
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#include "MHSamplingWeightFunction.h"

/*
 * MHSamplingWeightFunction.cc
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#include "MHSamplingWeightFunction.h"
#include "patNBParameters.h"
#include <boost/algorithm/string.hpp>
#include "patNetworkBase.h"
#include "patRouter.h"
#include "patNode.h"
#include "patMultiModalPath.h"
#include  <math.h>
using namespace boost::algorithm;

MHSamplingWeightFunction::MHSamplingWeightFunction(
		const map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
		const double &link_scale, const double &ps_scale,
		const double obs_scale) :
		patLinkAndPathCost::patLinkAndPathCost(link_coef, link_scale, ps_scale), m_obs_scale(
				obs_scale), m_path_probas(NULL), m_function_type(
				patNBParameters::the()->mh_function_type) {

}

MHSamplingWeightFunction* MHSamplingWeightFunction::clone() const {
	return new MHSamplingWeightFunction(*this);
}
MHSamplingWeightFunction::MHSamplingWeightFunction(
		const MHSamplingWeightFunction& another) :
		patLinkAndPathCost::patLinkAndPathCost(another), m_function_type(
				another.m_function_type) {
	m_path_probas = another.m_path_probas;
	m_obs_scale = another.m_obs_scale;

}
MHSamplingWeightFunction::~MHSamplingWeightFunction() {

}

double MHSamplingWeightFunction::computeObsWeight(
		const patMultiModalPath& path) const {

	double obs_weight = 0.0;
	if (m_path_probas != NULL) {

//		cout << m_path_probas->size() << endl;
		map<patMultiModalPath, double>::const_iterator find_path =
				m_path_probas->find(path);
		if (find_path != m_path_probas->end()) {
//			cout << m_obs_scale<<"*"<<find_path->second<<endl;

			if (m_function_type == 1) {
				obs_weight = log(1.0 + m_obs_scale * find_path->second); //option 1
			} else if (m_function_type == 2) {
				obs_weight = m_obs_scale * find_path->second; //option 2
			} else {
				throw RuntimeException("Wrong  mh function type");
			}

		}
	}
//	cout << "obs weight: " << obs_weight << endl;

	return obs_weight;
}
double MHSamplingWeightFunction::logWeigthOriginal(
		const patMultiModalPath& path) const {

	double pathCost = getCost(path);

	pathCost += computeObsWeight(path);
	return pathCost; // - this.nodeLoopScale * nodeLoopCnt);
}

void MHSamplingWeightFunction::setPathProbas(
		const map<const patMultiModalPath, double>* path_probas) {
	m_path_probas = path_probas;
}

double MHSamplingWeightFunction::getObsWeightScale() const {
	return m_obs_scale;
}

patMultiModalPath MHSamplingWeightFunction::getMostLikelyPath(const patMultiModalPath& sp_path) const {

	double highest_proba = 0.0;
	patMultiModalPath highest_path;

	double average_proba = 0.0;
	double average_cost = 0.0;

	if (m_path_probas != NULL) {
		for (map<patMultiModalPath, double>::const_iterator path_iter =
				m_path_probas->begin(); path_iter != m_path_probas->end();
				++path_iter) {
			if (path_iter->first.getUpNode() == sp_path.getUpNode()
					&& path_iter->first.getDownNode()
							== sp_path.getDownNode()) {
				average_proba += path_iter->second;
				average_cost += getCost(path_iter->first);
//			cout<<"path proba: "<<path_iter->second<<endl;
				if (path_iter->second > highest_proba) {
					highest_proba = path_iter->second;
					highest_path = path_iter->first;
				}
			}
		}
	}
	return highest_path;
}
double MHSamplingWeightFunction::calculateObsScale(
		const patMultiModalPath& sp_path) {

	if (patNBParameters::the()->mh_link_scale_relative == 1) {
		double length_coef = -log(2.0)
				/ ((m_link_cost_scale - 1.0) * sp_path.getLength());
		m_link_cost_scale = length_coef / m_link_coefficients[ENUM_LENGTH];
//		m_link_coefficients[ENUM_LENGTH]=length_coef;
		cout << "MHSamplingWeightFunction: new link cost scale"
				<< m_link_cost_scale << endl;
	}
	if (m_path_probas != NULL && patNBParameters::the()->mh_obs_scale>0) {
		cout << "calibrate obs scale" << endl;
		cout << "path proba size" << m_path_probas->size() << endl;
		double sp_proba = 0.0;
		map<patMultiModalPath, double>::const_iterator find_path =
				m_path_probas->find(sp_path);
		if (find_path != m_path_probas->end()) {
			sp_proba = find_path->second;

		}

		double highest_proba = 0.0;
		patMultiModalPath highest_path;

		double average_proba = 0.0;
		double average_cost = 0.0;
		for (map<patMultiModalPath, double>::const_iterator path_iter =
				m_path_probas->begin(); path_iter != m_path_probas->end();
				++path_iter) {
			if (path_iter->first.getUpNode() == sp_path.getUpNode()
					&& path_iter->first.getDownNode()
							== sp_path.getDownNode()) {
				average_proba += path_iter->second;
				average_cost += getCost(path_iter->first);
//			cout<<"path proba: "<<path_iter->second<<endl;
				if (path_iter->second > highest_proba) {
					highest_proba = path_iter->second;
					highest_path = path_iter->first;
				}
			}
		}
		average_proba /= (double) m_path_probas->size();
		average_cost /= (double) m_path_probas->size();
		double sp_utility = getCost(sp_path);
		double highest_utility = getCost(highest_path);
		if (sp_utility == highest_utility) {
//			m_obs_scale = 0.0;
		} else {
			if (m_function_type == 1) {
				m_obs_scale = patNBParameters::the()->mh_obs_scale
						* (exp(sp_utility - highest_utility) - 1.0)
						/ highest_proba; //option 1
			} else if (m_function_type == 2) {
				m_obs_scale = patNBParameters::the()->mh_obs_scale
						* (sp_utility - highest_utility)
						/ (highest_proba - sp_proba); //option 2
//			m_obs_scale = patNBParameters::the()->mh_obs_scale
//					* (sp_utility - average_cost) / (average_proba - sp_proba); //option 2 average cost
			} else {
				throw RuntimeException("Wrong  mh function type");
			}
		}
		cout << sp_utility << "," << highest_utility << "," << highest_proba
				<< "," << sp_proba << endl;
		cout << "obs scale:" << m_obs_scale << endl;
		if (!isfinite(m_obs_scale)) {
			throw RuntimeException("inifinit obs scale");
		}
		if (m_obs_scale < 0.0) {
			cout << "negative obs scale, set to zero istead" << endl;
			m_obs_scale = 0.0;
		}
		cout << "log weight for shortest:" << logWeigthOriginal(sp_path)
				<< ", most likely:" << logWeigthOriginal(highest_path) << endl;
	}
	return m_obs_scale;
}
void MHSamplingWeightFunction::calibrate(const patMultiModalPath& path) {
	calculateObsScale(path);
}
