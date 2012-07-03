#include "patDisplay.h"
#include "patPathDDR.h"
#include "patArc.h"
#include "patPathJ.h"
#include "patGpsPoint.h"
#include "patArcDDR.h"
patPathDDR::patPathDDR(){}


patPathDDR::patPathDDR(patPathJ* aPath,
		 vector<patGpsPoint>* aGpsSequence):
		 thePath(aPath),
		 theGpsSequence(aGpsSequence)
{

}
patReal patPathDDR::lowerBound = 0;
patReal patPathDDR::upperBound = 0;
patReal patPathDDR::lastEstimate = 0;
patULong patPathDDR::timeDiff = 0;
patReal patPathDDR::x0 = 0.0;
patReal patPathDDR::x1 = 0.0;
patReal patPathDDR::x2 = 0.0;
patReal patPathDDR::x3 = 0.0;

 ostream& operator<<(ostream& str, const patPathDDR& x) {
	str<<x.pathValue;
	return str;
 }

typedef patReal  (patPathDDR::*typefn)(patReal ,
						patULong ,
						map<patArc*, map<char*, patReal> >* ,
						patReal ,
						vector<patReal>* ,
						patReal );
/*
patPathDDR::getPathDDR(const patPathJ* thePath,const vector<patGpsPoint>* theGpsSequence){
	switch(algoName){
		case "raw": rawAlgorithm(thePath,theGpsSequence);
		case "timeSpace": timeSpaceAlgorithm(thePath,theGpsSequence);
	}
}
*/

patReal patPathDDR::getPathValue(){
	return pathValue;
}

vector<patReal> patPathDDR::getArcValues(){
	return arcValues;
}

patReal patPathDDR::rawAlgorithm( ){
	pathValue = 1;
	arcValues.assign(theGpsSequence->size(),0);

	for (int i=0;i<theGpsSequence->size();++i){

		for(map<patArc*,patReal>::iterator aIter = (*theGpsSequence)[i].getLinkDDR()->begin();
						aIter != (*theGpsSequence)[i].getLinkDDR()->end();
						++aIter){
			if(thePath->isLinkInPath(aIter->first)==patTRUE){
				arcValues[i] += aIter->second;
			}
		}
		pathValue*=arcValues[i];
	}
	return pathValue;
}

