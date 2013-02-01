//----------------------------------------------------------------
// File: patGpsPoint.cc
// Author: 
// Creation: 
//----------------------------------------------------------------

#include "patGpsPoint.h"
#include "patArc.h"
#include "patNetwork.h"
#include "patConst.h"
#include "patGeoCoordinates.h" 
#include "patShortestPathAlgoRange.h"
#include "patShortestPathTreeRange.h"
#include <math.h>
#include "patDisplay.h"
#include "patArcDDR.h"
#include "patString.h"
#include "patTripGraph.h"
#include "patArcTransition.h"
#include "patNBParameters.h"
#include "patPathDevelop.h"
#include "patPathJ.h"
const double threshold = 0.90;

patGpsPoint::patGpsPoint(
        unsigned long theUserId,
        unsigned long theTimeStamp,
        double lat,
        double lon,
        double theSpeed,
        double thehorizonAccuracy,
        double theHeading,
        double theVerticalAccuracy,
        double theSpeedAccuracy,
        double theHeadingAccuracy,
        map<patString, double>* theAlgoParams
        ) :
userId(theUserId),
timeStamp(theTimeStamp),
geoCoord(lat, lon),
speed(theSpeed),
heading(theHeading),
verticalAccuracy(theVerticalAccuracy),
        coordsHorizontalAccuracy(thehorizonAccuracy),
speedAccuracy(theSpeedAccuracy),
headingAccuracy(theHeadingAccuracy),
ddrDecreaseFactor(1.0) {
    if(theAlgoParams==NULL){
        networkAccuracy=patNBParameters::the()->networkAccuracy;
        minPointDDR = patNBParameters::the()->maxDistanceGPSLoc;
        minNormalSpeed=patNBParameters::the()->minNormalSpeed;
        maxNormalSpeedHeading=patNBParameters::the()->maxNormalSpeedHeading;
    }
    else{
        networkAccuracy=(*theAlgoParams)["networkAccuracy"];
        minPointDDR=(*theAlgoParams)["minPointDDR"];
        minNormalSpeed=(*theAlgoParams)["minNormalSpeed"];
        maxNormalSpeedHeading=(*theAlgoParams)["maxNormalSpeedHeading"];
    }
    horizonAccuracy = sqrt(pow(coordsHorizontalAccuracy, 2) + pow(networkAccuracy, 2));
    distanceThreshold_Loc = horizonAccuracy * sqrt(-2 * log(minPointDDR));
    //DEBUG_MESSAGE("distanceThreshold:"<<patNBParameters::the()->maxDistanceGPSLoc<<","<<distanceThreshold_Loc);
}
void patGpsPoint::setAlgoParams(map<patString, double>* theAlgoParams){
    if(theAlgoParams==NULL){
        networkAccuracy=patNBParameters::the()->networkAccuracy;
        minPointDDR = patNBParameters::the()->maxDistanceGPSLoc;
        minNormalSpeed=patNBParameters::the()->minNormalSpeed;
        maxNormalSpeedHeading=patNBParameters::the()->maxNormalSpeedHeading;
    }
    else{
        networkAccuracy=(*theAlgoParams)["networkAccuracy"];
        minPointDDR=(*theAlgoParams)["minPointDDR"];
        minNormalSpeed=(*theAlgoParams)["minNormalSpeed"];
        maxNormalSpeedHeading=(*theAlgoParams)["maxNormalSpeedHeading"];
    }
    horizonAccuracy = sqrt(pow(coordsHorizontalAccuracy, 2) + pow(networkAccuracy, 2));
    distanceThreshold_Loc = horizonAccuracy * sqrt(-2 * log(minPointDDR));
}
void patGpsPoint::setAlgoParams(patString paramName, double paramValue){
    if (paramName=="networkAccuracy"){
        networkAccuracy=paramValue;
    }
    else if (paramName=="minPointDDR"){
        minPointDDR=paramValue;
    }
    else if (paramName=="minNormalSpeed"){
        minNormalSpeed=paramValue;
    }
    else if (paramName=="maxNormalSpeedHeading"){
        maxNormalSpeedHeading=paramValue;
    }

    horizonAccuracy = sqrt(pow(coordsHorizontalAccuracy, 2) + pow(networkAccuracy, 2));
    distanceThreshold_Loc = horizonAccuracy * sqrt(-2 * log(minPointDDR));

}
bool operator<(const patGpsPoint& g1, const patGpsPoint& g2) {
    if (g1.timeStamp < g2.timeStamp) {
        return true;
    }
    if (g1.timeStamp > g2.timeStamp) {
        return false;
    }
    if (g1.geoCoord.latitudeInRadians < g2.geoCoord.latitudeInRadians) {
        return true;
    }
    if (g1.geoCoord.latitudeInRadians > g2.geoCoord.latitudeInRadians) {
        return false;
    }
    if (g1.geoCoord.longitudeInRadians < g2.geoCoord.longitudeInRadians) {
        return true;
    }
    if (g1.geoCoord.longitudeInRadians > g2.geoCoord.longitudeInRadians) {
        return false;
    }

    return false;

}

map<char*, double> patGpsPoint::distanceTo(patGeoCoordinates* upGeoCoord, patGeoCoordinates* downGeoCoord) {

    map<char*, double> distance;


    distance["up"] = geoCoord.distanceTo(*upGeoCoord);
    distance["down"] = geoCoord.distanceTo(*downGeoCoord);
    distance["length"] = upGeoCoord->distanceTo(*downGeoCoord);


    double cosUpNode = (distance["up"] * distance["up"] + distance["length"] * distance["length"] - distance["down"] * distance["down"]) / (2 * distance["up"] * distance["length"]);
    double cosDownNode = (distance["down"] * distance["down"] + distance["length"] * distance["length"] - distance["up"] * distance["up"]) / (2 * distance["down"] * distance["length"]);
    double p = (distance["up"] + distance["down"] + distance["length"]) / 2;

    double s = sqrt(p * (p - distance["up"])*(p - distance["down"])*(p - distance["length"]));
    distance["ver"] = 2 * s / distance["length"];
    /*
            if(isnan(distance["ver"])){
                            DEBUG_MESSAGE("distance: s:"<<s<<"p:"<<p<<"length:"<<distance["length"]<<"up:"<<distance["up"]<<"length:"<<distance["length"]);
            }*/

    if (distance["up"] + distance["down"] <= distance["length"]) {
        distance["position"] = 0;
        distance["ver"] = 0.0;
        distance["link"] = 0.0;
    } else if (distance["up"] + distance["length"] <= distance["down"]) {
        distance["position"] = -1;
        distance["ver"] = 0.0;
        distance["link"] = distance["up"];
    } else if (distance["down"] + distance["length"] <= distance["up"]) {
        distance["position"] = 1;
        distance["ver"] = 0.0;

        distance["link"] = distance["down"];
    }
    else if (cosUpNode < 0) {
        distance["link"] = distance["up"];
        distance["position"] = -1;
    } else if (cosDownNode < 0) {
        distance["link"] = distance["down"];
        distance["position"] = 1;
    } else {
        distance["link"] = distance["ver"];
        distance["position"] = 0;
    }



    return distance;
}

map<char*, double> patGpsPoint::distanceTo(patNetwork* theNetwork, patArc* theArc) {

	patNode* up_node = theArc->getUpNode();
	patNode* down_node = theArc->getDownNode();
	if(up_node==NULL || down_node==NULL){
		up_node = theNetwork->getNodeFromUserId(theArc->m_up_node_id);
		down_node = theNetwork->getNodeFromUserId(theArc->m_down_node_id);
	}
    patGeoCoordinates* upGeoCoord = &(up_node->geoCoord);
    patGeoCoordinates* downGeoCoord = &(down_node->geoCoord);
    return distanceTo(upGeoCoord, downGeoCoord);
}

map<char*, double> patGpsPoint::distanceTo( patArc* theArc) {

	patNode* up_node = theArc->getUpNode();
	patNode* down_node = theArc->getDownNode();
	if(up_node==NULL || down_node==NULL){
		WARNING("node doesn't exist");
		return				map<char*, double>();
	}
    patGeoCoordinates* upGeoCoord = &(up_node->geoCoord);
    patGeoCoordinates* downGeoCoord = &(down_node->geoCoord);
    return distanceTo(upGeoCoord, downGeoCoord);
}

double patGpsPoint::distanceTo(const patNode* theNode) {
    return geoCoord.distanceTo(theNode->geoCoord);
}

double patGpsPoint::calHeading(const patGpsPoint* nextGpsPoint) {
    return calHeading(&geoCoord, &(nextGpsPoint->getGeoCoord()));
}

double patGpsPoint::calHeading(patGpsPoint* prevGpsPoint, patGpsPoint* nextGpsPoint) {
    double incoming = prevGpsPoint->calHeading(this);
    double outGoing = this->calHeading(nextGpsPoint);
    //	DEBUG_MESSAGE("incoming:"<<incoming<<",outgoing:"<<outGoing);
    if (fabs(incoming - outGoing) <= 180.0) {

        double s1 = prevGpsPoint->getSpeed();
        double s2 = speed;

        double ratio = s1 / (s1 + s2);

        return ratio * incoming + (1 - ratio) * outGoing;
    } else {
        double h1 = incoming;
        double h2 = outGoing;
        double s1 = prevGpsPoint->getSpeed();
        double s2 = speed;
        if (incoming > 180.0) {
            h2 = incoming;
            h1 = outGoing;

            s1 = speed;
            s2 = prevGpsPoint->getSpeed();
        }
        double diff = 360.0 - (h2 - h1);
        double ratio = s1 / (s1 + s2);
        double rtn = h1 - (1.0 - ratio) * diff;
        if (rtn < 0.0) {
            rtn = h2 + ratio * diff;
        }
        return rtn;
    }

}

