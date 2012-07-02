/*
 * RWGenerator.cc
 *
 *  Created on: May 31, 2012
 *      Author: jchen
 */

#include "RWPathGenerator.h"
#include "patDisplay.h"
#include "patNBParameters.h"
#include "patRouter.h"
#include "patShortestPathTreeGeneral.h"
#include "patSampleDiscreteDistribution.h"
#include "patException.h"

#include "patRoadBase.h"
#include "patNetworkBase.h"
#include "patPathWriter.h"
#include "patAbstractCost.h"

RWPathGenerator::RWPathGenerator(unsigned long rng, double kumaA, double kumaB,
		const patLinkAndPathCost* link_cost) :
		m_rnd(rng), m_kumaA(kumaA), m_kumaB(kumaB), m_link_cost(link_cost) {

	m_msgInterval = patNBParameters::the()->MSGINTERVAL_ELEMENT;
	m_total_samples = patNBParameters::the()->SAMPLE_COUNT;

}

RWPathGenerator::~RWPathGenerator() {
	// TODO Auto-generated destructor stub
}

void RWPathGenerator::setNetwork(const patNetworkBase* network_environment) {
	m_network = network_environment;
}

void RWPathGenerator::setPathWriter(patPathWriter* path_writer) {
	m_path_writer = path_writer;
}
void RWPathGenerator::setLinkCost(const patLinkAndPathCost* link_cost) {
	m_link_cost = link_cost;
}
map<const patRoadBase*, double> RWPathGenerator::getOutgoingArcProbas(
		const patNode* from_node, const patNode* destination,
		const patShortestPathTreeGeneral* sp_tree) const {
	map<const patRoadBase*, double> arc_probas;

	set<const patRoadBase*> outgoing_roads = m_network->getOutgoingRoads(
			from_node);

	double sp_cost_up = DBL_MAX;
	if (from_node == destination) {

		for (set<const patRoadBase*>::const_iterator arc_iter =
				outgoing_roads.begin(); arc_iter != outgoing_roads.end();
				++arc_iter) {
			double spCostDN = sp_tree->getLabel((*arc_iter)->getDownNode());
			if (spCostDN == DBL_MAX) {
				continue;
			}
			double arc_cost = m_link_cost->getCost(*arc_iter);
			if (spCostDN + arc_cost < sp_cost_up) {
				sp_cost_up = spCostDN + arc_cost;
			}

		}

	} else {

		sp_cost_up = sp_tree->getLabel(from_node);
	}
	if (sp_cost_up == DBL_MAX) {
		WARNING("UP COST DLB_MAX");
		return map<const patRoadBase*, double>();
	}

	//Loop on the outgoing arcs, and compute the cost of each shortest path.
	double total(0.0);
	for (set<const patRoadBase*>::const_iterator arc_iter =
			outgoing_roads.begin(); arc_iter != outgoing_roads.end();
			++arc_iter) {
		double spCost = sp_tree->getLabel((*arc_iter)->getDownNode());
		if (spCost == DBL_MAX) {
			continue;
		}

//		DEBUG_MESSAGE(spCost);
		double theRatio = sp_cost_up
				/ (spCost + m_link_cost->getCost(*arc_iter));
		if (theRatio == 0.0) {
			stringstream str;
			str << "Ratio should be greater than zeros, and is " << theRatio;
			WARNING(str.str());
			throw RuntimeException(str.str().c_str());
		} else if (theRatio > 1.0) {
			stringstream str;
			str << "Ratio should be lower than 1, and is " << theRatio;
			str << ", shortest cost:" << sp_cost_up << ", sp cost" << spCost
					<< ", arc cost:" << m_link_cost->getCost(*arc_iter)
					<< ", length:" << (*arc_iter)->getLength();
			WARNING(str.str());
			throw RuntimeException(str.str().c_str());
			return map<const patRoadBase*, double>();
		}

		double kuma = 1 - pow((1 - pow(theRatio, m_kumaA)), m_kumaB);
		if (!finite(kuma)) {
			kuma = 0.0;
		}
//		DEBUG_MESSAGE(kuma);
		total += kuma;

		arc_probas.insert(
				pair<const patRoadBase*, const double>(*arc_iter, kuma));

	}
	if (arc_probas.size() == 0 || total == 0.0) {
		throw RuntimeException("Invalid probas for ougoing arcs");
		return map<const patRoadBase*, double>();
	}
	for (map<const patRoadBase*, double>::iterator arc_iter =
			arc_probas.begin(); arc_iter != arc_probas.end(); ++arc_iter) {
		arc_iter->second /= total;

	}

	return arc_probas;

}
pair<const patRoadBase*, const double> RWPathGenerator::getNextArcInRandomWalk(
		const patNode* from_node, const patNode* destination,
		const patShortestPathTreeGeneral* sp_tree) const {
	//DEBUG_MESSAGE("get the next arc from"<<*fromNode);

	map<const patRoadBase*, double> arc_probas = getOutgoingArcProbas(from_node,
			destination, sp_tree);
	vector<const patRoadBase*> theArcs;
	vector<double> probas;
//	DEBUG_MESSAGE(arc_probas.size());
	for (map<const patRoadBase*, double>::const_iterator arc_iter =
			arc_probas.begin(); arc_iter != arc_probas.end(); ++arc_iter) {
//		DEBUG_MESSAGE(arc_iter->second);
		theArcs.push_back(arc_iter->first);
		probas.push_back(arc_iter->second);

	}
	int selectId = patSampleDiscreteDistribution()(probas, m_rnd);

	return pair<const patRoadBase*, const double>(theArcs[selectId],
			probas[selectId]);
}

