#ifndef patPathProbaAlgoV2_h
#define patPathProbaAlgoV2_h

#include "patType.h"
#include <map>
#include <vector>
class patPathJ;
class patGpsPoint;
class patNetwork;
class patArc;
struct time_params{patReal mu_tau;
	patReal delta_tau;patReal mu_t;patReal delta_t;
	patReal arc_int;
		};
struct base_f_params{patReal d_j;patReal d_j_1;patReal l_a;
	patReal e_j;patReal e_j_1; patReal t_j; patReal t_j_1; 
	patReal delta_j;patReal delta_j_1;
	patReal mu_tau; patReal delta_tau;patReal mu_t;patReal delta_t;};
	
struct base_f_params_2{
	patReal d_a;patReal l_a;patReal delta_j;patReal e_d;
	patReal mu_t_a;patReal delta_t_a;
	patReal mu_tau_a;patReal delta_tau_a;
	patReal t_j;patReal t_j_1;
	list<struct inner_f_params> prev_arcs;
	
	};
	
struct inner_f_params{
	patReal mu_t_b;patReal delta_t_b;
	patReal mu_by;patReal delta_by;
	patReal mu_tau_b;patReal delta_tau_b;
	patReal ddr_b;
};
struct curr_arc_params{
	patReal d_a;patReal l_a;patReal delta_j;patReal e_d;
	patReal mu_t_a;patReal delta_t_a;
	patReal mu_tau_a;patReal delta_tau_a;
	patReal t_j;patReal t_j_1;
	};
struct prev_arc_params{
	patReal mu_t_b;patReal delta_t_b;
	patReal mu_by;patReal delta_by;
	patReal mu_tau_b;patReal delta_tau_b;
	patReal ddr_b;
	
}; 
struct base_f_param_3{
	struct curr_arc_params a_param;
	struct prev_arc_params b_param;
};
struct arc_params{
	patReal l; //length
	patReal d; //distance to arc
	patReal e_d; //perpendical foot
	patReal t; //time of point j
	patReal l_c;
	patReal delta_x; //vairiance of coordinate
	
	patReal mu_v;//mu: velocity
	patReal delta_v;//delta
	
	patReal mu_v_c;
	patReal delta_v_c;
	
}; 

struct base_f_param_4{
	struct arc_params a_param;
	struct arc_params b_param;
};


class patPathProbaAlgoV2{
	public:
		/**
	* Constructor
	* @param thePath, path to be parsed
	* @param theGpsSequence, sequence of GPS points
	* @param theNetwork, the base network
	*/
	patPathProbaAlgoV2(patPathJ* thePath, 
			vector<patGpsPoint>* theGpsSequence,
			patNetwork* theNetwork);
	
patPathProbaAlgoV2( vector<patGpsPoint>* theGpsSequence,
 patNetwork* theNetwork);
		/**
		* parse the init arc, set probability of init arc as 1.
		*/
		void initArc();
		
		/**
		* Get travel time of an arc;
		* @param theArc, pointer to the arc 
		*/
		patReal getArcTime(patArc* theArc);

		/**
		* Calculate mean and viriance for departure time and travel time for each arc
		*/
		void calTimeParams();
		
		/**
		* run the algorithm , return the path proba;
		*/
		patReal run_gps();
		
		/**
		* Calculate probability for GPS point
		* @param j, the index of the GPS point
		* return probability value
		*/
		patReal calPointProba(patULong j);
		
		
		/**
		* Set the parameters for base function to be integrated
		*@param j, index of GPS point
		*@param theArc, pointer to the arc
		*@param p, pointer to the paramter structure
		*/
		void setParams(patULong j,patArc* theArc, struct base_f_params * p);
		void setParams_2(patULong j,patArc* theArc, struct base_f_params_2 * p);
		void setParams_3(patULong j,patArc* theArc
							, struct curr_arc_params * p
							, map<patArc*, struct prev_arc_params>* lp);
		void setParams_V4(patULong j,patArc* theArc
							, struct arc_params * p
							, map<patArc*, struct arc_params>* lp);
		/**
		*Calculate  perpendicular foot of an GPS point on an arc
		*@param distanceToArc, include geometrical information about the relation between a GPS point and a straight arc
		*return value, the position of perpendicaular foot on arc, percentage to the arc length.
		*	<0, before to the up node; >0&&<1, on arc; >1 after  the down node
		*/
		patReal calPerpendicularFootOnArc(map<char*, patReal> distanceToArc);

		 patReal calPointArcProba_V4(patULong j,pair<patArc*,patReal> arcDDR);
		/**
		* Calculate probability of point-arc
		*@param j, the GPS point index
		*@param arcDDR, the arc and its ddr value
		* return value, probabilty value
		*/
		patReal calPointArcProba(patULong j,pair<patArc*,patReal> arcDDR);

		/**
		* Convert epsilon value to length value
		*/
		patReal epsitionToReal(patReal epsilon, patArc* theArc);
		
		/**
		*Calculate distance from point to arc
		*@param thepoint, the GPS point
		*@param theArc, the arc
		*/
		map<char*, patReal> calDistancePoint2Arc(patGpsPoint* thePoint,patArc* theArc);
		patReal calPointArcProba_Condition(patULong j, pair<patArc*,patReal> arcDDR);
		patReal calAverageSpeed();
		patReal calPointArcProba_Uncondition(patULong j, pair<patArc*,patReal> arcDDR);
		patReal calArcDDR(patULong j,patArc* theArc);
		
		pair<patReal, patReal> calPathLengthBound();
		
		patReal calPathLength();
		
		patBoolean isPathLengthReasonable();

		patReal run_arc();
		void setFirstArcTimeParams(patArc* newArc);
		 
		void dealFirstArc(patArc* newArc);
		void pushArc(patArc* newArc);
		void popArc();
		void setArcTimeParams(patArc* newArc);

		void calArcProba(patArc* newArc);

		patReal calPathProba();
	protected:
		vector<patReal> pointTime;
		map<patArc* ,struct time_params> timeParams;
		vector<patReal> proba;
		vector<patGpsPoint>* gpsSequence;
		patNetwork* baseNetwork;
		patPathJ* path;
		patReal pathLength;
		patReal initArcProba;
		vector<map<patArc*,patReal> > pointArcProba;
		map<patArc*,map<patULong,patReal> > arcContributes;
};


#endif

