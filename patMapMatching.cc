#include "patMapMatching.h"
#include "patMapMatchingRoute.h"
#include "patNetwork.h"
#include "patNode.h"
#include "patArc.h"
#include "patPathJ.h"
#include "patGpsPoint.h"
#include "patDisplay.h"
#include "patError.h"
#include "patErrMiscError.h"
#include "patErrNullPointer.h"
#include "patNBParameters.h"
#include "patShortestPathAlgoDest.h"
#include "patShortestPathTreeDest.h"
#include "patNBParameters.h"


#include <algorithm>

patMapMatching::patMapMatching() {
}

patMapMatching::patMapMatching(vector<patGpsPoint> theGpsSequence,
        patNetwork* network) :
theNetwork(network),
maxGapDistance(patNBParameters::the()->maxGapDistance),
maxGapTime(patNBParameters::the()->maxGapTime),
initialSearchRadius(patNBParameters::the()->initialSearchRadius),
minNbrOfStartNodes(patNBParameters::the()->minNbrOfStartNodes),
searchRadiusIncrement(patNBParameters::the()->searchRadiusIncrement),
maxNbrOfCandidates(patNBParameters::the()->maxNbrOfCandidates),
minNbrOfGpsPerSegment(patNBParameters::the()->minNbrOfGpsPerSegment),
routeJoiningQualityThreshold(patNBParameters::the()->routeJoiningQualityThreshold) {

    gpsSequence = theGpsSequence;
}

/**
 * Get nodes nearby a GPS point
 * 
 * @param theGpsPoint
 * @param searchRadius, the radius near the GPS point
 * @param err
 * @return a set of nodes
 */
set<patNode*> patMapMatching::getNearestNodes(patGpsPoint* theGpsPoint, patReal searchRadius, patError*& err) {
    //DEBUG_MESSAGE("get nearby nodes");
    set<patNode*> rtnNodes;
    //DEBUG_MESSAGE("nodes"<<theNetwork->internalNodes.size());
    //map<patULong, patNode>::iterator nodeIter = theNetwork->theNodes.begin();
    //DEBUG_MESSAGE("nodes"<<theNetwork->theNodes.size());
    for (map<patULong, patNode>::iterator nodeIter = theNetwork->theNodes.begin();
            nodeIter != theNetwork->theNodes.end();
            ++nodeIter) {
        //DEBUG_MESSAGE("OK");
        if (theGpsPoint->distanceTo(&(nodeIter->second)) <= searchRadius) {
            rtnNodes.insert(&(nodeIter->second));
        }
    }

    DEBUG_MESSAGE("nearby nodes: " << searchRadius << "-" << rtnNodes.size());

    return rtnNodes;
}

/**
 * Generate sub-routes from the whole GPS sequence
 * @param subRoutesWithGaps, the list of MapMatchingRoutes
 * @param err
 * @return
 */
