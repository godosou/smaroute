/*
 * patNetworkBus.cc
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#include "patNetworkBus.h"
#include "patPostGreSQLConnector.h"
#include "patPostGISDataType.h"
#include "patNetworkPublicTransport.h"

#include "patPublicTransportSegment.h"
#include "patDisplay.h"

patNetworkBus::patNetworkBus() {
	// TODO Auto-generated constructor stub

}

void getRoutes(patNetworkElements* network) {
	patString query_string =
			"select id,tags,route_type,sequence_id,member_type,member_role from pt_routes_relations;";
	result R = patPostGreSQLConnector::makeSelectQuery(query_string);

	DEBUG_MESSAGE("Total Records: " << R.size());

	//parse result

	//initial set up
	patULong last_id = -1;
	int short last_sequence_id = -1;
	patPublicTransportSegment* forward_segment =
			new patPublicTransportSegment();
	patPublicTransportSegment* backward_segment =
			new patPublicTransportSegment();
	patNode* previous_stop = NULL;
	patBoolean way_error_tag = patFALSE;
	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {

		//get new information
		patULong current_id;
		patString member_type;
		patString member_role;
		int short current_sequence_id;
		(*i)["id"].to(current_id);
		(*i)["member_type"].to(member_type);
		(*i)["sequence_id"].to(current_sequence_id);
		(*i)["member_role"].to(member_role);

		//if it is a way
		if (member_type == "W") {

			//if the way is errous or the previous stop is not valid, go forward to the next record until it goes to a node record
			//the first record is neither of these situations.
			if (way_error_tag || previous_stop == NULL) {
				continue;
			}

			//if it is a forward way, it is only stored to forward segment;
			if (member_role.find("forward") != string::npos) {
				way_error_tag = forward_segment->pushBack(network,current_id);
				if (way_error_tag) {
					continue;
				}
			}
			//If it is a backward way, it is only stored to backward segment;
			else if (member_role.find("backward") != string::npos) {
				way_error_tag = backward_segment->pushFront(network,current_id);
				if (way_error_tag) {
					continue;
				}
			}

			//otherwise both ways
			else {


				way_error_tag = forward_segment->pushBack(network,current_id);
				if (way_error_tag) {
					continue;
				}
				way_error_tag = backward_segment->pushFront(network,current_id);

				if (way_error_tag) {
					continue;
				}

			}
		}
		//If it is a node;
		else if (member_type == "N") {
			//Get the current node;
			patNode* current_node = network->getNode(current_id);

			//If it is a valid node;
			if (current_node != NULL) {

				//If the segs not errors;
				if (way_error_tag == patFALSE) {

					//If the forward segment is not errors, the previous stop exists and the current node is not a backward node;
					//Add the forward segment as the outgoing sergment of the previous stop;
					if (previous_stop != NULL && forward_segment->isError() && member_role.find("backward") == string::npos) {
						addPTSegment(previous_stop, *forward_segment);
					}

					//Opposite here
					if (backward_segment->isError() && member_role.find("forward") == string::npos ) {
						addPTSegment(current_node, *backward_segment);
					}
				}
			}


			way_error_tag = patFALSE;//clear way error tag
			patPublicTransportSegment* forward_segment =
					new patPublicTransportSegment();//create new temp
			patPublicTransportSegment* backward_segment =
					new patPublicTransportSegment();
			previous_stop = current_node;//get the current stop
		} else {
			WARNING("not valid relation member" << member_type);
		}
	}

}
patNetworkBus::~patNetworkBus() {
	// TODO Auto-generated destructor stub
}