patReal patPathDDR::rawAlgorithm_average( ){

	patReal ddrSum;
	for(vector<patReal>::iterator ddrIter = arcValues.begin();
			ddrIter!=arcValues.end();
			++ddrIter){
		ddrSum+=*ddrIter;
	}

	return ddrSum/thePath->computePathLength();

}
/**
*Following section is for time space algorithm
*/
void patPathDDR::timeSpaceAlgorithm( patNetwork* theNetwork){
	DEBUG_MESSAGE("PathLength:"<<thePath->getPathLength() ) ;
	vector<patReal> nodePosition;
	pathValue = 1;
		arcValues.assign(theGpsSequence->size(),0);
	calNodePosition(&nodePosition);

	//vector<patReal> pointProbas;
	patReal lastEstimate = 0.0;
	patReal currEstimate = 0.0;

	vector<map<patArc*, map<char*, patReal> > > theDistance;
	theDistance.resize(theGpsSequence->size());
	calDistance(&theDistance,theNetwork);

	//DEBUG_MESSAGE("OK4" ) ;
	patReal alpha = 0 ;

	for(patULong i = 0; i< theGpsSequence->size();++i){
		typefn pdf= &patPathDDR::pointPDFDenumerator;
		DEBUG_MESSAGE("gps:" <<i) ;

	if( i == 0){
		lowerBound = 0.0;
		upperBound = thePath->getPathLength();
	}
	else{
		lowerBound = lastEstimate;
		patReal speed = (theGpsSequence->at(i).getSpeed()>theGpsSequence->at(i-1).getSpeed())?
			theGpsSequence->at(i).getSpeed():theGpsSequence->at(i-1).getSpeed();
			speed*=1.20/3.6;
		timeDiff = (theGpsSequence->at(i).getTimeStamp()-theGpsSequence->at(i-1).getTimeStamp());
		patReal far = speed * timeDiff;
		upperBound = lastEstimate + far;
		DEBUG_MESSAGE("bound:"<<lowerBound<<"-"<<upperBound);
			patReal speed0 = theGpsSequence->at(i-1).getSpeed()/3.6;
	patReal speed1 = speed ;

	if(speed0>speed1){
		patReal speedt = speed1;
		speed1 = speed0;
		speed0 = speedt;
	}
	 x0 = lowerBound ;
	 patReal speedf = 10;
	 x3 = x0+timeDiff*speedf;
	 x1 = lastEstimate + timeDiff*speed0;
	 x2 = lastEstimate + timeDiff*speed1;

	}

		patReal pointProbaDenumerator = integration(pdf,
							0.0,
							lowerBound,
							 i,
							&theDistance[i],
							0.0,
							&nodePosition,
							alpha)+
							integration(pdf,
							lowerBound,
							upperBound,
							 i,
							&theDistance[i],
							0.0,
							&nodePosition,
							alpha)+
							integration(pdf,
							upperBound,
							thePath->getPathLength(),
							 i,
							&theDistance[i],
							0.0,
							&nodePosition,
							alpha);
		DEBUG_MESSAGE("pointProbaDenumerator:"<<pointProbaDenumerator ) ;
		patReal pointProba = calPointProba( i,

							&theDistance[i],
							pointProbaDenumerator,
							&nodePosition,
							alpha);
		arcValues[i]=pointProba;
		//arcValues[i]=	1/fabs(log(pointProba));
		pathValue*=	arcValues[i];
		DEBUG_MESSAGE("pointProba:"<<pointProba<<","<<arcValues[i] ) ;
		//pointProbas.push_back(pointProba);
		currEstimate = estimatePosition(i,  &theDistance[i],
								 pointProbaDenumerator,&nodePosition, alpha);
		DEBUG_MESSAGE("currEstimate:"<<currEstimate ) ;
		alpha = getAlpha(currEstimate,
								 i,

								&theDistance[i],
								 pointProbaDenumerator,
								&nodePosition,
								alpha);
		DEBUG_MESSAGE("next alpha:" <<alpha) ;
		lastEstimate = currEstimate;
	}

	patReal paN = 1/fabs(log(pathValue));
DEBUG_MESSAGE("path proba:"<<pathValue<<","<<paN ) ;
	//pathValue = paN;
}

patReal patPathDDR::locationPDF(patReal position,
								patULong currGpsNumber,
								map<patArc*, map<char*, patReal> >* distanceToPath,
								patReal pointProbaDenumerator,
								vector<patReal>* theNodePosition,
								patReal alpha){
	return distancePDF(position, currGpsNumber,distanceToPath,theNodePosition)*
		priorPDF(position, currGpsNumber,alpha)/pointProbaDenumerator;
}
patReal patPathDDR::pointPDF(patReal position,
						patULong currGpsNumber,
						map<patArc*, map<char*, patReal> >* distanceToPath,
						patReal pointProbaDenumerator,
						vector<patReal>* theNodePosition,
						patReal alpha){

	patReal rtnValue =  locationPDF( position, currGpsNumber,  distanceToPath,pointProbaDenumerator, theNodePosition,
						 alpha) * distancePDF( position, currGpsNumber,distanceToPath,theNodePosition);
	if(isnan(rtnValue)){

		rtnValue =0.0;
	}

	return rtnValue;
}
patReal patPathDDR::calPointProba(	patULong& currGpsNumber,
						map<patArc*, map<char*, patReal> >* distanceToPath,
						patReal pointProbaDenumerator,
						vector<patReal>* theNodePosition,
						patReal alpha){
			typefn pdf= &patPathDDR::pointPDF;

	return integration(pdf,0.0,lowerBound, currGpsNumber,distanceToPath,pointProbaDenumerator,theNodePosition, alpha)+
		integration(pdf,lowerBound,upperBound, currGpsNumber,distanceToPath,pointProbaDenumerator,theNodePosition, alpha)+
		integration(pdf,upperBound,thePath->getPathLength(), currGpsNumber,distanceToPath,pointProbaDenumerator,theNodePosition, alpha)
	;

}