patBoolean patMapMatching::stageMapMatchingGenerateSubRoutesWithGaps(list<patMapMatchingRoute>* subRoutesWithGaps, patError* err) {
    DEBUG_MESSAGE("start map matching");
    vector<patMapMatchingRoute>* routeCandidates = new vector<patMapMatchingRoute>;

    for (patULong i = 0;
            i < gpsSequence.size();
            ++i) {
        DEBUG_MESSAGE("gps [" << i + 1 << "]" << gpsSequence.at(i));
        patGpsPoint* currGps = &(gpsSequence.at(i));

        /*
         * If there is not route candidate,
         * 1. initiate with searching nodes nearby the current GPS point
         * 2. Search over outgoing links of the neareast nodes for new route candidates
         *     no condition applied for the search.
         */
        if (routeCandidates->empty()) {
            DEBUG_MESSAGE("initiate with radius"<<initialSearchRadius);
            patReal searchRadius = initialSearchRadius;
            set<patNode*> nearestNodes = getNearestNodes(currGps, searchRadius, err);
            while (nearestNodes.size() < minNbrOfStartNodes) {
                DEBUG_MESSAGE("not enough initial nodes, increase the radius by"<<searchRadius);
                searchRadius += searchRadiusIncrement;
                nearestNodes = getNearestNodes(currGps, searchRadius, err);

            }


            DEBUG_MESSAGE("nearby nodes: " << searchRadius << "-" << nearestNodes.size());
            for (set<patNode*>::iterator nodeIter = nearestNodes.begin();
                    nodeIter != nearestNodes.end();
                    ++nodeIter
                    ) {
                patNode* currNode = *nodeIter;
                for (set<patULong>::iterator succIter = currNode->userSuccessors.begin();
                        succIter != currNode->userSuccessors.end();
                        ++succIter) {
                    patArc* aArc = theNetwork->getArcFromNodesUserId(currNode->userId, *succIter);
                    if (aArc == NULL) {
                        err = new patErrNullPointer("patNode");
                        WARNING(err->describe());
                        return patFALSE;
                    }
                    if (aArc->name == "M1" ||
                            aArc->attributes.priority < 4 ||
                            aArc->attributes.priority > 14) {
                        continue;
                    }
                    patMapMatchingRoute newRoute;
                    newRoute.addArc(aArc, theNetwork, err);
                    newRoute.addGpsArcAssignment(aArc, currGps, theNetwork, err);
                    newRoute.reCalScore(aArc, currGps,true,theNetwork);
                    addRouteCandidate(routeCandidates, newRoute);
                }
            }
        }

        /*
         * If there are route candidates, 
         *   and the current GPS point is too far away from the last one
         *   in terms of time and distance:
         * 1. Break the current sub-route matching procedure
         * 2. Select the best route candidate for the current sub-route
         * 3. Start new sub-route matching procedure
         */
        else if (currGps->distanceTo(&gpsSequence.at(i - 1)) > maxGapDistance ||
                currGps->getTimeStamp() - gpsSequence.at(i - 1).getTimeStamp() > maxGapTime)
        {
            /*not used: filter segments that are too short because they can be totally off and lead to very weird routes when joined together with the other segments*/
            DEBUG_MESSAGE("a gap in trip");
            if ((routeCandidates->front().getNbrOfGpsInRoute() >= minNbrOfGpsPerSegment) &&
                    (routeCandidates->front().getRouteSize() > 1)) {
                DEBUG_MESSAGE("push the finished segment");
                subRoutesWithGaps->push_back(routeCandidates->front());

            }
            routeCandidates->erase(routeCandidates->begin(), routeCandidates->end());
            patReal searchRadius = initialSearchRadius;
            set<patNode*> nearestNodes = getNearestNodes(currGps, searchRadius, err);
            DEBUG_MESSAGE("nearby nodes: " << searchRadius << "-" << nearestNodes.size());

            for (set<patNode*>::iterator nodeIter = nearestNodes.begin();
                    nodeIter != nearestNodes.end();
                    ++nodeIter
                    ) {
                patNode* currNode = *nodeIter;
                for (set<patULong>::iterator succIter = currNode->userSuccessors.begin();
                        succIter != currNode->userSuccessors.end();
                        ++succIter) {
                    patArc* aArc = theNetwork->getArcFromNodesUserId(currNode->userId, *succIter);
                    if (aArc == NULL) {
                        err = new patErrNullPointer("patNode");
                        WARNING(err->describe());
                        return patFALSE;
                    }
                    if (aArc->name == "M1" ||
                            aArc->attributes.priority < 4 ||
                            aArc->attributes.priority > 14) {
                        continue;
                    }
                    patMapMatchingRoute newRoute;
                    newRoute.addArc(aArc, theNetwork, err);
                    newRoute.addGpsArcAssignment(aArc, currGps, theNetwork, err);
                    newRoute.calScore(theNetwork);
                    addRouteCandidate(routeCandidates, newRoute);
                }
            }

        }

        /*
         * If there are route candidates and
         *  the current GPS point is not too far away from the last one
         *  continue matching procedure for the current sub-route.
         * 
         */
        
        else {
            vector<patMapMatchingRoute> oldRouteCandidates = *routeCandidates;
            routeCandidates->erase(routeCandidates->begin(), routeCandidates->end());
            DEBUG_MESSAGE("old route size:" << oldRouteCandidates.size());
            patULong j = 0;
            for (vector<patMapMatchingRoute>::iterator routeIter = oldRouteCandidates.begin();
                    routeIter != oldRouteCandidates.end();
                    ++routeIter) {
                //DEBUG_MESSAGE(++j);
                patMapMatchingRoute currRoute = *routeIter;
                /*first, check if new Gpsinate reached the start of the link (for one link routes only!)
                 * if yes ignore the route*/
                patArc* lastArc = currRoute.getLastArc();
                //DEBUG_MESSAGE("arc size:"<<currRoute.getRouteSize()<<"==="<<*lastArc);
                if (!(currRoute.getRouteSize() == 1 && reachedStartOfArc(lastArc, currGps))) {
                    //DEBUG_MESSAGE("not reach the start of arc");
                    /*second, check if new coordinate reached the end of the link*/
                    if (reachedEndOfArc(lastArc, currGps, currRoute.getLastGps(), &currRoute)) {
                        DEBUG_MESSAGE("reached the end of last arc");
                        /*if yes, remove old route and generate for each outgoing link of lastArc.getToNode() a new route,
                         * except the one leading back to the start node of lastArc or one that contains a circle*/
                        patULong routesGenerated = 0;
                        patNode* lastNode = theNetwork->getNodeFromUserId(lastArc->downNodeId);
                        for (set<patULong>::iterator outIter = lastNode->userSuccessors.begin();
                                outIter != lastNode->userSuccessors.end();
                                ++outIter)
                        {
                            if (*outIter != lastArc->upNodeId && *outIter!=lastArc->downNodeId) {
                                patArc* currArc = theNetwork->getArcFromNodesUserId(lastArc->downNodeId, *outIter);

                                if (currArc == NULL) {
                                    err = new patErrNullPointer("patNode");
                                    WARNING(err->describe());
                                    return patFALSE;
                                }
                                if (currArc->name == "M1" ||
                                        currArc->attributes.priority < 4 ||
                                        currArc->attributes.priority > 14) {
                                    continue;
                                }

                                if (!currRoute.searchFromEnd(currArc)) {
                                    DEBUG_MESSAGE("new candidate by appending new arc");
                                    patMapMatchingRoute newRoute = currRoute;
                                    newRoute.addArc(currArc, theNetwork, err);
                                    newRoute.addGpsArcAssignment(currArc, currGps, theNetwork, err);
                                    newRoute.reCalScore(currArc, currGps,true,theNetwork);
                                    //DEBUG_MESSAGE(newRoute);
                                    addRouteCandidate(routeCandidates, newRoute);
                                    routesGenerated++;
                                }
                            }
                        }
                        /*account for dead ends at the end of the trip where coords might run off the network:
                         *assign new coord to old link and recalculate the score*/
                        if (routesGenerated == 0) {
                            DEBUG_MESSAGE("dead end");
                            currRoute.addGpsArcAssignment(lastArc, currGps, theNetwork, err);
                            currRoute.reCalScore(lastArc, currGps,false,theNetwork);
                            //DEBUG_MESSAGE(currRoute);
                            addRouteCandidate(routeCandidates, currRoute);
                        }
                    }
                    else {
                        DEBUG_MESSAGE("new candidate by appending new gps");

                        /*else, assign new coord to old link and recalculate the score*/
                        currRoute.addGpsArcAssignment(lastArc, currGps, theNetwork, err);
                        currRoute.reCalScore(lastArc, currGps,false,theNetwork);
                        //DEBUG_MESSAGE(currRoute);
                        addRouteCandidate(routeCandidates, currRoute);

                    }
                }

            }

        }

        DEBUG_MESSAGE("route candidate:" << routeCandidates->size() << "----" << maxNbrOfCandidates);
        sort(routeCandidates->begin(), routeCandidates->end());
        while (routeCandidates->size() > maxNbrOfCandidates) {
            DEBUG_MESSAGE("discard route with score"<<routeCandidates->back().getScore());
            routeCandidates->pop_back();
        }
    }

    patBoolean succ = patTRUE;
    /*if finished with all coords: evaluate the last set of route candidates and store the one with the best score*/
    if (!routeCandidates->empty()) {
        if ((routeCandidates->front().getNbrOfGpsInRoute() >= minNbrOfGpsPerSegment)
                && (routeCandidates->front().getRouteSize() >= 1)) {
            DEBUG_MESSAGE("new route : " << routeCandidates->front().getScore());
            subRoutesWithGaps->push_back(routeCandidates->front());
        } else {
            DEBUG_MESSAGE("invalid subroute"<<routeCandidates->front().getNbrOfGpsInRoute());
        }


    }
    else {
        succ = patFALSE;
    }


    routeCandidates->erase(routeCandidates->begin(), routeCandidates->end());
    delete routeCandidates;
    return succ;
}
/**
 * Run the map matching process:
 * 1. Generate map matched sub-routes with gaps
 * 2. Join the sub-routes
 * @param err
 * @return a map matched path
 */
