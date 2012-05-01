/*
 * patNetworkBase.h
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKBASE_H_
#define PATNETWORKBASE_H_
#include <map>
#include <set>
#include "patNode.h"
#include "patRoadBase.h"
#include "patArc.h"
#include "patTransportMode.h"
#include "patGeoBoundingBox.h"
#include "patNetworkElements.h"
class patNetworkBase {
public:
	patNetworkBase();
	virtual ~patNetworkBase();
	virtual void walkFromToStops(
			patNetworkElements* network_elements, patNetworkBase* walk_network)=0;
 virtual void walkOnTrack(
		patNetworkElements* network_elements, patNetworkBase* walk_network) const = 0;

	/**
	 * Get the pointer to the outgoing incidents.
	 */
	const map<const patNode*, set<const patRoadBase*> >* getOutgoingIncidents() const;
	const map<const patNode*, set<const patRoadBase*> >* getIncomingIncidents() const;
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

	virtual double getMinSpeed() const = 0;
	virtual double getMaxSpeed() const = 0;

	set<const patArc*> getAllArcs() const;
	void exportKML(const string file_path) const;
	void addArc(const patArc* arc);

	set<const patNode*> getNodes() const;

	void removeNode(const patNode* node);
protected:

	map<const patNode*, set<const patRoadBase*> > m_outgoing_incidents;
	map<const patNode*, set<const patRoadBase*> > m_incoming_incidents;
	double m_minimum_label;
	string m_network_type;
	TransportMode m_transport_mode;
};

#endif /* PATNETWORKBASE_H_ */
