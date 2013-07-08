/*
 * patPathSimilarity.cc
 *
 *  Created on: Jul 29, 2012
 *      Author: jchen
 */

#include "patPathSimilarity.h"
#include <tr1/unordered_set>
#include <vector>
#include "patArc.h"
#include "patNode.h"
using namespace std;
using namespace std::tr1;
patPathSimilarity::~patPathSimilarity() {
	// TODO Auto-generated destructor stub
}

patPathSimilarity::patPathSimilarity(const patArcSequence& a,
		const patArcSequence& b) :
		m_a(a), m_b(b) {

}

void patPathSimilarity::getArcsSet(unordered_set<const patArc*>& a_arcs_set,
		unordered_set<const patArc*>& b_arcs_set) const {
	vector<const patArc*> a_arcs = m_a.getArcList();
	vector<const patArc*> b_arcs = m_b.getArcList();
	for (vector<const patArc*>::const_iterator a_iter = a_arcs.begin();
			a_iter != a_arcs.end(); ++a_iter) {
		a_arcs_set.insert(*a_iter);
	}

	for (vector<const patArc*>::const_iterator b_iter = b_arcs.begin();
			b_iter != b_arcs.end(); ++b_iter) {
		b_arcs_set.insert(*b_iter);
	}

}
void patPathSimilarity::getNodesSet(unordered_set<const patNode*>& a_nodes_set,
		unordered_set<const patNode*>& b_nodes_set) const {
	vector<const patArc*> a_arcs = m_a.getArcList();
	vector<const patArc*> b_arcs = m_b.getArcList();

	a_nodes_set.insert(a_arcs.front()->getUpNode());
	b_nodes_set.insert(b_arcs.front()->getUpNode());
	for (vector<const patArc*>::const_iterator a_iter = a_arcs.begin();
			a_iter != a_arcs.end(); ++a_iter) {
		a_nodes_set.insert((*a_iter)->getDownNode());
	}

	for (vector<const patArc*>::const_iterator b_iter = b_arcs.begin();
			b_iter != b_arcs.end(); ++b_iter) {
		b_nodes_set.insert((*b_iter)->getDownNode());
	}

}

double patPathSimilarity::computeArcSim() const {
	unordered_set<const patArc*> a_arcs_set;
	unordered_set<const patArc*> b_arcs_set;
	getArcsSet(a_arcs_set, b_arcs_set)
	unsigned duplicates = 0;
	for (unordered_set<const patArc*>::const_iterator a_iter =
			a_arcs_set.begin(); a_iter != a_arcs_set.end(); ++a_iter) {
		if (b_arcs_set.find(*a_iter) != b_arcs_set.end()) {
			++duplicates;
		}
	}
	return (double) 2.0 * duplicates / (a_arcs_set.size() + b_arcs_set.size());
}
double patPathSimilarity::computeNodeSim() const {

	unordered_set<const patNode*> a_nodes_set;
	unordered_set<const patNode*> b_nodes_set;

	getNodesSet(a_nodes_set, b_nodes_set);
	unsigned duplicates = 0;
	for (unordered_set<const patNode*>::const_iterator a_iter =
			a_nodes_set.begin(); a_iter != a_nodes_set.end(); ++a_iter) {
		if (b_nodes_set.find(*a_iter) != b_nodes_set.end()) {
			++duplicates;
		}
	}
	return (double) 2.0 * duplicates / (a_nodes_set.size() + b_nodes_set.size());

}

double patPathSimilarity::computeDistance() const {

	unordered_set<const patNode*> a_nodes_set;
	unordered_set<const patNode*> b_nodes_set;

	getNodesSet(a_nodes_set, b_nodes_set);
	double distance = 0.0;
	for (unordered_set<const patNode*>::const_iterator a_iter =
			a_nodes_set.begin(); a_iter != a_nodes_set.end(); ++a_iter) {

		if (b_nodes_set.find(*a_iter) != b_nodes_set.end()) {
			continue;
		}

		double min_dist_node = DBL_MAX;
		for (unordered_set<const patNode*>::const_iterator b_iter =
				b_nodes_set.begin(); b_iter != b_nodes_set.end(); ++b_iter) {
			double dist_node = (*a_iter)->distanceTo(*b_iter);
			min_dist_node = dist_node<min_dist_node?dist_node:min_dist_node
		}
		distance+=min_dist_node;
	}
	return distance;
}