double patGpsPoint::calHeading(const patGeoCoordinates* startCoord, const patGeoCoordinates* nextCoord) {

    double lng1 = startCoord->longitudeInRadians;
    double lat1 = startCoord->latitudeInRadians;
    double lng2 = nextCoord->longitudeInRadians;
    double lat2 = nextCoord->latitudeInRadians;

    double numerator = sin(lat1) * sin(lng2 - lng1);
    double denumerator = sin(lat2) * cos(lat1) - cos(lat2) * sin(lat1) * cos(lng2 - lng1);

    double theArcHeading = atan(numerator / denumerator)*180 / pi;

    if (denumerator > 0) {
        theArcHeading += 360;
    } else {
        theArcHeading += 180;
    }
    if (theArcHeading < 0) {
        theArcHeading += 360;
    }
    if (theArcHeading >= 360) {
        theArcHeading -= 360;
    }
    return theArcHeading;
}

map<patArc*, double>* patGpsPoint::getLinkDDR() {

    return &linkDDR;
}

map<patNode*, double>* patGpsPoint::getNodeDDR() {
    return &nodeDDR;
}

set<unsigned long> patGpsPoint::getStartNodes(patNetwork* theNetwork) {
    set<unsigned long> startNodes;
    for (map<patArc*, double> ::const_iterator iter = linkDDR.begin(); iter != linkDDR.end(); ++iter) {

        startNodes.insert(theNetwork->getInternalNodeIdFromUserId(iter->first->m_up_node_id));
    }
    return startNodes;
}

map<double, set<patArc*> > patGpsPoint::sortDDRByValue() {
    map<double, set<patArc*> > rtnSet;
    for (map<patArc*, double> ::iterator iter = linkDDR.begin();
            iter != linkDDR.end();
            ++iter) {
        double ddrValue = iter->second;
        if (rtnSet.find(ddrValue) == rtnSet.end()) {
            rtnSet[ddrValue] = set<patArc*>();
        }
        rtnSet[ddrValue].insert(iter->first);
    }
    return rtnSet;

}

set<unsigned long> patGpsPoint::getEndNodes(patNetwork* theNetwork) {
    set<unsigned long> endNodes;
    for (map<patArc*, double> ::const_iterator iter = linkDDR.begin(); iter != linkDDR.end(); ++iter) {
        unsigned long aNode = theNetwork->getInternalNodeIdFromUserId(iter->first->m_down_node_id);
        if (aNode != NULL) {

            endNodes.insert(aNode);
        }
    }
    return endNodes;
}

double patGpsPoint::measureDDR(map<char*, double> theDistance, link_ddr_range ldr) {
    struct arc_ddr_params param;
    param.d = theDistance["ver"];
    param.l = theDistance["length"];
    param.e_d = calPerpendicularFootOnArc(theDistance);
    param.std_x = horizonAccuracy;
    param.ldr = ldr;
    //DEBUG_MESSAGE("d"<<param.d<<",l"<<param.l<<",e_d"<<param.e_d<<",deltax"<<param.std_x);
    patArcDDR theArcDDR(horizonAccuracy);
    return theArcDDR.errCDF(param);
}

/*
void patGpsPoint::genNodeDDR(patNetwork* theNetwork) {

    for (map<patULong, patNode>::iterator iter = theNetwork->theNodes.begin(); iter != theNetwork->theNodes.begin(); ++iter) {
        patReal mesureNodeDDR = measureDDR(distanceTo(&(iter->second)));
        if (mesureNodeDDR >= patNBParameters::the()->minNodeDDR) {
            nodeDDR.insert(nodeDDR.begin(), pair<patNode*, patReal > (&(iter->second), mesureNodeDDR));
        }
    }
}
*/
/*
void patGpsPoint::genLinkDDR( patGpsPoint* prevGpsPoint, patNetwork* theNetwork) {
        patArc* nextArc;
        map<char*, patReal> distanceLink;

        for (map<patArc*,patReal>::iterator iter = prevGpsPoint->linkDDR.begin();iter != prevGpsPoint->linkDDR.end();++iter){
                struct link_ddr_range
                patReal mesureDDRLink = mesureDDR(distanceTo(iter->first,theNetwork));
                if(mesureDDRLink>=0.1){
                        patArc* aArc= iter->first;
                        linkDDR.insert(linkDDR.begin(),pair<patArc*,patReal>( aArc,mesureDDRLink));
                }

                patULong downNodeId = iter->first->downNodeId;
                set<patULong> successorNodes = theNetwork->getNodeFromUserId(downNodeId)->userSuccessors;
                for (set<patULong>::const_iterator iter2 = successorNodes.begin(); iter2 != successorNodes.end();++iter2){
                        patArc* iterArc = theNetwork->getArcFromNodesUserId(downNodeId,*iter2);

			
                        map<patArc*,patReal>::iterator found = linkDDR.find(iterArc) ;
                        if (found != linkDDR.end()){
                                patReal heading1 = heading;
                                patReal heading2 = calHeading(&(prevGpsPoint->geoCoord),&geoCoord);
                                patReal headingArc = calHeading(&(theNetwork->getNodeFromUserId(downNodeId)->geoCoord),&(theNetwork->getNodeFromUserId(*iter2)->geoCoord));
                                if (fabs(headingArc-heading1)<30){

                                        distanceLink = distanceTo(iter->first,theNetwork);
                                        if (distanceLink["link"] <= distanceThreshold){
                                                patReal mesureDDRLink = mesureDDR(distanceLink);
                                                if (mesureDDRLink>=0.1){
                                                        linkDDR.insert(linkDDR.begin(),pair<patArc*,patReal>(iterArc,mesureDDRLink));
                                                }
                                        }
                                }
                        }
                }
		
        }

	
}
 */
void patGpsPoint::genInitDDR(patGpsPoint* nextGpsPoint, patNetwork* theNetwork) {
    patArc* nextArc;
    map<char*, double> distanceLink;
    //DEBUG_MESSAGE("next point. "<< nextGpsPoint->getTimeStamp() ) ;
    double heading1 = heading; //heading1 is heading value in Gps datum

    //heading2 is heading value calculated using the next Gps point.
    //if the next point == current point, i.e. the gps point is either alon in the sequence or the end of the sequence,
    ////heading2 take the heading valude in Gps datum
    double heading2 = calHeading(&geoCoord, &(nextGpsPoint->geoCoord));


    for (map<unsigned long, patArc> ::iterator iter = theNetwork->theArcs.begin(); iter != theNetwork->theArcs.end(); iter++) {
        patNode* upNode = theNetwork->getNodeFromUserId(iter->second.m_up_node_id);
        patNode* downNode = theNetwork->getNodeFromUserId(iter->second.m_down_node_id);

        if (upNode == NULL || downNode == NULL) {
            continue;
        } else if (iter->second.name == "M1") {
            continue;
        }

        map<patArc*, double>::iterator found = linkDDR.find(&(iter->second));
        detLinkDDR(&(iter->second), theNetwork);


        //
    }
    //DEBUG_MESSAGE("linkDDR size:"<<linkDDR.size());
    for (map<patArc*, double>::iterator iter = linkDDR.begin(); iter != linkDDR.end(); ++iter) {
        list<unsigned long> nodeList;

        unsigned long upNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter->first->m_up_node_id);
        unsigned long downNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter->first->m_down_node_id);
        nodeList.push_back(upNodeInternalId);
        nodeList.push_back(downNodeInternalId);

        pair<unsigned long, list<unsigned long> > elementToInsert = pair<unsigned long, list<unsigned long> >(upNodeInternalId, nodeList);

        if (predecessor.find(downNodeInternalId) == predecessor.end()) {
            vector<pair<unsigned long, list<unsigned long> > > nodeLists;
            nodeLists.push_back(elementToInsert);
            predecessor[downNodeInternalId] = nodeLists;
        } else {
            predecessor[downNodeInternalId].push_back(elementToInsert);
        }
    }
    calDDRSum();
}

ostream & operator<<(ostream& str, const patGpsPoint& x) {
    str << "Time:" << x.timeStamp << ";Speed:" << x.speed << ";Speed Accuracy:" << x.speedAccuracy << ";Heading:" << x.heading << ";Horizontal horizonAccuracy:" << x.horizonAccuracy << endl;
}

double patGpsPoint::getSpeed() {
    return speed;
}

double patGpsPoint::getSpeedMS() {
    return speed / 3.6;
}

double patGpsPoint::getSpeedAccuracyMS() {
    return speedAccuracy / 3.6;
}

double patGpsPoint::getHeadingAccuracy() {
    return headingAccuracy;
}

unsigned long patGpsPoint::getTimeStamp() const {
    return timeStamp;
}

patGeoCoordinates patGpsPoint::getGeoCoord() const {
    return geoCoord;
}

double patGpsPoint::getMaxSpeed(patGpsPoint* prevGpsPoint) {
    double maxSpeed = getSpeedMS();
    double geoSpeed = geoCoord.distanceTo(prevGpsPoint->geoCoord) / (timeStamp - prevGpsPoint->getTimeStamp());
    //geoSpeed = (geoSpeed < (120.0/3.6))?geoSpeed:(120.0/3.6);
    double nextSpeed = prevGpsPoint->getSpeedMS();
    maxSpeed = (maxSpeed < nextSpeed) ? nextSpeed : maxSpeed;
    maxSpeed = (maxSpeed < geoSpeed) ? geoSpeed : maxSpeed;

    return maxSpeed;

}

