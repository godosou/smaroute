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
#include <tr1/unordered_map>
using namespace std::tr1;
using namespace pqxx;
using namespace std;
class patNetworkElements;
struct pt_info {
	string type; /*< train, bus,...*/
	string route; /*< route number*/
	string name;
	map<string, string> attributes;
};

class patWay : public patArcSequence{

public:
	patWay();
	patWay(unsigned long the_way_id, unordered_map<string, string> the_tags);

	void appendArc(const patArc* new_arc);
	void appendReverseArc(const patArc* new_arc);
	unsigned long getId() const;
	unsigned long getId();
	signed short int isOneway() const;
	bool isWay(map<string, set<string> >& include_rules
			,map<string, set<string> >& exclude_rules) const;
	signed short int isCarOneWay() const;
	signed short int isBikeOneWay() const;
	signed short int isWalkOneWay() const;
	string getTagValue(string tag_key) const;
	bool readFromNodesIds(patNetworkElements* network,
			list<unsigned long> the_list_of_nodes_ids);
	string getHighwayType() const;
	bool isHighway() const;
	bool isHighway(string highway_type);
	void setId(unsigned long the_way_id);
	static void loadRule(map<string, set<string> >& rule_map
			, string key, string value);

	const vector<const patArc*>* getArcListPointer(bool forward = true) const;
	static void initiateNetworkTypeRules();
	static void initiateNetworkTypeRules(string file_name);
	virtual ~patWay();
	static map<string, set<string> > car_include_rules;
	static map<string, set<string> > car_exclude_rules;
	static map<string, set<string> > bike_include_rules;
	static map<string, set<string> > bike_exclude_rules;
	static map<string, set<string> > walk_include_rules;
	static map<string, set<string> > walk_exclude_rules;


	void setTags(unordered_map<string, string>& tags);
protected:
	unordered_map<string, string> m_tags;

	vector<const patArc*> m_reverse_arcs;
	unsigned long m_way_id;
//	double m_length;
	list<struct pt_info> m_pt_info_list;
};

#endif /* PATWAY_H_ */
