//----------------------------------------------------------------
// File: patGpsPoint.cc
// Author: 
// Creation: 
//----------------------------------------------------------------

#include "patGpsPoint.h"
#include "patArc.h"
#include "patConst.h"
#include "patGeoCoordinates.h" 
#include <math.h>
#include "patDisplay.h"
#include "patNBParameters.h"
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
        map<string, double>* theAlgoParams
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

map<char*, double> patGpsPoint::distanceTo(const patGeoCoordinates* upGeoCoord, const patGeoCoordinates* downGeoCoord) {

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


double patGpsPoint::distanceTo(const patGpsPoint* aGpsPoint) {
    return geoCoord.distanceTo(*(aGpsPoint->getGeoCoord()));
}


map<char*, double> patGpsPoint::distanceTo(const patArc* theArc) {

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

double patGpsPoint::calHeading(const patGpsPoint* prevGpsPoint, const patGpsPoint* nextGpsPoint) {
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

double patGpsPoint::getMaxSpeed(const patGpsPoint* prevGpsPoint) {
    double maxSpeed = getSpeedMS();
    double geoSpeed = geoCoord.distanceTo(prevGpsPoint->geoCoord) / (timeStamp - prevGpsPoint->getTimeStamp());
    //geoSpeed = (geoSpeed < (120.0/3.6))?geoSpeed:(120.0/3.6);
    double nextSpeed = prevGpsPoint->getSpeedMS();
    maxSpeed = (maxSpeed < nextSpeed) ? nextSpeed : maxSpeed;
    maxSpeed = (maxSpeed < geoSpeed) ? geoSpeed : maxSpeed;

    return maxSpeed;

}

double patGpsPoint::getHorizonAccuracy() {
    return horizonAccuracy;
}

patGeoCoordinates* patGpsPoint::getGeoCoord() {
    return &geoCoord;
}

pair<bool, double> patGpsPoint::isGpsPointInZone(const patGpsPoint* prevGpsPoint,
        const patGpsPoint* aGpsPoint) {
    double radiusCeiling = patNBParameters::the()->zoneRadius; //meter
    double timeCeiling = patNBParameters::the()->zoneTime;
    pair<bool, double> rtnValue = pair<bool, double > (patFALSE, 0.0);
    patGeoCoordinates center((geoCoord.latitudeInDegrees + prevGpsPoint->getGeoCoord()->latitudeInDegrees) / 2,
            (geoCoord.longitudeInDegrees + prevGpsPoint->getGeoCoord()->longitudeInDegrees) / 2);
    double distance = center.distanceTo(*(aGpsPoint->getGeoCoord()));
    double distance_prev = prevGpsPoint->distanceTo(aGpsPoint);
    if ((fabs(timeStamp - aGpsPoint->getTimeStamp()) < timeCeiling ||
            fabs(prevGpsPoint->getTimeStamp() - aGpsPoint->getTimeStamp()) < timeCeiling)
            && distance < radiusCeiling) {
        rtnValue.first = patTRUE;
        rtnValue.second = distance;
    }

    return rtnValue;
}

map<patGpsPoint*, double> patGpsPoint::detGpsPointsInZone(const patGpsPoint* prevGpsPoint
        , const vector<patGpsPoint>* gpsSequence) {
    map<patGpsPoint*, double> rtnValue;

    for (int i = 0; i < gpsSequence->size(); ++i) {
        //if(gpsSequence->at(i).getType()!= "normal_speed" && ){
        //		continue;
        //	}
        pair<bool, double> detTemp = isGpsPointInZone(prevGpsPoint, &(gpsSequence->at(i)));
        if (detTemp.first == patTRUE || &(gpsSequence->at(i)) == prevGpsPoint || &(gpsSequence->at(i)) == &(*this)) {
            rtnValue[&(gpsSequence->at(i))] = detTemp.second;
        }

    }

    return rtnValue;
}

pair<double, double> patGpsPoint::calSpeedInZone(const patGpsPoint* prevGpsPoint,
        const vector<patGpsPoint>* gpsSequence) {
    map<patGpsPoint*, double> gpsPointsInZone = detGpsPointsInZone(prevGpsPoint, gpsSequence);


    pair<double, double> speedProfile(0.0, 0.0);
    for (map<patGpsPoint*, double>::iterator gpsIter = gpsPointsInZone.begin();
            gpsIter != gpsPointsInZone.end();
            ++gpsIter
            ) {
        speedProfile.first += gpsIter->first->getSpeedMS();
    }
    speedProfile.first /= gpsPointsInZone.size();


    //double speedVariance;
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
        const patGpsPoint* prevGpsPoint) {
    p->time_diff = double(timeStamp) - double(prevGpsPoint->getTimeStamp());
	
	p->time_prev = double(prevGpsPoint->getTimeStamp());
	p->time_curr = double(timeStamp);
	
    p->mu_v_curr = getSpeedMS();
    p->std_v_curr = getSpeedAccuracyMS();

    p->mu_v_prev = prevGpsPoint->getSpeedMS();
    p->std_v_prev = prevGpsPoint->getSpeedAccuracyMS();


    p->std_x_prev = prevGpsPoint->getHorizonAccuracy();
    p->std_x_curr = getHorizonAccuracy();

    double lambda = patNBParameters::the()->pZeroLambda;
    double speedDividor = patNBParameters::the()->pZeroSpeedRatio;
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

string patGpsPoint::getType() {
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

void patGpsPoint::setType(string theType) {
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


void patGpsPoint::setSpeed(double v) {
    speed = v;
}

void patGpsPoint::setSpeedAccuracy(double v) {
    speedAccuracy = v;
}


void patGpsPoint::setHeading(double v) {
	heading = v;
}

patGpsDDR* patGpsPoint::getDDR(){
	return m_ddr;
}