double patGpsPoint::getDistanceCeilFromPrevDDR(patGpsPoint* prevGpsPoint) {
    double maxSpeed = getMaxSpeed(prevGpsPoint);
    maxSpeed = (maxSpeed > 3.0) ? maxSpeed : 15.0;
    return maxSpeed * (timeStamp - prevGpsPoint->getTimeStamp()) * patNBParameters::the()->maxDistanceRatioSP;
}
/*

 */

/*
void patGpsPoint::genSegmentDDR(patGpsPoint* prevGpsPoint,patNetwork* theNetwork, vector< list <  pair<patArc*,patULong> > >* adjList){
        patReal theDistanceCeil = getDistanceCeilFromPrevDDR(prevGpsPoint);
        set<patArc*> inherentDDR;
        //DEBUG_MESSAGE("previous point. "<< prevGpsPoint->getTimeStamp() ) ;
        for (map<patArc*,patReal>::iterator iter = prevGpsPoint->linkDDR.begin();iter != prevGpsPoint->linkDDR.end();++iter){
                if(detLinkDDR(iter->first,theNetwork)==patTRUE){
                        inherentDDR.insert(iter->first);
                }
        }

        for (map<patArc*,patReal>::iterator iter = linkDDR.begin();iter != linkDDR.end();++iter){
                list<patULong> nodeList;
                patULong internalDownNodeId =theNetwork->getInternalNodeIdFromUserId(iter->first->downNodeId);
		
                pair<patULong,list<patULong> >  elementToInsert = pair<patULong,list<patULong> >(internalDownNodeId,nodeList);
                if(predecessor.find(internalDownNodeId)==predecessor.end()){
                        vector<pair<patULong,list<patULong> > > nodeLists;
                        nodeLists.push_back(elementToInsert);
                        predecessor[internalDownNodeId]= nodeLists;
                }
                else{
                        predecessor[internalDownNodeId].push_back(elementToInsert);
                }
        }
	
        set<patULong> theEndNodes = prevGpsPoint->getEndNodes(theNetwork);

        patShortestPathAlgoRange theShortestPathAlgoRange(this,adjList,theNetwork,theNetwork->minimumLabelForShortestPath);

        theShortestPathAlgoRange.computeShortestPathTree(theEndNodes,theDistanceCeil);
        patShortestPathTreeRange theTree=theShortestPathAlgoRange.getTree();
        for( map<patArc*,patReal>::iterator iter=linkDDR.begin();iter!=linkDDR.end();++iter){
                if (inherentDDR.find(iter->first)!=inherentDDR.end()){
                        continue;
                }
                patULong upNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter->first->upNodeId);
                patULong downNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter->first->downNodeId);
		
                list<patULong> treeList = theTree.getShortestPathTo(upNodeInternalId,theNetwork,&linkDDR);
                if (treeList.empty()){
                        DEBUG_MESSAGE("No node");
                        continue;
                }
                else{
                        //DEBUG_MESSAGE(treeList.back());

                        treeList.push_back(downNodeInternalId);
		
                        patULong upNodeId = treeList.front();
                        treeList.pop_front();
                        pair<patULong,list<patULong> > elementToInsert = pair<patULong,list<patULong> >(upNodeId,treeList);

                        if(predecessor.find(downNodeInternalId)==predecessor.end()){
                                vector<pair<patULong,list<patULong> > > nodeLists;
                                nodeLists.push_back(elementToInsert);
                                predecessor[downNodeInternalId]= nodeLists;
                        }
                        else{
                                predecessor[downNodeInternalId].push_back(elementToInsert);
                        }
                        predecessor[downNodeInternalId].push_back(elementToInsert);
			
                }
        }
 
}
 */
bool patGpsPoint::detLinkDDR(patArc* theArc, unsigned long upNodeId, unsigned long downNodeId, patNetwork* theNetwork) {


    return detLinkDDR(theArc, theNetwork);
}

struct link_ddr_range patGpsPoint::detLinkDDR_Range(map<char*, double> distanceLink) {
    struct link_ddr_range ldr;
    ldr.lower_bound = 0.0;
    ldr.upper_bound = 1.0;
    if (distanceLink["link"] > distanceThreshold_Loc) {
        ldr.upper_bound = 0.0;
    } else {

        double e_d = calPerpendicularFootOnArc(distanceLink);
        if (e_d <= 0) {
            ldr.upper_bound = e_d + sqrt(-2.0 * log(minPointDDR) * pow(horizonAccuracy, 2) - pow(distanceLink["ver"], 2)) / distanceLink["length"];

        }
        if (e_d >= 1) {
            ldr.lower_bound = e_d - sqrt(-2.0 * log(minPointDDR) * pow(horizonAccuracy, 2) - pow(distanceLink["ver"], 2)) / distanceLink["length"];

        } else {
            ldr.upper_bound = e_d + sqrt(-2.0 * log(minPointDDR) * pow(horizonAccuracy, 2) - pow(distanceLink["ver"], 2)) / distanceLink["length"];
            ldr.lower_bound = e_d - sqrt(-2.0 * log(minPointDDR) * pow(horizonAccuracy, 2) - pow(distanceLink["ver"], 2)) / distanceLink["length"];

        }
        //	DEBUG_MESSAGE(1<<":"<<ldr.lower_bound<<"-"<<ldr.upper_bound);
        ldr.upper_bound = (ldr.upper_bound > 1.0) ? 1.0 : ldr.upper_bound;
        ldr.lower_bound = (ldr.lower_bound < 0.0) ? 0.0 : ldr.lower_bound;
        //	DEBUG_MESSAGE(2<<":"<<ldr.lower_bound<<"-"<<ldr.upper_bound);
    }

    return ldr;


}
bool patGpsPoint::detLinkDDR(patArc* theArc, patNetwork* theNetwork){

    patNode* upNode = theNetwork->getNodeFromUserId(theArc->m_up_node_id);
    patNode* downNode = theNetwork->getNodeFromUserId(theArc->m_down_node_id);
    return detLinkDDR(theArc, upNode, downNode);
}

bool patGpsPoint::detLinkDDR(patArc* theArc, patNode* upNode, patNode* downNode) {
	patGeoCoordinates* upNodeGeoCoord = &(upNode->geoCoord);
	patGeoCoordinates* downNodeGeoCoord = &(downNode->geoCoord);
    map<char*, double> distanceLink;
    double headingAccuracyBound = patNBParameters::the()->maxHeadingGPSArc;
    double arcHeading = theArc->m_attributes.heading;
    if (arcHeading < 0.0) {
        DEBUG_MESSAGE("bad arc" << *theArc);
        return false;
    }
    // if(theArc->upNodeId == 253265151){
    // DEBUG_MESSAGE(arcHeading<<*theArc);
    // }
    if (heading < 0.0 || type != "normal_speed"
            || fabs(arcHeading - heading) < headingAccuracyBound
            || (heading < headingAccuracyBound && (360 - arcHeading + heading) < headingAccuracyBound && (360 - arcHeading + heading) > 0.0)
            || (heading > 360 - headingAccuracyBound && (360 - heading + arcHeading) < headingAccuracyBound && (360 - heading + arcHeading) > 0.0)) {
        distanceLink = distanceTo(upNodeGeoCoord, downNodeGeoCoord);
            struct link_ddr_range ldr = detLinkDDR_Range(distanceLink);
            //			DEBUG_MESSAGE(ldr.lower_bound<<"-"<<ldr.upper_bound);
            if (ldr.lower_bound == 0.0 && ldr.upper_bound == 0.0) {
                return false;
            } else if (ldr.upper_bound - ldr.lower_bound < 0.00001) {

                return false;
            } else {
                double measureDDRLink = measureDDR(distanceLink, ldr);

                linkDDR.insert(linkDDR.end(), pair<patArc*, double > (theArc, measureDDRLink));
                linkDDR_Range.insert(linkDDR_Range.end(), pair<patArc*, struct link_ddr_range > (theArc, ldr));
                return true;
            }

        
    }
    return false;
}

double patGpsPoint::calLinkDDR(patArc* theArc, patNetwork* theNetwork) {
	patNode* up_node = theArc->getUpNode();
	patNode* down_node = theArc->getDownNode();
	if(up_node==NULL || down_node==NULL){
		up_node = theNetwork->getNodeFromUserId(theArc->m_up_node_id);
		down_node = theNetwork->getNodeFromUserId(theArc->m_down_node_id);
	}
    patGeoCoordinates* upNodeGeoCoord = &(up_node->geoCoord);
    patGeoCoordinates* downNodeGeoCoord = &(down_node->geoCoord);
    map<char*, double> distanceLink;

    distanceLink = distanceTo(upNodeGeoCoord, downNodeGeoCoord);
    struct link_ddr_range ldr;
    //if(patNBParameters::the()->algoInSelection=="sim2"){
    ldr = detLinkDDR_Range(distanceLink);
    linkDDR_Range.insert(linkDDR_Range.end(), pair<patArc*, struct link_ddr_range > (theArc, ldr));
    //	}
    //DEBUG_MESSAGE("when");
    if (ldr.lower_bound == 0.0 && ldr.upper_bound == 0.0) {
        return 0.0;
    } else if (ldr.upper_bound - ldr.lower_bound < 0.00001) {

        return 0.0;
    }
    double measureDDRLink = measureDDR(distanceLink, ldr);
    return measureDDRLink;
}

