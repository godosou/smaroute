/*
 * MHPath.cc
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#include "MHPath.h"

MHPath::MHPath() :
		m_cost(0.0), m_spliceable(-1), m_points(0, 0, 0), m_modified_fwd_cost(
				FWD), m_modified_bwd_cost(BWD) {

}

MHPath::MHPath(const patMultiModalPath& path, const MHPoints& points,
		const patRouter* router) :
		m_cost(0.0), m_spliceable(-1), m_points(points), m_router(router), m_modified_fwd_cost(
				FWD), m_modified_bwd_cost(BWD) {
	append(path);
}
void MHPath::setRouter(const patRouter* router) {
	m_router = router;
}

MHPath::~MHPath() {
}

patShortestPathTreeGeneral MHPath::modifiedFwdCost() {
	if (m_modified_fwd_cost.empty()) {
//		DEBUG_MESSAGE("foward cost");
		//Exclude nodes not between A and C
		set<const patNode*> excluded_nodes;
		list<pair<const patArc*, TransportMode> > a_arcs = getArcsWithMode();
		list<pair<const patArc*, TransportMode> >::const_iterator a_arc_iter =
				a_arcs.begin();
		for (int i = 0; i < m_points.getA(); ++i) {

			excluded_nodes.insert(a_arc_iter->first->getUpNode());
			++a_arc_iter;
		}
		//DEBUG_MESSAGE(excluded_nodes.size());
		a_arc_iter = a_arcs.end();
		for (int i = 0; i < size() - m_points.getC() + 1; ++i) {
			--a_arc_iter;
			excluded_nodes.insert(a_arc_iter->first->getDownNode());
		}
//		DEBUG_MESSAGE(
//				"build forward router excluds nodes" << excluded_nodes.size());

		m_modified_fwd_cost = m_router->fwdCostWithoutExcludedNodes(getNodeA(),
				m_router->getNetwork()->getNodes(), excluded_nodes, NULL);
	}
	return m_modified_fwd_cost;
}

patShortestPathTreeGeneral MHPath::modifiedBwdCost() {
	if (m_modified_bwd_cost.empty()) {

		//Exclude nodes not between A and C

		set<const patNode*> excluded_nodes;
		list<pair<const patArc*, TransportMode> > a_arcs = getArcsWithMode();
		list<pair<const patArc*, TransportMode> >::const_iterator a_arc_iter =
				a_arcs.begin();
		for (int i = 0; i < m_points.getA() + 1; ++i) {
			excluded_nodes.insert(a_arc_iter->first->getUpNode());
			++a_arc_iter;
		}
		a_arc_iter = a_arcs.end();
		for (int i = 0; i < size() - m_points.getC(); ++i) {
			--a_arc_iter;
			excluded_nodes.insert(a_arc_iter->first->getDownNode());
		}

//		DEBUG_MESSAGE(
//				"build backward router excluds nodes" << excluded_nodes.size());
		m_modified_bwd_cost = m_router->bwdCostWithoutExcludedNodes(getNodeC(),
				m_router->getNetwork()->getNodes(), excluded_nodes, NULL);

	}
	return m_modified_bwd_cost;
}
const patNode* MHPath::getNodeA() const {
	return getNode(m_points.getA());
}

const patNode* MHPath::getNodeB() const {
	return getNode(m_points.getB());
}

const patNode* MHPath::getNodeC() const {
	return getNode(m_points.getC());
}

unsigned long MHPath::pointCombinationSize() const {
	return 1 * (size() + 1) * (size()) * (size() - 1) / 6;
}

void MHPath::setPoints(MHPoints points) {

	if (points.getA() != m_points.getA() || points.getC() != m_points.getC()) {
		m_modified_fwd_cost.clear();
		m_modified_bwd_cost.clear();
	}
	m_points = points;
	m_spliceable = -1;
}

bool MHPath::equalsSubPath(patMultiModalPath& b_path, int start,
		int end) const {
	list<pair<const patArc*, TransportMode> > b_arcs = b_path.getArcsWithMode();
	list<pair<const patArc*, TransportMode> > a_arcs = getArcsWithMode();

	int b_size = b_arcs.size();
	int a_size = end-start;
	if (b_size != a_size) {
//		DEBUG_MESSAGE("size different"<<b_size<<","<<a_size);
		return false;
	}

	list<pair<const patArc*, TransportMode> >::const_iterator a_arc_iter =
			a_arcs.begin();
	list<pair<const patArc*, TransportMode> >::const_iterator b_arc_iter =
			b_arcs.begin();
	for (int i = 0; i < start; ++i) {
		++a_arc_iter;
	}
	for (int i = 0; i < end - start; ++i) {
		if (*a_arc_iter != *b_arc_iter) {
			return false;
		}
		++a_arc_iter;
		++b_arc_iter;
	}
	return true;
}

bool MHPath::isSpliceable() {
//	DEBUG_MESSAGE(size()<<":"<<m_points);
	if (m_spliceable == -1) {
		patMultiModalPath path_AB = fwdRoute(getNodeA(), getNodeB());
		if (!equalsSubPath(path_AB, m_points.getA(), m_points.getB())) {
			m_spliceable = 0;
			return false;
		} else {

			patMultiModalPath path_BC = bwdRoute(getNodeB(), getNodeC());
			bool sp = equalsSubPath(path_BC, m_points.getB(), m_points.getC());
			if (sp == true) {
				m_spliceable = 1;
			} else {
				m_spliceable = 0;
			}
		}
	}
	if (m_spliceable == 1) {
		return true;
	} else {
		return false;
	}
}
map<const patNode*, double> MHPath::getInsertProbs(
		const double detour_cost_scale) {
	/*
	 * (1) obtain reduced fwd and bwd tree cost
	 */
	map<const patNode*, double> fwd_cost = modifiedFwdCost().getLabels();
	map<const patNode*, double> bwd_cost = modifiedBwdCost().getLabels();
	/*
	 * (2) define set of candidate nodes
	 */
	set<const patNode*> candidate_nodes;
	// TODO (above) why linked?
	for (map<const patNode*, double>::const_iterator f_c_iter =
			fwd_cost.begin(); f_c_iter != fwd_cost.end(); ++f_c_iter) {
		map<const patNode*, double>::const_iterator find_b_c = bwd_cost.find(
				f_c_iter->first);
		if (find_b_c!=bwd_cost.end () &&find_b_c->second < DBL_MAX && f_c_iter->second < DBL_MAX) {
			candidate_nodes.insert(f_c_iter->first);
		}
	}

	// TODO only nodes A and C can possibly be in the candidate node set
	for (int i = 0; i <= m_points.getA(); i++) {
		candidate_nodes.erase(getNode(i));
	}
	for (int i = m_points.getC(); i < size() + 1; i++) {
		candidate_nodes.erase(getNode(i));
	}

	/*
	 * (3.1) initialize result data structure
	 */
	map<const patNode*, double> insert_probas;
	if (candidate_nodes.empty()) {
		return insert_probas;
	}
	/*
	 * (3.2) put costs into result
	 */
	double min_cost = DBL_MAX;
	for (set<const patNode*>::iterator c_n_iter = candidate_nodes.begin();
			c_n_iter != candidate_nodes.end(); ++c_n_iter) {
		double cost = fwd_cost[*c_n_iter];
		insert_probas[*c_n_iter] = cost;
		min_cost = min_cost < cost ? min_cost : cost;
	}

	/*
	 * (3.3) put exp(-detourScale * (cost - minCost)) into result
	 */
	double exp_sum = 0.0;
	for (set<const patNode*>::iterator c_n_iter = candidate_nodes.begin();
			c_n_iter != candidate_nodes.end(); ++c_n_iter) {

		double val = exp(
				-detour_cost_scale * (insert_probas[*c_n_iter] - min_cost));
		insert_probas[*c_n_iter] = val;
		exp_sum+=val;

	}
	/*
	 * (4) normalize result
	 */
	for (set<const patNode*>::iterator c_n_iter = candidate_nodes.begin();
			c_n_iter != candidate_nodes.end(); ++c_n_iter) {
		insert_probas[*c_n_iter] = insert_probas[*c_n_iter] / exp_sum;
	}

	return insert_probas;
}