patPathJ patMapMatching::run(patError* err) {
    list<patMapMatchingRoute>* subRoutesWithGaps = new list<patMapMatchingRoute>;
    patBoolean succ = stageMapMatchingGenerateSubRoutesWithGaps(subRoutesWithGaps, err);
    patPathJ tmpPath;
    if (!succ) {
        stringstream str;
        str << "err in generate sub routes";
        err = new patErrMiscError(str.str());
        WARNING(err->describe());
        return tmpPath;
    }
    patPathJ path = joinChosenRoutes(subRoutesWithGaps, succ, err);
    DEBUG_MESSAGE("finish join segments");
    delete subRoutesWithGaps;
    subRoutesWithGaps = NULL;
    if (err != NULL) {
        DEBUG_MESSAGE("wrong matching");
        return tmpPath;
    }
    return path;

}


/**
 * Add Route candidate to the candidates list.
 * Tasks:
 * 1. Add only if the route doesn't exist in the candidates list
 * 2. Add the new candidate to proper position,
 *    such that the list is always sorted with score high to low
 * @param routeCandidates
 * @param newRoute
 */
void patMapMatching::addRouteCandidate(vector<patMapMatchingRoute>* routeCandidates,
        patMapMatchingRoute newRoute) {

    if (routeCandidates->empty()) {
        routeCandidates->push_back(newRoute);
    } else {
        vector<patMapMatchingRoute> oldRouteCandidates = *routeCandidates;
        routeCandidates->erase(routeCandidates->begin(), routeCandidates->end());
        patBoolean containsEqual = patFALSE;
        for (patULong i = 0;
                i < oldRouteCandidates.size();
                ++i) {
            patBoolean isEqual = patTRUE;
            patMapMatchingRoute oldRoute = oldRouteCandidates.at(i);
            if (oldRoute.getRouteSize() != newRoute.getRouteSize()) {
                isEqual = patFALSE;
            } else {
                for (patULong j = 0; j < newRoute.getRouteSize(); ++j) {
                    if (newRoute.getArc(j) != oldRoute.getArc(j)) {
                        isEqual = patFALSE;
                        break;
                    }
                }
            }

            if (isEqual) {
                if (newRoute.getScore() < oldRoute.getScore()) {
                    routeCandidates->push_back(newRoute);
                } else {
                    routeCandidates->push_back(oldRoute);
                }
                containsEqual = patTRUE;
            } else {
                routeCandidates->push_back(oldRoute);
            }
        }
        if (!containsEqual) {
            routeCandidates->push_back(newRoute);
        }
    }
}