/*
patGpspoint::searchTree(patULong internalNodeId,vector<patULong>* pathTemp, patBoolean* signDDR){
	
        pathTemp->push_back(internalNodeId);
        if(theTree.successor[internalNodeId]==null){
 *signDDR = patFALSE;
        }
        else{
        for (list<pair<patULong,patArc*>>::iterator iter=theTree.successor[internalNodeId].begin();iter!=theTree.successor[iternalNodeId].end();++iter){
                searchTree(iter->first,vector<patULong> pathTemp);
                patGeoCoordinates* upNodeGeoCoord = &(theNetwork->internalNodes[internalNodeId]->geoCoord);
                patGeoCoordinates* downNodeGeoCoord = &(theNetwork->internalNodes[iter->first]->geoCoord);
		
                patReal arcHeading = calHeading(upNodeGeoCoord,downNodeGeoCoord);
                if (fabs(headingArc-heading)<30.0){
                        distanceLink = distanceTo(upNodeGeoCoord,downNodeGeoCoord);
                        if (distanceLink["link"] <= distanceThreshold){
                                patReal mesureDDRLink = mesureDDR(distanceLink);
                                if (mesureDDRLink>=0.1){
                                        linkDDR.insert(linkDDR.begin(),pair<patArc*,patReal>(*(iter->seconde),mesureDDRLink));
                                        if(*signDDR==patFalse){
                                                pathTemp->push_back(iter->first);
                                                theSegment.push_back(*pathTemp);
                                                pathTemp->pop_back(iter->first);
                                        }
 *signDDR = patTRUE;
                                }
                        }
			
                }
		
        }}
	
        pathTemp->pop_back();
}

patGpsPoint::genLinkDDRs(vector<patArc*> theLinkCandidates){
	
        for(patULong i=0;i<theTree->incomingArc.size();++i){
                patReal arcHeading = calHeading(&(theNetwork->internalNodes[theTree->predecessor[i]]->geoCoord),&(theNetwork->internalNodes[i]->geoCoord));
                if (fabs(headingArc-heading)<30.0){
                        distanceLink = distanceTo(incomingArc[i],theNetwork);
                        if (distanceLink["link"] <= distanceThreshold){
                                patReal mesureDDRLink = mesureDDR(distanceLink);
                                if (mesureDDRLink>=0.01){
                                        linkDDR.insert(linkDDR.begin(),pair<patArc*,patReal>(incomingArc[i],mesureDDRLink));
                                        if(*sign==0){
                                                genPath();
                                        }
                                }
                        }
                }
        }
}
 */

double patGpsPoint::calDDRSum() {
    DDRSum = 0.0;
    for (map<patArc*, double>::iterator iter = linkDDR.begin();
            iter != linkDDR.end();
            ++iter) {

        DDRSum += iter->second;

    }
    return DDRSum;
}

double patGpsPoint::getDDRSum() {
    return DDRSum;
}

double patGpsPoint::getHorizonAccuracy() {
    return horizonAccuracy;
}

patGeoCoordinates* patGpsPoint::getGeoCoord() {
    return &geoCoord;
}

set<patArc*> patGpsPoint::detInherentDDR(patGpsPoint* prevGpsPoint, patNetwork* theNetwork) {
    //check the element of previous domain is also domain of the current gps point.
    set<patArc*> inherentDDR;
    //DEBUG_MESSAGE("previous point. "<< prevGpsPoint->getTimeStamp() ) ;
    map<patArc*, double>* prevLinkDDR = prevGpsPoint->getLinkDDR();

    for (map<patArc*, double>::iterator arcIter = prevLinkDDR->begin();
            arcIter != prevLinkDDR->end();
            ++arcIter) {
        if (detLinkDDR(arcIter->first, theNetwork) == true) {
            inherentDDR.insert(arcIter->first);
        }
    }

    return inherentDDR;
}

map<unsigned long, set<patArc*> > patGpsPoint::detAssocDownNodes(map<patArc*, double> ddrMap) {
    map<unsigned long, set<patArc*> > assocEndNodes;

    for (map<patArc*, double>::iterator arcIter = ddrMap.begin();
            arcIter != ddrMap.end();
            ++arcIter) {

        if (assocEndNodes.find(arcIter->first->m_down_node_id) == assocEndNodes.end()) {
            assocEndNodes[arcIter->first->m_down_node_id] = set<patArc*>();
        }
        assocEndNodes[arcIter->first->m_down_node_id].insert(arcIter->first);
    }

    return assocEndNodes;
}

void patGpsPoint::selectInitByOrigNode(set<unsigned long> origId) {
    map<patArc*, double> ddrTemp;
    for (map<patArc*, double>::iterator arcIter = linkDDR.begin();
            arcIter != linkDDR.end();
            ++arcIter) {

        if (origId.find(arcIter->first->m_up_node_id) != origId.end()) {
            ddrTemp.insert(*arcIter);
        }
    }

    linkDDR = ddrTemp;

}

void patGpsPoint::genInitDDRV2(patGpsPoint* nextGpsPoint, patNetwork* theNetwork, patTripGraph* theTripGraph) {
    patArc* nextArc;
    map<char*, double> distanceLink;
    //DEBUG_MESSAGE("next point. "<< nextGpsPoint->getTimeStamp() ) ;

    for (map<unsigned long, patArc> ::iterator iter = theNetwork->theArcs.begin(); iter != theNetwork->theArcs.end(); iter++) {
        patNode* upNode = theNetwork->getNodeFromUserId(iter->second.m_up_node_id);
        patNode* downNode = theNetwork->getNodeFromUserId(iter->second.m_down_node_id);

        if (upNode == NULL || downNode == NULL) {
            continue;
        } else if (iter->second.name == "M1" || iter->second.m_attributes.priority < 4 || iter->second.m_attributes.priority > 14) {
            continue;
        }
        //DEBUG_MESSAGE("1");
        detLinkDDR(&(iter->second), theNetwork);

    }
    //selectInitByDDR(10,100);
    /*
    //selectInitBySortDDR(2);
    set<patString> arcNames;
    patString aName("Route Cantonale");
    arcNames.insert(aName);
	
    patString bName("Chemin de Veilloud");
    arcNames.insert(bName);
	
    selectInitByName(&arcNames);
     */

    //selectInitByOrigNode(origIds);
    if (linkDDR.empty()) {
        DEBUG_MESSAGE("no ddr");
        return;
    }
    DEBUG_MESSAGE("linkDDR size:" << linkDDR.size());
    selectInitBySortDDR(patNBParameters::the()->maxDomainSizeOrig);
    DEBUG_MESSAGE("linkDDR size:" << linkDDR.size());
    connectDDRArcs(theTripGraph);

    calDDRSum();
}

void patGpsPoint::selectInitByDDR(double lowerBound, double upperBound) {
    map<patArc*, double> ddrTemp;
    for (map<patArc*, double>::iterator arcIter = linkDDR.begin();
            arcIter != linkDDR.end();
            ++arcIter) {

        if ((arcIter->second >= lowerBound && arcIter->second <= upperBound) || (arcIter->second >= 39 && arcIter->second <= 40)) {
            ddrTemp.insert(*arcIter);
        }
    }

    linkDDR = ddrTemp;
}

void patGpsPoint::selectInitBySortDDR(unsigned long k) {
    map<double, set<patArc*> > ddrTemp;
    for (map<patArc*, double>::iterator arcIter = linkDDR.begin();
            arcIter != linkDDR.end();
            ++arcIter) {

        if (ddrTemp.find(arcIter->second) == ddrTemp.end()) {
            ddrTemp[arcIter->second] = set<patArc*>();
        }
        ddrTemp[arcIter->second].insert(arcIter->first);

    }

    linkDDR = map<patArc*, double > ();
    map<double, set<patArc*> >::iterator aIter = ddrTemp.end();
    for (unsigned long i = 0; i < k; ++i) {
        aIter--;
        for (set<patArc*>::iterator bIter = aIter->second.begin();
                bIter != aIter->second.end();
                ++bIter) {
            linkDDR[const_cast<patArc*> (*bIter)] = aIter->first;

        }

        if (aIter == ddrTemp.begin()) {
            break;
        }
    }
}

set<patArc*> patGpsPoint::selectDomainByDistance(patNetwork* theNetwork) {

    set<patArc*> dSet;
    if (linkDDR.empty()) {
        return dSet;
    }
    map<double, set<patArc*> > tmpDDRMap;
    for (map<patArc*, double>::iterator arcIter = linkDDR.begin();
            arcIter != linkDDR.end();
            ++arcIter) {
        double dl = distanceTo(theNetwork, arcIter->first)["link"];
        //		DEBUG_MESSAGE("distance to link"<<dl);
        if (tmpDDRMap.find(dl) == tmpDDRMap.end()) {
            tmpDDRMap.insert(pair<double, set<patArc*> >(dl, set<patArc*>()));
        }
        tmpDDRMap[dl].insert(arcIter->first);
    }
    map<double, set<patArc*> >::iterator tmpIter = tmpDDRMap.end();
    for (unsigned long k = 0; k < patNBParameters::the()->selectDDRByDistance; ++k) {
        if (tmpIter == tmpDDRMap.begin()) {
            break;
        } else {
            tmpIter--;
            dSet.insert(tmpIter->second.begin(), tmpIter->second.end());
        }
    }

    return dSet;
}

