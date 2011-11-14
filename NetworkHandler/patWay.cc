/*
 * patWay.cc
 *
 *  Created on: Jul 21, 2011
 *      Author: jchen
 */

#include "patWay.h"
#include "patPostGreSQLConnector.h"
#include "patDisplay.h"
#include "patNetworkElements.h"
map<patString, set<patString> > patWay::car_include_rules;
map<patString, set<patString> > patWay::car_exclude_rules;
map<patString, set<patString> > patWay::bike_include_rules;
map<patString, set<patString> > patWay::bike_exclude_rules;
map<patString, set<patString> > patWay::walk_include_rules;
map<patString, set<patString> > patWay::walk_exclude_rules;

patWay::patWay():
	length(-1.0){
	//

}

patWay::patWay(patULong the_way_id, map<patString, patString> the_attributes) :
		way_id(the_way_id), attributes(the_attributes),length(-1.0) {
}
patBoolean patWay::isHighway() const {
	map<patString, patString>::const_iterator find_highway = attributes.find(
			"highway");
	if (find_highway == attributes.end()) {
		return patFALSE;
	} else {
		return patTRUE;
	}
}

signed short int patWay::isOneway() const {
	map<patString, patString>::const_iterator find_oneway = attributes.find(
			"oneway");
	if (find_oneway == attributes.end()) {
		return 0;
	} else {
		patString one_way_stirng = find_oneway->second;
		if (one_way_stirng == "yes" || one_way_stirng == "1"
				|| one_way_stirng == "true") {
			return 1;
		} else if (one_way_stirng == "-1") {
			return -1;
		} else {
			return 0;
		}
	}

}
signed short int patWay::isCarOneWay() const {

	if (!isWay(patWay::car_include_rules, patWay::car_exclude_rules)) {
		return -2; //not car
	} else {
		return isOneway(); //"-1" reverse, "0" two ways, "1" one way;
	}
}
signed short int patWay::isBikeOneWay() const {
	if (!isWay(patWay::bike_include_rules, patWay::bike_exclude_rules)) {
		return -2; //not bike
	} else {
		return 0;
	}
}
signed short int patWay::isWalkOneWay() const {
	if (!isWay(patWay::bike_include_rules, patWay::bike_exclude_rules)) {
		return -2; //not walk
	} else {
		return 0;
	}
}
patString patWay::getHighwayType() const {

	map<patString, patString>::const_iterator find_highway = attributes.find(
			"highway");
	if (find_highway == attributes.end()) {
		return "";
	} else {
		return find_highway->second;
	}
}
patString patWay::getTagValue(patString tag_key) const {

	map<patString, patString>::const_iterator find_tag = attributes.find(
			tag_key);
	if (find_tag == attributes.end()) {
		return "";
	} else {
		return find_tag->second;
	}
}

patBoolean patWay::isWay(map<patString, set<patString> >& include_rules

,map<patString, set<patString> >& exclude_rules) const {
	patBoolean is_way = 0;
	for (map<patString, set<patString> >::iterator rule_iter =
			exclude_rules.begin(); rule_iter != exclude_rules.end();
			++rule_iter) {
		patString rule_key = rule_iter->first;
		patString tag_value = getTagValue(rule_key);
		if (!tag_value.empty()) {
			if (rule_iter->second.find(tag_value) != rule_iter->second.end()) {
				DEBUG_MESSAGE("Excluded");
				return 0;
			}
		}
	}
	for (map<patString, set<patString> >::iterator rule_iter =
			include_rules.begin(); rule_iter != include_rules.end();
			++rule_iter) {
		patString rule_key = rule_iter->first;
		patString tag_value = getTagValue(rule_key);
		if (tag_value.empty()) {
			//DEBUG_MESSAGE("no tag"<<rule_key);
			return 0;
		}
		if (rule_iter->second.find(tag_value) == rule_iter->second.end()) {
			is_way = 0;
		} else {
			is_way = 1;
		}
	}
	return is_way;
}
patBoolean patWay::isHighway(patString highway_type) {
	map<patString, patString>::iterator find_highway = attributes.find(
			"highway");
	if (find_highway == attributes.end()) {
		return patFALSE;
	} else {
		if (find_highway->second == highway_type) {

			return patTRUE;
		} else {
			return patFALSE;
		}
	}
}

