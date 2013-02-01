#ifndef patMapMatching_h
#define patMapMatching_h

#include <map>
#include <list>
#include <set>
#include <vector>
#include "patError.h"
#include "patType.h"
#include "patString.h"

class patNetwork;
class patNode;
class patArc;
class patGpsPoint;
class patPathJ;
class patMapMatchingRoute;

class patMapMatching{
	public:
		patMapMatching();
		patMapMatching(vector<patGpsPoint> theGpsSequence, patNetwork* network);
		set<patNode*> getNearestNodes(patGpsPoint* theGpsPoint,patReal searchRadius,patError*& err);
		patBoolean stageMapMatchingGenerateSubRoutesWithGaps(list<patMapMatchingRoute>* subRoutesWithGaps, patError* err);
		patPathJ run( patError* err);
		void addRouteCandidate(vector<patMapMatchingRoute>* routeCandidates,patMapMatchingRoute newRoute);
		patBoolean reachedEndOfArc(patArc* arc, patGpsPoint* gps, patGpsPoint* prevGps, patMapMatchingRoute* route);
		patBoolean reachedStartOfArc(patArc* arc, patGpsPoint* gps);
		patPathJ joinChosenRoutes(list<patMapMatchingRoute>* subRoutes,
			patBoolean succ,
			patError*& err
		);
	protected:
		vector<patGpsPoint> gpsSequence;
		patNetwork* theNetwork;
		patReal maxGapDistance,maxGapTime;
		patReal initialSearchRadius, searchRadiusIncrement;
		patReal minNbrOfStartNodes, maxNbrOfCandidates, minNbrOfGpsPerSegment;
		patReal routeJoiningQualityThreshold;
		
};

#endif
