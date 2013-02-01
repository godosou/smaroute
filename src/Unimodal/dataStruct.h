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

#include "patType.h";
struct gps_params{
	patULong time_diff;
	
	patReal max_v_inter;
	patReal p_0;
	
	patReal mu_v_curr;
	patReal std_v_curr;
	
	patReal mu_v_prev;
	patReal std_v_prev;
	
	
	patReal mu_v_inter;
	patReal std_v_inter;
	
	patReal std_x_prev;
	patReal std_x_curr;
	
	patReal v_denom_curr;
	patReal v_denom_prev;
	patReal v_denom_inter;
	
};

struct link_ddr_range {
	patReal lower_bound;
	patReal upper_bound;
};
struct network_params{
	patReal l_curr;
	patReal d_curr;
	patReal e_d_curr;
	
	patReal l_inter;
	
	patReal l_prev;
	patReal d_prev;
	patReal e_d_prev;
	
	patReal prev_ddr;
	patULong a_total;
	vector<patReal> t_w;
};
#endif