void patGpsPoint::selectDomainByNumber(unsigned long k) {
    if (linkDDR.empty() || k <= 0) {
        return;
    }

    map<double, set<patArc*> > ddrTemp = sortDDRByValue();
    linkDDR = map<patArc*, double > ();
    map<double, set<patArc*> >::iterator aIter = ddrTemp.end();
    for (unsigned long i = 0; i < k; ++i) {




        if (aIter != ddrTemp.begin()) {
            aIter--;
            for (set<patArc*>::iterator bIter = aIter->second.begin();
                    bIter != aIter->second.end();
                    ++bIter) {
                linkDDR[const_cast<patArc*> (*bIter)] = aIter->first;

            }
        }

    }
    calDDRSum();

}

set<patArc*> patGpsPoint::selectDomainByCdf(patNetwork* theNetwork) {
    if (linkDDR.empty() || linkDDR.size() < patNBParameters::the()->minDomainSize) {

        return getDomainSet();
    }

    set<patArc*> dSet = selectDomainByDistance(theNetwork);
    map<double, set<patArc*> > ddrTemp = sortDDRByValue();
    double total = calDDRSum();
    map<double, set<patArc*> >::iterator aIter = ddrTemp.end();
    double cdf = 0.0;
    double thre = total * patNBParameters::the()->minDomainDDRCdf;
    //DEBUG_MESSAGE("thre"<<thre<<"total:"<<total<<"ratio:"<<patNBParameters::the()->minDomainDDRCdf);
    while (cdf < thre && aIter != ddrTemp.begin()) {
        aIter--;
        cdf += aIter->first * aIter->second.size();
        dSet.insert(aIter->second.begin(), aIter->second.end());

    }

    DEBUG_MESSAGE(" select by cdf, domain size:" << dSet.size());
    return dSet;
}

void patGpsPoint::selectInitByName(set<patString>* arcName) {
    map<patArc*, double> ddrTemp;
    for (map<patArc*, double>::iterator arcIter = linkDDR.begin();
            arcIter != linkDDR.end();
            ++arcIter) {

        if (arcName->find(arcIter->first->name) != arcName->end()) {
            ddrTemp.insert(*arcIter);
        }
    }

    linkDDR = ddrTemp;

}

void patGpsPoint::selectInitById(set<unsigned long>* arcId) {
    map<patArc*, double> ddrTemp;
    for (map<patArc*, double>::iterator arcIter = linkDDR.begin();
            arcIter != linkDDR.end();
            ++arcIter) {

        if (arcId->find(arcIter->first->userId) != arcId->end()) {
            ddrTemp.insert(*arcIter);
        }
    }

    linkDDR = ddrTemp;

}
/*
map<patULong, set<patArc*> > patGpsPoint::buildAssocEndNodesArcs(set<patArc*>* arcSet){
        map<patULong, set<patArc*> > rtnMap;
        for(set<patArc*>::iterator arcIter = arcSet->begin();
                                        arcIter!=arcSet->end();
                                        ++arcIter
        ){
                if(rtnMap.find(arcIter->downNodeId)==rtnMap.end()){
                        rtnMap[arcIter->downNodeId]=set<patArc*>();
                }
                rtnMap[arcIter->downNodeId].insert(const_cast<patArc*>(*arcIter));
        }
        return rtnMap;
}
 */

/*
 *generate domain of gps point based on domain of the last one.
 *using algorithm that branch out shortest path tree from all end nodes from previous gps point.
 */
void patGpsPoint::genSegmentDDRV2(patGpsPoint* prevGpsPoint,
        patNetwork* theNetwork,
        vector< list < pair<patArc*, unsigned long> > >* adjList,
        patTripGraph* theTripGraph,
        set<patArcTransition>* arcTranSet,
        vector<patGpsPoint>* gpsSequence,
        set<patArc*>* interArcs,
        bool noStop) {
    //set<patULong, pair<patArc*, patArc*> > assoc
    //get the furthest travel distance from the previous domain
    double theDistanceCeil = getDistanceCeilFromPrevDDR(prevGpsPoint);

    set<patArc*> inherentDDR = detInherentDDR(prevGpsPoint, theNetwork);

    //DEBUG_MESSAGE("build associate end nodes and arcs");
    //* build end node list

    map<unsigned long, set<patArc*> > assocEndNodes = detAssocDownNodes(*(prevGpsPoint->getLinkDDR()));
    struct gps_params p;
    setGpsParams(&p, prevGpsPoint, gpsSequence);
    //DEBUG_MESSAGE("detecting ddrs");
    for (map<unsigned long, set<patArc*> >::iterator iter = assocEndNodes.begin();
            iter != assocEndNodes.end();
            ++iter) {
        //DEBUG_MESSAGE("branch out from node"<<iter->first);
        unsigned long endNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter->first);
        set<patArc*> assocArcs = iter->second;

        set<unsigned long> endNodesTemp;
        endNodesTemp.insert(endNodeInternalId);
        //DEBUG_MESSAGE("end node size"<<endNodesTemp.size());
        patShortestPathAlgoRange theShortestPathAlgoRange(this, adjList, theNetwork, theNetwork->minimumLabelForShortestPath);
        set<patArc*> ddrArcs;
        theShortestPathAlgoRange.computeShortestPathTree(endNodesTemp, theDistanceCeil, &ddrArcs);
        selectDomainByNumber(patNBParameters::the()->maxDomainSize);

        for (set<patArc*>::iterator raIter = ddrArcs.begin();
                raIter != ddrArcs.end();) {
            if (linkDDR.find(*raIter) == linkDDR.end()) {
                ddrArcs.erase(raIter++);
            } else {
                raIter++;
            }
        }
        DEBUG_MESSAGE("linking arcs" << endNodeInternalId << "," << ddrArcs.size());
        if (ddrArcs.empty() == true) {

            continue;
        }

        patShortestPathTreeRange theTree = theShortestPathAlgoRange.getTree();

        map<unsigned long, set<patArc*> > currAssocNodesArcs;
        for (set<patArc*>::iterator iter2 = ddrArcs.begin();
                iter2 != ddrArcs.end();
                ++iter2) {

            if (currAssocNodesArcs.find((*iter2)->m_up_node_id) == currAssocNodesArcs.end()) {
                currAssocNodesArcs[(*iter2)->m_up_node_id] = set<patArc*>();
            }
            currAssocNodesArcs[(*iter2)->m_up_node_id].insert(const_cast<patArc*> (*iter2));

        }

        for (map<unsigned long, set<patArc*> >::iterator iter2 = currAssocNodesArcs.begin();
                iter2 != currAssocNodesArcs.end();
                ++iter2) {


            unsigned long upNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter2->first);

            list<patArc*> treeList = theTree.getShortestPathToV2(upNodeInternalId);
            if (treeList.size() == 1 && treeList.back() == NULL) {//bad list
                continue;
            }
            if (treeList.empty()) {
                for (set<patArc*>::iterator iter3 = assocArcs.begin();
                        iter3 != assocArcs.end();
                        ++iter3) {

                    for (set<patArc*>::iterator iter4 = iter2->second.begin();
                            iter4 != iter2->second.end();
                            ++iter4) {
                        if (noStop) {
                            list<patArc*> pathSeg;
                            pathSeg.push_back(const_cast<patArc*> (*iter3));
                            pathSeg.push_back(const_cast<patArc*> (*iter4));

                            patArcTransition aTran(pathSeg, prevGpsPoint, this);
                            //DEBUG_MESSAGE("OK1");
                            if (arcTranSet->find(aTran) == arcTranSet->end()) {
                                //aTran.calProbability(theNetwork,p);
                                //DEBUG_MESSAGE("insert a tran"<<aTran);
                                arcTranSet->insert(aTran);
                            }
                        }
                        theTripGraph->setSuccessor(const_cast<patArc*> (*iter3), const_cast<patArc*> (*iter4));
                        interArcs->insert(const_cast<patArc*> (*iter3));
                        interArcs->insert(const_cast<patArc*> (*iter4));
                    }

                }

                continue;
            }

            bool validSeg = true;
            for (set<patArc*>::iterator iter3 = assocArcs.begin();
                    iter3 != assocArcs.end();
                    ++iter3) {
                if (detReverseArc(const_cast<patArc*> (*iter3), treeList.front()) == false) {

                    theTripGraph->setSuccessor(const_cast<patArc*> (*iter3), treeList.front());
                    interArcs->insert(const_cast<patArc*> (*iter3));
                    interArcs->insert(treeList.begin(), treeList.end());
                    if (noStop) {
                        list<patArc*> pathSeg = treeList;

                        pathSeg.push_front(const_cast<patArc*> (*iter3));
                        for (set<patArc*>::iterator iter4 = iter2->second.begin();
                                iter4 != iter2->second.end();
                                ++iter4) {
                            pathSeg.push_back(const_cast<patArc*> (*iter4));

                            patArcTransition aTran(pathSeg, prevGpsPoint, this);
                            //DEBUG_MESSAGE("OK2");
                            if (arcTranSet->find(aTran) == arcTranSet->end()) {
                                //aTran.calProbability(theNetwork,p);
                                //DEBUG_MESSAGE("insert a tran");
                                //DEBUG_MESSAGE("insert a tran"<<aTran);
                                arcTranSet->insert(aTran);
                            }
                            pathSeg.pop_back();
                        }
                    }
                } else {
                    validSeg = false;
                }
            }

            if (validSeg == false) {
                continue;
            }

            //connect the last arc of tree to successor of ddr arcs
            for (set<patArc*>::iterator iter3 = iter2->second.begin();
                    iter3 != iter2->second.end();
                    ++iter3) {
                theTripGraph->setSuccessor(treeList.back(), const_cast<patArc*> (*iter3));
                interArcs->insert(const_cast<patArc*> (*iter3));
            }




            patArc* prevArc = treeList.front();
            list<patArc*>::iterator iter3 = treeList.begin();
            iter3++;
            //DEBUG_MESSAGE("linking in the tree list ");

            for (;
                    iter3 != treeList.end();
                    iter3++) {
                theTripGraph->setSuccessor(prevArc, const_cast<patArc*> (*iter3));
                prevArc = const_cast<patArc*> (*iter3);
            }


        }


    }
    if (linkDDR.empty()) {
        return;
    } else {
        connectDDRArcs(theTripGraph);

        calDDRSum();
    }

}

