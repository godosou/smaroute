/*
 * patMapMatchingIteration.h
 *
 *  Created on: Oct 12, 2011
 *      Author: jchen
 */

#ifndef PATMAPMATCHINGITERATION_H_
#define PATMAPMATCHINGITERATION_H_
#include "patNode.h"
#include "patGpsPoint.h"
#include "patGpsDDR.h"
#include "patMultiModalPath.h"
#include "patMultiModalPathMatching.h"
#include "patNetworkEnvironment.h"
#include "patArcTransition.h"
#include "patMeasurement.h"
#include "patMeasurementDDR.h"
#include "patTransportMode.h"
#include<tr1/unordered_map>
using namespace std::tr1;
class patMapMatchingIteration {
public:
	patMapMatchingIteration(const patNetworkEnvironment* environment,
			patGpsPoint* gps, vector<patMeasurement*>* measurement_sequence,
			map<patMultiModalPathMatching, double> paths);
	patMapMatchingIteration();
	~patMapMatchingIteration();
	/**
	 * Generate DDR for the first iteration.
	 * Loop over all arcs and determine each one.
	 * TODO: improve the efficiency.
	 */
	void genInitDDR();

	/**
	 * The first map matching iteration.
	 * The precedures are performed:
	 * <ul>
	 * 	<li> Generate DDR from all arcs. (DDR only matters with geometry).</li>
	 * 	<li> Connect DDR in different transport networks.</li>
	 * </ul>
	 */
	void firstIteration();

	/**
	 * Sort the paths by their end nodes.
	 * @return a map with
	 * <ul>
	 * <li> key:  end node pointer.</li>
	 * <li> value: set of corresponding path with the same end node</li>
	 * </ul>
	 */
	map<const patNode*, set<const patMultiModalPathMatching*> > sortPathsByEndNodes() const;
	/**
	 * Map matching for normal iteration.
	 */
	bool normalIteration(patGpsPoint* prevNormalGps,
			vector<patMeasurement*>& inter_data, patError*& err);
	void lastIterationForLowSpeed(vector<patMeasurement*>& lowSpeedGpsPoints,
			patGpsPoint* prevNormalGps);
	/**
	 * Deal with intermediate low speed GPS points
	 */
	void dealInterData(vector<patMeasurement*>& inter_data,
			patGpsPoint* prevNormalGps,
			map<TransportMode, set<const patArc*> >& inter_sec_arcs,
			map<patMultiModalPathMatching, double>& path_set);/**
			 * Calculate path DDR.
			 */
	void calPathDDR(map<patMultiModalPathMatching, double>& path_set);
	void finalizeOneStage();/**
	 * Sum the likelihood of all paths
	 */
	double sumProba(const map<patMultiModalPathMatching, double>& path_set);
	;

	/**
	 * Select paths by CDF
	 * <ul>
	 * <li> Select shortest paths.</li>
	 * </ul>
	 */
	set<int> selectPathsByCdf(vector<patMultiModalPathMatching> path_vector,
			vector<double> path_probas);
	void selectPathsByNumber(list<patMultiModalPathMatching*>& pre_selected_paths,
			map<patMultiModalPathMatching, double>& path_set);
	set<int> selectPathsByRandom(
			int path_size, vector<double> path_probas,
			unsigned long count);

	set<int> selectPathsByShortest(vector<patMultiModalPathMatching> path_vector,
			vector<double> path_probas);

	set<int> selectPathByImportantDDR(vector<patMultiModalPathMatching> path_vector,
			vector<double> proba_vector);
	/*

	 select important ddr with simulations
	 */
	set<pair<const patArc*, TransportMode> >  selectImportantDDR();

	void selectPathsByPdf(list<patMultiModalPathMatching*>& pre_selected_paths,
			map<patMultiModalPath, double>& path_set);
	void selectPaths(map<patMultiModalPathMatching, double>& path_set);
	void writeKML(string trip_name,int iteration_number) ;
	map<patMultiModalPathMatching, double> getPaths();
	double getDistanceCeilFromGPS(const patGpsPoint* currGpsPoint,
			const patGpsPoint* prevGpsPoint,
			const patNetworkBase* network) const;
	set<int> selectPathsByDDR(vector<patMultiModalPathMatching>& path_vector, vector<double>& proba_vector);
	set<int> selectPathsByMode(vector<patMultiModalPathMatching>& path_vector,vector<double>& path_probas);
	set<int> selectPathsByChange(
			vector<patMultiModalPathMatching>& path_vector,vector<double>& path_probas) ;
protected:

	vector<patMeasurement*>* m_current_measurments; //The gps sequence up to previous one.
	patGpsPoint* m_gps;
	map<patMultiModalPathMatching, double> m_paths;
	double m_total_proba;
	const patNetworkEnvironment* m_environment;
};

#endif /* PATMAPMATCHINGITERATION_H_ */
