/*
 * MHNIPSP.cc
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#include "MHNIPSP.h"
#include "patNode.h"
#include <set>
using namespace std::tr1;
using namespace std;
#include "patNetworkBase.h"
#include "patMultiModalPath.h"
unordered_map<const patNode*, double> MHNIPSP::calculate(
		const patNode* origin, const patNode* destination) const {

	unordered_map<const patNode*, double> proposal_probabilities;
	set<const patNode*> all_nodes = m_network->getNodes();
	double minCost = DBL_MAX;
	for (set<const patNode*>::const_iterator node_iter = all_nodes.begin();
			node_iter != all_nodes.end(); ++node_iter) {
		double cost = -m_path->distanceTo(*node_iter);
		proposal_probabilities[*node_iter] = cost;
		minCost = cost < minCost ? cost : minCost;
	}

	double weightSum = 0.0;
	for (unordered_map<const patNode*, double>::iterator node_iter =
			proposal_probabilities.begin();
			node_iter != proposal_probabilities.end(); ++node_iter) {
		double weight = exp(node_iter->second - minCost); //weight
		weightSum += weight;
		node_iter->second = weight;
	}
	for (unordered_map<const patNode*, double>::iterator node_iter =
			proposal_probabilities.begin();
			node_iter != proposal_probabilities.end(); ++node_iter) {
		node_iter->second /= weightSum;
//		cout<<node_iter->first->getUserId()<< ":"<<node_iter->second<<endl;
	}
	return proposal_probabilities;
}
MHNIPSP::~MHNIPSP() {
	// TODO Auto-generated destructor stub
}

