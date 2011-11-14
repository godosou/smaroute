/*
 * patShortestPathGeneral.h
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#ifndef PATSHORTESTPATHGENERAL_H_
#define PATSHORTESTPATHGENERAL_H_

#include <set>
#include <deque>

#include "patNetworkBase.h"
#include "patNode.h"
#include "patGpsDDR.h"
#include "patArc.h"
#include "patShortestPathTreeGeneral.h"

class patShortestPathGeneral {
public:
	patShortestPathGeneral(patNetworkBase* a_network, double the_min_label);

	/**
	 * Compute the shortest path tree for single source;
	 * Simply call to multiple source version;
	 * @return true if successful; false otherwise.
	 */
	bool buildShortestPathTree(patNode* root_node, double ceil);

	/**
		 * Build the shortest path tree for multiple sources;
		 * @return false if negative cycle detected; true otherwise.
	*/
	bool buildShortestPathTree(set<patNode*> root_nodes, patGpsDDR* gps_ddr, set<patArc*> ddr_arcs,
				double ceil);


	patShortestPathTreeGeneral* getTree() {
		return shortest_path_tree;
	}

	virtual ~patShortestPathGeneral();

protected:
	patNetworkBase* network;
	double minimum_label;
	patShortestPathTreeGeneral shortest_path_tree;
	deque<patNode*> list_of_nodes;
};

#endif /* PATSHORTESTPATHGENERAL_H_ */
