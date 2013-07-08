//----------------------------------------------------------------
// File: patReadSample.cc
// Author:
// Creation:
//----------------------------------------------------------------
#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
#include<string>
#include "patReadSample.h"
#include "patDisplay.h"
#include "patSample.h"
#include "patErrMiscError.h"
#include "patString.h"
#include "patType.h"
#include "patGpsPoint.h"
#include "patTripParser.h"
#include "patArc.h"
#include "patNode.h"
#include "patNetwork.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "patNBParameters.h"

//csv file example
//file name: 5451.csv
//5451
//tripId,unix timestamp,latitude,longitude,speed,horizontal_accuracy,heading
//
patReadSample::patReadSample(patSample* sample, 
					 patNetwork* network,
					patError*& err)
{
	theNetwork = network;
	DEBUG_MESSAGE("build");
	buildAdjacencyLists(err);
	theSample = sample;
	
	//calArcPriority();
} 

void patReadSample::calArcPriority(){
	for(map<patULong,patArc>::iterator arcIter = theNetwork->theArcs.begin();
			arcIter!=theNetwork->theArcs.end();
			++arcIter){
		const_cast<patArc*>(&arcIter->second)->calPriority();
	}

}
void patReadSample::buildAdjacencyLists(patError*& err) {

	adjacencyLists.erase(adjacencyLists.begin(),adjacencyLists.end()) ;
	DEBUG_MESSAGE("node number"<<theNetwork->internalNodes.size());
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
	  if(theArc->name!="M1" && theArc->attributes.priority>=4 && theArc->attributes.priority<=14 ){
      patULong internalIdSucc = theNetwork->getInternalNodeIdFromUserId(*succ) ;
      currentList.push_back(pair<patArc*,patULong>(theArc,
						   internalIdSucc)) ;
						   }
    }

    adjacencyLists.push_back(currentList) ;
  }
  DEBUG_MESSAGE("adjacency list is build"<< adjacencyLists.size());
return ;
  }	
  
  vector< list <  pair<patArc*,patULong> > > patReadSample::getAdjacencyLists(){
 return  adjacencyLists;
  }
void patReadSample::readGpsTrack(vector<patGpsPoint>* gpsSequence,patULong theId,list<patULong> realPath){
	patTraveler* theTraveler = theSample->addTraveler(patULong(0));
	patULong time = gpsSequence->front().getTimeStamp();
	patTripParser theTripParser(theTraveler,theId,time);
	theTripParser.setEnviroment(theSample,theNetwork);
	theTripParser.inputGpsTrack(gpsSequence);
	theTripParser.addRealPath(realPath);
	theTripParser.endOfTrip(&adjacencyLists,gpsSequence->back().getTimeStamp());
}
vector<patString> patReadSample::listDirectory(patError*& err){

	const char* dirName = patNBParameters::the()->dataDirectory.c_str();
	DIR *dip;
	struct dirent *dit;
	
	DEBUG_MESSAGE("Try to open directory "<<dirName);
	if((dip = opendir(dirName)) == NULL){
		stringstream str ;
		str << "Directory "<<dirName<<" doesn't exist or no permission to read." ;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		return vector<patString>();
	}
	DEBUG_MESSAGE("Direcotry "<<dirName<<" is now open");
	vector<patString> dirContent;
	unsigned char isFile =0x8;
	patString esp("~");
	patString don("#");
	patString csvext("csv");

	while((dit = readdir(dip))!=NULL){
		if(dit->d_type == isFile){
		patString fileName(dit->d_name);
		if(fileName.find(csvext)!=string::npos && fileName.find(esp)==string::npos && fileName.find(don)==string::npos){
			DEBUG_MESSAGE("Found file:"<<fileName);
			dirContent.push_back(fileName);
		}
		
		}
	}
	
	
	closedir(dip);
	/*
	if(close(dip)){
		stringstream str ;
		str << "Error in closing directory." ;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		return vector<patString>();
	}
	*/
	return dirContent;
}

