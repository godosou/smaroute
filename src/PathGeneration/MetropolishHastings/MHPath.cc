/*
 * MHPath.cc
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#include "MHPath.h"
#include "patTimeFunctions.h"
MHPath::MHPath() :
		m_cost(0.0), m_spliceable(-1), m_points(0, 0, 0) {
}

MHPath::MHPath(const patMultiModalPath& path, const MHPoints& points,
		const patRouter* router) :
		m_cost(0.0), m_spliceable(-1), m_points(points), m_router(router), patMultiModalPath::patMultiModalPath(
				path) {
}
void MHPath::setRouter(const patRouter* router) {
	m_router = router;
}

MHPath::~MHPath() {
}
int MHPath::getA() const {
	return m_points.getA();
}
int MHPath::getB() const {
	return m_points.getB();
}
int MHPath::getC() const {
	return m_points.getC();
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
	double nbr_of_nodes = size() + 1;
	return (double) 1.0 * ((double) nbr_of_nodes)
			* ((double) nbr_of_nodes - 1.0) * ((double) nbr_of_nodes - 2.0)
			/ 6.0;
}

void MHPath::setPoints(MHPoints points) {
	m_points = points;
	m_spliceable = -1;
}

deque<const patNode*> MHPath::getOrderedNodes(int start, int end,
		Direction direct) const {

	deque<const patNode*> nodes;

	vector<const patArc*>::const_iterator a_arc_iter = m_arcs.begin();
	a_arc_iter += start;
	if (direct == FWD) {
		nodes.push_back((*a_arc_iter)->getUpNode());
	} else {
		nodes.push_front((*a_arc_iter)->getUpNode());
	}
	for (int i = 0; i < end - start; ++i) {
		if (direct == FWD) {
			nodes.push_back((*a_arc_iter)->getDownNode());
		} else {
			nodes.push_front((*a_arc_iter)->getDownNode());
		}
		++a_arc_iter;
	}
	return nodes;
}
bool MHPath::equalsSubPath(patMultiModalPath& b_path, int start,
		int end) const {
	const vector<const patArc*>& b_arcs = b_path.getArcsPointer();

	int b_size = b_arcs.size();
	int a_size = end - start;
	if (b_size != a_size) {
		//		DEBUG_MESSAGE("size different"<<b_size<<","<<a_size);
		return false;
	}

	vector<const patArc*>::const_iterator a_arc_iter = m_arcs.begin();
	vector<const patArc*>::const_iterator b_arc_iter = b_arcs.begin();
	a_arc_iter += start;
	//	for (int i = 0; i < start; ++i) {
	//		++a_arc_iter;
	//	}
	for (int i = 0; i < end - start; ++i) {
		if (*a_arc_iter != *b_arc_iter) {
			return false;
		}
		++a_arc_iter;
		++b_arc_iter;
	}
	return true;
}

bool MHPath::checkSpliceableAB(const patNode* insertNode) const {

	set<const patNode*> excluded_nodes = getNodesFront(getA() - 1);
	set<const patNode*> excluded_nodes_2 = getNodesBack(size() - getC());
	excluded_nodes.insert(excluded_nodes_2.begin(), excluded_nodes_2.end());
	if (excluded_nodes.find(insertNode) != excluded_nodes.end()) {
		throw RuntimeException("insert node excluded");
	}
	patShortestPathTreeGeneral sp_tree(FWD);
	bool spliceable = true;
	deque<const patNode*> ordered_nodes = getOrderedNodes(m_points.getA(),
			m_points.getB(), FWD);

	m_router->fwdCostWithoutExcludedNodes(sp_tree, getNodeA(), insertNode,
			excluded_nodes, NULL, &ordered_nodes, &spliceable);
	return spliceable;
}
bool MHPath::checkSpliceableBC(const patNode* insertNode) const {

	set<const patNode*> excluded_nodes = getNodesFront(getA());
	set<const patNode*> excluded_nodes_2 = getNodesBack(size() - getC() - 1);
	excluded_nodes.insert(excluded_nodes_2.begin(), excluded_nodes_2.end());
	if (excluded_nodes.find(insertNode) != excluded_nodes.end()) {
		throw RuntimeException("insert node excluded");
	}
	patShortestPathTreeGeneral sp_tree(BWD);
	bool spliceable = true;
	deque<const patNode*> ordered_nodes = getOrderedNodes(m_points.getB(),
			m_points.getC(), BWD);
	m_router->bwdCostWithoutExcludedNodes(sp_tree, getNodeC(), insertNode,
			excluded_nodes, NULL, &ordered_nodes, &spliceable);

	return spliceable;
}
bool MHPath::isSpliceable() {
	if (m_spliceable == -1) {
//		cout << "New: check sp " << endl;
		if (!checkSpliceableAB(getNodeB())) {
			m_spliceable = 0;
		} else {
			if (checkSpliceableBC(getNodeB())) {
				m_spliceable = 1;
			} else {
				m_spliceable = 0;
			}
		}
//
////		cout << "New check done" << m_spliceable << endl;
//		bool correct;
//
//		short new_spl = m_spliceable;
////		cout << "Old: check sp " << endl;
//		patMultiModalPath path_AB = newSpliceSegmentAB(getNodeB(), correct);
////        DEBUG_MESSAGE(path_AB.size()<<","<<getA()<<","<<getB());
//		if (!equalsSubPath(path_AB, m_points.getA(), m_points.getB())) {
//			m_spliceable = 0;
//		} else {
//			patMultiModalPath path_BC = newSpliceSegmentBC(getNodeB(), correct);
//
//			bool sp = equalsSubPath(path_BC, m_points.getB(), m_points.getC());
//			if (sp == true) {
//				m_spliceable = 1;
//			} else {
//				m_spliceable = 0;
//			}
//		}
//		if (new_spl != m_spliceable) {
//			cout << "spliceable inconsistent: " << new_spl << "!="
//					<< m_spliceable << endl;
//			cout <<checkSpliceableAB(getNodeB())<<","<<checkSpliceableBC(getNodeB());
//			patMultiModalPath path_BC = newSpliceSegmentBC(getNodeB(), correct);
//			cout << equalsSubPath(path_AB, m_points.getA(), m_points.getB())
//					<< ","
//					<< equalsSubPath(path_BC, m_points.getB(), m_points.getC())
//					<< endl;
//		}
//		cout << "Old check done" << m_spliceable << endl;
	}
	if (m_spliceable == 1) {
		return true;
	} else {
		return false;
	}
}
patMultiModalPath MHPath::newSpliceSegmentAB(const patNode* insertNode,
		bool& correct) {
	set<const patNode*> excluded_nodes = getNodesFront(getA() - 1);
	set<const patNode*> excluded_nodes_2 = getNodesBack(size() - getC());
	excluded_nodes.insert(excluded_nodes_2.begin(), excluded_nodes_2.end());
	if (excluded_nodes.find(insertNode) != excluded_nodes.end()) {
		throw RuntimeException("insert node excluded");
	}
	patShortestPathTreeGeneral sp_tree(FWD);
	m_router->fwdCostWithoutExcludedNodes(sp_tree, getNodeA(), insertNode,
			excluded_nodes, NULL);
	list<const patRoadBase*> list_of_roads;
	if (sp_tree.getShortestPathTo(list_of_roads, insertNode) == true) {
		correct = true;
		return patMultiModalPath(list_of_roads);

	} else {
		correct = false;
		return patMultiModalPath();
	}
}
patMultiModalPath MHPath::newSpliceSegmentBC(const patNode* insertNode,
		bool& correct) {
	set<const patNode*> excluded_nodes = getNodesFront(getA());
	set<const patNode*> excluded_nodes_2 = getNodesBack(size() - getC() - 1);
	excluded_nodes.insert(excluded_nodes_2.begin(), excluded_nodes_2.end());
	if (excluded_nodes.find(insertNode) != excluded_nodes.end()) {
		throw RuntimeException("insert node excluded");
	}
//    DEBUG_MESSAGE(getNodeC()->getUserId()<<","<<insertNode->getUserId());

	patShortestPathTreeGeneral sp_tree(BWD);
	m_router->bwdCostWithoutExcludedNodes(sp_tree, getNodeC(), insertNode,
			excluded_nodes, NULL);
//	DEBUG_MESSAGE("sp built");
	list<const patRoadBase*> list_of_roads;
	if (sp_tree.getShortestPathTo(list_of_roads, insertNode) == true) {
		correct = true;
		return patMultiModalPath(list_of_roads);

	} else {
		correct = false;
		return patMultiModalPath();
	}

}
bool MHPath::insertDetour(const patNode* nodeB) {
	/*
	 * (1) compute new path segments
	 */
