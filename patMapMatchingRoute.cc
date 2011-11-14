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

patMapMatchingRoute::patMapMatchingRoute() :
score(0.0),
realStartNode(NULL),
nbrOfGpsInRoute(0) {
}

ostream& operator<<(ostream& str,  patMapMatchingRoute& x) {
  list<patArc*> listOfArcs = x.getAllArcs();
 for(list<patArc*>::iterator arcIter=listOfArcs.begin();
 arcIter!=listOfArcs.end();
         arcIter++){
     str<<(*arcIter)->upNodeId<<"-"<<(*arcIter)->downNodeId<<",";
 }

}


/*
 Defin an operator for MapMatchingRoute.
The criteria is the route score.
Returns true if route1 has lower score than route2.
 */
bool operator<(const patMapMatchingRoute& route1, const patMapMatchingRoute& route2) {
    return route1.score < route2.score;

}

/*Add an arc to a MapMatchingRoute
Append the new arc to the end of the list of arcs..
If there is only one arc in the route, set the realStartNode to the start node of the new arc;
 */
void patMapMatchingRoute::addArc(patArc* aArc, patNetwork* theNetwork, patError* err) {
    vector<patGpsPoint*> assocGps;
    patArc* lastArc = getLastArc();

    route.push_back(pair<patArc*, vector<patGpsPoint*> >(aArc, assocGps));

    if (route.size() == 1) {
        patNode* node = theNetwork->getNodeFromUserId(aArc->upNodeId);
        if (node == NULL) {

            err = new patErrNullPointer("patNode");
            WARNING(err->describe());

            return;
        } else {
            realStartNode = node;
        }
    }else{
        if (lastArc->downNodeId!=aArc->upNodeId){
            DEBUG_MESSAGE("arcs don't match"<<lastArc->downNodeId<<","<<aArc->upNodeId);
        }
    }
}

/*
 * Assign GPS to an arc in the route
 * @params: an arc, an GPS points, the network, error caller
 * If the arc exists in the route, assign the GPS to it by appending the point to the associated GPS vector of the arc
 * Otherwise, add the arc to the route (the same procedure in patMapMatchingRoute::addArc, and associate the GPS point.

 */
void patMapMatchingRoute::addGpsArcAssignment(patArc* aArc,
        patGpsPoint* aGps, patNetwork* theNetwork, patError* err) {
    //DEBUG_MESSAGE(nbrOfGpsInRoute);
    patBoolean arcExists = patFALSE;
    nbrOfGpsInRoute++;
    lastGpsPoint = aGps;
    list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.end();
    while (aIter != route.begin()) {
        aIter--;
        //DEBUG_MESSAGE((*aIter).first <<","<<aArc);
        if ((*aIter).first == aArc) {
            (*aIter).second.push_back(aGps);
            arcExists = patTRUE;
            break;
        }

    }
    if (arcExists == patFALSE) {
        DEBUG_MESSAGE("ARC DONESN'T EXIST;")
        vector<patGpsPoint*> assocGps;
        assocGps.push_back(aGps);
        route.push_back(pair<patArc*, vector<patGpsPoint*> >(aArc, assocGps));
        if (route.size() == 1) {
            patNode* node = theNetwork->getNodeFromUserId(aArc->upNodeId);
            if (node == NULL) {

                err = new patErrNullPointer("patNode");
                WARNING(err->describe());

                return;
            } else {
                realStartNode = node;
            }
        }

    }
}
void patMapMatchingRoute::addStreet(patStreetSegment aStreet, patNetwork* theNetwork, patError*& err) {
    const list<patArc*>* arcList = aStreet.getArcList();
    for (list<patArc*>::const_iterator aIter = arcList->begin();
            aIter != arcList->end();
            ++aIter) {
        addArc(*aIter, theNetwork, err);
    }

    vector<patGpsPoint*> assocGps;
    StreetRoute.push_back(pair<patStreetSegment, vector<patGpsPoint*> >(aStreet, assocGps));
}

