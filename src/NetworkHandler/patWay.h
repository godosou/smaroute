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
#include <list>
#include "patError.h"
#include "patArcSequence.h"
using namespace pqxx;
using namespace std;
class patNetworkElements;
struct pt_info {
	patString type; /*< train, bus,...*/
	patString route; /*< route number*/
	patString name;
	map<patString, patString> attributes;
};

class patWay : public patArcSequence{

public:
	patWay();
	patWay(unsigned long the_way_id, map<patString, patString> the_attributes);

	void appendArc(const patArc* new_arc);
	void appendReverseArc(const patArc* new_arc);
	unsigned long getId() const;
	unsigned long getId();
	signed short int isOneway() const;
	bool isWay(map<patString, set<patString> >& include_rules
			,map<patString, set<patString> >& exclude_rules) const;
	signed short int isCarOneWay() const;
	signed short int isBikeOneWay() const;
	signed short int isWalkOneWay() const;
	patString getTagValue(patString tag_key) const;
	bool readFromNodesIds(patNetworkElements* network,
			list<unsigned long> the_list_of_nodes_ids, patError*& err);
	patString getHighwayType() const;
	bool isHighway() const;
	bool isHighway(patString highway_type);
	void setId(unsigned long the_way_id);
	static void loadRule(map<patString, set<patString> >& rule_map
			, patString key, patString value);

	const vector<const patArc*>* getArcListPointer(bool forward = true) const;
	static void initiateNetworkTypeRules();
	virtual ~patWay();
	static map<patString, set<patString> > car_include_rules;
	static map<patString, set<patString> > car_exclude_rules;
	static map<patString, set<patString> > bike_include_rules;
	static map<patString, set<patString> > bike_exclude_rules;
	static map<patString, set<patString> > walk_include_rules;
	static map<patString, set<patString> > walk_exclude_rules;


	void setTags(map<string, string>& tags);
protected:
	map<patString, patString> m_tags;

	map<patString, patString> m_attributes;
	vector<const patArc*> m_reverse_arcs;
	unsigned long m_way_id;
	double m_length;
	list<struct pt_info> m_pt_info_list;
};

#endif /* PATWAY_H_ */
