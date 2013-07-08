//----------------------------------------------------------------
// File: patReadSampleFromCsv.cc
// Author:
// Creation:
//----------------------------------------------------------------
#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
#include<string>
#include "patReadSampleFromCsv.h"
#include "patDisplay.h"
#include "patSample.h"
#include "patErrMiscError.h"
#include "patString.h"
#include "patType.h"
#include "patGpsPoint.h"
#include "patTrip.h"
#include "patArc.h"
#include "patNode.h"
#include "patNetwork.h"
//csv file example
//file name: 5451.csv
//5451
//tripId,unix timestamp,latitude,longitude,speed,horizontal_accuracy,heading
//
patReadSampleFromCsv::patReadSampleFromCsv(patString fName,patNetwork* network):
	fileName(fName)
{
	theNetwork = network;
} 

void patReadSampleFromCsv::buildAdjacencyLists( vector< list <  pair<patArc*,patULong> > >* adjacencyLists, patError*& err) {



  for (patULong n = 0 ; n < theNetwork->internalNodes.size() ; ++n) {

    list <  pair<patArc*,patULong> >  currentList ;
    patNode* theNode = theNetwork->internalNodes[n] ;
    for (set<patULong>::iterator succ = theNode->userSuccessors.begin() ;
	 succ != theNode->userSuccessors.end() ;
	 ++succ) {
      patArc* theArc = theNetwork->getArcFromNodesUserId(theNode->userId,*succ) ;
      if (theArc == NULL) {
	stringstream str ;
	str << "Arc " << theNode->userId << "->" << *succ << " does not exist" ;
	err = new patErrMiscError(str.str()) ;
	WARNING(err->describe()) ;
	return ;
      }
      patULong internalIdSucc = theNetwork->getInternalNodeIdFromUserId(*succ) ;
      currentList.push_back(pair<patArc*,patULong>(theArc,
						   internalIdSucc)) ;
    }

    adjacencyLists->push_back(currentList) ;
  }
}	

patBoolean patReadSampleFromCsv::readFile( patError*& err) {
	ifstream inFile;
	
	
	inFile.open(fileName.c_str(),ios::in);
	  if (!inFile) {
	    stringstream str ;
	    str << "Error while parsing " << fileName ;
	    err = new patErrMiscError(str.str()) ;
	    WARNING(err->describe());
	    return patFALSE ;
	  }
	  DEBUG_MESSAGE("Read file: 5479.csv" ) ;
	patULong currTripId=1,lastTripId=2000000;//current trip id
	patULong tripNumber=0,pointNumber=0;

	patULong currTime,currEndTime;
	patReal currLat, currLon,currSpeed,currHeading,currAccuracyH,currAccuracyV,currAccuracyS,currAccuracyHD;

	string line;
	getline (inFile, line);
	patULong userId=atol(line.c_str() );
	DEBUG_MESSAGE("UserID: "<<userId ) ;
	vector< list <  pair<patArc*,patULong> > > adjacencyLists;
	buildAdjacencyLists(&adjacencyLists, err);
	patTrip currTrip(theNetwork,&adjacencyLists);

	while (getline (inFile, line)){

		pointNumber++;
		istringstream linestream(line);

		string item;


		getline (linestream,item,',');
		currTripId=atol(item.c_str());

		getline (linestream,item,',');//get  time
		currTime=atof(item.c_str() );

		if(tripNumber==0){

			tripNumber++;
			DEBUG_MESSAGE("trip Number: " <<tripNumber) ;
			currTrip.newTrip(userId,currTripId,currTime);//create new trip
		}
		else if(currTripId!=lastTripId){//getTripId
			currTrip.endofTrip(currEndTime);//end of a trip, set end time

			tripNumber++;
			DEBUG_MESSAGE("trip Number: " <<tripNumber) ;

			currTrip.newTrip(userId,currTripId,currTime);//create new trip
		}


		getline (linestream,item,',');//get longitude
		currLon = atof(item.c_str() );
		getline (linestream,item,',');//get latitude
		currLat = atof(item.c_str() );
		getline (linestream,item,',');//altitude
		getline (linestream,item,',');//get speed
		currSpeed = atof(item.c_str() );

		getline (linestream,item,',');//get horizontal accuracy
		currHeading = atof(item.c_str() );
		getline (linestream,item,',');//get heading
		currAccuracyH= atof(item.c_str() );
		getline (linestream,item,',');//get heading
		currAccuracyV= atof(item.c_str() );
		getline (linestream,item,',');//get heading
		currAccuracyS= atof(item.c_str() );
		getline (linestream,item,',');//get heading
		currAccuracyHD= atof(item.c_str() );
		 
		patGpsPoint currPoint(userId,currTime,currLat,currLon,currSpeed,currAccuracyH, 
			currHeading,currAccuracyV,currAccuracyS,currAccuracyHD);
		currTrip.addPoint(currPoint);
		currEndTime=currTime;
		lastTripId=currTripId;

	}
	DEBUG_MESSAGE("end time: " << currEndTime) ;
	currTrip.endofTrip(currEndTime);//end of a trip, set end time
	currTrip.writeToKML("output.kml",err);
	DEBUG_MESSAGE("Travler: "<<userId<<",Trips:"<<tripNumber<<",GPS Points:" << pointNumber) ;

  return patTRUE ;
	
}