patReal patPathDDR::integration(patReal (patPathDDR::*fp	)(patReal ,
						patULong ,
						map<patArc*, map<char*, patReal> >* ,
						patReal ,
						vector<patReal>* ,
						patReal ),
			 patReal a,
			 patReal b,
			 patULong currGpsNumber,
			 map<patArc*, map<char*, patReal> >* distanceToPath,
			 patReal pointProbaDenumerator,
			vector<patReal>* theNodePosition,
			 patReal alpha
			 ){
	//from simpsonq
	//DEBUG_MESSAGE("length:"<<b-a) ;
	patReal epsilon = 0.000001;
	patReal result;
    int i;
    int n;
    patReal h;
    patReal s;
    patReal s1;
    patReal s2;
    patReal s3;
    patReal x;

	 patReal c;
	 if (a>b){
		c=a;
		a=b;
		b=c;
	 }

    s2 = 1;
    h = b-a;
    s = (this->*fp)(a, currGpsNumber,   distanceToPath,pointProbaDenumerator,theNodePosition,alpha)+
    (this->*fp)(b, currGpsNumber,   distanceToPath,pointProbaDenumerator,theNodePosition,alpha);

    do
    {
        s3 = s2;
        h = h/2;
        s1 = 0;
        x = a+h;
        do
        {
            s1 = s1+2 * (this->*fp)(x,currGpsNumber,   distanceToPath,pointProbaDenumerator,theNodePosition,alpha);
            x = x+2*h;
                    				//DEBUG_MESSAGE("OK8"<<x ) ;
        }
        while(x<b);
        s = s+s1;

        s2 = (s+s1)*h/3;
        x = fabs(s3-s2)/15;
        				//DEBUG_MESSAGE("OK8"<<x ) ;
    }
    while(x>epsilon);
    result = s2;
    //DEBUG_MESSAGE("inte:"<<result) ;
    return result;


}


patReal patPathDDR::pointPDFNumerator(patReal position,
								patULong currGpsNumber,
								map<patArc*, map<char*, patReal> >* distanceToPath,
								vector<patReal>* theNodePosition,
								patReal alpha){//integration f^2 * g
	return
	distancePDF(position,
				currGpsNumber,
				distanceToPath,
				theNodePosition) *
	distancePDF(position,
				currGpsNumber,
				distanceToPath,
				theNodePosition) *
	priorPDF(position,

				currGpsNumber,
				alpha);
}

patReal	 patPathDDR::pointPDFDenumerator(patReal position,
								patULong currGpsNumber,
								map<patArc*, map<char*, patReal> >* distanceToPath,
								patReal nothing,
								vector<patReal>* theNodePosition,
								patReal alpha){//integration f * g

	/*DEBUG_MESSAGE("DISTANCE proba:"<<distancePDF(position,
				currGpsNumber,
				distanceToPath,
				theNodePosition) <<"prior:"
				<<priorPDF(position,
				lastEstimate,
				currGpsNumber,
				alpha) );
	*/
	patReal distanceProba =
	distancePDF(position,
				currGpsNumber,
				distanceToPath,
				theNodePosition);
	patReal priorProba =
	priorPDF(position,

				currGpsNumber,
				alpha);

	patReal rtnValue = distanceProba * priorProba;
	if(isnan(rtnValue)){
		DEBUG_MESSAGE("distanceProba:"<<distanceProba<<",priorProba"<<priorProba);
		rtnValue = 0.0;
	}
	return rtnValue;
}


