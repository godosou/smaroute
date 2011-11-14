/*
 * patGpsDDR.h
 *
 *  Created on: Oct 12, 2011
 *      Author: jchen
 */

#ifndef PATGPSDDR_H_
#define PATGPSDDR_H_

#include "dataStruct.h"
#include "patArc.h"
#include "patGpsPoint.h"
#include <map>
class patGpsDDR {
public:
	patGpsDDR(patGpsPoint* the_gps);
	virtual ~patGpsDDR();

	/**
	 * Set DDR algorithm parameters.
	 * @param theAlgoParams map of the key-value pairs
	 */
	void patGpsDDR::setAlgoParams(const map<string, double>* theAlgoParams);

	/**
	 * Set DDR algorithm parameters.
	 * @param paramName the name of the parameter
	 * @param paramValue the value to be set.
	 */
	void patGpsDDR::setAlgoParams(string paramName, double paramValue);

	/**
	 * Detect whether an arc in DDR or not
	 */
	struct arc_ddr_range patGpsDDR::detArcDDR(const patArc* arc);

	/**
	 * Detect the DDR range of the arc
	 * @param arc_distance the arc distance
	 * @return link_ddr_range
	 */
	struct link_ddr_range patGpsDDR::detArcDDRRange(map<char*, double> arc_distance) const;

	double patGpsDDR::measureDDR(map<char*, double> distance, link_ddr_range ldr) const;
	/**
	 * The DDR is empty or not.
	 * @return bool.
	 */
	bool patGpsDDR::empty() const;

	/**
	 * Return the size of DDR
	 * @return int
	 */
	int patGpsDDR::size() const;


	/**
	 * Detect inherent DDR
	 * @param prev_ddr pointer, to the previous DDR
	 * @return the set of arcs.
	 */
	set<patArc*> patGpsDDR::detInherentDDR(const patGpsDDR* prev_ddr);

	/*
	 *
	 * Sort DDR by value.
	 * @return map key is the DDR value, value is the set of corresponding arcs
	 */
	map<double, set<patArc*> > patGpsDDR::sortDDRByValue() const;
	/**
	 * Calculate arc DDR.
	 * @param distance_to_arc the distance to arc
	 * @param ldr link_ddr_range struct
	 * @return double value.
	 */
	double patGpsDDR::measureDDR(map<char*, double> distance_to_arc
			, link_ddr_range ldr) const;
	/**
	 * Generate DDR of paths.
	 */
	void patGpsDDR::genDDRFromPaths(const set<patPathJ>* path_set);


	/**
	 * Select the initial ddr by using id of the arcs.
	 * m_ddr_arcs is set here.
	 * @param arcName the list of names of the arcs.
	 */
	void patGpsDDR::selectInitById(const set<patULong>* arc_ids) ;

	/**
	 * Select the initial ddr by using name of the arcs.
	 * m_ddr_arcs is set here.
	 * @param arcName the list of names of the arcs.
	 */
	void patGpsDDR::selectInitByName(const set<string>* arcName);

	/**
	 * Select DDR according to the CDF.
	 * The m_ddr_arcs is not set here.
	 * @return the set of ddr.
	 */
	set<patArc*> patGpsDDR::selectDomainByCdf();

	/**
	 * Select DDR by the order of the value
	 */
	void patGpsDDR::selectDomainByNumber(int k);

	/**
	 * Select initial DDR according the bounds of the DDR value.
	 * @param lowerBound the lower bound of the DDR value.
	 * @param upperBound the upper bound of the DDR value.
	 */
	void patGpsDDR::selectInitByDDR(double lowerBound, double upperBound) ;

	/**
	 * Select initial DDR with highest DDR value.
	 * @param k The least rank in the orderedd.
	 */
	void patGpsDDR::selectInitBySortDDR(int k);


	/**
	 * Select DDR according to the distance to the GPS point.
	 * @return the set of arcs in distance threshold.
	 */
	set<patArc*> patGpsDDR::selectDomainByDistance() ;

	/**
	 * Select initial DDR arcs with given origin nodes.
	 * @param origId The id of the selected origin nodes.
	 *
	 */
	void patGpsDDR::selectInitByOrigNode(set<unsigned long> origId);

	/**
	 * Calculate and update the sum of all DDR.
	 */
	double patGpsDDR::calDDRSum() ;

	/**
	 * Calculate DDR value for an arc.
	 * @param arc The arc to be calculated
	 */
	double patGpsDDR::calArcDDR(const patArc* arc) const;

	/**
	 * Calculate maximum possible distance traveled since the previous GPS point.
	 * @param prevGpsPoint The previous GPS point
	 * @return The distance
	 */
	double patGpsDDR::getDistanceCeilFromPrevDDR(const patGpsPoint* prevGpsPoint) const;

	/**
	 * Get the set of arcs in DDR;
	 * @return The set of arcs.
	 */
	set<patArc*> patGpsDDR::getDDRArcSet() const;

	/**
	 * Deal with low speed DDR.
	 * @param arc_set: The set of the all available arcs.
	 */
	void patGpsDDR::lowSpeedDDR(const set<patArc*>& arc_set) ;

	/**
	 * Get the DDR value of an arc.
	 * @param arc: The arc to be searched
	 * @return the DDR value
	 */
	double patGpsDDR::getArcDDRValue(const patArc* arc) const;

	/**
	 * Determin if an arc is in domain or not.
	 * @param arc: the arc to be searched.
	 * @param true/false
	 */
	bool patGpsDDR::isArcInDomain(const patArc* arc) const;

	/**
	 * Get the ddr range of an arc;
	 * @param arc: The arc.
	 * @param err: The error pointer, error is set if the arc is not in DDR
	 * @return link_ddr_range struct
	 */
	struct link_ddr_range patGpsDDR::getArcDDRRange(const patArc* arc, patError*& err) const;

	/**
	 * Get all DDR range;
	 */
	map<patArc*, struct link_ddr_range> patGpsDDR::getArcDDRRange() const;
protected:
	patGpsPoint* m_gps;
	double m_network_accuracy;
	double m_min_point_ddr;
	double m_horizon_accuracy;
	double m_distance_threshold_loc;
	double m_min_normal_speed;
	double m_max_normal_speed_heading;
	map<patArc*, double> m_ddr_arcs;
	map<patArc*, struct link_ddr_range> m_arc_ddr_ranges;
	double m_ddr_sum;
};

#endif /* PATGPSDDR_H_ */
