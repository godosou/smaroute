#ifndef patArcTransition_h
#define patArcTransition_h

#include "patGpsPoint.h"
#include "patPathJ.h"
#include "patType.h"
#include "patNetwork.h" 
#include "patError.h"
#include <list>
#include <vector>
#include "patArc.h"
#include "patNode.h"
struct discrete_params{
	patReal time_diff;
	patReal beta_0;
	patReal beta_a;
	patReal beta_b;
	patReal gamma_0;
	patReal gamma_a;
	patReal gamma_b;
};


class patArcTransition{
	public:
	patArcTransition(patPathJ aPath,
				patGpsPoint* aGpsPoint,
				patGpsPoint* bGpsPoint);
				
	patArcTransition(list<patArc*> listOfArcs,
				patGpsPoint* curr_GpsPoint,
				patGpsPoint* prev_GpsPoint);

	patArcTransition(list<pair<patArc*, TransportMode> > ,
				patGpsPoint* curr_GpsPoint,
				patGpsPoint* prev_GpsPoint);
	
	friend bool operator<(const patArcTransition& tran1, const patArcTransition& tran2) ;
	friend bool operator==(const patArcTransition& tran1, const patArcTransition& tran2) ;
	
friend ostream& operator<<(ostream& str, const patArcTransition& x);
	patReal calProbability(
	patString algoType,patNetwork* theNetwork, struct gps_params GP);
	//patReal calProbability(patNetwork* theNetwork, struct gps_params GP);
		patReal calProbability_raw(patNetwork* theNetwork, struct gps_params GP);
list<patReal>  calProbability(patNetwork* theNetwork, struct gps_params GP);

	void setNetworkParams(patNetwork* theNetwork, 
			struct network_params& p);
	vector<patReal> dealIntersections(
	patNetwork* theNetwork);
	patReal getValue(patString type,patNetwork* theNetwork, struct gps_params GP);
	patReal calWaitingTime(patArc* upArc, patArc* downArc,
	patNetwork* theNetwork,patError*& err);
	patReal calSignalWaiting(patArc* upArc,patArc* downArc,patNode* intersection,
	patNetwork* theNetwork);
	patPathJ getPath();
	patGpsPoint* getPrevGpsPoint();
	patGpsPoint* getCurrGpsPoint();
	patArc* getFirstArc();
	patArc* getLastArc();
	patReal calMultiModalProbability(patNetwork* theNetwork, struct gps_params GP);

	protected:
	patReal m_value;
	patReal m_value_simple;
	patReal m_value_raw;
	patGpsPoint* m_prev_gps_oint;
	patGpsPoint* m_curr_gps_point;
	patPathJ m_path;

};
#endif
