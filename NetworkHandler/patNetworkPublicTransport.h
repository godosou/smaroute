/*
 * patNetworkPublicTransport.h
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKPUBLICTRANSPORT_H_
#define PATNETWORKPUBLICTRANSPORT_H_
#include "patWay.h"
#include "patNode.h"
#include <vector>
#include <map>
#include "patArc.h"
#include "patPublicTransportSegment.h"
#include "patNetworkElements.h"
#include "patNetworkBase.h"

/**
 * Class for public transport routes. Abstract class for train and bus routes
 */
class patNetworkPublicTransport: public patNetworkBase {
public:
	patNetworkPublicTransport();

	/**
	 * Virtual method for getting routes from patNetworkElements::patNetworkElements() and postgresql database;
	 */

	const patNode* findNearestNode(const patNode* node,
			set<const patNode*> nodes_set);
	map<const patNode*, int> findStops(map<const patNode*, int>& stops
			,map<const patNode*, map<const patArc*, int> >& incidents);
	void getRoute(patNetworkElements* network_elements,
			vector<pair<unsigned long, string> >& way_ids
			, vector<pair<unsigned long, string> >& node_ids,
			map<patString, patString>& tags);
	void getRoutes(patNetworkElements* network, string table_name,
			patGeoBoundingBox bounding_box);
	virtual void getFromNetwork(patNetworkElements* network,
			patGeoBoundingBox bounding_box)=0;

	void getSegFromNode(patNetworkElements* network_elements,
			map<const patNode*, map<const patArc*, int> >& incidents,
			const patNode* up_node,
			patPublicTransportSegment& seg,
			map<const patNode*, int>& stops);
	/**
	 * Summarize the membership for
	 */
	void summarizeMembership();

	virtual set<const patRoadBase*> getRoadsContainArc(
			const patRoadBase* arc) const;
	virtual ~patNetworkPublicTransport();
protected:
	map<const patRoadBase*, set<const patRoadBase*> > m_pt_arc_membership; //An arc belongs to many pt segements.
};

#endif /* PATNETWORKPUBLICTRANSPORT_H_ */