void patGpsPoint::genDDRFromPaths(
        set<patPathJ>* pathSet,
        patNetwork* theNetwork
        ) {
    set<patArc*> excludedArcs;
    for(set<patPathJ>::iterator pathIter=pathSet->begin();
            pathIter!=pathSet->end();
            ++pathIter){
        list<patArc*>* arcList=const_cast<patPathJ*>(&*pathIter)->getArcList();
        for (list<patArc*>::iterator arcIter=arcList->begin();
                arcIter!=arcList->end();
                ++arcIter){
        	patArc* the_arc = const_cast<patArc*> (*arcIter);
        	patNode* up_node = the_arc->getUpNode();
        	patNode* down_node = the_arc->getDownNode();
        	bool isInDDR;
        	if (up_node==NULL || down_node==NULL){
        		isInDDR =detLinkDDR(const_cast<patArc*> (*arcIter), theNetwork);
        	}
        	else{
        		isInDDR = detLinkDDR(const_cast<patArc*> (*arcIter),up_node,down_node);
        	}
            if( isInDDR==false){
                excludedArcs.insert(*arcIter);
            }
        }
    }
    
    calDDRSum();

}

void patGpsPoint::genSegmentDDRV3(patGpsPoint* prevGpsPoint,
        patNetwork* theNetwork,
        vector< list < pair<patArc*, unsigned long> > >* adjList,
        patPathDevelop* thePathDevelop,
        vector<patGpsPoint>* gpsSequence,
        vector<patGpsPoint*>* lowSpeedGpsPoints
        ) {
    set<patArc*> interArcs;
    double theDistanceCeil = getDistanceCeilFromPrevDDR(prevGpsPoint);
    map<patArc*, double>* prevLinkDDR = prevGpsPoint->getLinkDDR();
    for (map<patArc*, double>::iterator arcIter = prevLinkDDR->begin();
            arcIter != prevLinkDDR->end();
            ++arcIter) {
        interArcs.insert(arcIter->first);
    }
    set<patArc*> inherentDDR = detInherentDDR(prevGpsPoint, theNetwork);

    struct gps_params p;
    setGpsParams(&p, prevGpsPoint, gpsSequence);
    //DEBUG_MESSAGE("detecting ddrs");
    set<unsigned long> prevEndNodes = thePathDevelop->genEndNodes();
    DEBUG_MESSAGE("from end nodes of previous domain:" << prevEndNodes.size() << "," << theDistanceCeil);
    map<unsigned long, set<patPathJ > > segSet;
    for (set<unsigned long>::iterator iter = prevEndNodes.begin();
            iter != prevEndNodes.end();
            ++iter) {

        //DEBUG_MESSAGE("branch out from node"<<iter->first);
        unsigned long endNodeInternalId = theNetwork->getInternalNodeIdFromUserId(*iter);
        set<unsigned long> endNodesTemp;
        endNodesTemp.insert(endNodeInternalId);
        //DEBUG_MESSAGE("end node size"<<endNodesTemp.size());
        patShortestPathAlgoRange theShortestPathAlgoRange(this, adjList, theNetwork, theNetwork->minimumLabelForShortestPath);
        set<patArc*> ddrArcs;
        theShortestPathAlgoRange.computeShortestPathTree(endNodesTemp, theDistanceCeil, &ddrArcs);


        DEBUG_MESSAGE("linking arcs" << *iter << "," << ddrArcs.size());
        if (ddrArcs.empty() == true) {

            continue;
        }

        patShortestPathTreeRange theTree = theShortestPathAlgoRange.getTree();

        map<unsigned long, set<patArc*> > currAssocNodesArcs;
        for (set<patArc*>::iterator iter2 = ddrArcs.begin();
                iter2 != ddrArcs.end();
                ++iter2) {

            if (currAssocNodesArcs.find((*iter2)->m_up_node_id) == currAssocNodesArcs.end()) {
                currAssocNodesArcs[(*iter2)->m_up_node_id] = set<patArc*>();
            }
            currAssocNodesArcs[(*iter2)->m_up_node_id].insert(const_cast<patArc*> (*iter2));

        }
        for (map<unsigned long, set<patArc*> >::iterator iter2 = currAssocNodesArcs.begin();
                iter2 != currAssocNodesArcs.end();
                ++iter2) {


            unsigned long upNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter2->first);

            if (upNodeInternalId == patBadId) {
                DEBUG_MESSAGE("bad node" << iter2->first);
                continue;
            }
            list<patArc*> treeList = theTree.getShortestPathToV2(upNodeInternalId);
            if (treeList.size() == 1 && treeList.back() == NULL) {//bad list
                continue;
            }

            set<patPathJ> currSet;
            for (set<patArc*>::iterator iter4 = iter2->second.begin();
                    iter4 != iter2->second.end();
                    ++iter4) {
                patPathJ bPath(treeList);

                bPath.addArcToBack(*iter4);

                if (!treeList.empty()) {
                    interArcs.insert(treeList.begin(), treeList.end());
                }
                interArcs.insert(*iter4);
                currSet.insert(bPath);

            }
            if (!currSet.empty()) {
                if (segSet.find(*iter) == segSet.end()) {
                    segSet.insert(pair<unsigned long, set<patPathJ> >(*iter, currSet));

                } else {
                    segSet[*iter].insert(currSet.begin(), currSet.end());
                }
            }
        }


    }
    unsigned long st = 0;


    if (linkDDR.empty()) {
        return;
    }
    if (heading < 0.0) {

        selectDomainByDistance(theNetwork);
    }

    if (!lowSpeedGpsPoints->empty()) {
        DEBUG_MESSAGE("low speed gps points in between" << lowSpeedGpsPoints->size());
        thePathDevelop->lowSpeedPoints(lowSpeedGpsPoints, &interArcs, theNetwork);
    }
    thePathDevelop->appendSeg(this, &segSet, &inherentDDR);
    calDDRSum();


}

void patGpsPoint::genSegmentDDRV2_2(patGpsPoint* prevGpsPoint,
        patNetwork* theNetwork,
        vector< list < pair<patArc*, unsigned long> > >* adjList,
        patTripGraph* theTripGraph,
        set<patArc*>* interArcs) {
    //get the furthest travel distance from the previous domain
    double theDistanceCeil = getDistanceCeilFromPrevDDR(prevGpsPoint);

    set<patArc*> inherentDDR = detInherentDDR(prevGpsPoint, theNetwork);

    //DEBUG_MESSAGE("build associate end nodes and arcs");
    //* build end node list

    map<unsigned long, set<patArc*> > assocEndNodes = detAssocDownNodes(*(prevGpsPoint->getLinkDDR()));

    set<unsigned long> endNodesTemp;

    for (map<unsigned long, set<patArc*> >::iterator iter = assocEndNodes.begin();
            iter != assocEndNodes.end();
            ++iter) {

        unsigned long endNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter->first);
        endNodesTemp.insert(endNodeInternalId);

    }

    patShortestPathAlgoRange theShortestPathAlgoRange(this, adjList, theNetwork, theNetwork->minimumLabelForShortestPath);
    set<patArc*> ddrArcs;
    theShortestPathAlgoRange.computeShortestPathTree(endNodesTemp, theDistanceCeil, &ddrArcs);
    patShortestPathTreeRange theTree = theShortestPathAlgoRange.getTree();
    //DEBUG_MESSAGE("ddr arc number:"<<ddrArcs.size());
    map<unsigned long, set<patArc*> > currAssocNodesArcs;
    for (set<patArc*>::iterator iter2 = ddrArcs.begin();
            iter2 != ddrArcs.end();
            ++iter2) {

        if (currAssocNodesArcs.find((*iter2)->m_up_node_id) == currAssocNodesArcs.end()) {
            currAssocNodesArcs[(*iter2)->m_up_node_id] = set<patArc*>();
        }
        currAssocNodesArcs[(*iter2)->m_up_node_id].insert(const_cast<patArc*> (*iter2));

    }

    for (map<unsigned long, set<patArc*> >::iterator iter2 = currAssocNodesArcs.begin();
            iter2 != currAssocNodesArcs.end();
            ++iter2) {
        //DEBUG_MESSAGE("node"<<iter2->first);
        unsigned long upNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter2->first);
        list<patArc*> treeList = theTree.getShortestPathToV2(upNodeInternalId);

        //DEBUG_MESSAGE("arc number"<<treeList.size());

        if (treeList.size() == 1 && treeList.back() == NULL) {//bad list
            DEBUG_MESSAGE("bad list");
            continue;
        }
        if (treeList.empty() == true) {
            map<unsigned long, set<patArc*> >::iterator nodeFound = assocEndNodes.find(iter2->first);
            //			DEBUG_MESSAGE("found node"<<nodeFound->second.size());
            if (nodeFound != assocEndNodes.end()) {
                for (set<patArc*>::iterator iter3 = nodeFound->second.begin();
                        iter3 != nodeFound->second.end();
                        ++iter3) {

                    for (set<patArc*>::iterator iter4 = iter2->second.begin();
                            iter4 != iter2->second.end();
                            ++iter4) {
                        theTripGraph->setSuccessor(const_cast<patArc*> (*iter3), const_cast<patArc*> (*iter4));
                        //DEBUG_MESSAGE("OK1");
                    }

                }
            }

        }            //connect ddr arc to the end of the tree list
        else {
            if (interArcs != NULL) {
                interArcs->insert(treeList.begin(), treeList.end());

            }
            for (set<patArc*>::iterator iter3 = iter2->second.begin();
                    iter3 != iter2->second.end();
                    ++iter3) {
                theTripGraph->setSuccessor(treeList.back(), const_cast<patArc*> (*iter3));
            }


            //connect the first arc of the list to the associate previous ddr

            unsigned long frontNodeId = treeList.front()->m_up_node_id;
            map<unsigned long, set<patArc*> >::iterator nodeFound = assocEndNodes.find(frontNodeId);

            for (set<patArc*>::iterator iter3 = nodeFound->second.begin();
                    iter3 != nodeFound->second.end();
                    ++iter3) {
                if (detReverseArc(const_cast<patArc*> (*iter3), treeList.front()) == false) {

                    theTripGraph->setSuccessor(const_cast<patArc*> (*iter3), treeList.front());
                }
            }
            patArc* prevArc = treeList.front();
            list<patArc*>::iterator iter3 = treeList.begin();
            iter3++;
            //DEBUG_MESSAGE("linking in the tree list ");

            for (;
                    iter3 != treeList.end();
                    iter3++) {
                theTripGraph->setSuccessor(prevArc, const_cast<patArc*> (*iter3));
                prevArc = const_cast<patArc*> (*iter3);
            }
        }

    }
    /*
    if(interArcs!=NULL){
            set<patArc*> arcSet = getDDRArcs();
            interArcs->insert(arcSet.begin(),arcSet.end());
		
            arcSet = prevGpsPoint->getDDRArcSet();
            interArcs->insert(arcSet.begin(),arcSet.end());
		
    }
    interArcs->insert(prevGpsPoint->getLinkDDR()->begin(),prevGpsPoint->getLinkDDR()->end());
    interArcs->insert(linkDDR.begin(),prevGpsPoint->getLinkDDR()->end());
     */
    connectDDRArcs(theTripGraph);

    calDDRSum();

}