/**
 *
 * Assign GPS both to Street and arcs
 * 1. Assign GPS to street
 * 2. Find the nearest link on the street and assign GPS to it.
 * @param aStreet
 * @param aGps
 * @param theNetwork
 * @param err
 */
void patMapMatchingRoute::addGpsStreetAssignment(patStreetSegment aStreet, patGpsPoint* aGps, patNetwork* theNetwork, patError*& err) {
    //part 1
    patBoolean StreetExists = patFALSE;
    for (list<pair<patStreetSegment, vector<patGpsPoint*> > >::iterator aIter = StreetRoute.begin();
            aIter != StreetRoute.end();
            ++aIter) {
        if ((*aIter).first == aStreet) {
            (*aIter).second.push_back(aGps);
            StreetExists = patTRUE;
            break;
        }
    }
    if (StreetExists == patFALSE) {
        addStreet(aStreet, theNetwork, err);
        for (list<pair<patStreetSegment, vector<patGpsPoint*> > >::iterator aIter = StreetRoute.begin();
                aIter != StreetRoute.end();
                ++aIter) {
            if ((*aIter).first == aStreet) {
                (*aIter).second.push_back(aGps);
                StreetExists = patTRUE;
                break;
            }
        }
    }
    //part 2
    patReal minScore = patMaxReal;
    patArc* assignedArc = NULL;
    const list<patArc*>* arcList = aStreet.getArcList();
    for (list<patArc*>::const_iterator aIter = arcList->begin();
            aIter != arcList->end();
            ++aIter) {
        patReal newScore = aGps->distanceTo(theNetwork, *aIter)["link"];
        if (newScore < minScore) {
            minScore = newScore;
            assignedArc = *aIter;
        }
    }
    if (assignedArc == NULL) {
        err = new patErrMiscError("GPS not assigned!");
        WARNING(err->describe());
        return;
    }
    addGpsArcAssignment(assignedArc, aGps, theNetwork, err);
    //nbrOfGpsInRoute++;

}


/*
 * get the last arc (pointer) in the route.
 */
patArc* patMapMatchingRoute::getLastArc() {
    return route.back().first;
}

patStreetSegment* patMapMatchingRoute::getLastStreet() {
    return &(StreetRoute.back().first);
}

/*
 * calculate the score of the MapMatchingRoute
 * There are three scores beining calculated.
 * 1. minArcScores, of each arc: the minimum score gained from a gps point associated with the arc
 * 2. totalArcScore, of each arc: the sum of the scores gained from all GPS point associated with the arc
 * 3. score, of the route:  the sum over totalArcScore
 */
void patMapMatchingRoute::calScore(patNetwork* theNetwork) {
    score = 0.0;
    //DEBUG_MESSAGE(route.size());
    for (list<pair<patArc*, vector<patGpsPoint*> > >::iterator arcIter = route.begin();
            arcIter != route.end();
            ++arcIter) {
        patReal totalArcScore = 0.0;
        patReal minArcScore = patMaxReal;
        for (vector<patGpsPoint*>::iterator gpsIter = (*arcIter).second.begin();
                gpsIter != (*arcIter).second.end();
                ++gpsIter) {

            patReal newScore = (*gpsIter)->distanceTo(theNetwork, arcIter->first)["link"];
            minArcScore = (newScore < minArcScore) ? newScore : minArcScore;
            //patReal speedDiff = (*gpsIter)->getSpeed() - patNBParameters::the()->arcFreeFlowSpeed ;
            //if(speedDiff>0.0){
            //	newScore += speedDiff * speedDiff;
            //}
            totalArcScore += newScore;
        }
        totalArcScores[arcIter->first] = totalArcScore;
        minArcScores[arcIter->first] = minArcScore;
        score += totalArcScore;
    }
}

