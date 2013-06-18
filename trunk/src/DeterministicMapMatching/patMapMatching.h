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
		set<patNode*> getNearestNodes(patGpsPoint* theGpsPoint,double searchRadius,patError*& err);
		bool stageMapMatchingGenerateSubRoutesWithGaps(list<patMapMatchingRoute>* subRoutesWithGaps, patError* err);
		patPathJ run( patError* err);
		void addRouteCandidate(vector<patMapMatchingRoute>* routeCandidates,patMapMatchingRoute newRoute);
		bool reachedEndOfArc(patArc* arc, patGpsPoint* gps, patGpsPoint* prevGps, patMapMatchingRoute* route);
		bool reachedStartOfArc(patArc* arc, patGpsPoint* gps);
		patPathJ joinChosenRoutes(list<patMapMatchingRoute>* subRoutes,
			bool succ,
			patError*& err
		);
	protected:
		vector<patGpsPoint> gpsSequence;
		patNetwork* theNetwork;
		double maxGapDistance,maxGapTime;
		double initialSearchRadius, searchRadiusIncrement;
		double minNbrOfStartNodes, maxNbrOfCandidates, minNbrOfGpsPerSegment;
		double routeJoiningQualityThreshold;
		
};

#endif