//    DEBUG_MESSAGE(getNodeA()->getUserId()<<"-"<<nodeB->getUserId());
	bool correct = true;
	patMultiModalPath pathAB = newSpliceSegmentAB(nodeB, correct);
	if (correct == false) {
		return false;
	}
//    DEBUG_MESSAGE("PATH AB");
	patMultiModalPath pathBC = newSpliceSegmentBC(nodeB, correct);
	if (correct == false) {
		return false;
	}

//    	DEBUG_MESSAGE(pathAB.size()<<","<<pathBC.size());
	/*
	 * (2) build new path
	 */
	patMultiModalPath new_path;

	if (getA() > 0) {
		if (!new_path.append(getSubPathWithNodesIndecis(0, m_points.getA()))) {
			WARNING("WRONG PATH 0A");
			//TODO check the append check the points inex
			throw RuntimeException("Wrong path 0A");
		}
	}
	//DEBUG_MESSAGE("CORRECT PATH 0A, "<<new_path.size());
	if (!new_path.append(pathAB)) {
		WARNING("WRONG PATH AB");
		throw RuntimeException("Wrong path AB");
	}
	//	DEBUG_MESSAGE("CORRECT PATH AB, "<<new_path.size());
	if (!new_path.append(pathBC)) {
		WARNING("WRONG PATH BC"<<pathAB<<","<<pathBC);
		throw RuntimeException("Wrong path BC");
	}
	//DEBUG_MESSAGE("CORRECT PATH BC, "<<new_path.size());

	if (getC() < size()) {
		if (!new_path.append(
				getSubPathWithNodesIndecis(m_points.getC(), size()))) {
			WARNING(
					"WRONG PATH C1"<<pathAB<<","<<pathBC<<","<<getSubPathWithNodesIndecis(m_points.getC(), size()));
			throw RuntimeException("Wrong path C1");
		}
	}
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

	m_spliceable = 1; // because this was created by a splice
	//	/*
	//	 * (5) update further data structures
	//	 */

	clear();
	append(new_path);
	m_points = new_points;
	return true;
}