void patMapMatchingRoute::reCalScore(patArc* theArc, patGpsPoint* theGpsPoint,patBoolean newLink,patNetwork* theNetwork){
    patReal scoreDistanceIncrement = theGpsPoint->distanceTo(theNetwork,theArc)["link"];
    score+=scoreDistanceIncrement;
    //DEBUG_MESSAGE("score"<<scoreDistanceIncrement<<","<<score);
    if (newLink){
        minArcScores[theArc]=scoreDistanceIncrement;
        totalArcScores[theArc]=scoreDistanceIncrement;

    }
    else{
        if (scoreDistanceIncrement <minArcScores[theArc]) {
                    minArcScores[theArc] = scoreDistanceIncrement;
           }
            patReal oldTotalLinkScore =  totalArcScores[theArc];
             totalArcScores[theArc]=(oldTotalLinkScore+scoreDistanceIncrement) ;

    }

}

map<patArc*, patReal> patMapMatchingRoute::getMinArcScores() {
    return minArcScores;
}

patULong patMapMatchingRoute::getNbrOfGpsInRoute() {
    patULong nbr = 0;
    for (list<pair<patArc*, vector<patGpsPoint*> > >::iterator arcIter = route.begin();
            arcIter != route.end();
            ++arcIter) {
        //DEBUG_MESSAGE(arcIter->second.size())
        nbr += arcIter->second.size();
    }
    return nbr;
}

patNode* patMapMatchingRoute::getRealStartNode() {
    return realStartNode;
}

patULong patMapMatchingRoute::getRouteSize() {
    return route.size();
}

patReal patMapMatchingRoute::getScore() {

    return score;
}

map<patArc*, patReal> patMapMatchingRoute::getotalArcScores() {
    return totalArcScores;
}

void patMapMatchingRoute::setRealStartNode(patNode* n) {
    realStartNode = n;
}

/*
 * get all GPS points matched to an arc
 */
vector<patGpsPoint*>* patMapMatchingRoute::getGpsAssignedToArc(patArc* arc) {
    list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.end();
    while (aIter != route.begin()) {
        --aIter;
        if (arc = (*aIter).first) {
            return &((*aIter).second);
        }
    }

    return NULL;
}

/*
 * get all GPS points matched to an arc
 */
vector<patGpsPoint*>* patMapMatchingRoute::getGpsAssignedToStreet(patStreetSegment* aStreet) {
    list<pair<patStreetSegment, vector<patGpsPoint*> > >::iterator aIter = StreetRoute.end();
    while (aIter != StreetRoute.begin()) {
        --aIter;
        if (aStreet = &((*aIter).first)) {
            return &((*aIter).second);
        }
    }

    return NULL;
}

/*
 * Returns path representation of the MapMatchingRoute
 */
patPathJ patMapMatchingRoute::getPath() {
    patPathJ path;
    for (list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.begin();
            aIter != route.end();
            ++aIter) {

        path.addArcToBack((*aIter).first);
    }
    return path;
}

/*
 * Return subpath with start node and end note
 * Params: a pointer to the new path, the start node and the end node
 */
patULong patMapMatchingRoute::getPath(patPathJ* newPath, patNode* startNode, patNode* endNode) {
    patPathJ pathtmp = getPath();
    return pathtmp.getSubPath(newPath, startNode, endNode);
}

/*
 *Returns the list of arcs that on the the route.
 */
list<patArc*> patMapMatchingRoute::getAllArcs() {
    list<patArc*> listOfArcs;
    //DEBUG_MESSAGE(route.size());
    for (list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.begin();
            aIter != route.end();
            ++aIter) {
        //DEBUG_MESSAGE(*((*aIter).first));
        listOfArcs.push_back((*aIter).first);
    }
    return listOfArcs;

}

patNode* patMapMatchingRoute::getStartNode(patNetwork* theNetwork, patError*& err) {
    patArc* firstArc = getFirstArc();
    patNode* node = theNetwork->getNodeFromUserId(firstArc->upNodeId);
    if (node == NULL) {
        err = new patErrNullPointer("patNode");
        WARNING(err->describe());
        return NULL;
    } else {
        return node;
    }
}

