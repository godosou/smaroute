/*
 * patNetworkV2.h
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKUNIMODAL_H_
#define PATNETWORKUNIMODAL_H_
#include <set>
#include <map>
#include "patArc.h"
#include "patWay.h"
#include "patType.h"
#include "patNetworkElements.h"
#include "patNetworkBase.h"
using namespace std;

class patNetworkUnimodal:public patNetworkBase {
public:
	patNetworkUnimodal();

	/**
	 * Add a way to the network, if it is reverse, add the way reversely.
	 * @param the_way, the way to be added.
	 * @param reverse, reverse the way or not.
	 */
	bool addWay(const patWay*  the_way,bool reverse);
	virtual ~patNetworkUnimodal();
	void walkFromToStops(
			patNetworkElements* network_elements, patNetworkBase* walk_network){};
		void walkOnTrack(
			patNetworkElements* network_elements, patNetworkBase* walk_network) const{};

	 bool isStop(const patNode* node) const {
		return true;
	}

	/**
	 * Find a arc by the up node and down node.
	 * @param up_node the up node
	 * @param down_node the down node
	 * @return the pointer the the found arc, NULL if not found.
	 */
	const patRoadBase* findArc(const patArc* const an_arc) const;

	/**
	 * Implement patNetworkBase::getRoadsContainArc()
	 * @see patNetworkBase::getRoadsContainArc()
	 */
	 virtual set<const patRoadBase*> getRoadsContainArc(const patRoadBase* arc) const ;
protected:
};

#endif /* PATNETWORKV2_H_ */