set<patArc*> patGpsPoint::getDDRArcSet() {
    set<patArc*> arcSet;
    for (map<patArc*, double>::iterator arcIter = linkDDR.begin();
            arcIter != linkDDR.end();
            ++arcIter) {

        arcSet.insert(arcIter->first);
    }

    return arcSet;
}

bool patGpsPoint::emptyDomain() {
    return linkDDR.empty();

}

void patGpsPoint::lowSpeedDDR(set<patArc*>* arcSet, patNetwork* theNetwork) {
    for (set<patArc*>::iterator arcIter = arcSet->begin();
            arcIter != arcSet->end();
            ++arcIter) {
        detLinkDDR(const_cast<patArc*> (*arcIter), theNetwork);
    }
    //selectDomainByCdf();
    //selectDomainByNumber(patNBParameters::the()->maxDomainSize);
    calDDRSum();

}

void patGpsPoint::traceBackDDRArcs_V1(patGpsPoint* prevGpsPoint,
        patNetwork* theNetwork,
        vector< list < pair<patArc*, unsigned long> > >* adjList,
        set<patArc*> inherentDDR

        ) {

    set<unsigned long> theEndNodes = prevGpsPoint->getEndNodes(theNetwork);
    set<patArc*> ddrArcs;
    double theDistanceCeil = getDistanceCeilFromPrevDDR(prevGpsPoint);

    patShortestPathAlgoRange theShortestPathAlgoRange(this, adjList, theNetwork, theNetwork->minimumLabelForShortestPath);

    theShortestPathAlgoRange.computeShortestPathTree(theEndNodes, theDistanceCeil, &ddrArcs);
    patShortestPathTreeRange theTree = theShortestPathAlgoRange.getTree();
    for (map<patArc*, double>::iterator iter = linkDDR.begin(); iter != linkDDR.end(); ++iter) {
        if (inherentDDR.find(iter->first) != inherentDDR.end()) {
            continue;
        }
        unsigned long upNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter->first->m_up_node_id);
        unsigned long downNodeInternalId = theNetwork->getInternalNodeIdFromUserId(iter->first->m_down_node_id);

        list<unsigned long> treeList = theTree.getShortestPathTo(upNodeInternalId, theNetwork, &linkDDR);
        if (treeList.empty()) {
            DEBUG_MESSAGE("No node");
            continue;
        } else {
            //DEBUG_MESSAGE(treeList.back());

            treeList.push_back(downNodeInternalId);

            unsigned long upNodeId = treeList.front();
            treeList.pop_front();
            pair<unsigned long, list<unsigned long> > elementToInsert = pair<unsigned long, list<unsigned long> >(upNodeId, treeList);

            if (predecessor.find(downNodeInternalId) == predecessor.end()) {
                vector<pair<unsigned long, list<unsigned long> > > nodeLists;
                nodeLists.push_back(elementToInsert);
                predecessor[downNodeInternalId] = nodeLists;
            } else {
                predecessor[downNodeInternalId].push_back(elementToInsert);
            }
            predecessor[downNodeInternalId].push_back(elementToInsert);

        }
    }
}

void patGpsPoint::connectDDRArcs(patTripGraph* theTripGraph) {

    //DEBUG_MESSAGE("Link between ddr arcs");

    map<unsigned long, set<patArc*> > assocEndNodes;
    map<unsigned long, set<patArc*> > assocStartNodes;

    for (map<patArc*, double>::iterator arcIter = linkDDR.begin();
            arcIter != linkDDR.end();
            ++arcIter) {

        if (assocEndNodes.find(arcIter->first->m_down_node_id) == assocEndNodes.end()) {
            assocEndNodes[arcIter->first->m_down_node_id] = set<patArc*>();
        }

        assocEndNodes[arcIter->first->m_down_node_id].insert(arcIter->first);

        if (assocStartNodes.find(arcIter->first->m_up_node_id) == assocStartNodes.end()) {
            assocStartNodes[arcIter->first->m_up_node_id] = set<patArc*>();
        }

        assocStartNodes[arcIter->first->m_up_node_id].insert(arcIter->first);


    }


    for (map<unsigned long, set<patArc*> >::iterator endIter = assocEndNodes.begin();
            endIter != assocEndNodes.end();
            ++endIter) {

        if (assocStartNodes.find(endIter->first) != assocStartNodes.end()) {
            for (set<patArc*>::iterator endArcIter = endIter->second.begin();
                    endArcIter != endIter->second.end();
                    ++endArcIter) {

                patArc* theEndArc = const_cast<patArc*> (*endArcIter);
                for (set<patArc*>::iterator startArcIter = assocStartNodes[endIter->first].begin();
                        startArcIter != assocStartNodes[endIter->first].end();
                        ++startArcIter) {
                    theTripGraph->setSuccessor(theEndArc, const_cast<patArc*> (*startArcIter));
                }
            }
        }
    }

}

bool patGpsPoint::detReverseArc(patArc* aArc, patArc* bArc) {
    //patBoolean rtnValue = patFALSE;
    //patULong aUp = aArc->upNodeId;
    //patULong aDown = aArc->downNodeId;
    if (aArc->m_up_node_id == bArc->m_down_node_id && bArc->m_up_node_id == aArc->m_down_node_id) {
        return true;
    }

    return false;
}

patArc* patGpsPoint::getReverseArc(patArc* aArc, patNetwork* theNetwork) {
    return theNetwork->getArcFromNodesUserId(aArc->m_down_node_id, aArc->m_up_node_id);
}

pair<patArc*, double> patGpsPoint::isInArcDomain(patArc* aArc) {
    map<patArc*, double>::iterator arcFound = linkDDR.find(aArc);
    if (arcFound != linkDDR.end()) {
        return pair<patArc*, double > (arcFound->first, arcFound->second);
    } else {
        return pair<patArc*, double > (aArc, -1.0);
    }
}

bool patGpsPoint::inReverseArc(patArc* aArc, patGpsPoint* prevGpsPoint, patNetwork* theNetwork) {
    map<patArc*, double>* prevLinkDDR = prevGpsPoint->getLinkDDR();
    patArc* rArc = getReverseArc(aArc, theNetwork);
    if (rArc != NULL && (prevLinkDDR->find(rArc) != prevLinkDDR->end() || linkDDR.find(rArc) == linkDDR.end())) {
        return true;
    }
    return false;

}

double patGpsPoint::getArcDDRValue(patArc* theArc, patNetwork* theNetwork) {
    map<patArc*, double>::iterator arcFound = linkDDR.find(theArc);
    if (arcFound == linkDDR.end()) {
        return calLinkDDR(theArc, theNetwork);
    }
    return arcFound->second;
}

double patGpsPoint::distanceTo(patGpsPoint* aGpsPoint) {
    return geoCoord.distanceTo(*(aGpsPoint->getGeoCoord()));
}

