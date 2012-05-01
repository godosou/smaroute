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
		patMapMatchingV2(vector<patGpsPoint> theGpsSequence, patNetwork* network, vector< list <  pair<patArc*,unsigned long> > >* adjList);
		set<patNode*> getNearestNodes(patGpsPoint* theGpsPoint,double searchRadius,patError*& err);
		bool stageMapMatchingGenerateSubRoutesWithGaps(list<patMapMatchingRoute>* subRoutesWithGaps, patError* err);
		patPathJ run( patError* err);
		void addRouteCandidate(vector<patMapMatchingRoute>* routeCandidates,patMapMatchingRoute newRoute);
		bool reachedEndOfStreet(patStreetSegment* aSegment, patGpsPoint* gps, patGpsPoint* prevGps, patMapMatchingRoute* route);
		bool reachedStartOfStreet(patStreetSegment* aSegment, patGpsPoint* gps);
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
                vector< list <  pair<patArc*,unsigned long> > >* networkAdjList;

};


#endif	/* PATMAPMATCHINGV2_H */