/**
 * Check if a GPS point reached the end of the arc.
 * Criteria:
 *  1. Relative position
 *  2. Heading difference >85
 *  3. Curve length of assigned GPS points > length of the arc
 * @param arc
 * @param gps
 * @param prevGps, previous GPS point
 * @param route
 * @return patTRUE/patFALSE
 */
patBoolean patMapMatching::reachedEndOfArc(patArc* arc, patGpsPoint* gps, patGpsPoint* prevGps, patMapMatchingRoute* route) {
    map<char*, patReal> dist = gps->distanceTo(theNetwork, arc);
    if (dist["position"] == 1) {
        DEBUG_MESSAGE("leave arc by position");
        return patTRUE;
    }

    patReal gpsHeading = prevGps->calHeading(gps);

    patReal headingDiff = fabs(gpsHeading - arc->attributes.heading);
    if(!(headingDiff<=85.0||fabs(360.0 - headingDiff) <= 85.0)){
            DEBUG_MESSAGE("leave arc by angle"<<gpsHeading<<","<<arc->attributes.heading<<"-"<<headingDiff<<","<<fabs(360.0 - headingDiff) );
            return patTRUE;
    }
    patReal distance = 0.0;
    patGpsPoint* lastOldGps = NULL;
    vector<patGpsPoint*>* assignedGps = route->getGpsAssignedToArc(arc);

    for (vector<patGpsPoint*>::iterator gIter = assignedGps->begin();
            gIter != assignedGps->end();
            ++gIter) {
        if (lastOldGps != NULL) {
            distance += (*gIter)->distanceTo(lastOldGps);
        }
        lastOldGps = (*gIter);
    }
    if (distance > arc->getLength()) {
        DEBUG_MESSAGE("leave arc by length");
        return patTRUE;
    }

    return patFALSE;

}

