/*
 * patMapMatchingIteration.cc
 *
 *  Created on: Oct 12, 2011
 *      Author: jchen
 */

#include "patMapMatchingIteration.h"
#include "patShortestPathGeneral.h"
#include "patShortestPathTreeGeneral.h"
#include "patArc.h"
#include <set>
#include "patDisplay.h"
#include "patGpsDDR.h"
#include "patSampleFromDiscreteUntilCdf.h"
#include "patNBParameters.h"
#include <vector>
#include "patMultiModalPathProba.h"
#include "patPathExtension.h"
#include <time.h>
#include "patCreateKmlStyles.h"
#include<tr1/unordered_map>
#include "patWriteMMResultKML.h"
using namespace std::tr1;
#include<iostream>
#include<sstream>
#include<fstream>
#include "kml/dom.h"
#include "patNetworkBase.h"
#include <boost/lexical_cast.hpp>
#include "patSpeedDistributions.h"
using kmldom::DocumentPtr;
using kmldom::KmlFactory;
using kmldom::KmlPtr;
using kmldom::PlacemarkPtr;
using kmldom::FolderPtr;
patMapMatchingIteration::patMapMatchingIteration(
		const patNetworkEnvironment* environment, patGpsPoint* gps,
		vector<patMeasurement*>* measurement_sequence,
		map<patMultiModalPathMatching, double> paths) :
		m_environment(environment), m_gps(gps), m_current_measurments(
				measurement_sequence), m_paths(paths) {
}
patMapMatchingIteration::patMapMatchingIteration(){

}
patMapMatchingIteration::~patMapMatchingIteration() {
}

/**
 * Generate DDR for the first iteration.
 * Loop over all arcs and determine each one.
 * TODO: improve the efficiency.
 */
void patMapMatchingIteration::genInitDDR() {

	const map<string, patNetworkBase*>* all_networks =
			m_environment->getAllNetworks();
	for (map<string, patNetworkBase*>::const_iterator network_iter =
			all_networks->begin(); network_iter != all_networks->end();
			++network_iter) {
		cout << "network " << network_iter->first << endl;
		bool found_ddr = false;
		set<const patArc*> all_arcs = network_iter->second->getAllArcs();
		for (set<const patArc*>::iterator arc_iter = all_arcs.begin();
				arc_iter != all_arcs.end(); ++arc_iter) {
			if (m_gps->getDDR()->detArcDDR(*arc_iter,
					network_iter->second->getTransportMode())) {
				found_ddr = true;
			}

		}

		cout << "found ddr yes/no? " << found_ddr << endl;
	}
	if (m_gps->getDDR()->empty()) {
		cout << ("no ddr") << endl;
		return;
	}
	cout << "ddr arc size:" << m_gps->getDDR()->size() << endl;
}

/**
 * The first map matching iteration.
 * The precedures are performed:
 * <ul>
 * 	<li> Generate DDR from all arcs. (DDR only matters with geometry).</li>
 * 	<li> Connect DDR in different transport networks.</li>
 * </ul>
 */
void patMapMatchingIteration::firstIteration() {
	DEBUG_MESSAGE("first iteration with GPS\n\t" << *m_gps);

	genInitDDR(); //Generate DDR for first iteration.

	map<patMultiModalPathMatching, double> paths_temp;
//Connect DDR in all networks.

	const map<string, patNetworkBase*>* all_networks =
			m_environment->getAllNetworks();
	for (map<string, patNetworkBase*>::const_iterator network_iter =
			all_networks->begin(); network_iter != all_networks->end();
			++network_iter) {

		if ((m_gps->getSpeed() > patNBParameters::the()->walkMaxSpeed
				&& network_iter->second->getTransportMode() == WALK)
				|| (m_gps->getSpeed() > patNBParameters::the()->bikeMaxSpeed
						&& network_iter->second->getTransportMode() == BIKE)) { //speed in km/h
			cout << "  speed " << m_gps->getSpeed()
					<< " too high skip network: " << network_iter->first
					<< endl;

			continue;
		}
		patPathExtension path_initiation(network_iter->second, m_environment);

		const map<const patArc*, double>* arc_list =
				m_gps->getDDR()->getDDRArcs(
						network_iter->second->getTransportMode());
		if (arc_list == NULL) {
			continue;
		}
		set<patMultiModalPathMatching> new_path_set = path_initiation.init(
				arc_list);

		for (set<patMultiModalPathMatching>::iterator path_iter =
				new_path_set.begin(); path_iter != new_path_set.end();
				++path_iter) {
			//DEBUG_MESSAGE("path with arcs"<<(*path_iter).size());
			paths_temp[*path_iter] = 0.0;
		}

	}

	for (map<patMultiModalPathMatching, double>::iterator path_iter =
			paths_temp.begin(); path_iter != paths_temp.end(); ++path_iter) {
		const_cast<patMultiModalPathMatching*>(&(path_iter->first))->addMeasurement(
				m_gps);
	}
	//m_paths = paths_temp;
	selectPaths(paths_temp); //Select paths
	finalizeOneStage();
	DEBUG_MESSAGE("finish an iteration with paths" << m_paths.size());
}

/**
 * Sort the paths by their end nodes.
 * @return a map with
 * <ul>
 * <li> key:  end node pointer.</li>
 * <li> value: set of corresponding path with the same end node</li>
 * </ul>
 */
