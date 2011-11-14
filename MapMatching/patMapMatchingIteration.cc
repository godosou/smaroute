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
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/discrete_distribution.hpp>

patMapMatchingIteration::patMapMatchingIteration(
		vector<patGpsPoint*>* gps_history, patGpsPoint* gps,
		set<patArcTransition*>* arc_tran_set) :
		m_gps(gps), gps_ddr(m_gps), m_gps_history(gps_history), m_arc_tran_set(
				arc_tran_set) {
}

patMapMatchingIteration::~patMapMatchingIteration() {
}

/**
 * Generate DDR for the first iteration.
 * Loop over all arcs and determine each one.
 * TODO: improve the efficiency.
 */
void patMapMatchingIteration::genInitDDR() {
	for (map<patULong, patArc>::const_iterator iter =
			m_environment->getNetworkElements()->getAllArcs()->begin();
			iter != m_environment->getNetworkElements()->getAllArcs()->end();
			iter++) {
		m_gps_ddr.detArcDDR(&(iter->second));
	}

	if (m_gps_ddr.empty()) {
		DEBUG_MESSAGE("no ddr");
		return;
	}
	DEBUG_MESSAGE("ddr arc size:" << m_gps_ddr.size());

	selectInitBySortDDR(patNBParameters::the()->maxDomainSizeOrig); //Select DDR by limit the size of the domain
	DEBUG_MESSAGE("after selection:" << m_gps_ddr.size());
	m_gps_ddr.calDDRSum();
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
	genInitDDR(); //Generate DDR for first iteration.

	//Connect DDR in all networks.
	const map<string, patNetworkBase>* all_networks =
			m_environment->getAllNetworks();
	for (map<string, patNetworkBase>::const_iterator network_iter =
			all_networks->begin(); network_iter != all_networks->end();
			++network_iter) {
		patPathExtension path_initiation(&(network_iter->second));
		path_initiation->init(m_gps_ddr);
	}
}

/**
 * Sort the paths by their end nodes.
 * @return a map with
 * <ul>
 * <li> key:  end node pointer.</li>
 * <li> value: set of corresponding path with the same end node</li>
 * </ul>
 */
map<const patNode*, set<const patPathJ*> > patMapMatchingIteration::sortPathsByEndNodes() {
	map<const patNode*, set<const patPathJ*> > end_nodes;
	for (map<patPathJ, double>::const_iterator path_iter = m_paths.begin();
			path_iter != m_paths.end(); ++path_iter) {
		patNode* curr_end_node = path_iter->first.getLastNode();
		if (end_nodes.find(curr_end_node) == end_nodes.end()) {
			end_nodes[curr_end_node] = set<patPathJ*>();
		}
		end_nodes[curr_end_node].insert(&path_iter->first);

	}
	return end_nodes;
}

/**
 * Map matching for normal iteration.
 */
void patMapMatchingIteration::normalIteration(patGpsPoint* prevNormalGps, patError*& err) {
	const map<string, patNetworkBase>* all_networks =
			m_environment->getAllNetworks();

	const map<const patNode*, set<const patPathJ*> > up_stream_paths_by_nodes =
			prev_iteration->sortPathsByEndNodes();

	double max_distance_from_las_gps = m_gps_ddr->getDistanceCeilFromPrevDDR(
			prevNormalGps);

	map<patPathJ, double> paths_temp;
	for (map<const patNode*, set<const patPathJ*> >::const_iterator end_node_iter =
			up_stream_paths_by_nodes.begin();
			end_node_iter != up_stream_paths_by_nodes.end(); ++end_node_iter) {
		//Loop over all up stream paths with the same node.

		for (map<string, patNetworkBase>::const_iterator network_iter =
				all_networks->begin(); network_iter != all_networks->end();
				++network_iter) {
			//Loop over all transport networks and extend the paths

			if (network_iter->second.hasDownStream(end_node_iter->first)) {
				//if the node has outgoing links, we extend.

				//Build shortest path tree from a node.
				set < pair<const patArc*, const patRoadBase*> > ddr_arcs; //Arc is the arc in ddr, roadbase is the corresponding road.

				set<patPathJ> downstream_paths; //Stores down stream paths
				DEBUG_MESSAGE("Extend node from " << *end_node_iter->first);

				patShortestPath shortest_path_from_node(&(network_iter->second),
						m_gps_ddr, &ddr_arcs, max_distance_from_las_gps); //build shortest path from an end node.

				if (shortest_path_from_node.computeShortestPathTree(node)
						== false) { //Build shortest path tree. Quit if false.

					WARNING("wrong in shortest path tree");
					return;
				}
				DEBUG_MESSAGE("Downstream roads in DDR: " << m_gps_ddr.size());
				if (m_gps_ddr.empty() == true) {
					return;
				}

				patShortestPathTreeGeneral* shortest_path_tree =
						shortest_path_from_node.getTree();

				//For each arc in DDR, get the connecting path.
				for (set<pair<patArc*, patRoadBase*> >::iterator arc_iter =
						ddr_arcs.begin(); arc_iter != ddr_arcs.end();
						++arc_iter) {
					patPathJ shortest_path =
							shortest_path_tree->getShortestPathTo(
									ddr_arcs->second->getUpNode());
					if (!shortest_path.isValid()) { //TODO implment patPathJ::isValid()
						continue;
					} else {
						downstream_paths.insert(shortest_path);
					}
				}

				patPathExtension path_extension(&(network_iter->second));
				set<patPathJ> new_paths = path_extension.extendFromNode(
						&end_node_iter->second, end_node_iter->first,
						&downstream_paths, &ddr); //Extend paths with connecting paths and the connection node.
				paths_temp.insert(new_paths.begin(), new_paths.end()); //Insert the new paths
			}
		}
	}
	selectPaths(paths_temp); //Select paths
	finalizeOneStage(paths_temp);
}

