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
#include "patNetworkEnvironment.h"
#include "patArcTransition.h"
#include "patMeasurement.h"
#include "patMeasurementDDR.h"
#include "patTransportMode.h"
class patMapMatchingIteration {
public:
	patMapMatchingIteration(const patNetworkEnvironment* environment,
			patGpsPoint* gps, vector<patMeasurement*>* measurement_sequence,
			set<patArcTransition>* arc_tran_set,
			map<patMultiModalPath, double> paths);
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
	map<const patNode*, set<const patMultiModalPath*> > sortPathsByEndNodes() const;
	/**
	 * Map matching for normal iteration.
	 */
	void normalIteration(patGpsPoint* prevNormalGps,
			vector<patMeasurement*>& inter_data, patError*& err);
	void lastIteration(vector<patMeasurement*>& lowSpeedGpsPoints,
			patGpsPoint* prevNormalGps);
	/**
	 * Deal with intermediate low speed GPS points
	 */
	void dealInterData(vector<patMeasurement*>& inter_data,
			patGpsPoint* prevNormalGps,
			map<TransportMode, set<const patArc*> >& inter_sec_arcs);/**
			 * Calculate path DDR.
			 */
	void calPathDDR(map<patMultiModalPath, double>& path_set);
	void finalizeOneStage();/**
	 * Sum the likelihood of all paths
	 */
	double sumProba(const map<patMultiModalPath, double>& path_set);
	;

	/**
	 * Select paths by CDF
	 * <ul>
	 * <li> Select shortest paths.</li>
	 * </ul>
	 */
	set<int> selectPathsByCdf(vector<patMultiModalPath> path_vector,
			vector<double> path_probas);
	void selectPathsByNumber(list<patMultiModalPath*>& pre_selected_paths,
			map<patMultiModalPath, double>& path_set);
	set<int> patMapMatchingIteration::selectPathsByRandom(
			vector<patMultiModalPath> path_vector, vector<double> path_probas,
			unsigned long count);

	set<int> selectPathsByShortest(vector<patMultiModalPath> path_vector);

	set<int> selectPathByImportantDDR(vector<patMultiModalPath> path_vector,
			vector<double> proba_vector);
	/*

	 select important ddr with simulations
	 */
	set<const patArc*> selectImportantDDR();

	void selectPathsByPdf(list<patMultiModalPath*>& pre_selected_paths,
			map<patMultiModalPath, double>& path_set);
	void selectPaths(map<patMultiModalPath, double>& path_set);
	void writeKML(int iteration_number) const;
	map<patMultiModalPath, double> getPaths();
	double getDistanceCeilFromGPS(const patGpsPoint* currGpsPoint,
			const patGpsPoint* prevGpsPoint,
			const patNetworkBase* network) const;

protected:

	vector<patMeasurement*>* m_current_measurments; //The gps sequence up to previous one.
	patGpsPoint* m_gps;
	map<patMultiModalPath, double> m_paths;
	double m_total_proba;
	set<patArcTransition>* m_arc_tran_set;
	const patNetworkEnvironment* m_environment;
};

#endif /* PATMAPMATCHINGITERATION_H_ */