map<const patNode*, set<const patMultiModalPathMatching*> > patMapMatchingIteration::sortPathsByEndNodes() const {
	map<const patNode*, set<const patMultiModalPathMatching*> > end_nodes;
	for (map<patMultiModalPathMatching, double>::const_iterator path_iter =
			m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {
		const patNode* curr_end_node = path_iter->first.getDownNode();
		if (curr_end_node == NULL) {
			continue;
		}
		end_nodes[curr_end_node];
		end_nodes[curr_end_node].insert(&(path_iter->first));

	}
	return end_nodes;
}

double patMapMatchingIteration::getDistanceCeilFromGPS(
		const patGpsPoint* currGpsPoint, const patGpsPoint* prevGpsPoint,
		const patNetworkBase* network) const {
	double maxSpeed = currGpsPoint->getMaxSpeed(prevGpsPoint); //speed in m/s
	/*
	 if (maxSpeed > network->getMaxSpeed()) {
	 maxSpeed = network->getMaxSpeed();
	 }
	 if (maxSpeed < network->getMinSpeed()) {
	 maxSpeed = network->getMinSpeed();
	 }
	 */

	return maxSpeed * (m_gps->getTimeStamp() - prevGpsPoint->getTimeStamp())
			* patNBParameters::the()->maxDistanceRatioSP;
}

/**
 * Map matching for normal iteration.
 */
bool patMapMatchingIteration::normalIteration(patGpsPoint* prevNormalGps,
		vector<patMeasurement*>& inter_data, patError*& err) {
//implement error
//	DEBUG_MESSAGE(m_gps);
	//DEBUG_MESSAGE(m_gps->getDDR());
	double distance_to_prev_normal = m_gps->distanceTo(prevNormalGps);
	if (distance_to_prev_normal
			< patNBParameters::the()->minDistanceFromLastPoint) {

		for (map<patMultiModalPathMatching, double>::iterator path_iter =
				m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {
			if (distance_to_prev_normal < 10.0
					&& (path_iter->first.backMode() != TransportMode(WALK))) {
				const_cast<patMultiModalPathMatching*>(&path_iter->first)->proposeStop(
						m_environment, m_gps->getSpeed(),
						patNBParameters::the()->proposeStopTime);
			}
		}
		m_gps->setType("low_speed");
		cout << "	not enough distance from last GPS point, skip."<<distance_to_prev_normal<<"<"<< patNBParameters::the()->minDistanceFromLastPoint<< endl;
		return false;
	}
	int too_long_count = 0;
	map<TransportMode, set<const patArc*> > inter_sec_arcs =
			m_gps->getDDR()->detInherentDDR(prevNormalGps->getDDR());
	double travel_time = m_current_measurments->back()->getTimeStamp()
			- m_current_measurments->front()->getTimeStamp();
	map<patMultiModalPathMatching, double> paths_temp;
	for (map<patMultiModalPathMatching, double>::iterator path_iter =
			m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {
		RoadTravel road_travel = path_iter->first.back_road_travel();
		bool in_domain = false;
		vector<const patArc*> arc_list = road_travel.road->getArcList();

		for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
				arc_iter != arc_list.end(); ++arc_iter) {

			if (m_gps->getDDR()->detArcDDR(*arc_iter, road_travel.mode)) {
				inter_sec_arcs[road_travel.mode];
				inter_sec_arcs[road_travel.mode].insert(*arc_iter);
				in_domain = true;
			}
		}

		if (in_domain) {

			/*
			 const_cast<patMultiModalPathMatching*>(&path_iter->first)->proposeStop(
			 m_environment, m_gps->getSpeed(), 10.0);
			 */
			paths_temp[path_iter->first] = 0.0;
		}
	}
	cout << "Inherits " << paths_temp.size() << " paths from the previous DDR"
			<< endl;
	const map<string, patNetworkBase*>* all_networks =
			m_environment->getAllNetworks();

	const map<const patNode*, set<const patMultiModalPathMatching*> > up_stream_paths_by_nodes =
			sortPathsByEndNodes();
	;
	cout << "Number of end node of the previous path set:"
			<< up_stream_paths_by_nodes.size() << endl;
	bool is_speed_reasonable = false;
	for (map<const patNode*, set<const patMultiModalPathMatching*> >::const_iterator end_node_iter =
			up_stream_paths_by_nodes.begin();
			end_node_iter != up_stream_paths_by_nodes.end(); ++end_node_iter) {
		//Loop over all up stream paths with the same node.
//		DEBUG_MESSAGE("OK");
		cout << "start from node" << (end_node_iter->first->getName()) << endl;
		for (map<string, patNetworkBase*>::const_iterator network_iter =
				all_networks->begin(); network_iter != all_networks->end();
				++network_iter) {

			//Loop over all transport networks and extend the paths
			if (patNBParameters::the()->speedCapacityCheckPrevious == 1) {
				if ((m_gps->getSpeed() > patNBParameters::the()->walkMaxSpeed
						&& prevNormalGps->getSpeed()
								> patNBParameters::the()->walkMaxSpeed
						&& network_iter->second->getTransportMode() == WALK)
						|| (m_gps->getSpeed()
								> patNBParameters::the()->bikeMaxSpeed
								&& prevNormalGps->getSpeed()
										> patNBParameters::the()->bikeMaxSpeed
								&& network_iter->second->getTransportMode()
										== BIKE)) { //speed in km/h
					cout << "  speed " << m_gps->getSpeed()
							<< " too high skip network: " << network_iter->first
							<< endl;

					continue;
				}
			} else {

				if ((m_gps->getSpeed() > patNBParameters::the()->walkMaxSpeed
						&& network_iter->second->getTransportMode() == WALK)
						|| (m_gps->getSpeed()
								> patNBParameters::the()->bikeMaxSpeed
								&& network_iter->second->getTransportMode()
										== BIKE)) { //speed in km/h
					cout << "  speed " << m_gps->getSpeed()
							<< " too high skip network: " << network_iter->first
							<< endl;

					continue;
				}
			}
			/*
			 if (m_gps->getMaxSpeed(prevNormalGps)
			 < network_iter->second->getMinSpeed()) { //speed in km/h
			 DEBUG_MESSAGE(
			 "  speed " << m_gps->getMaxSpeed(prevNormalGps)
			 << " too low skip network: "
			 << network_iter->first);
			 continue;
			 }
			 */
			is_speed_reasonable = true;
			TransportMode curr_mode = network_iter->second->getTransportMode();
			double max_distance_from_las_gps = getDistanceCeilFromGPS(m_gps,
					prevNormalGps, network_iter->second);
			cout << "  find in network " << network_iter->first
					<< " with distance " << max_distance_from_las_gps << endl;
			if (network_iter->second->hasDownStream(end_node_iter->first)) {
				if (network_iter->first == "bus") {
					cout << "  node has down stream" << endl;
				}
				//if the node has outgoing links, we extend.

				//Build shortest path tree from a node.
				set < pair<const patArc*, const patRoadBase*> > ddr_arcs; //Arc is the arc in ddr, roadbase is the corresponding road.

				set<patMultiModalPath> downstream_paths; //Stores down stream paths
				//DEBUG_MESSAGE("Extend node from " << *end_node_iter->first);

				patShortestPathGeneral shortest_path_from_node(
						network_iter->second); //build shortest path from an end node.

				if (shortest_path_from_node.buildShortestPathTree(
						end_node_iter->first, m_gps->getDDR(), &ddr_arcs,
						max_distance_from_las_gps) == false) { //Build shortest path tree. Quit if false.

					WARNING("wrong in shortest path tree");
					continue;
				}

				//DEBUG_MESSAGE("Downstream arcs in DDR: " << ddr_arcs.size());
				if (m_gps->getDDR()->empty() || ddr_arcs.empty()) {
					continue;
				}

				patShortestPathTreeGeneral* shortest_path_tree =
						shortest_path_from_node.getTree();
				//DEBUG_MESSAGE("TREE "<<shortest_path_tree);
				//For each arc in DDR, get the connecting path.
				for (set<pair<const patArc*, const patRoadBase*> >::const_iterator arc_iter =
						ddr_arcs.begin(); arc_iter != ddr_arcs.end();
						++arc_iter) {
					//DEBUG_MESSAGE("road size"<<arc_iter->second->size());
					list<const patRoadBase*> shortest_path_roads;

					shortest_path_tree->getShortestPathTo(shortest_path_roads,
							arc_iter->second->getUpNode());
					bool create_path_success = true;
					patMultiModalPath shortest_path(shortest_path_roads);
					if (shortest_path.addRoadTravelToBack(arc_iter->second)
							== false) {
						cout << "invalid path" << endl;
						continue;
					}
					shortest_path.setUnimodalTransportMode(curr_mode);

					if (!shortest_path.isValid()) {
						cout << "invalid path" << endl;
						continue;
					} else {
						downstream_paths.insert(shortest_path);
					}
				}

				for (set<patMultiModalPath>::iterator d_p_iter =
						downstream_paths.begin();
						d_p_iter != downstream_paths.end(); ++d_p_iter) {
					vector<const patArc*> arc_list = d_p_iter->getArcList();
					inter_sec_arcs[curr_mode];
					inter_sec_arcs[curr_mode].insert(arc_list.begin(),
							arc_list.end());
				}

				set<const patArc*> ddr_first =
						m_current_measurments->front()->getDDR()->getDDRArcSet(
								curr_mode);
				set<const patArc*> ddr_last =
						m_current_measurments->back()->getDDR()->getDDRArcSet(
								curr_mode);

				//DEBUG_MESSAGE("Path extenstion");
				if (network_iter->first == "bus") {
					cout << "BUS has down stream routes: "
							<< downstream_paths.size() << endl;
				}
				patPathExtension path_extension(network_iter->second,
						m_environment);
				set<patMultiModalPathMatching> new_paths =
						path_extension.extendFromNode(&(end_node_iter->second),
								end_node_iter->first, &downstream_paths,
								m_gps->getDDR()); //Extend paths with connecting paths and the connection node.
				cout << "  new paths created in network " << network_iter->first
						<< ":" << new_paths.size() << "\n" << endl;
				int new_created_path_count = 0;
				for (set<patMultiModalPathMatching>::iterator path_iter =
						new_paths.begin(); path_iter != new_paths.end();
						++path_iter) {

					double min_travel_time = (*path_iter).getMinimumTravelTime(
							m_environment, ddr_first, ddr_last);

					if (patNBParameters::the()->minPathTravelTimeRatio
							* travel_time < min_travel_time) {
						//Check if the path is too long according to time
						/*
						 DEBUG_MESSAGE(
						 "travel time: " << travel_time << "; min travel time: " << min_travel_time);
						 */
						too_long_count++;
						continue;
					}

					/*
					 if (!(*path_iter).isReasonableModeChange()) {
					 // DEBUG_MESSAGE("unreasonable change");
					 continue;
					 }
					 */
					//DEBUG_MESSAGE("Good path.");
					if (paths_temp.find(*path_iter) == paths_temp.end()) {
						new_created_path_count++;
						//mmm(*path_iter).proposeStop(m_environment, m_gps->getSpeed(), 10.0);
						paths_temp.insert(
								pair<patMultiModalPathMatching, double>(
										*path_iter, 0.0));
					}
				}
				cout << "NEW created " << new_created_path_count << endl;
			}
		}
	}
	cout << "too long path" << too_long_count << endl;
	if (is_speed_reasonable == false) {
		cout << "Do not extend in any network" << endl;
		m_gps->setType("low_speed");
		return false;
	}
	if (paths_temp.empty()) {
		m_gps->setType("low_speed");
		return false;
	}
	m_current_measurments->back()->getDDR()->finalize(m_environment);
	cout << "DDR size" << m_current_measurments->back()->getDDR()->size()
			<< endl;
	//calPathDDR(paths_temp);
	//m_paths=paths_temp;
	//writeKML(0);
	/*
	 calPathDDR(paths_temp);
	 m_paths = paths_temp;
	 writeKML(100000 * (m_current_measurments->size() - 1));
	 */
	patMeasurement* last_measurement = m_current_measurments->back();
	m_current_measurments->pop_back();
	dealInterData(inter_data, prevNormalGps, inter_sec_arcs, paths_temp);
	m_current_measurments->push_back(last_measurement);
	for (map<patMultiModalPathMatching, double>::iterator path_iter =
			paths_temp.begin(); path_iter != paths_temp.end(); ++path_iter) {
		const_cast<patMultiModalPathMatching*>(&(path_iter->first))->addMeasurement(
				last_measurement);
	}
	//calPathDDR(paths_temp)
	selectPaths(paths_temp); //Select paths
	finalizeOneStage();
	DEBUG_MESSAGE("finish an iteration with paths" << m_paths.size());
	return true;
}

void patMapMatchingIteration::lastIterationForLowSpeed(
		vector<patMeasurement*>& inter_data, patGpsPoint* prevNormalGps) {

	map<TransportMode, set<const patArc*> > inter_sec_arcs =
			m_gps->getDDR()->detInherentDDR(prevNormalGps->getDDR());
	map<patMultiModalPathMatching, double> paths_temp;
	for (map<patMultiModalPathMatching, double>::iterator path_iter =
			m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {
		RoadTravel road_travel = path_iter->first.back_road_travel();
		bool in_domain = false;
		vector<const patArc*> arc_list = road_travel.road->getArcList();

		for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
				arc_iter != arc_list.end(); ++arc_iter) {

			if (m_gps->getDDR()->detArcDDR(*arc_iter, road_travel.mode)) {
				inter_sec_arcs[road_travel.mode];
				inter_sec_arcs[road_travel.mode].insert(*arc_iter);
				in_domain = true;
			}
		}

		if (in_domain) {
			paths_temp[path_iter->first] = 0.0;
		}
	}
	DEBUG_MESSAGE(
			"Inherits " << paths_temp.size() << " paths from the previous DDR");

	m_current_measurments->back()->getDDR()->finalize(m_environment);
	DEBUG_MESSAGE(
			"DDR size" << m_current_measurments->back()->getDDR()->size());
	patMeasurement* last_measurement = m_current_measurments->back();
	m_current_measurments->pop_back();
	dealInterData(inter_data, prevNormalGps, inter_sec_arcs, paths_temp);
	m_current_measurments->push_back(last_measurement);

	for (map<patMultiModalPathMatching, double>::iterator path_iter =
			paths_temp.begin(); path_iter != paths_temp.end(); ++path_iter) {
		const_cast<patMultiModalPathMatching*>(&(path_iter->first))->addMeasurement(
				last_measurement);
	}
	/*
	 if (paths_temp.size()
	 <= patNBParameters::the()->minGeneratedInterMediatePath) {
	 calPathDDR(paths_temp);
	 }
	 */
	selectPaths(paths_temp); //Select paths

	finalizeOneStage();
	DEBUG_MESSAGE("finish last iteration with paths" << m_paths.size());
}
/**
 * Deal with intermediate low speed GPS points
 */
void patMapMatchingIteration::dealInterData(vector<patMeasurement*>& inter_data,
		patGpsPoint* prevNormalGps,
		map<TransportMode, set<const patArc*> >& inter_sec_arcs,
		map<patMultiModalPathMatching, double>& paths_temp) {
	if (inter_data.empty()) {
		return;
	}
	DEBUG_MESSAGE("Deall with intermediate data");
	for (vector<patMeasurement*>::iterator data_iter = inter_data.begin();
			data_iter != inter_data.end(); ++data_iter) {
		DEBUG_MESSAGE("	Type of data: " << (*data_iter)->getMeasurementType());
		(*data_iter)->getDDR()->detInherentDDR(prevNormalGps->getDDR());
		//DEBUG_MESSAGE("Inherited DDR");
		for (map<TransportMode, set<const patArc*> >::iterator mode_iter =
				inter_sec_arcs.begin(); mode_iter != inter_sec_arcs.end();
				++mode_iter) {
			//("amout of inter"<<mode_iter->first<<" arcs: "<<mode_iter->second.size());
			for (set<const patArc*>::iterator arc_iter =
					mode_iter->second.begin();
					arc_iter != mode_iter->second.end(); ++arc_iter) {
				(*data_iter)->getDDR()->detArcDDR(*arc_iter, mode_iter->first);
			}
		}

		//FIXME why GPS is different?
		/*
		 if ((*data_iter)->isGPS()) {
		 for (map<patMultiModalPathMatching, double>::iterator path_iter =
		 m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {
		 RoadTravel road_travel = path_iter->first.back_road_travel();

		 bool in_domain = false;
		 list<const patArc*> arc_list = road_travel.road->getArcList();

		 for (list<const patArc*>::const_iterator arc_iter =
		 arc_list.begin(); arc_iter != arc_list.end();
		 ++arc_iter) {

		 (*data_iter)->getDDR()->detArcDDR(*arc_iter,road_travel.mode);
		 }
		 }
		 }
		 */
		if (!(*data_iter)->getDDR()->empty()) {
			//DEBUG_MESSAGE("Non empty DDR");
			m_current_measurments->push_back(*data_iter);
			for (map<patMultiModalPathMatching, double>::iterator path_iter =
					paths_temp.begin(); path_iter != paths_temp.end();
					++path_iter) {
				const_cast<patMultiModalPathMatching*>(&(path_iter->first))->addMeasurement(
						*data_iter);
			}
		}
	}
}
/**
 * Calculate path DDR.
 */
void patMapMatchingIteration::calPathDDR(
		map<patMultiModalPathMatching, double>& path_set) {
	int i = 0;
	set<patArcTransition> arc_tran_set;
	for (map<patMultiModalPathMatching, double>::iterator path_iter =
			path_set.begin(); path_iter != path_set.end(); ++path_iter) {

//		DEBUG_MESSAGE("algo:"<<patNBParameters::the()->algoInSelection);
		const_cast<patMultiModalPathMatching*>(&(path_iter->first))->update(
				&arc_tran_set);
		++i;
		path_iter->second = path_iter->first.getLogLikelihood();
		if (i % 20 == 0) {
			DEBUG_MESSAGE("arc tran set size"<<arc_tran_set.size());
			DEBUG_MESSAGE(i);
		}
	}
	arc_tran_set.clear();
	DEBUG_MESSAGE(arc_tran_set.size());
}

void patMapMatchingIteration::finalizeOneStage() {

	if (m_current_measurments->back()->getDDR()->empty()) {
		//m_current_measurments->pop_back();
		DEBUG_MESSAGE("no path generated, invalid gps");
	}
	DEBUG_MESSAGE("final ddr " << m_gps->getDDR()->size());

	DEBUG_MESSAGE("number of paths generated:" << m_paths.size());
}

/**
 * Sum the likelihood of all paths
 */
double patMapMatchingIteration::sumProba(
		const map<patMultiModalPathMatching, double>& path_set) {
	double r = 0.0;
	for (map<patMultiModalPathMatching, double>::const_iterator iter1 =
			path_set.begin(); iter1 != path_set.end(); ++iter1) {
		r += iter1->second;
	}

	return r;
}

set<int> patMapMatchingIteration::selectPathsByShortest(
		vector<patMultiModalPathMatching> path_vector,
		vector<double> path_probas) {

	set<int> rtn_ids;
	map<double, set<int> > lengthPathSet;
	map<double, int> least_change;
	for (unsigned int i = 0; i < path_vector.size(); ++i) {
		double pl = path_vector[i].getLength();
		int change = path_vector[i].getChangePoints().size();
		int modes = path_vector[i].getNbrOfUniqueModes();
		map<double, int>::iterator ch_iter = least_change.find(pl);
		if (ch_iter == least_change.end()) {
			lengthPathSet[pl];
			lengthPathSet[pl].insert(i);
			least_change[pl] = change;
		} else {
			if (change < ch_iter->second
					&& modes < path_vector[i].getNbrOfUniqueModes()) {
				lengthPathSet[pl].clear();
				//lengthPathSet[pl].insert(i);
				least_change[pl] = change;

			}
			if (change == ch_iter->second
					&& modes == path_vector[i].getNbrOfUniqueModes()) {
				lengthPathSet[pl].insert(i);
			}

		}
	}
	map<double, set<int> >::iterator lengthPathIterFront =
			lengthPathSet.begin();
	unsigned long shortest = patNBParameters::the()->selectShortestPaths;
	for (unsigned long i = 0; i < shortest; ++i) {
		if (lengthPathIterFront != lengthPathSet.end()) {
			for (set<int>::const_iterator iter2 =
					lengthPathIterFront->second.begin();
					iter2 != lengthPathIterFront->second.end(); ++iter2) {
				if (path_probas[*iter2] > -DBL_MAX) {
					rtn_ids.insert(*iter2);
				}
			}
			lengthPathIterFront++;

		}
	}

	DEBUG_MESSAGE("	Paths selected by shortest: " << rtn_ids.size());
	return rtn_ids;
}

set<int> patMapMatchingIteration::selectPathsByDDR(
		vector<patMultiModalPathMatching>& path_vector,
		vector<double>& proba_vector) {

	//map< pair<const patArc*, TransportMode mode>, vector<int> > paths_grouped_by_arc;
	set < pair<const patArc*, TransportMode> > important_ddr =
			selectImportantDDR();
	set<int> path_ids;

	for (set<pair<const patArc*, TransportMode> >::iterator arc_iter =
			important_ddr.begin(); arc_iter != important_ddr.end();
			++arc_iter) {
		int change = 10000;
		int modes = 10000;
		vector<int> path_sample_set_ids;
		vector<double> pt_paths_proba;

		vector<int> path_sample_set_ids_all;
		vector<double> pt_paths_proba_all;

		double least_path_length = DBL_MAX;
		int least_length_path = 0;

		set<int> path_ids_temp;
		for (int i = 0; i < path_vector.size(); ++i) {
			if (path_vector[i].containsRoad(arc_iter->first,
					arc_iter->second)) {
				path_sample_set_ids_all.push_back(i);
				pt_paths_proba_all.push_back(proba_vector[i]);

				double current_path_length = path_vector[i].getLength();
				int curr_change = path_vector[i].getChangePoints().size();
				int curr_modes = path_vector[i].getNbrOfUniqueModes();

				if (curr_change < change || curr_modes < modes) {
					path_sample_set_ids.clear();
					pt_paths_proba.clear();
					if (curr_change < change) {
						change = curr_change;
					}
					if (curr_modes < modes) {
						modes = curr_modes;
					}
					least_path_length = current_path_length;
					least_length_path = i;
				}
				if (curr_change == change && curr_modes == modes) {
					path_sample_set_ids.push_back(i);
					pt_paths_proba.push_back(proba_vector[i]);
					if (current_path_length < least_path_length) {
						least_path_length = current_path_length;
						least_length_path = i;
					}
				}
			}
		}
		if (!pt_paths_proba.empty()) {
			path_ids_temp = selectPathsByRandom(pt_paths_proba.size(),
					pt_paths_proba, patNBParameters::the()->samplePathsByDDR);
			for (set<int>::iterator iter = path_ids_temp.begin();
					iter != path_ids_temp.end(); ++iter) {
				path_ids.insert(path_sample_set_ids[*iter]);
			}

			/*
			 if (proba_vector[least_length_path] > -DBL_MAX) {
			 path_ids.insert(least_length_path);
			 }
			 */

			//Without least mode constraint.
			if (patNBParameters::the()->sampleByDDRWithoutConstraint > 0) {
				path_ids_temp = selectPathsByRandom(pt_paths_proba_all.size(),
						pt_paths_proba_all,
						patNBParameters::the()->sampleByDDRWithoutConstraint);
				for (set<int>::iterator iter = path_ids_temp.begin();
						iter != path_ids_temp.end(); ++iter) {
					path_ids.insert(path_sample_set_ids_all[*iter]);
				}
			}
		}

	}

	DEBUG_MESSAGE("	Paths selected by DDR: " << path_ids.size());
	return path_ids;
}
/*

 select important ddr with simulations
 */
set<pair<const patArc*, TransportMode> > patMapMatchingIteration::selectImportantDDR() {
	set < pair<const patArc*, TransportMode> > important_ddr;
	const map<TransportMode, map<const patArc*, double> >* ddr_arcs =
			m_current_measurments->back()->getDDR()->getDDRArcs();
	double speed = m_gps->getSpeed();

	map<TransportMode, double> speed_proba;
	for (map<TransportMode, map<const patArc*, double> >::const_iterator mode_iter =
			ddr_arcs->begin(); mode_iter != ddr_arcs->end(); ++mode_iter) {
		speed_proba[mode_iter->first] = patSpeedDistributions::the()->pdf(speed,
				mode_iter->first);
		DEBUG_MESSAGE(
				"speed proba:" << getTransportModeString(mode_iter->first) << " " << speed_proba[mode_iter->first]);

	}

	for (map<TransportMode, map<const patArc*, double> >::const_iterator mode_iter =
			ddr_arcs->begin(); mode_iter != ddr_arcs->end(); ++mode_iter) {

		vector < pair<const patArc*, TransportMode> > ddr_vector;
		vector<double> proba_vector;
		for (map<const patArc*, double>::const_iterator arc_iter =
				mode_iter->second.begin(); arc_iter != mode_iter->second.end();
				++arc_iter) {
			ddr_vector.push_back(
					pair<const patArc*, TransportMode>(arc_iter->first,
							mode_iter->first));
			patError* err;
			struct link_ddr_range range =
					m_current_measurments->back()->getDDR()->getArcDDRRange(
							arc_iter->first, mode_iter->first, err);
			double ddr_part = range.upper_bound - range.lower_bound;
			proba_vector.push_back(arc_iter->second / ddr_part);
		}

		if (patNBParameters::the()->selectImportantDDRNumber
				>= ddr_vector.size()) {
			for (int i = 0; i < ddr_vector.size(); ++i) {
				important_ddr.insert(ddr_vector[i]);
			}
		} else {
			set<int> selected_ddr = selectPathsByRandom(proba_vector.size(),
					proba_vector,
					patNBParameters::the()->selectImportantDDRNumber);
			for (set<int>::iterator iter = selected_ddr.begin();
					iter != selected_ddr.end(); ++iter) {
				important_ddr.insert(ddr_vector[*iter]);
			}
		}
		DEBUG_MESSAGE(
				"importatnt ddr" << getTransportModeString(mode_iter->first) << ": " << important_ddr.size());
	}
	/*
	 if (patNBParameters::the()->selectImportantDDRNumber >= ddr_vector.size()) {
	 for (int i = 0; i < ddr_vector.size(); ++i) {
	 important_ddr.insert(ddr_vector[i]);
	 }
	 } else {
	 set<int> selected_ddr = selectPathsByRandom(proba_vector.size(),
	 proba_vector, patNBParameters::the()->selectImportantDDRNumber);
	 for (set<int>::iterator iter = selected_ddr.begin();
	 iter != selected_ddr.end(); ++iter) {
	 important_ddr.insert(ddr_vector[*iter]);
	 }
	 }
	 */
	return important_ddr;
}

void patMapMatchingIteration::selectPaths(
		map<patMultiModalPathMatching, double>& path_set) {
	DEBUG_MESSAGE("number of paths created" << path_set.size());
	m_paths.clear();
	calPathDDR(path_set);
	if (path_set.empty()) {
		return;
	}
	if (path_set.size()
			<= patNBParameters::the()->minGeneratedInterMediatePath) {
		m_paths = path_set;
	} else {

		vector<patMultiModalPathMatching> path_vector;
		vector<double> path_probas;
		for (map<patMultiModalPathMatching, double>::iterator path_iter =
				path_set.begin(); path_iter != path_set.end(); ++path_iter) {
			path_vector.push_back(path_iter->first);
			path_probas.push_back(path_iter->second);
		}

		set<int> path_ids;
		/*
		 if (patNBParameters::the()->selectPathCte == "pdf") {
		 selectPathsByPdf(pre_selected_paths, path_set);

		 }
		 */
		if (patNBParameters::the()->selectPathCte == "number") {
			/*
			 set<int> important_ddr_paths = selectPathByImportantDDR(path_vector,
			 path_probas);

			 path_ids.insert(important_ddr_paths.begin(),
			 important_ddr_paths.end());
			 */
			/*
			 set<int> shortest_paths = selectPathsByShortest(path_vector);
			 path_ids.insert(shortest_paths.begin(), shortest_paths.end());
			 */

			set<int> pdf_paths = selectPathsByRandom(path_vector.size(),
					path_probas, patNBParameters::the()->selectBestPaths);
			path_ids.insert(pdf_paths.begin(), pdf_paths.end());
			DEBUG_MESSAGE("	Paths selected randomly: " << path_ids.size());

			//set<int> mode_paths = selectPathsByMode(path_vector,path_probas);
			set<int> change_paths = selectPathsByChange(path_vector,
					path_probas);
			//path_ids.insert(mode_paths.begin(), mode_paths.end());
			//DEBUG_MESSAGE("AFTER SELECT mode SELECTATION" << path_ids.size());
			path_ids.insert(change_paths.begin(), change_paths.end());
			//DEBUG_MESSAGE("AFTER SELECT change SELECTATION" << path_ids.size());
		}
		/*
		 if (patNBParameters::the()->selectPathCte == "cdf") {
		 set<int> important_ddr_paths = selectPathByImportantDDR(path_vector,
		 path_probas);

		 set<int> shortest_paths = selectPathsByShortest(path_vector); //Select shortest paths

		 set<int> cdf_import_paths = selectPathsByCdf(path_vector,
		 path_probas);
		 path_ids.insert(important_ddr_paths.begin(),
		 important_ddr_paths.end());
		 path_ids.insert(shortest_paths.begin(), shortest_paths.end());
		 path_ids.insert(cdf_import_paths.begin(), cdf_import_paths.end());

		 }
		 */
		set<int> ddr_paths = selectPathsByDDR(path_vector, path_probas);
		path_ids.insert(ddr_paths.begin(), ddr_paths.end());
//DEBUG_MESSAGE("AFTER SELECT ddr" << path_ids.size());

		set<int> shortest_paths = selectPathsByShortest(path_vector,
				path_probas); //Select shortest paths
		path_ids.insert(shortest_paths.begin(), shortest_paths.end());
		//DEBUG_MESSAGE("AFTER SELECT shortest" << path_ids.size());

		for (set<int>::iterator iter = path_ids.begin(); iter != path_ids.end();
				++iter) {
			m_paths.insert(
					pair<patMultiModalPathMatching, double>(path_vector[*iter],
							path_probas[*iter]));
		}
	}

}
set<int> patMapMatchingIteration::selectPathsByMode(
		vector<patMultiModalPathMatching>& path_vector,
		vector<double>& path_probas) {
	short change = 10000;
	set<int> path_ids;
	int min_change_pt;
	vector<int> path_sample_set_ids;
	vector<double> pt_paths_proba;

	set<int> path_ids_temp;

	for (int i = 0; i < path_vector.size(); ++i) {
		if (path_vector[i].containsPT()) {
			int curr_change = path_vector[i].getNbrOfUniqueModes();
			if (curr_change < change) {
				path_sample_set_ids.clear();
				pt_paths_proba.clear();

				path_sample_set_ids.push_back(i);
				pt_paths_proba.push_back(path_probas[i]);
				change = curr_change;
			} else if (curr_change == change) {
				path_sample_set_ids.push_back(i);
				pt_paths_proba.push_back(path_probas[i]);
			}
		}
	}
	DEBUG_MESSAGE("least change pt" << path_sample_set_ids.size());
	path_ids_temp = selectPathsByRandom(pt_paths_proba.size(), pt_paths_proba,
			patNBParameters::the()->samplePathsByMode);
	for (set<int>::iterator iter = path_ids_temp.begin();
			iter != path_ids_temp.end(); ++iter) {
		path_ids.insert(path_sample_set_ids[*iter]);
	}
	DEBUG_MESSAGE("SELECTED BY MODE" << path_ids.size() << "change" << change);
	return path_ids;

}

set<int> patMapMatchingIteration::selectPathsByChange(
		vector<patMultiModalPathMatching>& path_vector,
		vector<double>& path_probas) {

	int modes = 10000;
	int change = 10000;
	set<int> path_ids;
	vector<int> least_change;
	vector<double> least_change_proba;
	double path_length;
	int selected;
	set<int> path_ids_temp;

	double least_path_length = DBL_MAX;
	int least_length_path;
	for (int i = 0; i < path_vector.size(); ++i) {
		int curr_change = path_vector[i].getChangePoints().size();
		int curr_modes = path_vector[i].getNbrOfUniqueModes();
		double current_path_length = path_vector[i].getLength();

		if (curr_change < change || curr_modes < modes) {
			least_change.clear();
			least_change_proba.clear();
			if (curr_change < change) {
				change = curr_change;
			}
			if (curr_modes < modes) {
				modes = curr_modes;
			}
			//least_path_length = least_path_length;
			//least_length_path = i;
		}
		if (curr_change == change && curr_modes == modes) {
			least_change.push_back(i);
			least_change_proba.push_back(path_probas[i]);
			if (current_path_length < least_path_length) {
				least_path_length = current_path_length;
				least_length_path = i;
			}
		}
	}
	path_ids_temp = selectPathsByRandom(least_change_proba.size(),
			least_change_proba, patNBParameters::the()->samplePathsByChange);
	for (set<int>::iterator iter = path_ids_temp.begin();
			iter != path_ids_temp.end(); ++iter) {
		path_ids.insert(least_change[*iter]);
	}
	//path_ids.insert(least_length_path);
	DEBUG_MESSAGE("SELECTED BY change (" << change << "): " << path_ids.size());
	return path_ids;

}

set<int> patMapMatchingIteration::selectPathsByRandom(int path_size,
		vector<double> path_probas, unsigned long count) {
	set<int> path_ids;
	if (path_size <= count) {
		for (int i = 0; i < path_size; ++i) {
			path_ids.insert(i);
		}
	} else {
		patSampleFromDiscreteUntilCdf s(path_probas);
		s.setAsLogLike();
		path_ids = s.sampleByCount(count);
	}
//	DEBUG_MESSAGE("sampled by count" << path_ids.size());
	return path_ids;
}
map<patMultiModalPathMatching, double> patMapMatchingIteration::getPaths() {
	return m_paths;
}
void patMapMatchingIteration::writeKML(string trip_name, int iteration_number) {
	//calPathDDR(m_paths);
	stringstream file_name;
	iteration_number += 1;
	file_name << patNBParameters::the()->resultPath;
	file_name << trip_name.substr(trip_name.rfind("/") + 1, trip_name.size());
	if (iteration_number < 1000000) {
		file_name << "_";
		file_name << iteration_number;
	}
	file_name << ".kml";
//	DEBUG_MESSAGE(patNBParameters::the()->resultPath);
//	DEBUG_MESSAGE(file_name.str().c_str());
//
	DEBUG_MESSAGE("write iteration " << iteration_number<<","<<file_name.str());
	{
		patWriteMMResultKML kml_writer(file_name.str());
		kml_writer.writeMeasurements(m_current_measurments);
		unsigned path_id = 0;

		double min_proba = DBL_MAX;
		double max_proba = -DBL_MAX;
		for (map<patMultiModalPathMatching, double>::const_iterator path_iter =
				m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {
			double path_proba = path_iter->second;
			min_proba = path_proba < min_proba ? path_proba : path_proba;
			max_proba = path_proba > max_proba ? path_proba : max_proba;
		}

		double sum_proba = 0.0;
		for (map<patMultiModalPathMatching, double>::const_iterator path_iter =
				m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {
			double path_proba = path_iter->second;
			if (max_proba - path_proba < 10.0) {
				sum_proba += exp(path_proba - max_proba + 10.0);
			}
		}

		for (map<patMultiModalPathMatching, double>::const_iterator path_iter =
				m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {

			if (max_proba - path_iter->second < 10.0) {
				double normal_proba = exp(path_iter->second - max_proba + 10.0)/sum_proba;
				map < string, string > attrs_true;
				attrs_true["id"] = boost::lexical_cast<string>(++path_id);
				attrs_true["proba"] = boost::lexical_cast<string>(normal_proba);
				kml_writer.writePath(path_iter->first, attrs_true);

			}
		}
		kml_writer.close();
	}
//
//	ofstream kml_file(file_name.str().c_str());
//	patCreateKmlStyles doc;
//	DocumentPtr document = doc.createStylesForKml();
//
//	KmlFactory* factory = KmlFactory::GetFactory();
//
//	FolderPtr gps_folder = factory->CreateFolder();
//	FolderPtr ddr_folder = factory->CreateFolder();
//
//	ddr_folder->set_name(string("DDR"));
//	gps_folder->set_name(string("GPS"));
//	for (int i = 0; i < m_current_measurments->size(); ++i) {
//
//		gps_folder->add_feature(m_current_measurments->at(i)->getKML(i + 1));
//		ddr_folder->add_feature(
//				m_current_measurments->at(i)->getDDR()->getKML(i + 1));
//	}
//
//	FolderPtr path_folder = factory->CreateFolder();
//	path_folder->set_name("Path");
//
//	stringstream ss;
//	ss << m_paths.size();
//	path_folder->set_description(ss.str());
//	int i = 0;
//	for (map<patMultiModalPathMatching, double>::const_iterator path_iter =
//			m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {
//		++i;
////		DEBUG_MESSAGE(m_paths.size()<< " "<<i);
//		FolderPtr p = path_iter->first.getKML(i);
//		stringstream p_proba;
//		p_proba << path_iter->second;
//		p->set_description(p_proba.str());
//
//		path_folder->add_feature(p);
//	}
//
//	document->add_feature(path_folder);
//	document->add_feature(gps_folder);
//	document->add_feature(ddr_folder);
//	KmlPtr kml = factory->CreateKml();
//	kml->set_feature(document);
//
//	kml_file << kmldom::SerializePretty(kml);
//	kml_file.close();
}
