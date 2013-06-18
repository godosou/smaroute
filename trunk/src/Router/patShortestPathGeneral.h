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
#include "patArc.h"
#include "patShortestPathTreeGeneral.h"
class patMeasurementDDR;
class patGpsDDR;
class patShortestPathGeneral {
public:
	patShortestPathGeneral(patNetworkBase* a_network);

	/**
	 * Compute the shortest path tree for single source;
	 * Simply call to multiple source version;
	 * @return true if successful; false otherwise.
	 */
	bool buildShortestPathTree(const patNode* root_node, double ceil);

	/**
	 * Build the shortest path tree for multiple sources;
	 * @return false if negative cycle detected; true otherwise.
	 */
	bool buildShortestPathTree(const patNode* root_node, patMeasurementDDR* gps_ddr ,
			set<pair<const patArc*, const patRoadBase*> >* ddr_arcs
			, double ceil );
	/**
	 * Build the shortest path tree for multiple sources;
	 * @return false if negative cycle detected; true otherwise.
	 */
	bool buildShortestPathTree(set<const patNode*> root_nodes, patMeasurementDDR* gps_ddr =
			NULL, set<pair<const patArc*, const patRoadBase*> >* ddr_arcs= NULL
			, double ceil = DBL_MAX);

	patShortestPathTreeGeneral* getTree();

	virtual ~patShortestPathGeneral();

protected:
	patNetworkBase* network;
	double minimum_label;
	patShortestPathTreeGeneral shortest_path_tree;
	deque<const patNode*> m_list_of_nodes;
};

#endif /* PATSHORTESTPATHGENERAL_H_ */