patReal patPathDDR::distancePDF(patReal position,
						patULong currGpsNumber,
						map<patArc*, map<char*, patReal> >* distanceToPath,
						vector<patReal>* theNodePosition){

	patULong arcNumber = getArcNumberFromPosition(position,theNodePosition);
	//DEBUG_MESSAGE("OK" <<position<<","<<arcNumber) ;
	patArc* theArc = thePath->getArc(arcNumber);

	patReal positionOnArc = position - (*theNodePosition)[arcNumber];
	map<char*, patReal> distanceToArc  = (*distanceToPath)[theArc];
		        				//DEBUG_MESSAGE("OK8" ) ;
			patReal rtnValue;
	if(isnan(distanceToArc["ver"])){
		patDDR theDDR (theGpsSequence->at(currGpsNumber).getHorizonAccuracy());


		if ( distanceToArc["position"]==-1 ){//gps point at left

			rtnValue = theDDR.errPDF(distanceToArc["up"]+positionOnArc);
		}
		else if(distanceToArc["position"]==1){//gps point at right
			rtnValue = theDDR.errPDF(distanceToArc["up"]-positionOnArc);
		}
		else{
			rtnValue = theDDR.errPDF(distanceToArc["up"]+positionOnArc/2);
		}
		 rtnValue;
	}
	else{
		patReal startLength = sqrt( distanceToArc["up"]* distanceToArc["up"]-distanceToArc["ver"]*distanceToArc["ver"]);

		patArcDDR theArcDDR(theGpsSequence->at(currGpsNumber).getHorizonAccuracy());

		if ( distanceToArc["position"]==-1 ){//gps point at left

			rtnValue = theArcDDR.errPDF(distanceToArc["ver"],startLength+positionOnArc);
		}
		else if(distanceToArc["position"]==1){//gps point at right
			rtnValue = theArcDDR.errPDF(distanceToArc["ver"],startLength-positionOnArc);
		}
		else{
			rtnValue = theArcDDR.errPDF(distanceToArc["ver"],fabs(positionOnArc));
		}
	}
	//DEBUG_MESSAGE("distanceToArc:"<<distanceToArc["ver"]);

	if(isnan(rtnValue)){
		DEBUG_MESSAGE("startLength:"<<arcNumber<<","<<positionOnArc<<","<<distanceToArc["up"]<<","<<distanceToArc["ver"]);
	}

	return rtnValue;
}


patReal patPathDDR::priorPDF(patReal position,
						patULong currGpsNumber,
						patReal alpha){
	if (alpha == 0.0){
		return uniform();
	}
	patReal mg = manipG(position,
					currGpsNumber);
	patReal g = alpha * mg+
			(1-alpha) * uniform();
	//DEBUG_MESSAGE("g"<<g);
	if(isnan(g)){
	DEBUG_MESSAGE("alpha"<<alpha);
	}
	return g;
}

//uniform
patReal patPathDDR::manipG(patReal position,
					patULong currGpsNumber){

	if (position<=upperBound && position >= lowerBound){
		return 1.0/(upperBound-lowerBound);
	}
	else{
		return 0.0;
	}
}

/*
//triangular
patReal patPathDDR::manipG(patReal position,
					patULong currGpsNumber){

	patReal h = 2/(x3-x0+x2-x1);
	//DEBUG_MESSAGE("x"<<x0<<","<<x1<<","<<x2<<","<<x3<<","speed0<<","<<speed1);
	if(position<=x0){
		return 0.0;
	}
	else if(position>x0 && position <x1){
		return (position- x0) * h/(x1-x0);
	}
	else if(position >=x1 && position <= x2){
		return h;
	}
	else if(position >x2 && position < x3){
		return h - (position-x2)*h/(x3-x2);
	}
	else{
		return 0.0;
	}
}
*/
patReal patPathDDR::getAlpha(patReal position,
					patULong currGpsNumber,
					map<patArc*, map<char*, patReal> >* distanceToPath,
					patReal pointProbaDenumerator,
					vector<patReal>* theNodePosition,
					patReal alpha){

	patReal locProba = locationPDF( position,
								 currGpsNumber,
								distanceToPath,
								 pointProbaDenumerator,
								theNodePosition,
								alpha);
	locProba = fabs(log(locProba));
	locProba = 1/locProba;

	return 0.8 + 0.2*locProba;
}


