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
#include "patGPSDDR.h"
#include "patSampleFromDiscreteUntilCdf.h"
#include "patNBParameters.h"
#include <vector>
#include "patMultiModalPathProba.h"
#include "patPathExtension.h"
#include "patDiscreteDistribution.h"
#include "patUnixUniform.h"
#include <time.h>
#include "patCreateKmlStyles.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include "kml/dom.h"
#include "patNetworkBase.h"
using kmldom::DocumentPtr;
using kmldom::KmlFactory;
using kmldom::KmlPtr;
using kmldom::PlacemarkPtr;
using kmldom::FolderPtr;

patMapMatchingIteration::patMapMatchingIteration(
		const patNetworkEnvironment* environment, patGpsPoint* gps,
		vector<patMeasurement*>* measurement_sequence,
		set<patArcTransition>* arc_tran_set,
		map<patMultiModalPath, double> paths) :
		m_environment(environment), m_gps(gps), m_current_measurments(
				measurement_sequence), m_arc_tran_set(arc_tran_set), m_paths(
				paths) {
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
		DEBUG_MESSAGE("network "<<network_iter->first);
		bool found_ddr = false;
		set<const patArc*> all_arcs =  network_iter->second->getAllArcs();
		for(set<const patArc*>::iterator arc_iter = all_arcs.begin();arc_iter!=all_arcs.end();++arc_iter){
			if(m_gps->getDDR()->detArcDDR(*arc_iter, network_iter->second->getTransportMode())){
				found_ddr=true;
			}

		}
		DEBUG_MESSAGE("found ddr yes/no? "<<found_ddr);
	}
	/*
	for (map<unsigned long, patArc>::const_iterator iter =
			m_environment->getNetworkElements().getAllArcs()->begin();
			iter != m_environment->getNetworkElements().getAllArcs()->end();
			iter++) {

		const map<string, patNetworkBase*>* all_networks =
				m_environment->getAllNetworks();
		for (map<string, patNetworkBase*>::const_iterator network_iter =
				all_networks->begin(); network_iter != all_networks->end();
				++network_iter) {
			DEBUG_MESSAGE(network_iter->first);
			TransportMode mode = network_iter->second->getTransportMode();
			if (!network_iter->second->getRoadsContainArc(&(iter->second)).empty()) {
				DEBUG_MESSAGE(iter->second);
				m_gps->getDDR()->detArcDDR(&(iter->second), mode);
			}

		}
	}
*/
	if (m_gps->getDDR()->empty()) {
		DEBUG_MESSAGE("no ddr");
		return;
	}
	DEBUG_MESSAGE("ddr arc size:" << m_gps->getDDR()->size());
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

	map<patMultiModalPath, double> paths_temp;
//Connect DDR in all networks.

	const map<string, patNetworkBase*>* all_networks =
			m_environment->getAllNetworks();
	for (map<string, patNetworkBase*>::const_iterator network_iter =
			all_networks->begin(); network_iter != all_networks->end();
			++network_iter) {
		if (m_gps->getSpeed() > network_iter->second->getMaxSpeed()) {
			DEBUG_MESSAGE(
					"speed " << m_gps->getSpeed() << " too high skip network: "
							<< network_iter->first);
			continue;
		}
		patPathExtension path_initiation(network_iter->second);

		const map<const patArc*, double>* arc_list =
				m_gps->getDDR()->getDDRArcs(
						network_iter->second->getTransportMode());
		if (arc_list == NULL) {
			continue;
		}
		set<patMultiModalPath> new_path_set = path_initiation.init(arc_list);
		for (set<patMultiModalPath>::iterator path_iter = new_path_set.begin();
				path_iter != new_path_set.end(); ++path_iter) {
			paths_temp[*path_iter] = m_gps->getDDR()->computePathDDRRaw(
					*path_iter);
		}
	}
	m_paths = paths_temp;
	finalizeOneStage();
}

/**
 * Sort the paths by their end nodes.
 * @return a map with
 * <ul>
 * <li> key:  end node pointer.</li>
 * <li> value: set of corresponding path with the same end node</li>
 * </ul>
 */
