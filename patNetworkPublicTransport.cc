/*
 * patNetworkPublicTransport.cc
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#include "patNetworkPublicTransport.h"

patNetworkPublicTransport::patNetworkPublicTransport() {
	// TODO Auto-generated constructor stub

}
void patNetworkPublicTransport::addPTSegment(patNode* up_node,patPublicTransportSegment the_seg){
	//map<patNode*,list<patPublicTransportSegment> > network;
	if(network.find(up_node)==network.end()){
		list<patPublicTransportSegment> new_list;
		network[up_node] = new_list;
	}
	network[up_node].push_back(the_seg);

}
patNetworkPublicTransport::~patNetworkPublicTransport() {
	// TODO Auto-generated destructor stub
}