/**
 * Check if a GPS point reached an arc or not.
 * Criteria: relative position.
 * @param arc
 * @param gps
 * @return
 */
patBoolean patMapMatching::reachedStartOfArc(patArc* arc, patGpsPoint* gps) {
    map<char*, patReal> dist = gps->distanceTo(theNetwork, arc);
    //DEBUG_MESSAGE(dist["position"]<<*gps<<"===="<<*arc);
    if (dist["position"] == -1) {
        return patTRUE;
    }
    return patFALSE;
}

patPathJ patMapMatching::joinChosenRoutes(list<patMapMatchingRoute>* subRoutes,
        patBoolean succ,
        patError*& err
        ) {
    DEBUG_MESSAGE(subRoutes->size());
    patPathJ path;
    if (subRoutes->size() == 0) {
        succ = patFALSE;
        return path;
    } else if (subRoutes->size() == 1) {
        return subRoutes->back().getPath();
    } else {
        list<patMapMatchingRoute>::iterator itSR = subRoutes->begin();
        patMapMatchingRoute lastSubRoute = *itSR;

        itSR++;
        for (; itSR != subRoutes->end(); ++itSR) {
            patMapMatchingRoute currSubRoute = *itSR;
            vector<patNode*> listOfStartNodes;

            patNode* lastNode = theNetwork->getNodeFromUserId(lastSubRoute.getLastArc()->downNodeId);
            if (lastNode == NULL) {
                err = new patErrNullPointer("patNode");
                WARNING(err->describe());
                return path;
            }
            listOfStartNodes.push_back(lastNode);
            list<patArc*> lastSubRouteArcs = lastSubRoute.getAllArcs();

            list<patArc*>::iterator arcIter = lastSubRouteArcs.end();
            while (arcIter != lastSubRouteArcs.begin() &&
                    find(listOfStartNodes.begin(), listOfStartNodes.end(), lastSubRoute.getRealStartNode()) == listOfStartNodes.end()) {
                arcIter--;
                if (lastSubRoute.getArcScore(*arcIter) >= routeJoiningQualityThreshold) {
                    DEBUG_MESSAGE("last: an arc not in threshold" << lastSubRoute.getArcScore(*arcIter));
                    lastNode = theNetwork->getNodeFromUserId((*arcIter)->upNodeId);
                    if (lastNode == NULL) {
                        err = new patErrNullPointer("patNode");
                        WARNING(err->describe());
                        return path;
                    }
                    listOfStartNodes.push_back(lastNode);
                } else {
                    DEBUG_MESSAGE("an arc in threshold" << **arcIter);
                    break;
                }
            }

            /*Determine the potential end nodes for the gap filling, same logic as for start nodes only reverse*/

            vector<patNode*> listOfEndNodes;

            lastNode = theNetwork->getNodeFromUserId(currSubRoute.getFirstArc()->upNodeId);
            if (lastNode == NULL) {
                err = new patErrNullPointer("patNode");
                WARNING(err->describe());
                return path;
            }
            listOfEndNodes.push_back(lastNode);
            list<patArc*> currSubRouteArcs = currSubRoute.getAllArcs();
            arcIter = currSubRouteArcs.begin();
            DEBUG_MESSAGE("curr route size" << currSubRouteArcs.size());
            while (arcIter != currSubRouteArcs.end()) {

                if (currSubRoute.getArcScore(*arcIter) >= routeJoiningQualityThreshold) {
                    //					DEBUG_MESSAGE("an arc not in threshold"<<**arcIter<<"-"<<currSubRoute.getArcScore(*arcIter));
                    lastNode = theNetwork->getNodeFromUserId((*arcIter)->downNodeId);
                    if (lastNode == NULL) {
                        err = new patErrNullPointer("patNode");
                        WARNING(err->describe());
                        return path;
                    }
                    listOfEndNodes.push_back(lastNode);
                } else {
                    DEBUG_MESSAGE("an arc in threshold");
                    break;
                }

                arcIter++;
            }


            map<patReal, set<list<patULong> > > paths;
            for (vector<patNode*>::iterator eIter = listOfEndNodes.begin();
                    eIter != listOfEndNodes.end();
                    ++eIter) {
                patNode* endNode = *eIter;

                patShortestPathAlgoDest destAlgo(endNode->internalId,
                        theNetwork->adjacencyLists,
                        theNetwork->minimumLabelForShortestPath);

                destAlgo.computeShortestPathTree(err);
                if (err != NULL) {
                    WARNING(err->describe());
                    return path;
                }
                patShortestPathTreeDest destTree = destAlgo.getTree();
                for (vector<patNode*>::iterator sIter = listOfStartNodes.begin();
                        sIter != listOfStartNodes.end();
                        ++sIter) {

                    patNode* startNode = *sIter;

                    if (startNode != endNode) {
                        list<patULong> seg = destTree.getShortestPathFrom(startNode->internalId, err);

                        if (err != NULL) {
                            WARNING(err->describe());
                            return path;
                        }
                        patReal cost = destTree.labels[startNode->internalId];

                        if (paths.find(cost) == paths.end()) {
                            set<list<patULong> > ele;
                            ele.insert(seg);
                            paths[cost] = ele;
                        }
                        else {
                            paths[cost].insert(seg);
                        }

                    }

                }
            }

            if (paths.empty()) {


                stringstream str;
                str << "shortest path is empty";
                err = new patErrMiscError(str.str());
                WARNING(err->describe());
                return path;


            } else {
                /*Filter cases in which the entire lastSubRoute or entire currentSubroute would be replaced,
                 * there is something wrong with the mapmatching and those routes should not be included*/
                set<list<patULong> > shortestPaths = paths.begin()->second;
                list<patULong> shortestPath = *(shortestPaths.begin());

                if (shortestPath.front() == theNetwork->getInternalNodeIdFromUserId(lastSubRoute.getFirstArc()->upNodeId) ||
                        shortestPath.back() == theNetwork->getInternalNodeIdFromUserId(lastSubRoute.getLastArc()->downNodeId)) {

                    stringstream str;
                    str << "the entire route is replace by shortest path";
                    err = new patErrMiscError(str.str());
                    WARNING(err->describe());
                    return path;

                } else {
                    /*Take the shortest path connection with the lowest travel cost,
                     * check whether the shortest path contains part of the last or the next subrouteWithGap and
                     * derive the subroutes in the way that no link is used twice*/
                    list<patNode*> shortestPathNodes;
                    DEBUG_MESSAGE("nbr of nodes" << shortestPath.size());
                    for (list<patULong>::iterator nIdIter = shortestPath.begin();
                            nIdIter != shortestPath.end();
                            ++nIdIter) {
                        patNode* aNode = theNetwork->internalNodes[*nIdIter];
                        if (aNode == NULL) {
                            err = new patErrNullPointer("patNode");
                            WARNING(err->describe());
                            return path;
                        }
                        //						DEBUG_MESSAGE("node"<<*aNode);
                        shortestPathNodes.push_back(aNode);

                    }
                    patNode* startNodeShortestPath = shortestPathNodes.front();
                    patNode* endNodeShortestPath = shortestPathNodes.back();


                    list<patNode*>::iterator nodeIter = shortestPathNodes.end();
                    while (nodeIter != shortestPathNodes.begin()) {
                        nodeIter--;
                        if (lastSubRoute.searchFromEnd(*nodeIter, lastSubRoute.getRealStartNode())) {
                            startNodeShortestPath = *nodeIter;
                            break;
                        }
                    }
                    DEBUG_MESSAGE("start shortest path" << *startNodeShortestPath);
                    while (!shortestPathNodes.empty() && shortestPathNodes.front() != startNodeShortestPath) {
                        shortestPathNodes.pop_front();
                    }

                    nodeIter = shortestPathNodes.begin();
                    for (; nodeIter != shortestPathNodes.end(); ++nodeIter) {
                        if (currSubRoute.searchFromBegin(*nodeIter)) {
                            currSubRoute.setRealStartNode(*nodeIter);
                            endNodeShortestPath = *nodeIter;
                            break;
                        }
                    }
                    while (!shortestPathNodes.empty() && shortestPathNodes.back() != endNodeShortestPath) {
                        shortestPathNodes.pop_back();
                    }
                    DEBUG_MESSAGE("end shortest path" << *endNodeShortestPath);
                    /*Filter again those stages where the entire lastSubRoute or the entire currentSubroute would be replaced
                     * indicated by the start node of the shortest path equaling the realStartNode of lastSubRoute
                     * or the end node of the shortest path equaling the end node of currentSubroute
                     * and filter for all empty shortest paths */
                    //DEBUG_MESSAGE(*startNodeShortestPath<<"-"<<*endNodeShortestPath);

                    if (lastSubRoute.getRealStartNode() != startNodeShortestPath
                            && currSubRoute.getEndNode(theNetwork, err) != endNodeShortestPath
                            && startNodeShortestPath != endNodeShortestPath) {


                        /*Check if the resulting shortest paths are feasible according to network speeds (+50% Buffer)
                         * if they are not, set stage as filtered for mapmatching*/
                        patPathJ shortestPathJ(shortestPathNodes, theNetwork, err);

                        if (err != NULL) {
                            return path;
                        }
                        patReal travelTime = currSubRoute.getFirstGps()->getTimeStamp() - lastSubRoute.getLastGps()->getTimeStamp();
                        if (false) {
                            //if (shortestPathJ.computePathLength() > 1.5 * travelTime * patNBParameters::the()->arcFreeFlowSpeed) {
                            DEBUG_MESSAGE(shortestPathJ.computePathLength() << "-" << 1.5 * travelTime * patNBParameters::the()->arcFreeFlowSpeed);
                            stringstream str;
                            str << "path is too long";
                            err = new patErrMiscError(str.str());
                            WARNING(err->describe());
                            return path;

                        } else {
                            DEBUG_MESSAGE("joining segments");
                            /*Take the shortest path connection with the lowest travel cost and add links to chosenRouteLinks
                             * first, add all links of lastSubRoute until the start node of the gap filling path is reached*/
                            patPathJ lastRealPath;
                            patULong getPathStatus = lastSubRoute.getPath(&lastRealPath, lastSubRoute.getRealStartNode(), startNodeShortestPath);
                            if (getPathStatus != 3) {
                                stringstream str;
                                str << "fail to get path" << getPathStatus;
                                err = new patErrMiscError(str.str());
                                WARNING(err->describe());
                                return path;
                            }
                            DEBUG_MESSAGE(lastRealPath);
                            path.append(&lastRealPath);

                            /*second, add all the links of the gap filling path*/
                            patPathJ realShortestPath;
                            getPathStatus = shortestPathJ.getSubPath(&realShortestPath, startNodeShortestPath, endNodeShortestPath);
                            if (getPathStatus != 3) {
                                stringstream str;
                                str << "fail to get path" << getPathStatus;
                                err = new patErrMiscError(str.str());
                                WARNING(err->describe());
                                return path;
                            }
                            DEBUG_MESSAGE(shortestPathJ);
                            path.append(&shortestPathJ);

                            /*third, reset the start node of current subroute to the end node of the gap filling path*/
                            currSubRoute.setRealStartNode(endNodeShortestPath);
                        }
                    } else {
                        DEBUG_MESSAGE(*lastSubRoute.getStartNode(theNetwork, err));

                        DEBUG_MESSAGE(*lastSubRoute.getRealStartNode());
                        DEBUG_MESSAGE(*currSubRoute.getEndNode(theNetwork, err));
                        stringstream str;
                        str << "the entire route is replace by shortest path";
                        err = new patErrMiscError(str.str());
                        WARNING(err->describe());
                        return path;
                    }


                }

            }
            lastSubRoute = currSubRoute;
        }

        patPathJ lastSubPath;
        patULong getPathStatus = lastSubRoute.getPath(&lastSubPath, lastSubRoute.getRealStartNode(), lastSubRoute.getEndNode(theNetwork, err));
        if (getPathStatus != 3) {
            stringstream str;
            str << "fail to get path" << getPathStatus;
            err = new patErrMiscError(str.str());
            WARNING(err->describe());
            return path;
        }
        DEBUG_MESSAGE(lastSubPath);
        path.append(&lastSubPath);
        return path;
    }
}