void patPathDDR::calDistance(vector<map<patArc*, map<char*, patReal> > >* theDistance,patNetwork* theNetwork){

	list<patArc*>* arcList = thePath->getArcList();
	for(list<patArc*>::iterator arcIter = arcList->begin();
						arcIter != arcList->end();
						++arcIter){

		patCoordinates* upNodeGeoCoord = &(theNetwork->getNodeFromUserId((*arcIter)->upNodeId)->geoCoord);
		patCoordinates* downNodeGeoCoord = &(theNetwork->getNodeFromUserId((*arcIter)->downNodeId)->geoCoord);

		for(int i = 0; i< theGpsSequence->size();++i){
						//DEBUG_MESSAGE("OK" ) ;
			theDistance->at(i)[*arcIter] = theGpsSequence->at(i).distanceTo(upNodeGeoCoord,downNodeGeoCoord);
		}

	}

}

void patPathDDR::calNodePosition(vector<patReal>* theNodePosition){

	theNodePosition->push_back(0.0);
	list<patArc*>* arcList = thePath->getArcList();
	patULong length_temp = 0;
	for(list<patArc*>::iterator arcIter = arcList->begin();
						arcIter != arcList->end();
						++arcIter){

		length_temp+=(*arcIter)->getLength();
		theNodePosition->push_back(length_temp);
		//DEBUG_MESSAGE("OK"<<(*arcIter)->userId) ;
	}
}


patULong patPathDDR::getArcNumberFromPosition(patReal position,
						vector<patReal>* theNodePosition){

	for(int i=0; i< theNodePosition->size();++i){
		if ((*theNodePosition)[i] > position){
			return (i-1);
		}
	}
	return theNodePosition->size()-2;
}



patReal patPathDDR::uniform(){
	return 1.0/thePath->getPathLength();
}



