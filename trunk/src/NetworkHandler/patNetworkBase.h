/*
 * patNetworkBase.h
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKBASE_H_
#define PATNETWORKBASE_H_
#include <tr1/unordered_map>
#include <tr1/unordered_set>

//#include <unordered_map>
#include <set>
#include <list>
#include "patNode.h"
#include "patRoadBase.h"
#include "patArc.h"
#include "patTransportMode.h"
#include "patGeoBoundingBox.h"
#include "patNetworkElements.h"
class patMultiModalPath;
using namespace std::tr1;
class patNetworkBase {
public:
	patNetworkBase();
	patNetworkBase(const patNetworkBase& other);
	virtual patNetworkBase* clone() const=0;

	virtual patNetworkBase* getSubNetwork(const patGeoBoundingBox& bb) const;
	virtual ~patNetworkBase();
	virtual void walkFromToStops(patNetworkElements* network_elements,
			patNetworkBase* walk_network)=0;
	virtual void walkOnTrack(patNetworkElements* network_elements,
			patNetworkBase* walk_network) const = 0;

	/**
	 * Get the pointer to the outgoing incidents.
	 */
	const unordered_map<const patNode*, set<const patRoadBase*> >* getOutgoingIncidents() const;
	const set<const patRoadBase*> getOutgoingRoads(const patNode* node) const;
	const unordered_map<const patNode*, set<const patRoadBase*> >* getIncomingIncidents() const;
	virtual bool isStop(const patNode* node) const = 0;

	bool isPT() const;
	/**
	 * Determine if a node has downstream roads or not.
	 */
	bool hasDownStream(const patNode * const a_node) const;

	/**
	 * Get the number of conjunction nodes
	 */
	unsigned long getNodeSize() const;

	/**
	 * Get the transport mode;
	 *
	 */
	TransportMode getTransportMode() const;

	/**
	 * Read transport network with network elements
	 * @param network the network elements.
	 */
	virtual void getFromNetwork(patNetworkElements* network,
			patGeoBoundingBox bounding_box)=0;
	double computeMinimumLabel();
	double getMinimumLabel() const;
	void finalizeNetwork();
	void buildIncomingIncidents();
	/**
	 * Get the roads that contain a gien arc.
	 * @param arc the arc to be searched
	 * @return set<patRoadBase*>  a set of roads.
	 */
	virtual set<const patRoadBase*> getRoadsContainArc(
			const patRoadBase* arc) const = 0;
	bool exportShpFiles(const string file_path) const;

	void exportCadytsOSM(const string file_path) const;
	virtual double getMinSpeed() const = 0;
	virtual double getMaxSpeed() const = 0;

	set<const patArc*> getAllArcs() const;
	void exportKML(const string file_path) const;
	void addArc(const patArc* arc);

	set<const patNode*> getNodes() const;

	void removeNode(const patNode* node);

	NODE_STATUS checkNodeStatus(const patNode* node) const;

	void setOutgoingIncidents(
			unordered_map<const patNode*, set<const patRoadBase*> >& oi) {
		m_outgoing_incidents = oi;
	}

	const patNode* getNearestNode(const patCoordinates* geo) const;
	unordered_map<const patNode*, double> getNearbyNodes(const patNode* center,
			const double& distance) const;

	patNetworkBase(string network_type, TransportMode transport_mode,
			unordered_map<const patNode*, set<const patRoadBase*> >& outgoing_incidents);

	patMultiModalPath recoverPath(const patMultiModalPath& path) const;

	void assignRoadBaseId();

	const list< int> findPrevRoadIds(const patNode* node) const;
	const list< int> findNextRoadIds(const patNode* node) const;
	const patRoadBase* findOpposite(const patRoadBase* road) const;
	const int getRoadId(const patRoadBase* road) const;

	const unordered_map<const patRoadBase*, const int>* getAllRoads() const;
protected:


	unordered_map<const patRoadBase*, const int> m_road_id;
	unordered_map<const patNode*, set<const patRoadBase*> > m_outgoing_incidents;
	unordered_map<const patNode*, set<const patRoadBase*> > m_incoming_incidents;
	set<const patNode*> m_nodes;
	double m_minimum_label;
	string m_network_type;
	TransportMode m_transport_mode;
};

#endif /* PATNETWORKBASE_H_ */
