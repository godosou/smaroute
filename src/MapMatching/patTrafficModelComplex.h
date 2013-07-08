/*
 * convex combination of negative exponential distribution and lognormal distribution
 *  patTrafficModelComplex.h
 *  newbioroute
 *
 *  Created by Jingmin Chen on 8/5/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#ifndef patTrafficModelComplex_h
#define patTrafficModelComplex_h
#include "patTrafficModel.h"
#include "dataStruct.h"
#include "patType.h"
#include "patTransportMode.h"
#include <map>
class patTrafficModelComplex:public patTrafficModel {
public:
	patTrafficModelComplex(const patMeasurement* measurement_prev,
			const patMeasurement* measurement_curr,
			const patMultiModalPath* path);
	void initiate();

	/**
	 * Compute speed from previous true location to the change point.
	 * @param ell_prev \f$ \ell_{k-1}\f$, the current true location on the previous true link.
	 * @param t_c \f$ t_c \f$, the change time.
	 * @return speed
	 */
	double computeSpeedUntilChangePoint(double ell_prev, double t_c);

		/**
		 * Compute speed from change point to the current true location.
		 * @param t_c \f$ t_c \f$, the change time
		 * @param ell_curr \f$ \ell_{k}\f$, the current true location on the current true link.
		 * @return speed
		 */
	double computeSpeedFromChangePoint(double ell_curr, double t_c);

	/**
	 * Compute speed from previous true location to the current true location;
	 * @param ell_prev, previous true location \f$\ell_{k-1} \f$
	 * @param ell_curr, current true location \f$ \ell_{k} \f$
	 * @return speed
	 */
	double computeSpeedBetweenTwoLocations(double ell_prev, double ell_curr);
	/**
	 * Compute speed between two change points.
	 * @param t_c_1 \f$ t_{k-1} \f$, the change time
	 * @param t_c_2 \f$ t_{k} \f$, the change time
	 * @param segment \f$ (k-1_^{th} \f$ segment;
	 * @return speed
	 */
	double computeSpeedBetweenTwoChangePoints(
			double t_c_1, double t_c_2, int segment);
	double integral(double ell_prev, double ell_curr);

	double integral(double ell_prev,double ell_curr,double t_c);

 ~patTrafficModelComplex();
};


#endif


