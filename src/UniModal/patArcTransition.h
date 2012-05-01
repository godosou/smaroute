#ifndef patArcTransition_h
#define patArcTransition_h

#include "patGpsPoint.h"
#include "patMultiModalPath.h"
#include "patType.h"
#include "patError.h"
#include <list>
#include <vector>
#include "patArc.h"
#include "patNode.h"
struct discrete_params{
	double time_diff;
	double beta_0;
	double beta_a;
	double beta_b;
	double gamma_0;
	double gamma_a;
	double gamma_b;
};


class patArcTransition{
	public:
	patArcTransition(patMultiModalPath aPath,
				patGpsPoint* aGpsPoint,
				patGpsPoint* bGpsPoint);
	
	friend bool operator<(const patArcTransition& tran1, const patArcTransition& tran2) ;
	friend bool operator==(const patArcTransition& tran1, const patArcTransition& tran2) ;
	
friend ostream& operator<<(ostream& str, const patArcTransition& x);
	double calProbability(
	patString algoType, struct gps_params GP);
	//patReal calProbability(patNetwork* theNetwork, struct gps_params GP);
		double calProbability_raw(struct gps_params GP);
list<double>  calProbability( struct gps_params GP);

	void setNetworkParams(
			struct network_params& p);
	double getValue(patString type, struct gps_params GP);
	patMultiModalPath getPath();
	patGpsPoint* getPrevGpsPoint();
	patGpsPoint* getCurrGpsPoint();
	const patArc* getFirstArc();
	const patArc* getLastArc();

	protected:
	double m_value;
	double m_value_simple;
	double m_value_raw;
	patGpsPoint* m_prev_gps_point;
	patGpsPoint* m_curr_gps_point;
	patMultiModalPath m_path;

};
#endif
