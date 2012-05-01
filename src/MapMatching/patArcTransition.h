#ifndef patArcTransition_h
#define patArcTransition_h

#include "patGpsPoint.h"
#include "patMeasurement.h"
#include "patMultiModalPath.h"
#include "patType.h"
#include "patError.h"
#include <list>
#include <vector>
#include "patArc.h"
#include "patNode.h"
#include <functional>
#include <boost/functional/hash.hpp>
using namespace std;

using namespace std::tr1;

struct base_params {
	patTrafficModel* TM;
	patMeasurementModel* MM_prev;
	patMeasurementModel* MM_curr;
};

class patArcTransition {
public:
	patArcTransition(patMultiModalPath path, patMeasurement* measurement_prev,
			patMeasurement* measurement_curr);
	virtual ~patArcTransition();

	friend bool operator<(const patArcTransition& tran1,
			const patArcTransition& tran2);
	friend bool operator==(const patArcTransition& tran1,
			const patArcTransition& tran2);
friend struct hash_patArcTransition;
	friend ostream& operator<<(ostream& str, const patArcTransition& x);

	double calProbability(patString algoType);
	list<double> calProbability(struct gps_params GP);

	double getValue(patString type);
	patMultiModalPath getPath();
	patMeasurement* getPrevGpsPoint();
	patMeasurement* getCurrGpsPoint();
	const patArc* getFirstArc();
	const patArc* getLastArc();
double getValue(patString type, patMeasurement *prev_measurement, patMeasurement *curr_measurement);
	bool setModels();
	void deleteModels();
protected:
	double m_value;
	patMeasurement* m_prev_measurement;
	patMeasurement* m_curr_measurement;
	patMultiModalPath m_path;
	struct base_params m_param;

};

struct hash_patArcTransition {
	size_t operator() (const patArcTransition &arc_tran ) const {
		std::size_t seend= 0;
		boost::hash_combine(seend,arc_tran.m_prev_measurement);
		boost::hash_combine(seend,arc_tran.m_curr_measurement);
		list<pair<const patArc*, TransportMode> > arcs = arc_tran.m_path.getArcsWithMode();
		for(list<pair<const patArc*, TransportMode> >::iterator arc_iter = arcs.begin();
				arc_iter!=arcs.end();
				++arc_iter) {
			boost::hash_combine(seend,arc_iter->first);
			boost::hash_combine(seend,arc_iter->second);
		}
		return seend;

	}
};
#endif