/**
 * Deal with intermediate low speed GPS points
 */
void patMapMatchingIteration::dealLowSpeedPoints(const vector<patGpsPoint*>& lowSpeedGpsPoints,
		const set<patArc*>& interArcs) {

	for (vector<patGpsPoint*>::iterator gpsIter = lowSpeedGpsPoints.begin();
			gpsIter != lowSpeedGpsPoints.end(); ++gpsIter) {
		patGpsPoint* currGps = *gpsIter;
		currGps->getDDR()->lowSpeedDDR(interArcs);
		if (!currGps->getDDR()->empty()) {
			m_gps_history.push_back(currGps);
		}

	}

}
/**
 * Calculate path DDR.
 */
void patMapMatchingIteration::calPathDDR() {
	for (map<patPathJ, double>::iterator path_iter = m_paths->begin();
			path_iter != path_set->end(); ++path_iter) {

		//DEBUG_MESSAGE("algo:"<<patNBParameters::the()->algoInSelection);

		patMultiModalPathProba::patMultiModalPathProba(&path_iter->first, this,
				m_arc_tran_set);
		double proba_value = path_proba.run("complex");

		//DEBUG_MESSAGE("path DDR"<<ddr);
		iter->second = proba_value;
	}

}

void patMapMatchingIteration::finalizeOneStage(map<patPathJ, patReal> path_set) {

	m_paths = path_set;
	if (m_paths->size() == 0 || m_paths->empty()) {
		DEBUG_MESSAGE("no path generated, invalid gps");
	}
	else{
		m_gps_history.push_back(m_gps)
	}
	DEBUG_MESSAGE("final ddr " << m_gps_ddr->size());

}


/**
 * Sum the likelihood of all paths
 */