patNode* patMapMatchingRoute::getEndNode(patNetwork* theNetwork, patError*& err) {
    patArc* lastArc = getLastArc();
    patNode* node = theNetwork->getNodeFromUserId(lastArc->downNodeId);
    if (node == NULL) {
        err = new patErrNullPointer("patNode");
        WARNING(err->describe());
        return NULL;
    } else {
        return node;
    }
}

patArc* patMapMatchingRoute::getFirstArc() {
    return route.front().first;
}

patGpsPoint* patMapMatchingRoute::getFirstGps() {
    return route.front().second.front();
}

patGpsPoint* patMapMatchingRoute::getLastGps() {
    return lastGpsPoint;
}

patReal patMapMatchingRoute::getArcScore(patArc* arc) {
    if (totalArcScores.find(arc) == totalArcScores.end()) {
        return patMaxReal;
    } else {
        return totalArcScores[arc];
    }
}

/*
 * Search an arc from the begining of the route
 * Returns patTRUE if the search is successful. patFALSE otherwise.
 */
patBoolean patMapMatchingRoute::searchFromBegin(patArc* aArc) {
    for (list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.begin();
            aIter != route.end();
            ++aIter) {
        if ((*aIter).first == aArc) {
            return patTRUE;
        }
    }
    return patFALSE;
}

patBoolean patMapMatchingRoute::searchFromEnd(patArc* aArc) {
    list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.end();
    while (aIter != route.begin()) {
        aIter--;
        if ((*aIter).first == aArc) {
            return patTRUE;
        }

    }
    return patFALSE;
}

patBoolean patMapMatchingRoute::searchFromBegin(patNode* aNode) {
    if (route.front().first->upNodeId == aNode->userId) {
        return patTRUE;
    }
    for (list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.begin();
            aIter != route.end();
            ++aIter) {
        if ((*aIter).first->downNodeId == aNode->userId) {
            return patTRUE;
        }
    }
    return patFALSE;
}

patBoolean patMapMatchingRoute::searchFromEnd(patNode* aNode) {
    list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.end();
    if (route.back().first->downNodeId == aNode->userId) {
        return patTRUE;
    }
    while (aIter != route.begin()) {
        aIter--;
        if ((*aIter).first->upNodeId == aNode->userId) {
            return patTRUE;
        }

    }
    return patFALSE;
}

/**
 * Search aNode in the route with a stopping condition stopNode
 *
 * @param aNode
 * @param stopNode
 * @return
 */
patBoolean patMapMatchingRoute::searchFromEnd(patNode* aNode, patNode* stopNode) {
    //DEBUG_MESSAGE(*aNode);
    //DEBUG_MESSAGE(*stopNode);
    list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.end();
    if (route.back().first->downNodeId == aNode->userId) {
        return patTRUE;
    }
    //	DEBUG_MESSAGE(route.size()<<"-"<<route.back().first->downNodeId);
    while (aIter != route.begin()) {
        aIter--;
        //DEBUG_MESSAGE((*aIter).first->upNodeId);
        if ((*aIter).first->upNodeId == aNode->userId) {
            return patTRUE;
        } else if ((*aIter).first->upNodeId == stopNode->userId) {
            break;
        }
    }

    return patFALSE;
}


/**
 * Get the jth arc in the route.
 * @param j
 * @return
 */
patArc* patMapMatchingRoute::getArc(patULong j) {
    if (j >= route.size()) {
        return NULL;
    }
    list<pair<patArc*, vector<patGpsPoint*> > >::iterator aIter = route.begin();

    patULong k = 0;
    while (k < j) {
        aIter++;
        k++;
    }

    return (*aIter).first;
}

patULong patMapMatchingRoute::getRouteStreetSize() {
    return StreetRoute.size();
}