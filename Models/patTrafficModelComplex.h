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
class patTrafficModelComplex:public patTrafficModel {
public:
	void initiate();

	/**
	 * Compute speed from previous true location to the change point.
	 * @param ell_prev \f$ \ell_{k-1}\f$, the current true location on the previous true link.
	 * @param t_c \f$ t_c \f$, the change time.
	 * @return speed
	 */
	patReal computeSpeedUntilChangePoint(patReal ell_prev, patReal t_c);

		/**
		 * Compute speed from change point to the current true location.
		 * @param t_c \f$ t_c \f$, the change time
		 * @param ell_curr \f$ \ell_{k}\f$, the current true location on the current true link.
		 * @return speed
		 */
	patReal computeSpeedFromChangePoint(patReal ell_curr, patReal t_c);

	/**
	 * Compute speed from previous true location to the current true location;
	 * @param ell_prev, previous true location \f$\ell_{k-1} \f$
	 * @param ell_curr, current true location \f$ \ell_{k} \f$
	 * @return speed
	 */
	patReal computeSpeedBetweenTwoLocations(patReal ell_prev, patReal ell_curr);
	/**
	 * Compute speed between two change points.
	 * @param t_c_1 \f$ t_{k-1} \f$, the change time
	 * @param t_c_2 \f$ t_{k} \f$, the change time
	 * @param segment \f$ (k-1_^{th} \f$ segment;
	 * @return speed
	 */
	patReal patTrafficModelComplex::computeSpeedBetweenTwoChangePoints(
			patReal t_c_1, patReal t_c_2, int segment);
	patReal integral(patReal v, TrafficModelParam tm_param);
	patReal integral(patReal ell_prev, patReal ell_curr);

	patReal integral(patReal ell_prev,patReal ell_curr,patReal t_c);

	static map<TransportMode, TrafficModelParam> tm_params;

};


#endif


