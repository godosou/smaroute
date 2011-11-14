/*
 * patPathExtension.h
 *
 *  Created on: Nov 9, 2011
 *      Author: jchen
 */

#ifndef PATPATHEXTENSION_H_
#define PATPATHEXTENSION_H_
#include <set>
#include "patPathJ.h"
#include "patNode.h"
#include "patNetworkBase.h"
/**
 * A connection of methods for path extension.
 */
class patPathExtension {
public:
	patPathExtension(const patNetworkBase* the_network);

	/**
	 * Extend from a node with upstream paths and downstream paths.
	 * @param up_streams: upstream paths;
	 * @param connect_node: connection node;
	 * @param down_streams: downstream paths;
	 * @param ddr: the ddr of the current gps point;
	 * @return a set of connected paths.
	 */
	set<patPathJ> extendFromNode(const set<patPathJ>* up_streams, const patNode* connect_node, const set<patPathJ>* down_streams, patGpsDDR* ddr);

	/**
	 * Build initiate paths by connecting arcs in ddr.
	 */
	set<patPathJ> init(patGpsDDR* ddr);

	void assignMode();
	virtual ~patPathExtension();
protected:
	const patNetworkBase* network;
};

#endif /* PATPATHEXTENSION_H_ */