map<const patNode*, set<const patMultiModalPath*> > patMapMatchingIteration::sortPathsByEndNodes() const {
	map<const patNode*, set<const patMultiModalPath*> > end_nodes;
	for (map<patMultiModalPath, double>::const_iterator path_iter =
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
	double maxSpeed = currGpsPoint->getMaxSpeed(prevGpsPoint);
	if (maxSpeed > network->getMaxSpeed()) {
		maxSpeed = network->getMaxSpeed();
	}
	if (maxSpeed < network->getMinSpeed()) {
		maxSpeed = network->getMinSpeed();
	}

	return maxSpeed * (m_gps->getTimeStamp() - prevGpsPoint->getTimeStamp())
			* patNBParameters::the()->maxDistanceRatioSP;
}

/**
 * Map matching for normal iteration.
 */
void patMapMatchingIteration::normalIteration(patGpsPoint* prevNormalGps,
		vector<patMeasurement*>& inter_data, patError*& err) {
//implement error
//	DEBUG_MESSAGE(m_gps);
	//DEBUG_MESSAGE(m_gps->getDDR());
	m_gps->getDDR()->detInherentDDR(prevNormalGps->getDDR());
	double travel_time = m_current_measurments->back()->getTimeStamp()
			- m_current_measurments->front()->getTimeStamp();
	map<patMultiModalPath, double> paths_temp;
	for (map<patMultiModalPath, double>::iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {
		RoadTravel road_travel = path_iter->first.back_road_travel();
		bool in_domain = false;
		list<const patArc*> arc_list =road_travel.road->getArcList();

		for (list<const patArc*>::const_iterator arc_iter =
				arc_list.begin(); arc_iter != arc_list.end(); ++arc_iter) {

			if (m_gps->getDDR()->detArcDDR(*arc_iter, road_travel.mode)) {
				in_domain = true;
			}
		}

		if (in_domain) {

			const_cast<patMultiModalPath*>(&path_iter->first)->proposeStop(
					m_environment, m_gps->getSpeed(), 10.0);
			paths_temp[path_iter->first] = 0.0;
		}
	}
	DEBUG_MESSAGE(
			"Inherits " << paths_temp.size() << " paths from the previous DDR");
	const map<string, patNetworkBase*>* all_networks =
			m_environment->getAllNetworks();

	const map<const patNode*, set<const patMultiModalPath*> > up_stream_paths_by_nodes =
			sortPathsByEndNodes();
	;
	DEBUG_MESSAGE(
			"Number of end node of the previous path set:"
					<< up_stream_paths_by_nodes.size());

	map<TransportMode, set<const patArc*> > inter_sec_arcs;
	for (map<const patNode*, set<const patMultiModalPath*> >::const_iterator end_node_iter =
			up_stream_paths_by_nodes.begin();
			end_node_iter != up_stream_paths_by_nodes.end(); ++end_node_iter) {
		//Loop over all up stream paths with the same node.
//		DEBUG_MESSAGE("OK");
		DEBUG_MESSAGE("start from node" << (end_node_iter->first->getName()));
		for (map<string, patNetworkBase*>::const_iterator network_iter =
				all_networks->begin(); network_iter != all_networks->end();
				++network_iter) {

			//Loop over all transport networks and extend the paths
			if (m_gps->getSpeed() > network_iter->second->getMaxSpeed()) {
				DEBUG_MESSAGE(
						"speed " << m_gps->getSpeed()
								<< " too high skip network: "
								<< network_iter->first);
				continue;
			}

			if (3.6 * m_gps->getMaxSpeed(prevNormalGps)
					< network_iter->second->getMinSpeed()) {
				DEBUG_MESSAGE(
						"speed " << 3.6 * m_gps->getMaxSpeed(prevNormalGps)
								<< " too low skip network: "
								<< network_iter->first);
				continue;
			}
			TransportMode curr_mode = network_iter->second->getTransportMode();
			double max_distance_from_las_gps = getDistanceCeilFromGPS(m_gps,
					prevNormalGps, network_iter->second);
			DEBUG_MESSAGE("  find in network: " << network_iter->first);
			if (network_iter->second->hasDownStream(end_node_iter->first)) {
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
					return;
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
					list<const patRoadBase*> shortest_path_roads =
							shortest_path_tree->getShortestPathTo(
									arc_iter->second->getUpNode());
					bool create_path_success = true;
					patMultiModalPath shortest_path(shortest_path_roads,
							create_path_success);
					if (create_path_success == false) {
						DEBUG_MESSAGE("invalid path");
						continue;
					}
					if (shortest_path.addRoadTravelToBack(arc_iter->second)
							== false) {
						DEBUG_MESSAGE("invalid path");
						continue;
					}
					shortest_path.setUnimodalTransportMode(curr_mode);

					if (!shortest_path.isValid()) {
						DEBUG_MESSAGE("invalid path");
						continue;
					} else {
						downstream_paths.insert(shortest_path);
					}
				}

				for (set<patMultiModalPath>::iterator d_p_iter =
						downstream_paths.begin();
						d_p_iter != downstream_paths.end(); ++d_p_iter) {
					list<const patArc*> arc_list = d_p_iter->getArcList();
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
					DEBUG_MESSAGE(downstream_paths.size());
				}
				patPathExtension path_extension(network_iter->second);
				set<patMultiModalPath> new_paths =
						path_extension.extendFromNode(&(end_node_iter->second),
								end_node_iter->first, &downstream_paths,
								m_gps->getDDR()); //Extend paths with connecting paths and the connection node.
				DEBUG_MESSAGE(
						"  new paths created in network " << network_iter->first
								<< ":" << new_paths.size());
				for (set<patMultiModalPath>::iterator path_iter =
						new_paths.begin(); path_iter != new_paths.end();
						++path_iter) {
					double min_travel_time = (*path_iter).getMinimumTravelTime(
							m_environment, ddr_first, ddr_last);
					if (patNBParameters::the()->minPathTravelTimeRatio
							* travel_time < min_travel_time) {
						DEBUG_MESSAGE(
								"travel time: " << travel_time
										<< "; min travel time: "
										<< min_travel_time);
						continue;
					}
					if (!(*path_iter).isReasonableModeChange()) {
						//DEBUG_MESSAGE("unreasonable change");
						continue;
					}
					if (paths_temp.find(*path_iter) == paths_temp.end()) {

						//mmm(*path_iter).proposeStop(m_environment, m_gps->getSpeed(), 10.0);
						paths_temp.insert(
								pair<patMultiModalPath, double>(*path_iter,
										0.0));
					}
				}
			}
		}
	}
	DEBUG_MESSAGE(
			"DDR size" << m_current_measurments->back()->getDDR()->size());
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
	dealInterData(inter_data, prevNormalGps, inter_sec_arcs);
	m_current_measurments->push_back(last_measurement);
	selectPaths(paths_temp); //Select paths
	finalizeOneStage();
	DEBUG_MESSAGE("finish an iteration with paths" << m_paths.size());
}

void patMapMatchingIteration::lastIteration(vector<patMeasurement*>& inter_data,
		patGpsPoint* prevNormalGps) {
	map<TransportMode, set<const patArc*> > empty_arc_set;
	dealInterData(inter_data, prevNormalGps, empty_arc_set);
}
/**
 * Deal with intermediate low speed GPS points
 */
void patMapMatchingIteration::dealInterData(vector<patMeasurement*>& inter_data,
		patGpsPoint* prevNormalGps,
		map<TransportMode, set<const patArc*> >& inter_sec_arcs) {
	if (inter_data.empty()) {
		return;
	}

	for (vector<patMeasurement*>::iterator data_iter = inter_data.begin();
			data_iter != inter_data.end(); ++data_iter) {

		(*data_iter)->getDDR()->detInherentDDR(prevNormalGps->getDDR());
		for (map<TransportMode, set<const patArc*> >::iterator mode_iter =
				inter_sec_arcs.begin(); mode_iter != inter_sec_arcs.end();
				++mode_iter) {
			for (set<const patArc*>::iterator arc_iter =
					mode_iter->second.begin();
					arc_iter != mode_iter->second.end(); ++arc_iter) {

				(*data_iter)->getDDR()->detArcDDR(*arc_iter, mode_iter->first);
			}
		}
		if (!(*data_iter)->getDDR()->empty()) {
			m_current_measurments->push_back(*data_iter);
		}
	}
}
/**
 * Calculate path DDR.
 */
void patMapMatchingIteration::calPathDDR(
		map<patMultiModalPath, double>& path_set) {
	for (map<patMultiModalPath, double>::iterator path_iter = path_set.begin();
			path_iter != path_set.end(); ++path_iter) {

//		DEBUG_MESSAGE("algo:"<<patNBParameters::the()->algoInSelection);

		patMultiModalPathProba::patMultiModalPathProba path_proba(
				&(path_iter->first), m_current_measurments, m_arc_tran_set);
		double proba_value = path_proba.run("complex");

//		DEBUG_MESSAGE("path DDR"<<proba_value);
		path_iter->second = proba_value;
	}

}

void patMapMatchingIteration::finalizeOneStage() {

	if (m_paths.size() == 0 || m_paths.empty()) {
		m_current_measurments->pop_back();
		DEBUG_MESSAGE("no path generated, invalid gps");
	}
	DEBUG_MESSAGE("final ddr " << m_gps->getDDR()->size());

	DEBUG_MESSAGE("number of paths generated:" << m_paths.size());
}

/**
 * Sum the likelihood of all paths
 */
double patMapMatchingIteration::sumProba(
		const map<patMultiModalPath, double>& path_set) {
	double r = 0.0;
	for (map<patMultiModalPath, double>::const_iterator iter1 =
			path_set.begin(); iter1 != path_set.end(); ++iter1) {
		r += iter1->second;
	}

	return r;
}
void patMapMatchingIteration::selectPathsByPdf(
		list<patMultiModalPath*>& pre_selected_paths,
		map<patMultiModalPath, double>& path_set) {

	double proba_sum = sumProba(path_set);
	set<patMultiModalPath*> tmp_set;

	tmp_set.insert(pre_selected_paths.begin(), pre_selected_paths.end());
	for (map<patMultiModalPath, double>::iterator iter1 = path_set.begin();
			iter1 != path_set.end();) {
		if (iter1->second * patNBParameters::the()->selectPathInversePercent
				>= proba_sum) {
			tmp_set.insert(const_cast<patMultiModalPath*>(&(iter1->first)));
			path_set.erase(iter1++);
		} else {
			iter1++;
		}
	}
	if (tmp_set.empty()) {
		//FIXME selectPathsByCdf(pre_selected_paths, path_set);
	} else {
		for (map<patMultiModalPath, double>::iterator iter1 = path_set.begin();
				iter1 != path_set.end();) {

			if (tmp_set.find(const_cast<patMultiModalPath*>(&(iter1->first)))
					== tmp_set.end()) {
				path_set.erase(iter1++);
			} else {
				iter1++;
			}
		}

	}

}

/**
 * Select paths by CDF
 * <ul>
 * <li> Select shortest paths.</li>
 * </ul>
 */
set<int> patMapMatchingIteration::selectPathsByCdf(
		vector<patMultiModalPath> path_vector, vector<double> path_probas) {

	patSampleFromDiscreteUntilCdf sample(path_probas);
	set<int> sampled = sample.sample(
			patNBParameters::the()->selectPathCdfThreshold);
	DEBUG_MESSAGE("paths selected by cdf" << sampled.size());

	return sampled;

}

void patMapMatchingIteration::selectPathsByNumber(
		list<patMultiModalPath*>& pre_selected_paths,
		map<patMultiModalPath, double>& path_set) {
	map<double, set<const patMultiModalPath*> > paths_by_proba;

//Get
	for (map<patMultiModalPath, double>::iterator iter1 = path_set.begin();
			iter1 != path_set.end(); ++iter1) {
		const patMultiModalPath* currPath = &(iter1->first);

		paths_by_proba[iter1->second];
		paths_by_proba[iter1->second].insert(currPath);
	}

	map<double, set<const patMultiModalPath*> >::iterator ddrPathIterBack =
			paths_by_proba.end();
	unsigned long best = patNBParameters::the()->selectBestPaths;
	unsigned long worst = patNBParameters::the()->selectWorstPaths;
	unsigned long shortest = patNBParameters::the()->selectShortestPaths;

	set<const patMultiModalPath*> tmpPathSet;
	tmpPathSet.insert(pre_selected_paths.begin(), pre_selected_paths.end());
	for (unsigned long i = 0; i < best; ++i) {
		if (ddrPathIterBack != paths_by_proba.begin()) {
			ddrPathIterBack--;
			for (set<const patMultiModalPath*>::iterator path_iter =
					ddrPathIterBack->second.begin();
					path_iter != ddrPathIterBack->second.end(); ++path_iter) {
				tmpPathSet.insert(*path_iter);
			}
		}

	}
	for (map<patMultiModalPath, double>::iterator pIter = path_set.begin();
			pIter != path_set.end();) {

		if (tmpPathSet.find(&(pIter->first)) == tmpPathSet.end()) {
			path_set.erase(pIter++);
		} else {
			pIter++;
		}
	}

}

set<int> patMapMatchingIteration::selectPathsByShortest(
		vector<patMultiModalPath> path_vector) {

	set<int> rtn_ids;
	map<double, set<int> > lengthPathSet;

	for (unsigned int i = 0; i < path_vector.size(); ++i) {
		double pl = path_vector[i].getLength();
		lengthPathSet[pl];
		lengthPathSet[pl].insert(i);
	}
	map<double, set<int> >::iterator lengthPathIterFront =
			lengthPathSet.begin();
	unsigned long shortest = patNBParameters::the()->selectShortestPaths;
	for (unsigned long i = 0; i < shortest; ++i) {
		if (lengthPathIterFront != lengthPathSet.end()) {
			rtn_ids.insert(lengthPathIterFront->second.begin(),
					lengthPathIterFront->second.end());
			lengthPathIterFront++;

		}
	}
	DEBUG_MESSAGE("paths selected by shortest" << rtn_ids.size());
	return rtn_ids;
}

set<int> patMapMatchingIteration::selectPathByImportantDDR(
		vector<patMultiModalPath> path_vector, vector<double> proba_vector) {
	/*
	 set<int> rtn_ids;

	 set<const patArc*> impDDR = selectImportantDDR();
	 DEBUG_MESSAGE("number of important ddrs" << impDDR.size());

	 for (set<const patArc*>::iterator impIter = impDDR.begin();
	 impIter != impDDR.end(); ++impIter) {

	 vector<int> pathViaImpDDR;
	 vector<double> pathDDR;

	 for (int i = 0; i < path_vector.size(); ++i) {
	 if (path_vector[i].isLinkInPath(*impIter)) {
	 pathViaImpDDR.push_back(i);
	 pathDDR.push_back(proba_vector[i]);
	 }
	 }
	 if (pathDDR.size() == 0) {
	 continue;
	 }
	 patUnixUniform m_ran_uniform(time(NULL));
	 patDiscreteDistribution discreteDraw(&pathDDR, &m_ran_uniform);
	 int d = discreteDraw();

	 rtn_ids.insert(pathViaImpDDR[d]);
	 }

	 DEBUG_MESSAGE("paths selected by import ddr" << rtn_ids.size());
	 return rtn_ids;
	 */
}
/*

 select important ddr with simulations
 */
set<const patArc*> patMapMatchingIteration::selectImportantDDR() {
	/*
	 set<const patArc*> importantDDR;
	 const map<const patArc*, double>* linkDDR =
	 m_current_gps_sequence->back()->getDDR()->getDDRArcs();
	 if (patNBParameters::the()->selectImportantDDRCdf == 1.0) {
	 DEBUG_MESSAGE("the whole ddr is selected as import ddr");
	 for (map<const patArc*, double>::const_iterator ddrIter =
	 linkDDR->begin(); ddrIter != linkDDR->end(); ++ddrIter) {
	 importantDDR.insert(ddrIter->first);
	 }
	 return importantDDR;
	 }

	 double sumTotal = m_current_gps_sequence->back()->getDDR()->calDDRSum();

	 vector<const patArc*> ddrArcs;
	 vector<double> ddrValues;

	 for (map<const patArc*, double>::const_iterator ddrIter = linkDDR->begin();
	 ddrIter != linkDDR->end(); ++ddrIter) {
	 ddrArcs.push_back(ddrIter->first);
	 ddrValues.push_back(ddrIter->second);
	 }

	 patUnixUniform ranNumUniform(time(NULL));
	 patDiscreteDistribution discreteDraw(&ddrValues, &ranNumUniform);

	 double selectedDDRSum = 0.0;

	 while (selectedDDRSum / sumTotal
	 < patNBParameters::the()->selectImportantDDRCdf
	 || m_current_gps_sequence->back()->getDDR()->size()
	 < patNBParameters::the()->minDomainSize) {

	 unsigned long selectedArcIndex = discreteDraw();
	 pair<set<const patArc*>::iterator, bool> insertResult =
	 importantDDR.insert(ddrArcs[selectedArcIndex]);
	 if (insertResult.second == true) {
	 //DEBUG_MESSAGE("selected ddr"<<ddrValues[selectedArcIndex]);
	 selectedDDRSum += ddrValues[selectedArcIndex];
	 }

	 }

	 return importantDDR;
	 */
}

void patMapMatchingIteration::selectPaths(
		map<patMultiModalPath, double>& path_set) {
	DEBUG_MESSAGE("number of paths created" << path_set.size());
	m_paths.clear();
	if (path_set.empty()) {
		return;
	}
	if (path_set.size()
			<= patNBParameters::the()->minGeneratedInterMediatePath) {
		m_paths = path_set;
	} else {

		calPathDDR(path_set);
		vector<patMultiModalPath> path_vector;
		vector<double> path_probas;
		for (map<patMultiModalPath, double>::iterator path_iter =
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

			set<int> pdf_paths = selectPathsByRandom(path_vector, path_probas,
					patNBParameters::the()->selectBestPaths);

			path_ids.insert(pdf_paths.begin(), pdf_paths.end());
		}
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
		for (set<int>::iterator iter = path_ids.begin(); iter != path_ids.end();
				++iter) {
			m_paths.insert(
					pair<patMultiModalPath, double>(path_vector[*iter],
							path_probas[*iter]));
		}
	}

}

set<int> patMapMatchingIteration::selectPathsByRandom(
		vector<patMultiModalPath> path_vector, vector<double> path_probas,
		unsigned long count) {
	set<int> path_ids;
	if (path_vector.size() <= count) {
		for (int i = 0; i < path_vector.size(); ++i) {
			path_ids.insert(i);
		}
	} else {
		patSampleFromDiscreteUntilCdf s(path_probas);
		path_ids = s.sampleByCount(count);
	}
	DEBUG_MESSAGE("sampled by count" << path_ids.size());
	return path_ids;
}
map<patMultiModalPath, double> patMapMatchingIteration::getPaths() {
	return m_paths;
}
void patMapMatchingIteration::writeKML(int iteration_number) const {

	stringstream file_name;
	iteration_number += 1;
	file_name << time(NULL);
	file_name << "_";
	file_name << iteration_number;
	file_name << ".kml";
	ofstream kml_file(file_name.str().c_str());
	DEBUG_MESSAGE("write iteration " << iteration_number);
	patCreateKmlStyles doc;
	DocumentPtr document = doc.createStylesForKml();

	KmlFactory* factory = KmlFactory::GetFactory();

	FolderPtr gps_folder = factory->CreateFolder();
	FolderPtr ddr_folder = factory->CreateFolder();

	ddr_folder->set_name(string("DDR"));
	gps_folder->set_name(string("GPS"));
	for (int i = 0; i < m_current_measurments->size(); ++i) {

		gps_folder->add_feature(m_current_measurments->at(i)->getKML(i + 1));
		ddr_folder->add_feature(
				m_current_measurments->at(i)->getDDR()->getKML(i + 1));
	}

	FolderPtr path_folder = factory->CreateFolder();
	path_folder->set_name("Path");

	stringstream ss;
	ss << m_paths.size();
	path_folder->set_description(ss.str());
	int i = 0;
	for (map<patMultiModalPath, double>::const_iterator path_iter =
			m_paths.begin(); path_iter != m_paths.end(); ++path_iter) {
		++i;
//		DEBUG_MESSAGE(m_paths.size()<< " "<<i);
		FolderPtr p = path_iter->first.getKML(i);
		stringstream p_proba;
		p_proba << path_iter->second;
		p->set_description(p_proba.str());

		path_folder->add_feature(p);
	}

	document->add_feature(path_folder);
	document->add_feature(gps_folder);
	document->add_feature(ddr_folder);
	KmlPtr kml = factory->CreateKml();
	kml->set_feature(document);

	kml_file << kmldom::SerializePretty(kml);
	kml_file.close();
}