void patWay::loadRule(map<patString, set<patString> >& rule_map, patString key
		, patString value) {
	map<patString, set<patString> >::iterator find_key = rule_map.find(value);
	if (find_key == rule_map.end()) {
		set<patString> value_set;
		rule_map.insert(pair<patString, set<patString> >(key, value_set));
	}
	rule_map[key].insert(value);
}
void patWay::initiateNetworkTypeRules() {

	patString query_string = "select type,key,value,include from network_rules;";
	result R = patPostGreSQLConnector::makeSelectQuery(query_string);

	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {
		patBoolean include;
		patString type;
		patString key;
		patString value;

		(*i)["include"].to(include);
		(*i)["type"].to(type);
		(*i)["key"].to(key);
		(*i)["value"].to(value);
		if (type == "car") {
			if (include) {
				loadRule(car_include_rules, key, value);
			} else {

				loadRule(car_exclude_rules, key, value);
			}
		} else if (type == "bike") {

			if (include) {
				loadRule(bike_include_rules, key, value);
			} else {

				loadRule(bike_exclude_rules, key, value);
			}
		} else if (type == "walk") {

			if (include) {
				loadRule(walk_include_rules, key, value);
			} else {

				loadRule(walk_exclude_rules, key, value);
			}
		}
	}DEBUG_MESSAGE("Network configuration loaded");
	DEBUG_MESSAGE("car include rules:"<<car_include_rules.size());
	DEBUG_MESSAGE("car exclude rules:"<<car_exclude_rules.size());
	DEBUG_MESSAGE("bike include rules:"<<bike_include_rules.size());
	DEBUG_MESSAGE("bike exclude rules:"<<bike_exclude_rules.size());
	DEBUG_MESSAGE("walk include rules:"<<walk_include_rules.size());
	DEBUG_MESSAGE("walk exclude rules:"<<walk_exclude_rules.size());
}
const list<patArc*>* patWay::getListOfArcs(bool forward) const {
	if (forward) {
		return &arcs;

	} else {
		return &reverse_arcs;

	}
}

void patWay::appendArc(patArc* new_arc) {
	arcs.push_back(new_arc);
}

void patWay::appendReverseArc(patArc* new_arc) {
	reverse_arcs.push_front(new_arc);
}
patULong patWay::getId() {
	return way_id;
}

patULong patWay::getId() const {
	return way_id;
}
void patWay::setId(patULong the_way_id) {
	way_id = the_way_id;
}
patWay::~patWay() {
	//
}

patBoolean patWay::readFromNodesIds(patNetworkElements* network,
		list<patULong> the_list_of_nodes_ids, patError*& err) {

	if (the_list_of_nodes_ids.size() <= 1) {
		DEBUG_MESSAGE("no enough node"<<the_list_of_nodes_ids.size());
		return patFALSE;
	}
	list<patULong>::iterator node_iter = the_list_of_nodes_ids.begin();
	patULong up_node_id = *node_iter;
	patNode* up_node = network->getNode(up_node_id);
	patULong down_node_id;
	patNode* down_node;
	node_iter++;

	patULong wrong_arc_counts = 0;
	for (; node_iter != the_list_of_nodes_ids.end(); ++node_iter) {
		down_node_id = *node_iter;
		down_node = network->getNode(down_node_id);
		if(getId()==191887){
				DEBUG_MESSAGE(down_node_id);
		}

		if (up_node == NULL || down_node == NULL) {
			//WARNING("Nodes "<<up_node_id<<" or "<<down_node_id<<"not found;");
			wrong_arc_counts++;
		} else {
			patArc* new_arc = up_node->getOutgoingArc(down_node_id);
			if (new_arc == NULL) {
				new_arc = network->addArc(up_node, down_node, this, err);
			}

			if (new_arc != NULL) {
				appendArc(new_arc);
			}
			else{
				DEBUG_MESSAGE("WRONG ARC"<<up_node<<","<<down_node)
			}
			patArc* new_arc_reverse = down_node->getOutgoingArc(up_node_id);
			if (new_arc_reverse == NULL) {
				new_arc_reverse = network->addArc(down_node, up_node, this,
						err);
			}

			if (new_arc_reverse != NULL) {
				appendReverseArc(new_arc_reverse);
				if(getId()==191887){
					DEBUG_MESSAGE(*new_arc_reverse);
					DEBUG_MESSAGE(*up_node);
					DEBUG_MESSAGE(*down_node);
				}
			}
		}
		up_node = down_node;
		up_node_id = down_node_id;
	}
	if (wrong_arc_counts>0){
		return false;
	}
	else{
		return true;
	}
}

double patWay::getLength(){
	if(length<0){
		return calLength();
	}
}

double patWay::calLength(){
	length=0.0;
	list<patArc*> arcs;
	for(list<patArc*>::iterator arc_iter = arcs.begin();
			arc_iter!=arcs.end();
			++arc_iter){
		length += (*arc_iter)->getLength();
	}
	return length;
}
