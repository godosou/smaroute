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
double patPathDDR::lowerBound = 0;
double patPathDDR::upperBound = 0;
double patPathDDR::lastEstimate = 0;
unsigned long patPathDDR::timeDiff = 0;
double patPathDDR::x0 = 0.0;
double patPathDDR::x1 = 0.0;
double patPathDDR::x2 = 0.0;
double patPathDDR::x3 = 0.0;

 ostream& operator<<(ostream& str, const patPathDDR& x) {
	str<<x.pathValue;
	return str;
 }

typedef double  (patPathDDR::*typefn)(double ,
						unsigned long ,
						map<patArc*, map<char*, double> >* ,
						double ,
						vector<double>* ,
						double );
/*
patPathDDR::getPathDDR(const patPathJ* thePath,const vector<patGpsPoint>* theGpsSequence){
	switch(algoName){
		case "raw": rawAlgorithm(thePath,theGpsSequence);
		case "timeSpace": timeSpaceAlgorithm(thePath,theGpsSequence);
	}
}
*/

double patPathDDR::getPathValue(){
	return pathValue;
}

vector<double> patPathDDR::getArcValues(){
	return arcValues;
}

double patPathDDR::rawAlgorithm( ){
	pathValue = 1;
	arcValues.assign(theGpsSequence->size(),0);

	for (int i=0;i<theGpsSequence->size();++i){
		
		for(map<patArc*,double>::iterator aIter = (*theGpsSequence)[i].getLinkDDR()->begin();
						aIter != (*theGpsSequence)[i].getLinkDDR()->end();
						++aIter){
			if(thePath->isLinkInPath(aIter->first)==true){
				arcValues[i] += aIter->second;
			}
		}
		pathValue*=arcValues[i];
	}
	return pathValue;
}