pair<bool, double> patGpsPoint::isGpsPointInZone(patGpsPoint* prevGpsPoint,
        patGpsPoint* aGpsPoint) {
    double radiusCeiling = patNBParameters::the()->zoneRadius; //meter
    double timeCeiling = patNBParameters::the()->zoneTime;
    pair<bool, double> rtnValue = pair<bool, double > (false, 0.0);
    patGeoCoordinates center((geoCoord.latitudeInDegrees + prevGpsPoint->getGeoCoord()->latitudeInDegrees) / 2,
            (geoCoord.longitudeInDegrees + prevGpsPoint->getGeoCoord()->longitudeInDegrees) / 2);
    double distance = center.distanceTo(*(aGpsPoint->getGeoCoord()));
    double distance_prev = prevGpsPoint->distanceTo(aGpsPoint);
    if ((fabs(timeStamp - aGpsPoint->getTimeStamp()) < timeCeiling ||
            fabs(prevGpsPoint->getTimeStamp() - aGpsPoint->getTimeStamp()) < timeCeiling)
            && distance < radiusCeiling) {
        rtnValue.first = true;
        rtnValue.second = distance;
    }

    return rtnValue;
}

map<patGpsPoint*, double> patGpsPoint::detGpsPointsInZone(patGpsPoint* prevGpsPoint
        , vector<patGpsPoint>* gpsSequence) {
    map<patGpsPoint*, double> rtnValue;

    for (unsigned long i = 0; i < gpsSequence->size(); ++i) {
        //if(gpsSequence->at(i).getType()!= "normal_speed" && ){
        //		continue;
        //	}
        pair<bool, double> detTemp = isGpsPointInZone(prevGpsPoint, &(gpsSequence->at(i)));
        if (detTemp.first == true || &(gpsSequence->at(i)) == prevGpsPoint || &(gpsSequence->at(i)) == &(*this)) {
            rtnValue[&(gpsSequence->at(i))] = detTemp.second;
        }

    }

    return rtnValue;
}

pair<double, double> patGpsPoint::calSpeedInZone(patGpsPoint* prevGpsPoint,
        vector<patGpsPoint>* gpsSequence) {
    map<patGpsPoint*, double> gpsPointsInZone = detGpsPointsInZone(prevGpsPoint, gpsSequence);


    pair<double, double> speedProfile(0.0, 0.0);
    for (map<patGpsPoint*, double>::iterator gpsIter = gpsPointsInZone.begin();
            gpsIter != gpsPointsInZone.end();
            ++gpsIter
            ) {
        speedProfile.first += gpsIter->first->getSpeedMS();
    }
    speedProfile.first /= gpsPointsInZone.size();


    //patReal speedVariance;
    for (map<patGpsPoint*, double>::iterator gpsIter = gpsPointsInZone.begin();
            gpsIter != gpsPointsInZone.end();
            ++gpsIter
            ) {
        speedProfile.second += pow(gpsIter->first->getSpeedAccuracyMS(), 2);

        //speedProfile.second += pow( (gpsIter->first->getSpeedMS()-speedProfile.first),2);
    }
    speedProfile.second /= pow(gpsPointsInZone.size(), 2);

    //speedProfile.second/=(gpsPointsInZone.size()-1);

    speedProfile.second = sqrt(speedProfile.second);

    return speedProfile;
}

/**
approximated CDF of standard normal distribution
code adpated from http://www.ma.ic.ac.uk/~mdavis/course_material/MOP/CumNormDist.txt

 **/

double CDF_Normal(double x) {
    int neg = (x < 0);
    if (neg) x *= -1;
    double k(1 / (1 + 0.2316419 * x));
    double y = ((((1.330274429 * k - 1.821255978) * k + 1.781477937) * k - 0.356563782) * k + 0.319381530) * k;
    y = 1.0 - 0.398942280401 * exp(-0.5 * x * x) * y;
    return (1 - neg)*y + neg * (1 - y);
}

void patGpsPoint::setGpsParams(struct gps_params * p,
        patGpsPoint* prevGpsPoint,
        vector<patGpsPoint>* gpsSequence) {
    pair<double, double> speedProfile = calSpeedInZone(prevGpsPoint, gpsSequence);
    p->time_diff = double(timeStamp) - double(prevGpsPoint->getTimeStamp());
	
	p->time_prev = double(prevGpsPoint->getTimeStamp());
	p->time_curr = double(timeStamp);
	
    p->mu_v_curr = getSpeedMS();
    p->std_v_curr = getSpeedAccuracyMS();

    p->mu_v_prev = prevGpsPoint->getSpeedMS();
    p->std_v_prev = prevGpsPoint->getSpeedAccuracyMS();

    p->mu_v_inter = speedProfile.first;
    p->std_v_inter = speedProfile.second;
    p->max_v_inter = patNBParameters::the()->maxMotorSpeed;

    p->std_x_prev = prevGpsPoint->getHorizonAccuracy();
    p->std_x_curr = getHorizonAccuracy();

    //if (type=="normal_speed" && prevGpsPoint->getType()=="normal_speed"){
    //	p->p_0=patNBParameters::the()->zeroSpeedProba0;
    //}
    //else if(type=="normal_speed" && prevGpsPoint->getType()!="normal_speed"){
    //	p->p_0=patNBParameters::the()->zeroSpeedProba1;
    //}
    //else {
    //	p->p_0=patNBParameters::the()->zeroSpeedProba2;
    //}
    double lambda = patNBParameters::the()->pZeroLambda;
    double speedDividor = patNBParameters::the()->pZeroSpeedRatio;
    //p->p_0=0.5*lambda*(exp(-lambda*getSpeedMS()*speedDividor)+exp(-lambda*prevGpsPoint->getSpeedMS()*speedDividor));

    p->p_0 = exp(-0.5 * lambda * (getSpeedMS() + prevGpsPoint->getSpeedMS()));
    //DEBUG_MESSAGE("prev speed:"<<prevGpsPoint->getSpeedMS()<<",curr speed:"<<getSpeedMS()<<",p_0:"<<p->p_0);
    p->v_denom_curr = 1.0 - CDF_Normal(-p->mu_v_curr / p->std_v_curr);
    p->v_denom_prev = 1.0 - CDF_Normal(-p->mu_v_prev / p->std_v_prev);
    p->v_denom_inter = 1.0 - CDF_Normal(-p->mu_v_inter / p->std_v_inter);
}

double patGpsPoint::calPerpendicularFootOnArc(map<char*, double> distanceToArc) {
    if (isnan(distanceToArc["ver"])) {
        DEBUG_MESSAGE("wrong ver" << distanceToArc["position"]);
        if (distanceToArc["position"] == -1) {
            return 0;
        }
        if (distanceToArc["position"] == 1) {
            return 1;
        }
        return distanceToArc["ver"];
    } else if (distanceToArc["position"] == -1) {
        return -sqrt(pow(distanceToArc["link"], 2) - pow(distanceToArc["ver"], 2)) / distanceToArc["length"];
    }
    else if (distanceToArc["position"] == 0) {
        return sqrt(pow(distanceToArc["up"], 2) - pow(distanceToArc["ver"], 2)) / distanceToArc["length"];
    }
    else if (distanceToArc["position"] == 1) {
        return 1 + sqrt(pow(distanceToArc["link"], 2) - pow(distanceToArc["ver"], 2)) / distanceToArc["length"];
    }

}

patString patGpsPoint::getType() {
    return type;
}
void patGpsPoint::setSpeedType(){
    if (speed>minNormalSpeed){
        type = "normal_speed";
    }
    else{
        type = "slow_speed";
    }
    /*
     * Originally also use min average speed.
    if (headingAccuracy < maxNormalSpeedHeading){
        type = "normal_speed";
    }
    else{
        type = "slow_speed";
    }
*/
    setType(type);
}

void patGpsPoint::setType(patString theType) {
    type = theType;
}

double patGpsPoint::getLatitude() {
    return geoCoord.latitudeInRadians;
}

double patGpsPoint::getLongitude() {
    return geoCoord.longitudeInRadians;
}

double patGpsPoint::getHeading() {
    return heading;
}

double patGpsPoint::getSpeedAccuracy() {
    return speedAccuracy;
}

void patGpsPoint::updateLinkDDR(set<patArc*>* newDomain) {

    for (map<patArc*, double>::iterator iter1 = linkDDR.begin();
            iter1 != linkDDR.end();
            ) {
        if (newDomain->find(iter1->first) == newDomain->end()) {
            linkDDR.erase(iter1++);

        } else {
            iter1++;
        }

    }
    calDDRSum();
}

bool patGpsPoint::isArcInDomain(patArc* theArc) {
    if (linkDDR.find(theArc) == linkDDR.end()) {
        return false;
    } else {
        return true;
    }
}

void patGpsPoint::setHeading(double v) {
    heading = v;
}

void patGpsPoint::setSpeed(double v) {
    speed = v;
}

set<patArc*> patGpsPoint::getDomainSet() {
    set<patArc*> rtnSet;
    for (map<patArc*, double>::iterator aIter = linkDDR.begin();
            aIter != linkDDR.end();
            ++aIter) {
        rtnSet.insert(aIter->first);

    }
    return rtnSet;
}

void patGpsPoint::setSpeedAccuracy(double v) {
    speedAccuracy = v;
}

struct link_ddr_range patGpsPoint::getLinkDDRRange(patArc* arc) {
    return linkDDR_Range[arc];
}

map<patArc*, struct link_ddr_range> patGpsPoint::getLinkDDRRange() {

    return linkDDR_Range;
}