patReal patPathDDR::estimatePosition(patULong currGpsNumber,
								map<patArc*, map<char*, patReal> >* distanceToPath,
								patReal pointProbaDenumerator,
								vector<patReal>* theNodePosition,
								patReal alpha){

	list<patArc*>* arcList = thePath->getArcList();//arc list of the path
	//DEBUG_MESSAGE("OK90");
	//get boundary of current location
	//base on estimated location of last gps point, time difference between two gps points and free flow

		//DEBUG_MESSAGE("OK91");
	//get max likly distance from a gps point to a location on path
	//i.e. the peak point of err distance distribution
	patReal maxLikliDistance = theGpsSequence->at(currGpsNumber).getHorizonAccuracy()/sqrt(2.0);

	patReal estimatePositionOnPath,estimatePositionInBound,estimatePositionNotInBound;

	//search through all arcs in the path, to get the locations with maxLikliDistance to GPS point

	patReal leastDistanceInBound = patMaxReal;//There are maybe several peak points. Among those, choose the one with least distance from gps to arc.
	patReal leastDistanceNotInBound = patMaxReal;
	patULong i = 0;
	for(list<patArc*>::iterator arcIter = arcList->begin();
						arcIter != arcList->end();
						++arcIter){


		map<char*, patReal> distanceToArc = (*distanceToPath)[*arcIter];

	//DEBUG_MESSAGE("OK92");
		patReal lowerDistance;
		patReal upperDistance;

		//if projection of gps point to arc is on arc
		if (distanceToArc["position"]!=-1  && distanceToArc["position"]!=1){

			//if the max likly location is between upnode and projection
			if(maxLikliDistance <= distanceToArc["up"] && maxLikliDistance>=distanceToArc["ver"]){

				//position of the location
				patReal estPosition = (*theNodePosition)[i] +
						sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"])-
						sqrt(maxLikliDistance * maxLikliDistance - distanceToArc["ver"] * distanceToArc["ver"]);

				//if the position is within boundary
				if (estPosition  >= lowerBound && estPosition  <= upperBound){

					//if the distance is the least among all possible positions
					//	take it as the most likly position
					if ( distanceToArc["link"] < leastDistanceInBound){
						estimatePositionInBound = estPosition;
						leastDistanceInBound = distanceToArc["link"];
												continue;

					}
				}

				//if not in boundary and have no peak point in boundary
				//if there is a peak point in boundary, this peak point must be the most likly position
				else if(leastDistanceInBound == patMaxReal){

					//if the distance is the least among all possible positions
					if(distanceToArc["link"] < leastDistanceNotInBound){
						estimatePositionNotInBound = estPosition;
						leastDistanceNotInBound = distanceToArc["link"];
						continue;
					}
				}
			}
			if(maxLikliDistance<=distanceToArc["down"] && maxLikliDistance>=distanceToArc["ver"]){
				patReal estPosition = (*theNodePosition)[i] -
						sqrt(distanceToArc["down"] * distanceToArc["down"] - distanceToArc["ver"] * distanceToArc["ver"])+
						sqrt(maxLikliDistance * maxLikliDistance - distanceToArc["ver"] * distanceToArc["ver"]);

				if (estPosition  >= lowerBound && estPosition  <= upperBound){
					if ( distanceToArc["link"] < leastDistanceInBound){
						estimatePositionInBound = estPosition;
						leastDistanceInBound = distanceToArc["link"];
						continue;

					}

				}
				else if(leastDistanceInBound == patMaxReal){
					if(distanceToArc["link"] < leastDistanceNotInBound){
						estimatePositionNotInBound = estPosition;
						leastDistanceNotInBound = distanceToArc["link"];
						continue;
					}
				}

			}
		}

		//if projection not on arc
		else {

			//compare the distance of two termintal nodes to gps points
			lowerDistance = (distanceToArc["up"]<distanceToArc["down"])?distanceToArc["up"]:distanceToArc["down"];
			upperDistance = (distanceToArc["up"]>=distanceToArc["down"])?distanceToArc["up"]:distanceToArc["down"];

			//if maxLikliDistance is within distance Range, get the most Likely position
			if(maxLikliDistance<=upperDistance && maxLikliDistance>=lowerDistance){

				patReal estPosition =  (*theNodePosition)[i] +
						sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"])-
						sqrt(maxLikliDistance * maxLikliDistance - distanceToArc["ver"] * distanceToArc["ver"]);

				//if the most likly position is in boundary(use the same logic as the case which projection is on arc)
				if (estPosition  >= lowerBound && estPosition  <= upperBound){
					if ( distanceToArc["link"] < leastDistanceInBound){
						estimatePositionInBound = estPosition;
						leastDistanceInBound = distanceToArc["link"];
						continue;
					}
				}
				else if(leastDistanceInBound == patMaxReal){
					if(distanceToArc["link"] < leastDistanceNotInBound){
						estimatePositionNotInBound = estPosition;
						leastDistanceNotInBound = distanceToArc["link"];
						continue;
					}
				}
			}

		}
		++i;

	}


	//if no peak point found
	//search for potential positions with max probability
	//potential positions are nodes, projections on arc, and boundarys
	if(leastDistanceInBound == patMaxReal){

		//get where is the boundary on
		patULong startArcNum = getArcNumberFromPosition(lowerBound,theNodePosition);
		patULong endArcNum = getArcNumberFromPosition(upperBound,theNodePosition);


		vector<patReal> positionToBeTested;//potentail positions
			//DEBUG_MESSAGE("OK93");
		//insert the boundaries
		positionToBeTested.push_back(lowerBound);
		positionToBeTested.push_back(upperBound);

		//if the boundaries are on the same arc
		if(startArcNum == endArcNum){
			patArc* onlyArc = thePath->getArc(startArcNum);
			map<char*, patReal> distanceToArc = (*distanceToPath)[onlyArc];

			//if the projection is on arc,take the projection position
			if(distanceToArc["position"]!=1 && distanceToArc["position"]!=-1){
				patReal v = (*theNodePosition)[startArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);

				if(v > lowerBound && v < upperBound){
					positionToBeTested.push_back(v);
				}
			}

		}

		//if the boundaries cotains several arcs
		else{
			patArc* startArc = thePath->getArc(startArcNum);
			patArc* endArc = thePath->getArc(endArcNum);

			positionToBeTested.push_back((*theNodePosition)[startArcNum+1]);
			map<char*, patReal> distanceToArc = (*distanceToPath)[startArc];
			if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
				patReal v = (*theNodePosition)[startArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);

				if(v > lowerBound && v < (*theNodePosition)[startArcNum+1]){
					positionToBeTested.push_back(v);
				}
			}

			 distanceToArc = (*distanceToPath)[endArc];
			if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
				patReal v = (*theNodePosition)[endArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);

				if( v > (*theNodePosition)[endArcNum] && v < upperBound){
					positionToBeTested.push_back(v);
				}
			}


			for(patULong i=startArcNum+1; i < endArcNum && i+1<theNodePosition->size();++i){

			//DEBUG_MESSAGE("OK940"<<i+1<<","<<theNodePosition->size());
				positionToBeTested.push_back((theNodePosition->at(i+1)));
			//DEBUG_MESSAGE("OK941");
				map<char*, patReal> distanceToArc = (*distanceToPath)[thePath->getArc(i)];
				if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){

					patReal v =(*theNodePosition)[i] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
					positionToBeTested.push_back(v);
				}
			}
		}

		//if peak points outside of boundary are also not found,
		//also search for potential positions
		if(leastDistanceNotInBound == patMaxReal){
					patArc* startArc = thePath->getArc(startArcNum);
			patArc* endArc = thePath->getArc(endArcNum);
			for(patULong i=0; i < startArcNum;++i){
				positionToBeTested.push_back((theNodePosition->at(i)));
				map<char*, patReal> distanceToArc = (*distanceToPath)[thePath->getArc(i)];

				if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
					patReal v = (*theNodePosition)[i] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
					positionToBeTested.push_back(v);
				}
			}

			positionToBeTested.push_back((theNodePosition->at(startArcNum)));
			map<char*, patReal> distanceToArc = (*distanceToPath)[startArc];
			if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
				patReal v = (*theNodePosition)[startArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);

				if(v < lowerBound){
					positionToBeTested.push_back(v);
				}
			}
			DEBUG_MESSAGE("OK94");
			if(endArcNum+1<theNodePosition->size()){
						positionToBeTested.push_back((theNodePosition->at(endArcNum+1)));
			}

			distanceToArc = (*distanceToPath)[endArc];
			if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
				patReal v = (*theNodePosition)[endArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);

				if(v > upperBound){
					positionToBeTested.push_back(v);
				}
			}

			for(patULong i=endArcNum+1; i < arcList->size();++i){

				positionToBeTested.push_back((theNodePosition->at(i+1)));
				map<char*, patReal> distanceToArc = (*distanceToPath)[thePath->getArc(i)];

				if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
					patReal v =  (*theNodePosition)[i] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
					positionToBeTested.push_back(v);
				}
			}



		}
		else{
			positionToBeTested.push_back(estimatePositionInBound);

		}

		//calculate location probability, take the location with the highest probability
		patReal maxLiklihood = 0;
		for(patULong i=0;i<positionToBeTested.size();++i){
		//	DEBUG_MESSAGE("OK94");
			patReal currProba = locationPDF(positionToBeTested[i],
								 currGpsNumber,
								 distanceToPath,
								 pointProbaDenumerator,
								 theNodePosition,
								 alpha);

			if(currProba > maxLiklihood){
				estimatePositionOnPath = positionToBeTested[i];
				maxLiklihood = currProba;
			}
		}
	}
	else{//if a peak point in boundary, take it as estimated position
		estimatePositionOnPath = estimatePositionInBound;
	}

	return estimatePositionOnPath;
}


