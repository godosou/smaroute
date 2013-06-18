#include "patMapMatchingV2.h"
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
#include "patShortestPathAlgoRange.h"
#include "patShortestPathTreeRange.h"
#include "patNBParameters.h"
#include "patStreetSegment.h"

#include <algorithm>

patMapMatchingV2::patMapMatchingV2()  :
maxGapDistance(patNBParameters::the()->maxGapDistance),
maxGapTime(patNBParameters::the()->maxGapTime),
initialSearchRadius(patNBParameters::the()->initialSearchRadius),
minNbrOfStartNodes(patNBParameters::the()->minNbrOfStartNodes),
searchRadiusIncrement(patNBParameters::the()->searchRadiusIncrement),
maxNbrOfCandidates(patNBParameters::the()->maxNbrOfCandidates),
minNbrOfGpsPerSegment(patNBParameters::the()->minNbrOfGpsPerSegment),
routeJoiningQualityThreshold(patNBParameters::the()->routeJoiningQualityThreshold){
}

patMapMatchingV2::patMapMatchingV2(vector<patGpsPoint> theGpsSequence,
        patNetwork* network, vector< list <  pair<patArc*,unsigned long> > >* adjList) :
theNetwork(network),
        networkAdjList(adjList),
maxGapDistance(patNBParameters::the()->maxGapDistance),
maxGapTime(patNBParameters::the()->maxGapTime),
initialSearchRadius(patNBParameters::the()->initialSearchRadius),
minNbrOfStartNodes(patNBParameters::the()->minNbrOfStartNodes),
searchRadiusIncrement(patNBParameters::the()->searchRadiusIncrement),
maxNbrOfCandidates(patNBParameters::the()->maxNbrOfCandidates),
minNbrOfGpsPerSegment(patNBParameters::the()->minNbrOfGpsPerSegment),
routeJoiningQualityThreshold(patNBParameters::the()->routeJoiningQualityThreshold) {
    DEBUG_MESSAGE(maxGapDistance);
    DEBUG_MESSAGE(maxGapTime);
    DEBUG_MESSAGE(initialSearchRadius);
    DEBUG_MESSAGE(minNbrOfStartNodes);
    DEBUG_MESSAGE(searchRadiusIncrement);
    DEBUG_MESSAGE(maxNbrOfCandidates);
    DEBUG_MESSAGE(minNbrOfGpsPerSegment);

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
set<patNode*> patMapMatchingV2::getNearestNodes(patGpsPoint* theGpsPoint, double searchRadius, patError*& err) {
    //DEBUG_MESSAGE("get nearby nodes");
    set<patNode*> rtnNodes;
    //DEBUG_MESSAGE("nodes"<<theNetwork->internalNodes.size());
    //map<patULong, patNode>::iterator nodeIter = theNetwork->theNodes.begin();
    //DEBUG_MESSAGE("nodes"<<theNetwork->theNodes.size());
    for (map<unsigned long, patNode>::iterator nodeIter = theNetwork->theNodes.begin();
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
bool patMapMatchingV2::stageMapMatchingGenerateSubRoutesWithGaps(list<patMapMatchingRoute>* subRoutesWithGaps, patError* err) {
    DEBUG_MESSAGE("start map matching");
    vector<patMapMatchingRoute>* routeCandidates = new vector<patMapMatchingRoute>;

    for (unsigned long i = 0;
            i < gpsSequence.size();
            ++i) {
        DEBUG_MESSAGE("gps [" << i + 1 << "]" << gpsSequence.at(i));
        patGpsPoint currGps = gpsSequence.at(i);

        /*
         * If there is not route candidate,
         * 1. initiate with searching nodes nearby the current GPS point
         * 2. Search over outgoing links of the neareast nodes for new route candidates
         *     no condition applied for the search.
         */
        if (routeCandidates->empty()) {

            double searchRadius = patNBParameters::the()->initialSearchRadius;
            set<patNode*> nearestNodes = getNearestNodes(&currGps, searchRadius, err);
            while (nearestNodes.size() < patNBParameters::the()->minNbrOfStartNodes) {
                searchRadius += patNBParameters::the()->searchRadiusIncrement;
                nearestNodes = getNearestNodes(&currGps, searchRadius, err);

            }


            DEBUG_MESSAGE("nearby nodes: " << searchRadius << "-" << nearestNodes.size());
            for (set<patNode*>::iterator nodeIter = nearestNodes.begin();
                    nodeIter != nearestNodes.end();
                    ++nodeIter
                    ) {
                patNode* currNode = *nodeIter;
                for (set<unsigned long>::iterator succIter = currNode->userSuccessors.begin();
                        succIter != currNode->userSuccessors.end();
                        ++succIter) {
                    patArc* aArc = theNetwork->getArcFromNodesUserId(currNode->userId, *succIter);
                    if (aArc == NULL) {
                        err = new patErrNullPointer("patNode");
                        WARNING(err->describe());
                        return false;
                    }
                    if (aArc->m_name == "M1" ||
                            aArc->m_attributes.priority < 4 ||
                            aArc->m_attributes.priority > 14) {
                        continue;
                    }
                    patStreetSegment currStreet(theNetwork, aArc, err);
                    patMapMatchingRoute newRoute;
                    newRoute.addStreet(currStreet, theNetwork, err);
                    newRoute.addGpsStreetAssignment(currStreet, &currGps, theNetwork, err);
                    newRoute.calScore(theNetwork);
                    addRouteCandidate(routeCandidates, newRoute);
                }
            }
        }/*
             * If there are route candidates,
             *   and the current GPS point is too far away from the last one
             *   in terms of time and distance:
             * 1. Break the current sub-route matching procedure
             * 2. Select the best route candidate for the current sub-route
             * 3. Start new sub-route matching procedure
             */
        else if (currGps.distanceTo(&gpsSequence.at(i - 1)) > patNBParameters::the()->maxGapDistance ||
                currGps.getTimeStamp() - gpsSequence.at(i - 1).getTimeStamp() > patNBParameters::the()->maxGapTime) {
            /*not used: filter segments that are too short because they can be totally off and lead to very weird routes when joined together with the other segments*/
            DEBUG_MESSAGE("a gap in trip");
            if ((routeCandidates->front().getNbrOfGpsInRoute() >= patNBParameters::the()->minNbrOfGpsPerSegment) &&
                    (routeCandidates->front().getRouteSize() > 1)) {
                DEBUG_MESSAGE("push the finished segment");
                subRoutesWithGaps->push_back(routeCandidates->front());

            }
            routeCandidates->erase(routeCandidates->begin(), routeCandidates->end());
            double searchRadius = patNBParameters::the()->initialSearchRadius;
            set<patNode*> nearestNodes = getNearestNodes(&currGps, searchRadius, err);
            DEBUG_MESSAGE("nearby nodes: " << searchRadius << "-" << nearestNodes.size());

            for (set<patNode*>::iterator nodeIter = nearestNodes.begin();
                    nodeIter != nearestNodes.end();
                    ++nodeIter
                    ) {
                patNode* currNode = *nodeIter;
                for (set<unsigned long>::iterator succIter = currNode->userSuccessors.begin();
                        succIter != currNode->userSuccessors.end();
                        ++succIter) {
                    patArc* aArc = theNetwork->getArcFromNodesUserId(currNode->userId, *succIter);
                    if (aArc == NULL) {
                        err = new patErrNullPointer("patNode");
                        WARNING(err->describe());
                        return false;
                    }
                    if (aArc->m_name == "M1" ||
                            aArc->m_attributes.priority < 4 ||
                            aArc->m_attributes.priority > 14) {
                        continue;
                    }
                    patStreetSegment currStreet(theNetwork, aArc, err);

                    patMapMatchingRoute newRoute;

                    newRoute.addStreet(currStreet, theNetwork, err);

                    newRoute.addGpsStreetAssignment(currStreet, &currGps, theNetwork, err);

                    newRoute.calScore(theNetwork);

                    addRouteCandidate(routeCandidates, newRoute);


                }
            }

        }/*
             * If there are route candidates and
             *  the current GPS point is not too far away from the last one
             *  continue matching procedure for the current sub-route.
             *
             */

        else {
            vector<patMapMatchingRoute> oldRouteCandidates = *routeCandidates;
            routeCandidates->erase(routeCandidates->begin(), routeCandidates->end());
            DEBUG_MESSAGE("old route size:" << oldRouteCandidates.size());
            unsigned long j = 0;
            for (vector<patMapMatchingRoute>::iterator routeIter = oldRouteCandidates.begin();
                    routeIter != oldRouteCandidates.end();
                    ++routeIter) {
                patMapMatchingRoute currRoute = *routeIter;
                /*first, check if new Gpsinate reached the start of the link (for one link routes only!)
                 * if yes ignore the route*/
                patStreetSegment* lastStreet = currRoute.getLastStreet();
                patArc* lastArc = currRoute.getLastArc();
                if (currRoute.getRouteSize() != 1 || !reachedStartOfStreet(lastStreet, &currGps)) {
                    /*second, check if new coordinate reached the end of the link*/
                    if (reachedEndOfStreet(lastStreet, &currGps, currRoute.getLastGps(), &currRoute)) {
                        /*if yes, remove old route and generate for each outgoing link of lastArc.getToNode() a new route,
                         * except the one leading back to the start node of lastArc or one that contains a circle*/
                        unsigned long routesGenerated = 0;
                        patNode* lastNode = theNetwork->getNodeFromUserId(lastArc->m_down_node_id);
                        for (set<unsigned long>::iterator outIter = lastNode->userSuccessors.begin();
                                outIter != lastNode->userSuccessors.end();
                                ++outIter) {
                            if (*outIter != lastArc->m_up_node_id) {
                                patArc* currArc = theNetwork->getArcFromNodesUserId(lastArc->m_down_node_id, *outIter);

                                if (currArc == NULL) {
                                    err = new patErrNullPointer("patNode");
                                    WARNING(err->describe());
                                    return false;
                                }

                                if (currArc->m_name == "M1" ||
                                        currArc->m_attributes.priority < 4 ||
                                        currArc->m_attributes.priority > 14) {
                                    continue;
                                }


                                if (!currRoute.searchFromEnd(currArc)) {
                                    //       DEBUG_MESSAGE("new route candidate with new street");
                                    patStreetSegment currStreet(theNetwork, currArc, err);
                                    patMapMatchingRoute newRoute = currRoute;
                                    newRoute.addStreet(currStreet, theNetwork, err);
                                    newRoute.addGpsStreetAssignment(currStreet, &currGps, theNetwork, err);
                                    newRoute.calScore(theNetwork);
                                    addRouteCandidate(routeCandidates, newRoute);
                                    routesGenerated++;

                                }
                                //DEBUG_MESSAGE(routesGenerated);
                            }
                        }
                        /*account for dead ends at the end of the trip where coords might run off the network:
                         *assign new coord to old link and recalculate the score*/
                        if (routesGenerated == 0) {
                            currRoute.addGpsStreetAssignment(*lastStreet, &currGps, theNetwork, err);
                            currRoute.calScore(theNetwork);
                            addRouteCandidate(routeCandidates, currRoute);
                        }
                    } else {
                        /*else, assign new coord to old link and recalculate the score*/
                        currRoute.addGpsStreetAssignment(*lastStreet, &currGps, theNetwork, err);

                        currRoute.calScore(theNetwork);
                        addRouteCandidate(routeCandidates, currRoute);

                    }
                }

            }

        }

        DEBUG_MESSAGE("route candidate:" << routeCandidates->size() << "----" << patNBParameters::the()->maxNbrOfCandidates);
        DEBUG_MESSAGE("number of gps" << routeCandidates->front().getNbrOfGpsInRoute());
        DEBUG_MESSAGE("number of arcs" << routeCandidates->front().getRouteSize());
        sort(routeCandidates->begin(), routeCandidates->end());
        while (routeCandidates->size() > patNBParameters::the()->maxNbrOfCandidates) {

            routeCandidates->pop_back();
        }
    }

    bool succ = true;
    DEBUG_MESSAGE("route candidate:" << routeCandidates->size() << "----" << patNBParameters::the()->maxNbrOfCandidates);
    /*if finished with all coords: evaluate the last set of route candidates and store the one with the best score*/
    if (!routeCandidates->empty()) {
        if ((routeCandidates->front().getNbrOfGpsInRoute() >= patNBParameters::the()->minNbrOfGpsPerSegment)
                && (routeCandidates->front().getRouteSize() >= 1)) {
            DEBUG_MESSAGE("new route : " << routeCandidates->front().getScore());
            for (vector<patMapMatchingRoute>::iterator routeIter = routeCandidates->begin();
                    routeIter != routeCandidates->end();
                    ++routeIter) {
                DEBUG_MESSAGE(routeIter->getScore());
            }
            subRoutesWithGaps->push_back(routeCandidates->front());
        } else {
            DEBUG_MESSAGE("invalid subroute:" << routeCandidates->front().getNbrOfGpsInRoute());
        }


    } else {
        succ = false;
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
patPathJ patMapMatchingV2::run(patError* err) {
    list<patMapMatchingRoute>* subRoutesWithGaps = new list<patMapMatchingRoute>;
    bool succ = stageMapMatchingGenerateSubRoutesWithGaps(subRoutesWithGaps, err);
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
 *    such that the list is always sorted with score low to high
 * @param routeCandidates
 * @param newRoute
 */
void patMapMatchingV2::addRouteCandidate(vector<patMapMatchingRoute>* routeCandidates,
        patMapMatchingRoute newRoute) {

    if (routeCandidates->empty()) {
        routeCandidates->push_back(newRoute);
    } else


        /*{
            vector<patMapMatchingRoute>::iterator routeIter = routeCandidates->begin();
            for (patULong i = 0;
                    i < routeCandidates->size();
                    ++i) {
                if (newRoute.getScore()<(*routeIter).getScore()){
                    routeCandidates->insert(routeIter,newRoute);
                    break;
                }
                routeIter++;
            }
            if(routeIter==routeCandidates->end()){
                routeCandidates->push_back(newRoute);
            }
        }
         */ {
        vector<patMapMatchingRoute> oldRouteCandidates = *routeCandidates;
        routeCandidates->erase(routeCandidates->begin(), routeCandidates->end());
        bool containsEqual = false;
        for (unsigned long i = 0;
                i < oldRouteCandidates.size();
                ++i) {
            bool isEqual = true;
            patMapMatchingRoute oldRoute = oldRouteCandidates.at(i);
            if (oldRoute.getRouteSize() != newRoute.getRouteSize()) {
                isEqual = false;
            } else {
                for (unsigned long j = 0; j < newRoute.getRouteSize(); ++j) {
                    if (newRoute.getArc(j) != oldRoute.getArc(j)) {
                        isEqual = false;
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
                containsEqual = true;
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
 *  1. Relative position, or
 *  2. Heading difference >85, or
 *  3. Curve length of assigned GPS points > length of the arc
 * @param arc
 * @param gps
 * @param prevGps, previous GPS point
 * @param route
 * @return patTRUE/patFALSE
 */
bool patMapMatchingV2::reachedEndOfStreet(patStreetSegment* aSegment, patGpsPoint* gps, patGpsPoint* prevGps, patMapMatchingRoute* route) {

    bool distExceed = true;
    const list<patArc*>* arcList = aSegment->getArcList();
    for (list<patArc*>::const_iterator aIter = arcList->begin();
            aIter != arcList->end();
            ++aIter) {
        map<char*, double> dist = gps->distanceTo(theNetwork, *aIter);
        if (dist["position"] != 1) {
            distExceed = false;
        }
    }
    if (distExceed == true) {
        return distExceed;
    }
    bool headingExceed = true;
    double gpsHeading = prevGps->calHeading(gps);
    for (list<patArc*>::const_iterator aIter = arcList->begin();
            aIter != arcList->end();
            ++aIter) {
        double headingDiff = fabs(gpsHeading - (*aIter)->m_attributes.heading);
        if ((headingDiff <= 85.0 || fabs(360.0 - headingDiff) <= 85.0)) {
            headingExceed = false;
        }
    }
    if (headingExceed == true) {
        return headingExceed;
    }

    bool curveExceed = true;
    double distance = 0;
    patGpsPoint* lastOldGps = NULL;
    vector<patGpsPoint*>* assignedGps = route->getGpsAssignedToStreet(aSegment);

    for (vector<patGpsPoint*>::iterator gIter = assignedGps->begin();
            gIter != assignedGps->end();
            ++gIter) {
        if (lastOldGps != NULL) {
            distance += (*gIter)->distanceTo(lastOldGps);
        }
        lastOldGps = (*gIter);
    }
    if (distance <= aSegment->getLength()) {
        curveExceed = false;
    }

    if (curveExceed == true) {
        return curveExceed;
    }
    return false;
}

/**
 * Check if a GPS point reached an street or not.
 * Criteria: relative position.
 * @param arc
 * @param gps
 * @return
 */
bool patMapMatchingV2::reachedStartOfStreet(patStreetSegment* aSegment, patGpsPoint* gps) {

    const list<patArc*>* arcList = aSegment->getArcList();

    map<char*, double> dist = gps->distanceTo(theNetwork, arcList->front());
    if (dist["position"] == -1) {
        return true;
    }
    return false;
}

patPathJ patMapMatchingV2::joinChosenRoutes(list<patMapMatchingRoute>* subRoutes,
        bool succ,
        patError*& err
        ) {
    DEBUG_MESSAGE(subRoutes->size());
    DEBUG_MESSAGE(subRoutes->front().getRouteSize());
    DEBUG_MESSAGE(subRoutes->front().getRouteStreetSize());
    //DEBUG_MESSAGE(subRoutes->front().getRouteSize());

    patPathJ path;

    /*
     If no sub routes are generated*/
    if (subRoutes->size() == 0) {
        WARNING("no sub routes generated")
        succ = false;
        return path;

    }        /*If there are only one sub route (segment)
     return the  path
     */
    else if (subRoutes->size() == 1) {
        return subRoutes->front().getPath();
    }
        /*
         If ther are several segments. Need to connect between them
         */
    else {
        list<patMapMatchingRoute>::iterator itSR = subRoutes->begin();
        patMapMatchingRoute lastSubRoute = *itSR; //the previous segment

        itSR++;
        for (; itSR != subRoutes->end(); ++itSR) {
            patMapMatchingRoute currSubRoute = *itSR; //the current segment

            /*get the list of potential start nodes for the connection*/
            vector<patNode*> listOfStartNodes;

            patNode* lastNode = theNetwork->getNodeFromUserId(lastSubRoute.getLastArc()->m_down_node_id); //the last node of the last segment
            if (lastNode == NULL) {
                err = new patErrNullPointer("patNode");
                WARNING(err->describe());
                return path;
            }
            listOfStartNodes.push_back(lastNode);
            list<patArc*> lastSubRouteArcs = lastSubRoute.getAllArcs();

            list<patArc*>::iterator arcIter = lastSubRouteArcs.end();


            while (arcIter != lastSubRouteArcs.begin() &&
                    find(listOfStartNodes.begin(), listOfStartNodes.end(), lastSubRoute.getRealStartNode()) == listOfStartNodes.end())
                /*Loop condition
                 * 1. not reached the first arc
                 * 2. not reached the real start node
                 */ {
                arcIter--;

                //get all the arcs until an arc reached joining threshold.
                double lastArcScore = lastSubRoute.getArcScore(*arcIter);
                if (lastArcScore >= patNBParameters::the()->routeJoiningQualityThreshold) {
                    DEBUG_MESSAGE("last: an arc not in threshold" << lastArcScore << "," << patNBParameters::the()->routeJoiningQualityThreshold);
                    lastNode = theNetwork->getNodeFromUserId((*arcIter)->m_up_node_id);
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

            lastNode = theNetwork->getNodeFromUserId(currSubRoute.getFirstArc()->m_up_node_id);
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

                if (currSubRoute.getArcScore(*arcIter) >= patNBParameters::the()->routeJoiningQualityThreshold) {
                    //					DEBUG_MESSAGE("an arc not in threshold"<<**arcIter<<"-"<<currSubRoute.getArcScore(*arcIter));
                    lastNode = theNetwork->getNodeFromUserId((*arcIter)->m_down_node_id);
                    if (lastNode == NULL) {
                        err = new patErrNullPointer("patNode");
                        WARNING(err->describe());
                        return path;
                    }
                    listOfEndNodes.push_back(lastNode);
                } else {
                    DEBUG_MESSAGE("an arc in threshold"<<**arcIter);
                    break;
                }

                arcIter++;
            }

            DEBUG_MESSAGE("number of start/end nodes: "<<listOfStartNodes.size()<<"/"<<listOfEndNodes.size());
            map<double, set<list<patArc*> > > paths;
            for (vector<patNode*>::iterator sIter = listOfStartNodes.begin();
                    sIter != listOfStartNodes.end();
                    ++sIter) {
                patNode* startNode = *sIter;
                set<unsigned long> sp_tree_start;
                sp_tree_start.insert(startNode->internalId);
                patShortestPathAlgoRange theShortestPathAlgoRange(NULL, networkAdjList, theNetwork, theNetwork->minimumLabelForShortestPath);
                theShortestPathAlgoRange.computeShortestPathTree(sp_tree_start, double(5000), NULL);

                patShortestPathTreeRange theTree = theShortestPathAlgoRange.getTree();

                for (vector<patNode*>::iterator eIter = listOfEndNodes.begin();
                        eIter != listOfEndNodes.end();
                        ++eIter) {

                    patNode* endNode = *sIter;

                    if (startNode != endNode) {
                        list<patArc*> seg = theTree.getShortestPathToV2(endNode->internalId);

                        double cost = theTree.getShortestPathCost(endNode->internalId);

                        if (paths.find(cost) == paths.end()) {
                            set<list<patArc*> > ele;
                            ele.insert(seg);
                            paths[cost] = ele;
                        } else {
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
                set<list<patArc*> > shortestPaths = paths.begin()->second;
                list<patArc*> shortestPath = *(shortestPaths.begin());

                if (shortestPath.front() == lastSubRoute.getFirstArc() ||
                        shortestPath.back() == lastSubRoute.getLastArc()) {

                    stringstream str;
                    str << "the entire route is replace by shortest path";
                    err = new patErrMiscError(str.str());
                    WARNING(err->describe());
                    return path;

                } else {
                    /*Take the shortest path connection with the lowest travel cost,
                     * check whether the shortest path contains part of the last or the next subrouteWithGap and
                     * derive the subroutes in the way that no link is used twice*/

                    patNode* startNodeShortestPath = theNetwork->getNodeFromUserId(shortestPath.front()->m_up_node_id);
                    patNode* endNodeShortestPath = theNetwork->getNodeFromUserId(shortestPath.back()->m_down_node_id);

                    list<patArc*>::iterator arcIter = shortestPath.end();
                    arcIter--;
                    patNode* listedNode = theNetwork->getNodeFromUserId((*arcIter)->m_down_node_id);
                    if (lastSubRoute.searchFromEnd(listedNode, lastSubRoute.getRealStartNode())) {
                        startNodeShortestPath = listedNode;
                    }

                    while (arcIter != shortestPath.begin()) {
                        arcIter--;
                        listedNode = theNetwork->getNodeFromUserId((*arcIter)->m_up_node_id);
                        if (lastSubRoute.searchFromEnd(listedNode, lastSubRoute.getRealStartNode())) {
                            startNodeShortestPath = listedNode;
                            break;
                        }
                    }
                    DEBUG_MESSAGE("start shortest path" << *startNodeShortestPath);
                    while (!shortestPath.empty() && shortestPath.front()->m_down_node_id != startNodeShortestPath->userId) {
                        shortestPath.pop_front();
                    }

                    arcIter = shortestPath.begin();
                    listedNode = theNetwork->getNodeFromUserId((*arcIter)->m_up_node_id);
                    if (currSubRoute.searchFromBegin(listedNode)) {
                        currSubRoute.setRealStartNode(listedNode);
                        endNodeShortestPath = listedNode;
                    }
                    for (; arcIter != shortestPath.end(); ++arcIter) {
                        listedNode = theNetwork->getNodeFromUserId((*arcIter)->m_down_node_id);
                        if (currSubRoute.searchFromBegin(listedNode)) {
                            currSubRoute.setRealStartNode(listedNode);
                            endNodeShortestPath = listedNode;
                            break;
                        }
                    }
                    while (!shortestPath.empty() && shortestPath.back()->m_up_node_id != endNodeShortestPath->userId) {
                        shortestPath.pop_back();
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
                        patPathJ shortestPathJ(shortestPath);

                        if (err != NULL) {
                            return path;
                        }
                        double travelTime = currSubRoute.getFirstGps()->getTimeStamp() - lastSubRoute.getLastGps()->getTimeStamp();
                        if (false) {
                            //if (shortestPathJ.computePathLength() > 1.5 * travelTime * patNBParameters::the()->arcFreeFlowSpeed) {
                            DEBUG_MESSAGE(shortestPathJ.computeLength() << "-" << 1.5 * travelTime * patNBParameters::the()->arcFreeFlowSpeed);
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
                            unsigned long getPathStatus = lastSubRoute.getPath(&lastRealPath, lastSubRoute.getRealStartNode(), startNodeShortestPath);
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
        unsigned long getPathStatus = lastSubRoute.getPath(&lastSubPath, lastSubRoute.getRealStartNode(), lastSubRoute.getEndNode(theNetwork, err));
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


