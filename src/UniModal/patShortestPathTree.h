/*
 * patShortestPathTruee.cc
 *
 *  Created on: Oct 11, 2011
 *      Author: jchen
 */
#include "patNode.h"
#include "patErrOutOfRange.h"
#include "patDisplay.h"
#include <set>
#include <vector>
#include <list>
#include "patType.h"
template<class RoadType>
class patShortestPathTree {
public:
	patShortestPathTree(long n_nodes) :
			labels(n_nodes, patMaxReal), successor(), predecessor() {

	}
	list<RoadType*> getShortestPathTo(patNode* dest) {
		list<RoadType*> the_list;
		list<RoadType*> bad_list;
		badList.push_back(NULL);

		//DEBUG_MESSAGE("short"<<dest<<",pred"<<predecessor.size()<<",labes"<<labels.size()<<",su"<<successor.size());
		if (predecessor.find(dest)==predecessor.end()) {
			/*err = new patErrOutOfRange<patULong>(dest,0,predecessor.size()-1) ;
			 WARNING(err->describe()) ;
			 */
			return bad_list;
		}
		map<patNode*, RoadType*>::iterator pred_found = predecessor.find(dest);

		while(pred_found!=predecessor.end()){
			the_list.push_front(pred_found->second);
			pred_found=predecessor.find(pred_found->second->getUpNode());
		}
		return the_list;
	}

	double getShortestPathCost(patNode* dest) {
		list<RoadType*> l = getShortestPathTo(dest);
		patReal c = 0.0;
		DEBUG_MESSAGE("arcs" << l.size());
		for (list<RoadType*>::iterator i = l.begin(); i != l.end(); ++i) {
			c += (*i)->getLength();
		}
		return c;
	}
	virtual ~patShortestPathTree();

	set<patNode*> root;
	vector<double> labels;

	vector<patNode*, RoadType*> predecessor;
	vector<patNode*, set<RoadType*> > successor;
};