double patPathDDR::rawAlgorithm_average( ){
	
	double ddrSum;
	for(vector<double>::iterator ddrIter = arcValues.begin();
			ddrIter!=arcValues.end();
			++ddrIter){
		ddrSum+=*ddrIter;
	}
	
	return ddrSum/thePath->computeLength();
	
}
/**
*Following section is for time space algorithm
*/
void patPathDDR::timeSpaceAlgorithm( patNetwork* theNetwork){
	DEBUG_MESSAGE("PathLength:"<<thePath->getLenth() ) ;
	vector<double> nodePosition;	 
	pathValue = 1;
		arcValues.assign(theGpsSequence->size(),0);
	calNodePosition(&nodePosition);
	
	//vector<patReal> pointProbas;
	double lastEstimate = 0.0;
	double currEstimate = 0.0;

	vector<map<patArc*, map<char*, double> > > theDistance;
	theDistance.resize(theGpsSequence->size());
	calDistance(&theDistance,theNetwork);
	
	//DEBUG_MESSAGE("OK4" ) ;
	double alpha = 0 ;
	
	for(unsigned long i = 0; i< theGpsSequence->size();++i){
		typefn pdf= &patPathDDR::pointPDFDenumerator;
		DEBUG_MESSAGE("gps:" <<i) ;

	if( i == 0){
		lowerBound = 0.0;
		upperBound = thePath->getLenth();
	}
	else{
		lowerBound = lastEstimate;
		double speed = (theGpsSequence->at(i).getSpeed()>theGpsSequence->at(i-1).getSpeed())?
			theGpsSequence->at(i).getSpeed():theGpsSequence->at(i-1).getSpeed();
			speed*=1.20/3.6; 
		timeDiff = (theGpsSequence->at(i).getTimeStamp()-theGpsSequence->at(i-1).getTimeStamp());
		double far = speed * timeDiff;
		upperBound = lastEstimate + far;
		DEBUG_MESSAGE("bound:"<<lowerBound<<"-"<<upperBound);
			double speed0 = theGpsSequence->at(i-1).getSpeed()/3.6;
	double speed1 = speed ;
	
	if(speed0>speed1){
		double speedt = speed1;
		speed1 = speed0;
		speed0 = speedt;
	}
	 x0 = lowerBound ;
	 double speedf = 10;
	 x3 = x0+timeDiff*speedf;
	 x1 = lastEstimate + timeDiff*speed0;
	 x2 = lastEstimate + timeDiff*speed1;
	
	}
	
		double pointProbaDenumerator = integration(pdf,
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
							thePath->getLenth(),
							 i,
							&theDistance[i],
							0.0,
							&nodePosition, 
							alpha);
		DEBUG_MESSAGE("pointProbaDenumerator:"<<pointProbaDenumerator ) ;
		double pointProba = calPointProba( i, 
							  
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
	
	double paN = 1/fabs(log(pathValue));
DEBUG_MESSAGE("path proba:"<<pathValue<<","<<paN ) ;
	//pathValue = paN;
}

double patPathDDR::locationPDF(double position,
								unsigned long currGpsNumber,
								map<patArc*, map<char*, double> >* distanceToPath,
								double pointProbaDenumerator,
								vector<double>* theNodePosition,
								double alpha){
	return distancePDF(position, currGpsNumber,distanceToPath,theNodePosition)*
		priorPDF(position, currGpsNumber,alpha)/pointProbaDenumerator;
}
double patPathDDR::pointPDF(double position,
						unsigned long currGpsNumber,
						map<patArc*, map<char*, double> >* distanceToPath,
						double pointProbaDenumerator,
						vector<double>* theNodePosition,
						double alpha){
						
	double rtnValue =  locationPDF( position, currGpsNumber,  distanceToPath,pointProbaDenumerator, theNodePosition,
						 alpha) * distancePDF( position, currGpsNumber,distanceToPath,theNodePosition);
	if(isnan(rtnValue)){
	
		rtnValue =0.0;	
	}
	
	return rtnValue;
}
double patPathDDR::calPointProba(	unsigned long& currGpsNumber,
						map<patArc*, map<char*, double> >* distanceToPath,
						double pointProbaDenumerator,
						vector<double>* theNodePosition,
						double alpha){
			typefn pdf= &patPathDDR::pointPDF;

	return integration(pdf,0.0,lowerBound, currGpsNumber,distanceToPath,pointProbaDenumerator,theNodePosition, alpha)+
		integration(pdf,lowerBound,upperBound, currGpsNumber,distanceToPath,pointProbaDenumerator,theNodePosition, alpha)+
		integration(pdf,upperBound,thePath->getLenth(), currGpsNumber,distanceToPath,pointProbaDenumerator,theNodePosition, alpha)
	;
	
}

double patPathDDR::integration(double (patPathDDR::*fp	)(double ,
						unsigned long ,
						map<patArc*, map<char*, double> >* ,
						double ,
						vector<double>* ,
						double ),
			 double a, 
			 double b, 
			 unsigned long currGpsNumber,
			 map<patArc*, map<char*, double> >* distanceToPath,
			 double pointProbaDenumerator,
			vector<double>* theNodePosition,
			 double alpha
			 ){
	//from simpsonq
	//DEBUG_MESSAGE("length:"<<b-a) ;
	double epsilon = 0.000001;
	double result;
    int i;
    int n;
    double h;
    double s;
    double s1;
    double s2;
    double s3;
    double x;
	 
	 double c;
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


double patPathDDR::pointPDFNumerator(double position, 
								unsigned long currGpsNumber, 
								map<patArc*, map<char*, double> >* distanceToPath,
								vector<double>* theNodePosition,
								double alpha){//integration f^2 * g
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

double	 patPathDDR::pointPDFDenumerator(double position, 
								unsigned long currGpsNumber, 
								map<patArc*, map<char*, double> >* distanceToPath,
								double nothing,
								vector<double>* theNodePosition,
								double alpha){//integration f * g

	/*DEBUG_MESSAGE("DISTANCE proba:"<<distancePDF(position,
				currGpsNumber,
				distanceToPath,
				theNodePosition) <<"prior:"
				<<priorPDF(position,
				lastEstimate,
				currGpsNumber,
				alpha) );
	*/
	double distanceProba = 	
	distancePDF(position,
				currGpsNumber,
				distanceToPath,
				theNodePosition);
	double priorProba = 
	priorPDF(position,
				 
				currGpsNumber,
				alpha);
				
	double rtnValue = distanceProba * priorProba;
	if(isnan(rtnValue)){
		DEBUG_MESSAGE("distanceProba:"<<distanceProba<<",priorProba"<<priorProba);
		rtnValue = 0.0;
	}
	return rtnValue;
}


double patPathDDR::distancePDF(double position,
						unsigned long currGpsNumber,
						map<patArc*, map<char*, double> >* distanceToPath,
						vector<double>* theNodePosition){

	unsigned long arcNumber = getArcNumberFromPosition(position,theNodePosition);
	//DEBUG_MESSAGE("OK" <<position<<","<<arcNumber) ;	
	patArc* theArc = thePath->getArc(arcNumber);

	double positionOnArc = position - (*theNodePosition)[arcNumber];
	map<char*, double> distanceToArc  = (*distanceToPath)[theArc];
		        				//DEBUG_MESSAGE("OK8" ) ;	
			double rtnValue;	        				
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
		double startLength = sqrt( distanceToArc["up"]* distanceToArc["up"]-distanceToArc["ver"]*distanceToArc["ver"]);
		
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


double patPathDDR::priorPDF(double position, 
						unsigned long currGpsNumber,
						double alpha){
	if (alpha == 0.0){
		return uniform();
	}
	double mg = manipG(position, 
					currGpsNumber);
	double g = alpha * mg+
			(1-alpha) * uniform();
	//DEBUG_MESSAGE("g"<<g);
	if(isnan(g)){
	DEBUG_MESSAGE("alpha"<<alpha);
	}
	return g;
}

//uniform
double patPathDDR::manipG(double position, 
					unsigned long currGpsNumber){

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
double patPathDDR::getAlpha(double position,
					unsigned long currGpsNumber,
					map<patArc*, map<char*, double> >* distanceToPath,
					double pointProbaDenumerator,
					vector<double>* theNodePosition,
					double alpha){
	
	double locProba = locationPDF( position,
								 currGpsNumber,
								distanceToPath,
								 pointProbaDenumerator,
								theNodePosition,
								alpha);
	locProba = fabs(log(locProba));
	locProba = 1/locProba;							

	return 0.8 + 0.2*locProba;
}


void patPathDDR::calDistance(vector<map<patArc*, map<char*, double> > >* theDistance,patNetwork* theNetwork){
	
	list<patArc*>* arcList = thePath->getArcList();
	for(list<patArc*>::iterator arcIter = arcList->begin();
						arcIter != arcList->end();
						++arcIter){
		
		patGeoCoordinates* upNodeGeoCoord = &(theNetwork->getNodeFromUserId((*arcIter)->upNodeId)->geoCoord);
		patGeoCoordinates* downNodeGeoCoord = &(theNetwork->getNodeFromUserId((*arcIter)->downNodeId)->geoCoord);
		
		for(int i = 0; i< theGpsSequence->size();++i){
						//DEBUG_MESSAGE("OK" ) ;
			theDistance->at(i)[*arcIter] = theGpsSequence->at(i).distanceTo(upNodeGeoCoord,downNodeGeoCoord);
		}
	
	}

}

void patPathDDR::calNodePosition(vector<double>* theNodePosition){
	
	theNodePosition->push_back(0.0);
	list<patArc*>* arcList = thePath->getArcList();
	unsigned long length_temp = 0;
	for(list<patArc*>::iterator arcIter = arcList->begin();
						arcIter != arcList->end();
						++arcIter){

		length_temp+=(*arcIter)->getLength();
		theNodePosition->push_back(length_temp);
		//DEBUG_MESSAGE("OK"<<(*arcIter)->userId) ;
	}
}


unsigned long patPathDDR::getArcNumberFromPosition(double position,
						vector<double>* theNodePosition){

	for(int i=0; i< theNodePosition->size();++i){
		if ((*theNodePosition)[i] > position){
			return (i-1);
		}
	}
	return theNodePosition->size()-2;
}



double patPathDDR::uniform(){
	return 1.0/thePath->getLenth();
}



double patPathDDR::estimatePosition(unsigned long currGpsNumber,
								map<patArc*, map<char*, double> >* distanceToPath,
								double pointProbaDenumerator,
								vector<double>* theNodePosition,
								double alpha){

	list<patArc*>* arcList = thePath->getArcList();//arc list of the path
	//DEBUG_MESSAGE("OK90");
	//get boundary of current location
	//base on estimated location of last gps point, time difference between two gps points and free flow 

		//DEBUG_MESSAGE("OK91");
	//get max likly distance from a gps point to a location on path
	//i.e. the peak point of err distance distribution
	double maxLikliDistance = theGpsSequence->at(currGpsNumber).getHorizonAccuracy()/sqrt(2.0);
	
	double estimatePositionOnPath,estimatePositionInBound,estimatePositionNotInBound;
	
	//search through all arcs in the path, to get the locations with maxLikliDistance to GPS point
	
	double leastDistanceInBound = patMaxReal;//There are maybe several peak points. Among those, choose the one with least distance from gps to arc.
	double leastDistanceNotInBound = patMaxReal;
	unsigned long i = 0;
	for(list<patArc*>::iterator arcIter = arcList->begin();
						arcIter != arcList->end();
						++arcIter){
		

		map<char*, double> distanceToArc = (*distanceToPath)[*arcIter];
		
	//DEBUG_MESSAGE("OK92");
		double lowerDistance;
		double upperDistance;
		
		//if projection of gps point to arc is on arc
		if (distanceToArc["position"]!=-1  && distanceToArc["position"]!=1){
			
			//if the max likly location is between upnode and projection
			if(maxLikliDistance <= distanceToArc["up"] && maxLikliDistance>=distanceToArc["ver"]){
			
				//position of the location
				double estPosition = (*theNodePosition)[i] + 
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
				double estPosition = (*theNodePosition)[i] - 
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
			
				double estPosition =  (*theNodePosition)[i] + 
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
		unsigned long startArcNum = getArcNumberFromPosition(lowerBound,theNodePosition);
		unsigned long endArcNum = getArcNumberFromPosition(upperBound,theNodePosition);
		
		
		vector<double> positionToBeTested;//potentail positions
			//DEBUG_MESSAGE("OK93");
		//insert the boundaries
		positionToBeTested.push_back(lowerBound);
		positionToBeTested.push_back(upperBound);

		//if the boundaries are on the same arc
		if(startArcNum == endArcNum){
			patArc* onlyArc = thePath->getArc(startArcNum);
			map<char*, double> distanceToArc = (*distanceToPath)[onlyArc];
			
			//if the projection is on arc,take the projection position
			if(distanceToArc["position"]!=1 && distanceToArc["position"]!=-1){
				double v = (*theNodePosition)[startArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
				
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
			map<char*, double> distanceToArc = (*distanceToPath)[startArc];
			if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
				double v = (*theNodePosition)[startArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
				
				if(v > lowerBound && v < (*theNodePosition)[startArcNum+1]){
					positionToBeTested.push_back(v);
				}
			}

			 distanceToArc = (*distanceToPath)[endArc];
			if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
				double v = (*theNodePosition)[endArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
				
				if( v > (*theNodePosition)[endArcNum] && v < upperBound){
					positionToBeTested.push_back(v);
				}
			}


			for(unsigned long i=startArcNum+1; i < endArcNum && i+1<theNodePosition->size();++i){
				
			//DEBUG_MESSAGE("OK940"<<i+1<<","<<theNodePosition->size());
				positionToBeTested.push_back((theNodePosition->at(i+1)));
			//DEBUG_MESSAGE("OK941");
				map<char*, double> distanceToArc = (*distanceToPath)[thePath->getArc(i)];
				if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
					
					double v =(*theNodePosition)[i] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
					positionToBeTested.push_back(v);
				}
			}
		}
		
		//if peak points outside of boundary are also not found,
		//also search for potential positions
		if(leastDistanceNotInBound == patMaxReal){
					patArc* startArc = thePath->getArc(startArcNum);
			patArc* endArc = thePath->getArc(endArcNum);
			for(unsigned long i=0; i < startArcNum;++i){
				positionToBeTested.push_back((theNodePosition->at(i)));
				map<char*, double> distanceToArc = (*distanceToPath)[thePath->getArc(i)];
				
				if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
					double v = (*theNodePosition)[i] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
					positionToBeTested.push_back(v);
				}
			}
			
			positionToBeTested.push_back((theNodePosition->at(startArcNum)));
			map<char*, double> distanceToArc = (*distanceToPath)[startArc];
			if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
				double v = (*theNodePosition)[startArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
				
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
				double v = (*theNodePosition)[endArcNum] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
				
				if(v > upperBound){
					positionToBeTested.push_back(v);
				}
			}

			for(unsigned long i=endArcNum+1; i < arcList->size();++i){
			
				positionToBeTested.push_back((theNodePosition->at(i+1)));
				map<char*, double> distanceToArc = (*distanceToPath)[thePath->getArc(i)];

				if(distanceToArc["position"] != 1 && distanceToArc["position"]!=-1){
					double v =  (*theNodePosition)[i] + sqrt(distanceToArc["up"] * distanceToArc["up"] - distanceToArc["ver"] * distanceToArc["ver"]);
					positionToBeTested.push_back(v);
				}
			}



		}
		else{
			positionToBeTested.push_back(estimatePositionInBound);

		}
		
		//calculate location probability, take the location with the highest probability
		double maxLiklihood = 0;
		for(unsigned long i=0;i<positionToBeTested.size();++i){
		//	DEBUG_MESSAGE("OK94");
			double currProba = locationPDF(positionToBeTested[i],
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


