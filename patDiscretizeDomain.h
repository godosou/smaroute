#ifndef patArcDDR_h
#define patArcDDR_h

include <map>
include <set>
include "patType.h"

class patGpsPoint;
class patArc;
class patDiscretizeDomain{
	public:
	patDiscretizeDomain(patGpsPoint theGps);
	run();
	set<patArc*> getArcs();
	set<patReal> getPositions(patArc* theArc);
	map<patArc,set<patReal> > getAll();
	protected:
	patGpsPoint* gps;
	map<patArc*,set<patReal> > positions;
}
#endif