void RWPathGenerator::sampleOnePath(const patNode* origin,
		const patNode* destination, const patShortestPathTreeGeneral* sp_tree,
		patChoiceSetWriter& csw) {
	patMultiModalPath new_path;
	patMultiModalPath empty_path;
	double logProba = 0.0;
	double dest_count = 0.0;
	const patNode* current_node = origin;
	while (true) {
		if (current_node == destination) {
			if (m_network->getOutgoingRoads(current_node).size() == 0) {
				break;
			}
			dest_count += 1.0;
			vector<double> proba;
			vector<patArc*> arcVector;
			proba.push_back(pow(0.5, dest_count));
			proba.push_back(1.0 - pow(0.5, dest_count));
			unsigned long selectId = patSampleDiscreteDistribution()(proba,
					m_rnd);
			logProba += log(proba[selectId]);
			if (selectId == proba.size() - 1) {
				break;
			}
		}
		pair<const patRoadBase*, const double> result = getNextArcInRandomWalk(
				current_node, destination, sp_tree);

		if (result.first == NULL
				|| !new_path.addRoadTravelToBack(result.first, CAR)) {
			WARNING("fail to add downstream road"<<result.second);
			throw RuntimeException("fail to add downstream road");
			return;
		}

		logProba += log(result.second);

		current_node = result.first->getDownNode();

	}
	if (logProba <= DBL_MAX || logProba >= -DBL_MAX) {
		csw.processState(new_path, logProba);
//		map<patMultiModalPath, pair<int, double> >::iterator find_sampled_path =
//				m_sampled_set.find(new_path);
//
//		pair<int, double> d(1, logProba);
//		if (find_sampled_path == m_sampled_set.end()) {
//			m_sampled_set.insert(
//					pair<patMultiModalPath, pair<int, double> >(new_path, d));
//		} else {
//			find_sampled_path->second.first += 1;
//			find_sampled_path->second.second += logProba;
//		}

	} else {
		stringstream ss;
		ss << "sampling algorithm is not valid:" << exp(logProba);
		WARNING(ss.str());
		return;

	}
}
double RWPathGenerator::calculatePathLogWeight(
		const patMultiModalPath& path) const {
	vector<const patArc*> arc_list = path.getArcList();
	patRouter router(m_network, m_link_cost);
	patShortestPathTreeGeneral bwdCost(BWD);
	const patNode* origin = path.getUpNode();
	const patNode* destination = path.getDownNode();
	router.bwdCost(bwdCost, destination);

	double path_log_proba(0.0);
	double dest_count = 0.0;
	for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		if ((*arc_iter)->getUpNode() == destination) {
			dest_count += 1.0;
			path_log_proba += log(pow(0.5, dest_count));
		}
		map<const patRoadBase*, double> arc_probas = getOutgoingArcProbas(
				(*arc_iter)->getUpNode(), destination, &bwdCost);
		map<const patRoadBase*, double>::const_iterator find_arc =
				arc_probas.find(*arc_iter);
		if (find_arc == arc_probas.end()) {
			throw RuntimeException("arc no in ougoing choice set");
		}
		path_log_proba += log(find_arc->second);

	}
	if (m_network->getOutgoingRoads(destination).size() != 0) {
		path_log_proba += log(1 - pow(0.5, dest_count + 1.0));
	}

	return path_log_proba;
}
void RWPathGenerator::run(const patNode* origin, const patNode* destination) {
	patRouter router(m_network, m_link_cost);
	patShortestPathTreeGeneral bwdCost(BWD);
	router.bwdCost(bwdCost, destination);
	patChoiceSetWriter csw(m_path_writer, 1);
	csw.start();
	for (int iter = 0; iter < m_total_samples; ++iter) {
		sampleOnePath(origin, destination, &bwdCost, csw);
	}

	csw.end();
}
