/*
 * patSimulateProbabilisticPaths.cc
 *
 *  Created on: Jun 8, 2012
 *      Author: jchen
 */

#include "patSimulateProbabilisticPaths.h"
#include "patNetworkBase.h"
#include "patMultiModalPath.h"
#include "patRouter.h"
#include "MHPath.h"
#include "MHPoints.h"
#include "patRandomNumber.h"
#include "patNode.h"
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include "patSampleDiscreteDistribution.h"
using namespace std::tr1;
patSimulateProbabilisticPaths::patSimulateProbabilisticPaths(
		const patNetworkBase* network, const patRouter* router,
		const patRandomNumber* rnd) :
		m_rnd(rnd), m_router(router), m_network(network), m_distance_scale(
				patNBParameters::the()->ObsErrorDistanceScale), m_obs_error_distance(
				patNBParameters::the()->ObsErrorDistance) {

}
MHPoints patSimulateProbabilisticPaths::drawPoints(int n) const {
	/*
	 * (1) check
	 */
	if (n < 3) {
		throw RuntimeException("less than three alternatives!");
	}
	/*
	 * (2) draw three disjoint numbers
	 */
	int u1 = m_rnd->nextInt(n);
	int u2 = m_rnd->nextInt(n - 1);
	if (u2 >= u1) {
		u2++;
	}
	int u3 = m_rnd->nextInt(n - 2);
	if (u3 >= u1 || u3 >= u2) {
		u3++;
	}
	if (u3 >= u1 && u3 >= u2) {
		u3++;
	}
//	DEBUG_MESSAGE(u1 << "," << u2 << "," << u3);
	/*
	 * (3) return sorted numbers
	 */
	vector<int> us;
	us.push_back(u1);
	us.push_back(u2);
	us.push_back(u3);
	sort(us.begin(), us.end());
	MHPoints new_points(us[0], us[1], us[2]);
	return new_points;
}

map<patMultiModalPath, double> patSimulateProbabilisticPaths::run(
		const patMultiModalPath& path, const unsigned int& count,
		const double& error) const {
	map<patMultiModalPath, double> simulated_paths;

	double total_simulated_proba = 0.0;

	unsigned int i = 0;
	while (i < count) {

		MHPath new_path(path, drawPoints(path.nbrOfNodes()), m_router);

		unordered_map<const patNode*, double> nearby_nodes =
				m_network->getNearbyNodes(new_path.getNodeB(),
						m_obs_error_distance); //TODO 200m as radius, include it in patNBParameters
		vector<const patNode*> nodes;
		vector<double> probas;

		for (unordered_map<const patNode*, double>::const_iterator node_iter =
				nearby_nodes.begin(); node_iter != nearby_nodes.end();
				++node_iter) {
			nodes.push_back(node_iter->first);
			double cost = -m_distance_scale * node_iter->second;
			cout << cost << endl;
			probas.push_back(cost);

		}
		cout << endl;

		for (unsigned i = 0; i < probas.size(); ++i) {
			probas[i] = exp(probas[i]); //TODO check the scale
		}

		unsigned int sampled = patSampleDiscreteDistribution()(probas, *m_rnd);
		const patNode* insert_node = nodes[sampled];
		if (new_path.containsNodeFront(insert_node, new_path.getA())
				|| new_path.containsNodeBack(insert_node,
						new_path.size() - new_path.getC())) {

			unsigned int sampled = patSampleDiscreteDistribution()(probas,
					*m_rnd);

			const patNode* insert_node = nodes[sampled];
		}

		if (new_path.insertDetour(insert_node, nearby_nodes)
				&& new_path != path) {

			patMultiModalPath simulated_path(new_path);
			simulated_paths[simulated_path] = probas[sampled];
			total_simulated_proba += probas[sampled];
			++i;
		}
	}
	for (map<patMultiModalPath, double>::iterator path_iter =
			simulated_paths.begin(); path_iter != simulated_paths.end();
			++path_iter) {
		path_iter->second *= error / total_simulated_proba;
		cout << "\t path proba: " << path_iter->second << endl;
	}
	cout << endl;
	return simulated_paths;
}
patSimulateProbabilisticPaths::~patSimulateProbabilisticPaths() {
	// TODO Auto-generated destructor stub
}

