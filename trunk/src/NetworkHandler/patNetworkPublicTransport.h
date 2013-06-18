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
#include "patArcSequence.h"
#include <list>
/**
 * Class for public transport routes. Abstract class for train and bus routes
 */
class patNetworkPublicTransport: public patNetworkBase {
public:
	patNetworkPublicTransport();


	int getDirection(string direction_string);
	bool findArcInForbidenList(list<map<const patArc*, const patNode*> >& arcs_to_stop,const patArc* arc);
	map<string, map< const patArc*, const patNode*> > findStopsIncomingLinks(map<const patNode*, int>& stops,
																				map<const patNode*, list< const patArc*> >& outgoing_incidents,
																				map<const patNode*, list< const patArc*> >& incoming_incidents,
																				set<const patArc*>& forward_arcs,
																				set<const patArc*>& backward_arcs);
	map<const patNode*, map<const patNode*, patArcSequence*> > getRouteWaysOutgoingIncidents(list<patArcSequence>& ways,bool outgoing);
	list<patArcSequence> getRouteWays(
			patNetworkElements* network_elements,
			vector<pair<unsigned long, string> >& way_ids);
	void getDownStream(
			map<const patNode*, map<const patNode*, patArcSequence*> >& incidents,
			list<patArcSequence*>& line
			, const patNode* up_up_node,const patNode* up_node, const patNode* origin,bool forward);

	const patNode* findNearestNode(const patNode* node,
								   set<const patNode*> nodes_set);
	map<const patNode*,int>  getStops(patNetworkElements* network_elements,
			vector<pair<unsigned long, string> >& node_ids);
	map<const patNode*, int> findStops(map<const patNode*, int>& stops
			,map<const patNode*, map<const patArc*, int> >& incidents);
	void getRoute(patNetworkElements* network_elements,
			vector<pair<unsigned long, string> >& way_ids
			, vector<pair<unsigned long, string> >& node_ids,
			unordered_map<string, string>& tags);
	void getRoutes(patNetworkElements* network, string table_name,
			patGeoBoundingBox bounding_box);
	virtual void getFromNetwork(patNetworkElements* network,
			patGeoBoundingBox bounding_box)=0;

	int findDirection(map<string, map<string, int> >& stop_direction
			, string up_stop , string down_stop);

	map<string, map<string, int> > buildDirection(
			map<pair<string, string> , map<int, patPublicTransportSegment> >& pt_incidents);
	void getSegFromNode(
			patNetworkElements* network_elements,
			map<const patNode*, list<const patArc*> >& incidents
			,
			const patNode* up_node
			,
			const patArc* incoming_arc,
			patPublicTransportSegment& seg
			,
			map<string, map< const patArc*, const patNode*> >& stops
			,
			list<map<const patArc*, const patNode*> >& arcs_to_stop,
			list<string>& stop_names,
			map<pair<string, string> , set< patPublicTransportSegment> >& pt_incidents) ;
	/**
	 * Summarize the membership for
	 */
	void summarizeMembership();
	bool isStop(const patNode* node) const;
	virtual set<const patRoadBase*> getRoadsContainArc(
			const patRoadBase* arc) const;
	virtual ~patNetworkPublicTransport();
	set<const patRoadBase*> getPTSegsContainArc(const patRoadBase* arc) const;
	set<const patNode*> getStops() const;
	void walkFromToStops(
			patNetworkElements* network_elements, patNetworkBase* walk_network) ;
void walkOnTrack(
		patNetworkElements* network_elements, patNetworkBase* walk_network) const;
protected:
	map<const patRoadBase*, set<const patRoadBase*> > m_pt_arc_membership; //An arc belongs to many pt segements.
	map<const patNode*, set<const patRoadBase*> > m_pt_outgoing_incidents;
	set<const patNode*> m_stops;
};

#endif /* PATNETWORKPUBLICTRANSPORT_H_ */
