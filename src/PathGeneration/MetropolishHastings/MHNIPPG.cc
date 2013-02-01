/*
 * MHNIPPG.cc
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#include "MHNIPPG.h"
#include "patNode.h"
#include "patShortestPathTreeGeneral.h"
#include "patRouter.h"
#include "MHWeightFunction.h"
#include <list>
using namespace std::tr1;
using namespace std;
MHNIPPG::~MHNIPPG() {
	// TODO Auto-generated destructor stub
}

unordered_map<const patNode*, double> MHNIPPG::calculate(const patNode* origin,
		const patNode* destination) const {

	if (m_network->getOutgoingIncidents()->find(origin)
			== m_network->getOutgoingIncidents()->end()) {
		cout << "origin" << origin->getUserId() << " not in network" << endl;
	}
	if (m_network->getIncomingIncidents()->find(destination)
			== m_network->getIncomingIncidents()->end()) {
		cout << "destination" << destination->getUserId() << " not in network"
				<< endl;
	}
	unordered_map<const patNode*, double> proposal_probabilities;
	//==============Start derive insertion probability for each node==============//
	patShortestPathTreeGeneral fwdTree(FWD);
	m_router->fwdCost(fwdTree, origin);

	patShortestPathTreeGeneral bwdTree(BWD);
	m_router->bwdCost(bwdTree, destination);

	const unordered_map<const patNode*, double>& fwdCost = fwdTree.getLabels();
	const unordered_map<const patNode*, double>& bwdCost = bwdTree.getLabels();
	double minCost = DBL_MAX;
	double maxCost = -DBL_MAX;
	set<const patNode*> all_nodes = m_router->getNetwork()->getNodes();
	cout << "MHNIPPG: node size" << all_nodes.size() << "fwd" << fwdCost.size() << "bwd"
			<< bwdCost.size() << "," << origin->getUserId() << "-"
			<< destination->getUserId() << endl;
	unsigned invalid_fwd_connection=0;
	unsigned invalid_bwd_connection=0;
	for (unordered_map<const patNode*, double>::const_iterator node_iter =
			fwdCost.begin(); node_iter != fwdCost.end(); ++node_iter) {
		unordered_map<const patNode*, double>::const_iterator bc_iter =
				bwdCost.find(node_iter->first);
		if(node_iter->first==NULL){
			cout<<"NULL node"<<endl;
		}
		if (bc_iter != bwdCost.end() && bc_iter->second != DBL_MAX
				&& node_iter->second != DBL_MAX) {
			list<const patRoadBase*> sp_roads;
			fwdTree.getShortestPathTo(sp_roads, node_iter->first);
			bwdTree.getShortestPathTo(sp_roads, node_iter->first);
			patMultiModalPath sp(sp_roads);
			double cost = m_MHWeight->logWeigthOriginal(sp);
			if (!isfinite(cost)) {
				stringstream ss;
				ss << "MHNIPPG: wrong cost";
				ss << node_iter->first->getUserId();
				throw RuntimeException(ss.str().c_str());

			}
//			double cost = node_iter->second + bc_iter->second;
			minCost = cost < minCost ? cost : minCost;
			maxCost = cost > maxCost ? cost : maxCost;
			proposal_probabilities[node_iter->first] = cost;

		}
		else{
			if(bc_iter == bwdCost.end() ){
//				cout<<"bwd not found"<<endl;
				invalid_bwd_connection+=1;
			}
			else if( !isfinite(bc_iter->second )){
				cout <<"bwd DBL_MAX"<<endl;
				invalid_bwd_connection+=1;
			}
			else if(!isfinite(node_iter->second)){
//				cout<<"fwd DBL_MAX"<<endl;
				invalid_fwd_connection+=1;
			}
		}
		//TODO add obs weight
	}
	cout<< origin->getUserId() << "-"
			<< destination->getUserId()<<"invalid connections: fwd: "<<invalid_fwd_connection<<", bwd: "<<invalid_bwd_connection<<endl;
	double weightSum = 0.0;

	bool use_min = true;
	cout <<"MHNIPPG"<< origin->getUserId() << "-"
			<< destination->getUserId()<< minCost << "-" << maxCost << endl;
//	if (minCost > 0) {
//		use_min = false;
//	}
	for (unordered_map<const patNode*, double>::iterator node_iter =
			proposal_probabilities.begin();
			node_iter != proposal_probabilities.end(); ++node_iter) {
		double weight;
		if (use_min) {
			weight = exp(node_iter->second - minCost); //weight
		} else {

			weight = exp(node_iter->second - maxCost);
		}
		if (!isfinite(weight)) {
			cout  <<"MHNIPPG"<< node_iter->second << "," << minCost << endl;
		}
		weightSum += weight;
		node_iter->second = weight;
	}
	cout <<"MHNIPPG" << weightSum << endl;
	for (unordered_map<const patNode*, double>::iterator node_iter =
			proposal_probabilities.begin();
			node_iter != proposal_probabilities.end(); ++node_iter) {
//		cout << node_iter->first->getUserId() << ":" << node_iter->second << "/"
//				<< weightSum << endl;
		node_iter->second /= weightSum;
		if (isnan(node_iter->second) || !isfinite(node_iter->second)
				|| node_iter->second == 0.0) {
			stringstream ss;
			ss << "MHNIPPG:";
			ss << " nan node insertion probability"
					<< node_iter->first->getUserId() << ":" << node_iter->second
					<< "/" << weightSum << endl;
			throw RuntimeException(ss.str().c_str());
		}
	}
	cout<< origin->getUserId() << "-"
			<< destination->getUserId() << "MHNIPPG node proba size:" << proposal_probabilities.size() << endl;
	return proposal_probabilities;
	//=========END drive insertion probaiblity=======================//
}
