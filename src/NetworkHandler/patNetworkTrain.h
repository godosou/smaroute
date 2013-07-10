/*
 * patNetworkTrain.h
 *
 *  Created on: Oct 11, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKTRAIN_H_
#define PATNETWORKTRAIN_H_

#include "patGeoBoundingBox.h"
#include "patNetworkPublicTransport.h"
#include "patWay.h"
#include <set>
#include <list>
#include "patNode.h"
#include "patArc.h"
#include "patNetworkElements.h"
#include <tr1/unordered_map>
using namespace std;
using namespace std::tr1;

class patNetworkTrain: public patNetworkPublicTransport {
public:
	patNetworkTrain();

	virtual patNetworkBase* clone() const {
		return new patNetworkTrain(*this);
	}
	;



	void addWay(const patWay* the_way,
			unordered_map<const patNode*, set<const patArc*> >* arc_outgoing) const;

	void getWays(const patNetworkElements* network,
			unordered_map<const patNode*, set<const patArc*> >* arc_outgoing) const;
	void createAdjancencyList(const patNetworkElements* network,
			unordered_map<const patNode*, set<const patArc*> >* arc_outgoing) const;
	void getFromNetwork(patNetworkElements* network,
			patGeoBoundingBox bounding_box);
	void newConnection(patNetworkElements* network,patPublicTransportSegment* tmp_pt_seg);
	void buildNetworkFromArcsNodes(patNetworkElements* network,
			const unordered_map<const patNode*, set<const patArc*> > * arc_outgoing,
			const set<const patNode*>* railway_stations) ;
	void connectStations(patNetworkElements* network,const patNode* station,
			patPublicTransportSegment* tmp_pt_seg, const patArc* incoming_arc,
			unordered_map<const patNode*, unsigned int>* node_count,
			const unordered_map<const patNode*, set<const patArc*> > * arc_outgoing,
			const set<const patNode*>* railway_stations);
	double getLabel(
			const unordered_map<const patNode*, double>* label,
			const patNode* node) const ;
	void newConnectionByShortestPath(patNetworkElements* network,
			const patNode* from_station,
			const unordered_map<const patNode*, const patArc*>* predecessors,
			const patNode* to_station);
	void buildConnectionByShortestPath(const patNode* from_station,
			const unordered_map<const patNode*, set<const patArc*> > * arc_outgoing,
			const set<const patNode*>* railway_stations,
			patNetworkElements* network);
	virtual ~patNetworkTrain();
	double getMinSpeed() const;
	double getMaxSpeed() const;
protected:
	unordered_map<const patNode*, set<patPublicTransportSegment> > m_train_segments;
};

#endif /* PATNETWORKTRAIN_H_ */
