/*
 * patPathExtension.h
 *
 *  Created on: Nov 9, 2011
 *      Author: jchen
 */

#ifndef PATPATHEXTENSION_H_
#define PATPATHEXTENSION_H_
#include <set>
#include "patMultiModalPath.h"
#include "patNode.h"
#include "patNetworkBase.h"
#include "patNetworkEnvironment.h"
#include "patGpsDDR.h"
#include "patMeasurementDDR.h"
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
	set<patMultiModalPath> extendFromNode(const set<const patMultiModalPath*>* up_streams, const patNode* connect_node, const set<patMultiModalPath>* down_streams, patMeasurementDDR* ddr);

	/**
	 * Build initiate paths by creating a path for each arc.
	 */
	set<patMultiModalPath> init(const map<const patArc*, double>* ddr_arcs);

	virtual ~patPathExtension();
protected:
	const patNetworkBase* m_network;
	//const patNetworkEnvironment* m_network_environment;
};

#endif /* PATPATHEXTENSION_H_ */
