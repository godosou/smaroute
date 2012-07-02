/*
 * patSimulateProbabilisticPaths.cc
 *
 *  Created on: Jun 8, 2012
 *      Author: jchen
 */

#include "patSimulateProbabilisticPaths.h"

#include "patMultiModalPath.h"
#include "patRouter.h"
#include "MHPath.h"
#include "MHPoints.h"
#include "patRandomNumber.h"
#include "patNode.h"
#include <tr1/unordered_map>
using namespace std::tr1;
patSimulateProbabilisticPaths::patSimulateProbabilisticPaths(
		const patMultiModalPath& path, const patRouter* router) :
		m_path(path), m_router(router) {

}
MHPoints patSimulateProbabilisticPaths::drawPoints(int n) {
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
map<patMultiModalPath, double> patSimulateProbabilisticPaths::run(unsigned int count, double error) {
	map<patMultiModalPath, double> simulated_paths;
	double total_simulated_proba=0.0;
    //TODO
//	for (unsigned int i = 0; i < count; ++i) {
//		MHPath new_path(m_path, drawPoints(m_path.size() + 1), m_router);
//
//		unordered_map<const patNode*, double> node_insert_probas =
//				new_path.getInsertProbs( 3.0);//detour_cost_scale=3.0
//		double threshold = m_rnd->nextDouble();
//		double sum = 0.0;
//
//		const patNode* insert_node;
//		double simulated_proba = error / (double) count;
//
//		for (unordered_map<const patNode*, double>::const_iterator iter =
//				node_insert_probas.begin(); iter != node_insert_probas.end();
//				++iter) {
//			sum += iter->second;
//			if (sum >= threshold) {
//				insert_node = iter->first;
//				simulated_proba = iter->second;
//				total_simulated_proba += iter->second;
//			}
//		}
//
//		new_path.insertDetour(insert_node);
//
//		patMultiModalPath simulated_path(new_path);
//
//		simulated_paths[simulated_path] = simulated_proba;
//	}
//	for (map<patMultiModalPath, double>::iterator path_iter =
//			simulated_paths.begin(); path_iter != simulated_paths.end();
//			++path_iter) {
//		path_iter->second *= error/total_simulated_proba;
//	}
	return simulated_paths;
}
patSimulateProbabilisticPaths::~patSimulateProbabilisticPaths() {
	// TODO Auto-generated destructor stub
}