double patMapMatchingIteration::sumProba(
		const map<patPathJ, double>& path_set) {
	double r = 0.0;
	for (map<patPathJ, double>::iterator iter1 = path_set.begin();
			iter1 != path_set->end(); ++iter1) {
		r += iter1->second;
	}

	return r;
}
void patMapMatchingIteration::selectPathsByPdf(
		list<patPathJ*>& pre_selected_paths, map<patPathJ, double>& path_set) {

	double proba_sum = sumProba(path_set);
	set<patPathJ*> tmp_set;

	tmp_set.insert(pre_selected_paths.begin(), pre_selected_paths.end());
	for (map<patPathJ, double>::iterator iter1 = path_set.begin();
			iter1 != path_set.end();) {
		if (iter1->second * patNBParameters::the()->selectPathInversePercent
				>= proba_sum) {
			tmp_set.insert(&(iter1->first));
			path_set->erase(iter1++);
		} else {
			iter1++
		}
	}
	if (tmp_set.empty()) {
		selectPathsByCdf(pre_selected_paths, path_set);
	} else {
		for (map<patPathJ, double>::iterator iter1 = path_set.begin();
				iter1 != path_set.end();) {

			if (tmp_set.find(&(iter1->first)) == tmp_set.end()) {
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
void patMapMatchingIteration::selectPathsByCdf(
		list<patPathJ*>& pre_selected_paths, map<patPathJ, double>& path_set) {

	set<patPathJ*> tmp_set;
	selectPathsByShortest(path_set, &tmp_set); //Select shortest paths

	DEBUG_MESSAGE("select shortest" << tmp_set.size());
	tmp_set.insert(pre_selected_paths.begin(), pre_selected_paths.end());

	double total_proba = sumProba(path_set);
	double th = patNBParameters::the()->selectPathCdfThreshold * totalProba;

	//Order the paths with DDR
	map<double, set<patPathJ*> > paths_ordered_by_ddr;
	for (map<patPathJ, double>::iterator iter1 = path_set->begin();
			iter1 != path_set->end(); ++iter1) {
		if (iter1->second == 0.0) {
			continue;
		}
		patPathJ* currPath = const_cast<patPathJ*>(&(iter1->first));

		if (paths_ordered_by_ddr.find(iter1->second)
				== paths_ordered_by_ddr.end()) {

			paths_ordered_by_ddr[iter1->second] = set<patPathJ*>();
		}
		paths_ordered_by_ddr[iter1->second].insert(currPath);

	}

	map<double, set<patPathJ*> >::iterator path_ddr_iter =
			paths_ordered_by_ddr.end();
	double cdfProba = 0.0;
	while (path_ddr_iter != pre_selected_paths.begin()) {
		path_ddr_iter--;
		cdfProba += path_ddr_iter->first * path_ddr_iter->second.size();
//		DEBUG_MESSAGE("proba: "<<ddrPathIter->first<<", number: "<<ddrPathIter->second.size()<<", cdf: "<<cdfProba/totalProba);
		tmp_set.insert(path_ddr_iter->second.begin(),
				path_ddr_iter->second.end());
		if (cdfProba >= th) {
			break;
		}
	}

	if (path_ddr_iter == paths_ordered_by_ddr.begin()) {

		DEBUG_MESSAGE("no path is excluded by cdf criteria");
		return;
	} else {
		DEBUG_MESSAGE(
				tmp_set.size() << " out of " << path_set.size()
						<< " paths are remained by cdf criteria");

		for (map<patPathJ, double>::iterator iter1 = path_set.begin();
				iter1 != path_set.end();) {
			if (tmp_set.find(&(iter1->first)) == tmp_set.end()) {
				path_set.erase(iter1++);
			} else {
				iter1++;
			}
		}

		if (path_set.size() > 100) {
			selectPathsByPdf(pre_selected_paths, path_set);
		}
	}

}

/**
 * Select random paths from a set according to likelihood.
 * TODO
 */
patPathJ* patMapMatchingIteration::selectPathsByCdfRandom(
		map<patPathJ, double>& path_set) {

}
void patMapMatchingIteration::selectPathsByNumber(
		list<patPathJ*>& pre_selected_paths, map<patPathJ, double>& path_set) {
	map<double, set<patPathJ*> > pre_selected_paths;

	for (map<patPathJ, double>::iterator iter1 = path_set.begin();
			iter1 != path_set.end(); ++iter1) {
		patPathJ* currPath = &(iter1->first);

		if (pre_selected_paths.find(iter1->second)
				== pre_selected_paths.end()) {

			pre_selected_paths[iter1->second] = set<patPathJ*>();
		}
		pre_selected_paths[iter1->second].insert(currPath);
	}

	map<double, set<patPathJ*> >::iterator ddrPathIterBack =
			pre_selected_paths.end();
	patULong best = patNBParameters::the()->selectBestPaths;
	patULong worst = patNBParameters::the()->selectWorstPaths;
	patULong shortest = patNBParameters::the()->selectShortestPaths;

	set<patPathJ*> tmpPathSet;
	tmpPathSet.insert(pre_selected_paths.begin(), pre_selected_paths.end());
	for (patULong i = 0; i < best; ++i) {
		if (ddrPathIterBack != pre_selected_paths.begin()) {
			ddrPathIterBack--;
			tmpPathSet.insert(ddrPathIterBack->second.begin(),
					ddrPathIterBack->second.end());
		}

	}
	for (map<patPathJ, double>::iterator pIter = path_set->begin();
			pIter != path_set->end();) {

		if (tmpPathSet.find(const_cast<patPathJ*>(&(pIter->first)))
				== tmpPathSet.end()) {
			path_set->erase(pIter++);
		} else {
			pIter++;
		}
	}

}

void patMapMatchingIteration::selectPathsByShortest(
		map<patPathJ, double>& path_set,
		set<patPathJ*>& tmpPathSet) {
	map<double, set<patPathJ*> > pre_selected_paths;
	map<double, set<patPathJ*> > lengthPathSet;

	for (map<patPathJ, double>::iterator iter1 = path_set.begin();
			iter1 != path_set.end(); ++iter1) {
		patPathJ* currPath = const_cast<patPathJ*>(&(iter1->first));
		double pl = currPath->computePathLength();
		if (lengthPathSet.find(pl) == lengthPathSet.end()) {
			lengthPathSet[pl] = set<patPathJ*>();
		}
		lengthPathSet[pl].insert(currPath);
	}
	map<double, set<patPathJ*> >::iterator lengthPathIterFront =
			lengthPathSet.begin();
	patULong shortest = patNBParameters::the()->selectShortestPaths;
	for (patULong i = 0; i < shortest; ++i) {
		if (lengthPathIterFront != lengthPathSet.end()) {
			tmpPathSet.insert(lengthPathIterFront->second.begin(),
					lengthPathIterFront->second.end());
			lengthPathIterFront++;

		}
	}
}

list<patPathJ*> patMapMatchingIteration::selectPathByImportantDDR(
		map<patPathJ, double> path_set) {
	list<patPathJ*> impPath;
	set<patArc*> impDDR = selectImportantDDR();
	DEBUG_MESSAGE("number of important ddrs" << impDDR.size());

	for (set<patArc*>::iterator impIter = impDDR.begin();
			impIter != impDDR.end(); ++impIter) {

		vector<patPathJ*> pathViaImpDDR;
		vector<double> pathDDR;
		for (map<patPathJ, double>::iterator pathIter = path_set.begin();
				pathIter != path_set.end(); ++pathIter) {
			if (pathIter->first.isLinkInPath(*impIter)) {
//				DEBUG_MESSAGE(pathIter->second)
				pathViaImpDDR.push_back(
						const_cast<patPathJ*>(&(pathIter->first)));
				pathDDR.push_back(pathIter->second);
			}
		}
		if (pathDDR.size() == 0) {
			continue;
		}
		patDiscreteDistribution discreteDraw(&pathDDR, &ranNumUniform);
		patULong d = discreteDraw();
		impPath.push_back(pathViaImpDDR[d]);
	}
	DEBUG_MESSAGE("select important ddrs' paths" << impPath.size());
	return impPath;
}
/*

 select important ddr with simulations
 */
set<patArc*> patMapMatchingIteration::selectImportantDDR() {
	set<patArc*> importantDDR;
	map<patArc*, double>* linkDDR = gpsSequence.back()->getLinkDDR();
	if (patNBParameters::the()->selectImportantDDRCdf == 1.0) {
		DEBUG_MESSAGE("the whole ddr is selected as import ddr");
		for (map<patArc*, double>::iterator ddrIter = linkDDR->begin();
				ddrIter != linkDDR->end(); ++ddrIter) {
			importantDDR.insert(ddrIter->first);
		}
		return importantDDR;
	}

	double sumTotal = gpsSequence.back()->calDDRSum();

	vector<patArc*> ddrArcs;
	vector<double> ddrValues;

	for (map<patArc*, double>::iterator ddrIter = linkDDR->begin();
			ddrIter != linkDDR->end(); ++ddrIter) {
		ddrArcs.push_back(ddrIter->first);
		ddrValues.push_back(ddrIter->second);
	}

	patDiscreteDistribution discreteDraw(&ddrValues, &ranNumUniform);

	double selectedDDRSum = 0.0;

	while (selectedDDRSum / sumTotal
			< patNBParameters::the()->selectImportantDDRCdf
			|| gpsSequence.back()->getLinkDDR()->size()
					< patNBParameters::the()->minDomainSize) {

		patULong selectedArcIndex = discreteDraw();
		pair<set<patArc*>::iterator, bool> insertResult = importantDDR.insert(
				ddrArcs[selectedArcIndex]);
		if (insertResult.second == true) {
			//DEBUG_MESSAGE("selected ddr"<<ddrValues[selectedArcIndex]);
			selectedDDRSum += ddrValues[selectedArcIndex];
		}

	}

	return importantDDR;
}

void patMapMatchingIteration::selectPaths(map<patPathJ, double>& path_set) {

	if (path_set->empty()) {
		return;
	}

	calPathDDR(path_set);

	list<patPathJ*> pre_selected_paths = selectPathByImportantDDR(path_set);
	if (patNBParameters::the()->selectPathCte == "pdf") {
		selectPathsByPdf(pre_selected_paths, path_set);

	}
	if (patNBParameters::the()->selectPathCte == "number") {
		selectPathsByNumber(pre_selected_paths, path_set);

	}
	if (patNBParameters::the()->selectPathCte == "cdf") {
		selectPathsByCdf(pre_selected_paths, path_set);

	}

}
