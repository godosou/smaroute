/* 
 * File:   patMapMatchingV2.h
 * Author: jchen
 *
 * Created on April 7, 2011, 1:36 PM
 */

#ifndef PATMAPMATCHINGV2_H
#define	PATMAPMATCHINGV2_H


#include <map>
#include <list>
#include <set>
#include <vector>
#include "patError.h"
#include "patType.h"
#include "patString.h"
#include "patStreetSegment.h"

class patNetwork;
class patNode;
class patArc;
class patGpsPoint;
class patPathJ;
class patMapMatchingRoute;

class patMapMatchingV2{
	public:
		patMapMatchingV2();
		patMapMatchingV2(vector<patGpsPoint> theGpsSequence, patNetwork* network, vector< list <  pair<patArc*,patULong> > >* adjList);
		set<patNode*> getNearestNodes(patGpsPoint* theGpsPoint,patReal searchRadius,patError*& err);
		patBoolean stageMapMatchingGenerateSubRoutesWithGaps(list<patMapMatchingRoute>* subRoutesWithGaps, patError* err);
		patPathJ run( patError* err);
		void addRouteCandidate(vector<patMapMatchingRoute>* routeCandidates,patMapMatchingRoute newRoute);
		patBoolean reachedEndOfStreet(patStreetSegment* aSegment, patGpsPoint* gps, patGpsPoint* prevGps, patMapMatchingRoute* route);
		patBoolean reachedStartOfStreet(patStreetSegment* aSegment, patGpsPoint* gps);
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
                vector< list <  pair<patArc*,patULong> > >* networkAdjList;

};


#endif	/* PATMAPMATCHINGV2_H */

