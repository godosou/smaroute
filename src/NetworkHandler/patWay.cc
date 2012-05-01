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

patWay::patWay() :
		m_length(-1.0) {
	//

}

patWay::patWay(unsigned long the_way_id,
		map<patString, patString> the_attributes) :
		m_way_id(the_way_id), m_attributes(the_attributes), m_length(-1.0) {
}
bool patWay::isHighway() const {
	map<patString, patString>::const_iterator find_highway = m_attributes.find(
			"highway");
	if (find_highway == m_attributes.end()) {
		return false;
	} else {
		return true;
	}
}

signed short int patWay::isOneway() const {
	map<patString, patString>::const_iterator find_oneway = m_attributes.find(
			"oneway");
	if (find_oneway == m_attributes.end()) {
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
	if (!isWay(patWay::walk_include_rules, patWay::walk_exclude_rules)) {
		return -2; //not walk
	} else {
		return 0;
	}
}
patString patWay::getHighwayType() const {

	map<patString, patString>::const_iterator find_highway = m_attributes.find(
			"highway");
	if (find_highway == m_attributes.end()) {
		return "";
	} else {
		return find_highway->second;
	}
}
patString patWay::getTagValue(patString tag_key) const {

	map<patString, patString>::const_iterator find_tag = m_attributes.find(
			tag_key);
	if (find_tag == m_attributes.end()) {
		return "";
	} else {
		return find_tag->second;
	}
}

bool patWay::isWay(map<patString, set<patString> >& include_rules
		,map<patString, set<patString> >& exclude_rules) const {
	for (map<patString, set<patString> >::iterator rule_iter =
			exclude_rules.begin(); rule_iter != exclude_rules.end();
			++rule_iter) {
		patString rule_key = rule_iter->first;
		patString tag_value = getTagValue(rule_key);
		if (!tag_value.empty()) {
			if (rule_iter->second.find(tag_value) != rule_iter->second.end()) {
				//if the tag is set as excluded, exclude it.
//				DEBUG_MESSAGE("Excluded");
				return false;
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
			//no tag avaiable, exclude it
			continue;
		}
	//	DEBUG_MESSAGE(rule_key<<":"<<tag_value);
		if (rule_iter->second.find(tag_value) != rule_iter->second.end()) {
			return true;
		}
	}
	return false;
}
bool patWay::isHighway(patString highway_type) {
	map<patString, patString>::iterator find_highway = m_attributes.find(
			"highway");
	if (find_highway == m_attributes.end()) {
		return false;
	} else {
		if (find_highway->second == highway_type) {

			return true;
		} else {
			return false;
		}
	}
}

void patWay::loadRule(map<patString, set<patString> >& rule_map, patString key
		, patString value) {
	map<patString, set<patString> >::iterator find_key = rule_map.find(value);
	if (find_key == rule_map.end()) {
		set < patString > value_set;
		rule_map.insert(pair<patString, set<patString> >(key, value_set));
	}
	rule_map[key].insert(value);
	//DEBUG_MESSAGE(rule_map[key].size());
}
void patWay::initiateNetworkTypeRules() {

	patString query_string = "select type,key,value,include from network_rules;";
	result R = patPostGreSQLConnector::makeSelectQuery(query_string);

	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {
		bool include;
		patString type;
		patString key;
		patString value;
		(*i)["include"].to(include);
		(*i)["type"].to(type);
		(*i)["key"].to(key);
		(*i)["value"].to(value);
		DEBUG_MESSAGE(include << " " << type << " " << key << " " << value);
		if (type == "car") {
			if (include) {
				loadRule(car_include_rules, key, value);
			} else {

				loadRule(car_exclude_rules, key, value);
			}
		} else if (type == "bike") {

			if (include) {
				loadRule(bike_include_rules, key, value);
				loadRule(walk_include_rules, key, value);
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
	}
	DEBUG_MESSAGE("Network configuration loaded");
	DEBUG_MESSAGE("car include rules:" << car_include_rules.size());
	DEBUG_MESSAGE("car exclude rules:" << car_exclude_rules.size());
	DEBUG_MESSAGE("bike include rules:" << bike_include_rules.size());
	DEBUG_MESSAGE("bike exclude rules:" << bike_exclude_rules.size());
	DEBUG_MESSAGE("walk include rules:" << walk_include_rules.size());
	DEBUG_MESSAGE("walk exclude rules:" << walk_exclude_rules.size());
}
const vector<const patArc*>* patWay::getArcListPointer(bool forward) const {
	if (forward) {
		return &m_arcs;

	} else {
		return &m_reverse_arcs;

	}
}

void patWay::appendArc(const patArc* new_arc) {
	m_arcs.push_back(new_arc);
}

void patWay::appendReverseArc(const patArc* new_arc) {
	m_reverse_arcs.insert(m_reverse_arcs.begin(),new_arc);
}
unsigned long patWay::getId() {
	return m_way_id;
}

unsigned long patWay::getId() const {
	return m_way_id;
}
void patWay::setId(unsigned long the_way_id) {
	m_way_id = the_way_id;
}
patWay::~patWay() {
	//
}

bool patWay::readFromNodesIds(patNetworkElements* network,
		list<unsigned long> the_list_of_nodes_ids, patError*& err) {

	if (the_list_of_nodes_ids.size() <= 1) {
		DEBUG_MESSAGE("no enough node" << the_list_of_nodes_ids.size());
		return false;
	}
	list<unsigned long>::iterator node_iter = the_list_of_nodes_ids.begin();
	unsigned long up_node_id = *node_iter;
	const patNode* up_node = network->getNode(up_node_id);
	unsigned long down_node_id;
	const patNode* down_node;
	node_iter++;
	if(getId()==27335918){
		DEBUG_MESSAGE(the_list_of_nodes_ids.size());
	}
	unsigned long wrong_arc_counts = 0;
	for (; node_iter != the_list_of_nodes_ids.end(); ++node_iter) {
		down_node_id = *node_iter;
		down_node = network->getNode(down_node_id);
		if (getId() == 191887) {
			DEBUG_MESSAGE(down_node_id);
		}

		if (up_node == NULL || down_node == NULL) {
			//WARNING("Nodes "<<up_node_id<<" or "<<down_node_id<<"not found;");
			wrong_arc_counts++;
		} else {
			const patArc* new_arc = up_node->getOutgoingArc(down_node_id);
			if (new_arc == NULL) {
				new_arc = network->addArc(up_node, down_node, this, err);
			}

			if (new_arc != NULL) {
				appendArc(new_arc);
			} else {
				DEBUG_MESSAGE("WRONG ARC" << up_node << "," << down_node)
			}
			const patArc* new_arc_reverse = down_node->getOutgoingArc(
					up_node_id);
			if (new_arc_reverse == NULL) {
				new_arc_reverse = network->addArc(down_node, up_node, this,
						err);
			}

			if (new_arc_reverse != NULL) {
				appendReverseArc(new_arc_reverse);
				if (getId() == 191887) {
					DEBUG_MESSAGE(*new_arc_reverse);
					DEBUG_MESSAGE(*up_node);
					DEBUG_MESSAGE(*down_node);
				}
			}
		}
		up_node = down_node;
		up_node_id = down_node_id;
	}
	if (wrong_arc_counts > 0) {
		return false;
	} else {
		return true;
	}
}


void patWay::setTags(map<string, string>& tags){
	m_tags = tags;
}