void patReadSample::readDataDirectory(patError*& err){
	vector<patString> dataFiles = listDirectory(err);
	/*
	for(int i=0;i<dataFiles.size();++i){
		patString filename = "data/" +dataFiles[i];
		DEBUG_MESSAGE("Read file:" <<dataFiles[i]) ; 
		readFile(filename,err);
	}
	*/
		DEBUG_MESSAGE("files:"<<dataFiles.size());
	 
	for(patULong i = 0;i< dataFiles.size();++i){
		patString fileName = patNBParameters::the()->dataDirectory+"/";
		fileName +=dataFiles[i];
		//DEBUG_MESSAGE("Read file:" <<fileName) ;
		readFile(fileName,err);
	}
}

void patReadSample::readFile( patString& fileName,patError*& err) {
	ifstream inFile;
	
	
	inFile.open(fileName.c_str(),ios::in);
	  if (!inFile) {
	    stringstream str ;
	    str << "Error while parsing " << fileName ;
	    err = new patErrMiscError(str.str()) ;
	    WARNING(err->describe());
	    return  ;
	  }
	  DEBUG_MESSAGE("Read file:" <<fileName) ;
	patULong currTripId=1,lastTripId=patBadId;//current trip id
	patULong tripNumber=0,pointNumber=0;

	patULong currTime,currEndTime;
	patReal currLat, currLon,currSpeed,currHeading,currAccuracyH,currAccuracyV,currAccuracyS,currAccuracyHD;

	string line;
	getline (inFile, line);
	patULong userId=atol(line.c_str() );
	DEBUG_MESSAGE("UserID: "<<userId ) ;
	patTraveler* theTraveler = theSample->addTraveler(userId);
	patTripParser* theTripParser = new patTripParser;

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
			delete theTripParser;
			theTripParser = new patTripParser(theTraveler,currTripId,currTime);
			theTripParser->setEnviroment(theSample,theNetwork);
		}
		else if(currTripId!=lastTripId){//getTripId
			theTripParser->endOfTrip(&adjacencyLists,currEndTime);//end of a trip, set end time
			tripNumber++;
			DEBUG_MESSAGE("trip Number: " <<tripNumber) ;
			delete theTripParser;
			theTripParser = new patTripParser(theTraveler,currTripId,currTime);
			theTripParser->setEnviroment(theSample,theNetwork);

		}


		getline (linestream,item,',');//get longitude
		currLon = atof(item.c_str() );
		getline (linestream,item,',');//get latitude
		currLat = atof(item.c_str() );
		getline (linestream,item,',');//altitude
		getline (linestream,item,',');//get speed
		currSpeed = atof(item.c_str() );

		
		getline (linestream,item,',');//get 
		currHeading = atof(item.c_str() );
		getline (linestream,item,',');//get 
		currAccuracyH= atof(item.c_str() );
		getline (linestream,item,',');//get 
		currAccuracyV= atof(item.c_str() );
		getline (linestream,item,',');//get 
		currAccuracyS= atof(item.c_str() );
		getline (linestream,item,',');//get 
		currAccuracyHD= atof(item.c_str() );
//		DEBUG_MESSAGE("OK"<<currHeading<<","<<currAccuracyH<<","<<currAccuracyV<<","<<currAccuracyS<<","<<currAccuracyHD); 
		patGpsPoint currPoint(userId,currTime,currLat,currLon,currSpeed,currAccuracyH, 
			currHeading,currAccuracyV,currAccuracyS,currAccuracyHD,NULL);
		
		theTripParser->addPoint(currPoint);
		currEndTime=currTime;
		lastTripId=currTripId;

	}
	DEBUG_MESSAGE("end time: " << currEndTime) ;
	theTripParser->endOfTrip(&adjacencyLists,currEndTime);//end of a trip, set end time
	delete theTripParser;
	theTripParser = NULL;
	//theTripParser.writeToKML("output.kml");
	DEBUG_MESSAGE("Travler: "<<userId<<",Trips:"<<tripNumber<<",GPS Points:" << pointNumber) ;
	
}


void patReadSample::finalizeReading(){

}


patSample* patReadSample::getSample(){
	return theSample;
}