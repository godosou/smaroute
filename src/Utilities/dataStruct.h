/*
 *  dataStruct.h
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/30/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */
#ifndef dataStruct_h
#define dataStruct_h
#include <vector>
#include "patTransportMode.h"
#include "patType.h"
#include <list>
#include <time.h>
#include <sys/time.h>
#include "patException.h"
struct MeasurementModelParam {
	double distance_to_arc;
	double foot_on_arc;
	double length_of_arc;
	double std_measurement;
	TransportMode mode;
};
struct ArcTranParam {
	double time_prev;
	double time_curr;
	double time_diff;

	double length_prev;
	double length_curr;

	int arc_amount;

	double inter_stop_time;
	double inter_length;

	vector<double> inter_seg_lengths;
	vector<TransportMode> transport_modes;
	double change_penalty;
};

struct gps_params {
	double time_diff;

	double time_prev;
	double time_curr;
	double max_v_inter;
	double mu_v_curr;
	double std_v_curr;

	double mu_v_prev;
	double std_v_prev;

	double mu_v_inter;
	double std_v_inter;

	double std_x_prev;
	double std_x_curr;
};

struct link_ddr_range {
	double lower_bound;
	double upper_bound;
};
struct network_params {
	double l_curr;
	double d_curr;
	double e_d_curr;

	double l_inter;

	double l_prev;
	double d_prev;
	double e_d_prev;
	double inter_stop_time; //intermediate stop time;
	double prev_ddr;
	unsigned long a_total;
	vector<double> intermediate_lengths;
	vector<TransportMode> transport_modes;
	double change_penalty;
	vector<double> t_w;
};

struct TrafficModelParam {
	double w;
	double lambda;
	double sigma;
	double mu;
	TransportMode mode;
};
struct AccelModelParam {
	int components;
	vector<double> w;
	vector<double> mu;
	vector<double> sigma;
};

enum ARC_ATTRIBUTES_TYPES {
	ENUM_LENGTH, ENUM_SPEED_BUMP, ENUM_TRAFFIC_SIGNAL
};

//string getArcAttributeTypeString(ARC_ATTRIBUTES_TYPES A_TYPE) {
//	swtich(A_TYPE)
//	{
//		case ENUM_LENGTH:
//		return "length";
//		case ENUM_SPEED_BUMP:
//		return "speed_bump";
//		case ENUM_TRAFFIC_SIGNAL:
//		return "traffic_signal";
//		default:
//		throw RuntimeException("invalid arc attribute type");
//	}
//}
//
#endif