patMultiModalPath MHPath::getForwardShortestPath(const patNode* A,
		const patNode* B) const {

}
patMultiModalPath MHPath::getBackwardShortestPath(const patNode* A,
		const patNode* B) const {

}

patMultiModalPath MHPath::fwdRoute(const patNode* origin,
		const patNode* destination) {
	return m_router->bestRouteFwd(origin, destination, &modifiedFwdCost());
}

patMultiModalPath MHPath::bwdRoute(const patNode* origin,
		const patNode* destination) {
	return m_router->bestRouteBwd(origin, destination, &modifiedBwdCost());
}

void MHPath::insertDetour(const patNode* nodeB) {
	/*
	 * (1) compute new path segments
	 */

	patMultiModalPath pathAB = fwdRoute(getNodeA(), nodeB);
	patMultiModalPath pathBC = bwdRoute(nodeB, getNodeC());

//	DEBUG_MESSAGE(pathAB.size()<<","<<pathBC.size());
	/*
	 * (2) build new path
	 */
	patMultiModalPath new_path;
//	DEBUG_MESSAGE(
//			nbrOfNodes() << ":" << m_points.getA() << "," << m_points.getB()
//					<< "," << m_points.getC());
	if (!new_path.append(getSubPathWithNodesIndecis(0, m_points.getA()))) {
		WARNING("WRONG PATH 0A"); //TODO check the append check the points inex
		throw RuntimeException("Wrong path 0A");
	}
	//DEBUG_MESSAGE("CORRECT PATH 0A, "<<new_path.size());
	if (!new_path.append(pathAB)) {
		WARNING("WRONG PATH AB");
		throw RuntimeException("Wrong path AB");
	}
//	DEBUG_MESSAGE("CORRECT PATH AB, "<<new_path.size());
	if (!new_path.append(pathBC)) {
		WARNING("WRONG PATH BC");
		throw RuntimeException("Wrong path BC");
	}
	//DEBUG_MESSAGE("CORRECT PATH BC, "<<new_path.size());
	if (!new_path.append(getSubPathWithNodesIndecis(m_points.getC(), size()))) {
		WARNING("WRONG PATH C1");
		throw RuntimeException("Wrong path C1");
	}
	//DEBUG_MESSAGE("CORRECT PATH C1, "<<new_path.size());
	//CHECK
	//DEBUG_MESSAGE("new path created");
//	newNodes.addAll(this.nodes.subList(0, this.points.getA()));
//	newNodes.addAll(pathAB);
//	newNodes.addAll(pathBC.subList(1, pathBC.size() - 1));
//	newNodes.addAll(this.nodes.subList(this.points.getC(), this.nodes
//			.size()));

	/*
	 * (3) update indices
	 */

//TODO
	int newB = m_points.getA() + pathAB.nbrOfNodes() - 1;
	int newC = newB + pathBC.nbrOfNodes() - 1;

	MHPoints new_points(m_points.getA(), newB, newC);
	/*
	 * (4) clear/update internal cache
	 */

	m_spliceable = 1;
//	this.links = null;
//	this.cost = null;
//	this.spliceable = true; // because this was created by a splice
//	/*
//	 * (5) update further data structures
//	 */

	clear();
	append(new_path);
	m_points = new_points;
//	this.nodes.clear();
//	this.nodes.addAll(newNodes);
//	this.points = newPoints;
}

/**
 * Path representation for the Metropolis-Hastings <code>PathGenerator</code>.
 *
 * @author Gunnar Flötteröd
 *
 */
//public class MHPath {
//
//// -------------------- CONSTANTS -------------------
//
//	double getCost() {
//		if (this.cost == null) {
//			this.cost = m_router->cost(this.getLinks());
//		}
//		return this.cost;
//	}
// -------------------- CONSTRUCTION --------------------
// -------------------- GETTERS --------------------
// -------------------- PATH MANIPULATIONS --------------------
int MHPath::getA() const {
	return m_points.getA();
}
int MHPath::getB() const {
	return m_points.getB();
}
int MHPath::getC() const {
	return m_points.getC();
}
