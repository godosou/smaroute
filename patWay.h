
/*
 * patWay.h
 *
 *  Created on: Jul 21, 2011
 *      Author: jchen
 */

#ifndef PATWAY_H_
#define PATWAY_H_
#include "patArc.h"
#include "patType.h"
#include <map>
#include <pqxx/pqxx>

using namespace pqxx;
using namespace std;

struct pt_info{
	patString type; /*< train, bus,...*/
	patString route; /*< route number*/
	patString name ;
	map<patString,patString> attributes;
};

class patWay {

public:
	patWay();
	patWay(patULong the_way_id,map<patString, patString> the_attributes);
	void appendArc(patArc* new_arc);
	void appendReverseArc(patArc* new_arc);
	patULong getId() const;
	patULong getId();
	signed short int  isOneway() const;
	patBoolean isWay(map<patString,set<patString> >& include_rules,map<patString,set<patString> >& exclude_rules) const;
	signed short int  isCarOneWay() const;
	signed short int  isBikeOneWay() const;
	signed short int  isWalkOneWay() const;
	patString getTagValue(patString tag_key) const;

	patString getHighwayType() const;
	patBoolean isHighway() const;
	patBoolean isHighway(patString highway_type);
	void setId(patULong the_way_id);
	static void loadRule(map<patString,set<patString> >& rule_map, patString key, patString value);

	/**
	 * return pointer to the first node;
	 */
	patNode* getUpNode();
	/**
	 * return pointer to the last node;
	 */
	patNode* getDownNode();
	const list<patArc*>* getListOfArcs(patBoolean reverse) const;
	static void initiateNetworkTypeRules();
	virtual ~patWay();
	static map<patString,set<patString> > car_include_rules;
	static map<patString,set<patString> > car_exclude_rules;
	static map<patString,set<patString> > bike_include_rules;
	static map<patString,set<patString> > bike_exclude_rules;
	static map<patString,set<patString> > walk_include_rules;
	static map<patString,set<patString> > walk_exclude_rules;
protected:
	map<patString,patString> attributes;
	list<patArc*> arcs;
	list<patArc*> reverse_arcs;
	patULong way_id;
	list<struct pt_info> pt_info_list;
};

#endif /* PATWAY_H_ */
