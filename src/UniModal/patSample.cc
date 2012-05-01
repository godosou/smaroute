#include "patWriteKML.h"
#include <fstream>
#include <sstream>
#include "patSample.h"
#include "patTripParser.h"
#include "patString.h"
#include "patType.h"
#include "patError.h"
#include "patReadSample.h"
#include <set>
#include "patDisplay.h"
#include "patTraveler.h"
#include "patPathJ.h"
#include "patOdJ.h"
#include "patGpsPoint.h"
#include "patObservation.h"
#include <map>
/**
*Sample utilities
*/

patSample::patSample()
{

}

void patSample::setMapBounds(double minLat, 
			      double maxLat, 
			      double minLon, 
			      double maxLon) {
}

/**
*Traveler utilities
*/

patTraveler* patSample::addTraveler(patTraveler theTraveler){
	pair<set<patTraveler>::iterator,bool> insertResult = travelerSet.insert(theTraveler);
	return const_cast<patTraveler*>(&(*(insertResult.first)));
}


patTraveler* patSample::addTraveler(const unsigned long& theTravelerId){
	patTraveler* travelerFound = findTraveler(theTravelerId);
	if(travelerFound!=NULL){
		return travelerFound;
	}
	else{
		return addTraveler(patTraveler(theTravelerId));
	}
}

patTraveler* patSample::findTraveler(const unsigned long& theTravelerId){
	patTraveler travelerToFind(theTravelerId);
	set<patTraveler>::iterator travelerFound = travelerSet.find(travelerToFind);
	if(travelerFound == travelerSet.end()){
		return NULL;
	}
	return const_cast<patTraveler*>(&(*travelerFound));
}

set<patTraveler>* patSample::getAllTravelers(){
	return &travelerSet;
}

/**
*Od utilities
*/
patOdJ* patSample::addOd( patOdJ theOd){
	pair<set<patOdJ>::iterator,bool> insertResult = odSet.insert(theOd);
	return const_cast<patOdJ*>(&(*(insertResult.first)));
}
patOdJ* patSample::findOd( patOdJ theOd){
	set<patOdJ>::iterator odFound = odSet.find(theOd);
		if(odFound == odSet.end()){
		return NULL;
	}
	
	return const_cast<patOdJ*>(&(*odFound));
}

set<patOdJ>* patSample::getAllOds(){
	return &odSet;
}

/**
*Path utilities
*/

patPathJ* patSample::findPath(patPathJ& thePath){
	
	patOdJ* pathOd = thePath.getOd();
	
	patOdJ* odFound = findOd(*pathOd);
	
	if(odFound == NULL){
		return NULL;
	}
	
	patPathJ* pathFound = odFound->findPath(thePath);
	
	if(pathFound == NULL){
		return NULL;
	}
	
	return pathFound;

}

/**
*Observation utilities
*/

unsigned long patSample::getNumberOfObservations()  {
	unsigned long nbrOfObservations = 0;
	for(set<patTraveler>::iterator travelerIter = travelerSet.begin();
							travelerIter !=travelerSet.end();
							++travelerIter){
		nbrOfObservations += const_cast<patTraveler*>(&(*travelerIter))->getNumberOfObservations();
	}
	return nbrOfObservations;
}

void patSample::assignPathIds(){
	unsigned long i =1;
	for(set<patOdJ>::iterator odIter = odSet.begin();
				odIter!=odSet.end();
				++odIter){
		patError* err;
		set<patPathJ>* pathSet = const_cast<patOdJ*>(&*(odIter))->getAllPaths();
		for(set<patPathJ>::iterator pathIter = pathSet->begin();
					pathIter!=pathSet->end();
					++pathIter){
			const_cast<patPathJ*>(&(*pathIter))->assignId(i++);
		}
		
	}

}

void patSample::writeKML(patString fileName){

 ofstream kml(fileName.c_str()) ;
	 kml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl ;
  	kml << "      <kml xmlns=\"http://earth.google.com/kml/2.0\">" << endl ;
  kml << "      <Document>" << endl ;

  patString theName(fileName) ;
  replaceAll(&theName,patString("&"),patString("and")) ;
  
  kml << "            <name>" << theName <<"</name>" << endl ;
  kml << "            <description>File created by bioroute</description>" << endl ;
   	kml<<"<Folder>";
   	kml << "            <name>paths</name>" << endl ;
	for(set<patOdJ>::iterator odIter = odSet.begin();
			odIter!=odSet.end();
			++odIter){
			
		kml<< "<Folder>";
	 kml<<"<name>"<<endl;
	 kml<<"OD:"<<*odIter<<endl;
	 kml<<"</name>"<<endl;
	 patWriteKML wk;
	 patError* err;
	 set<patPathJ>* pathSet = const_cast<patOdJ*>(&(*odIter))->getAllPaths();
//	 DEBUG_MESSAGE("path number"<<pathSet->size());
	 for (set<patPathJ>::iterator pathIter = pathSet->begin();
				pathIter!=pathSet->end();
				++pathIter){
		patPathJ* path = const_cast<patPathJ*>(&(*pathIter));
		patString desc = path->genDescription();
		
		//DEBUG_MESSAGE("OK");
		kml<<wk.writePath(path,desc);
				
	}
	
	kml<<"</Folder>"<<endl;
	}

	 kml << " " << endl ;
	kml<<"</Folder>"<<endl;
	 kml << " " << endl ;
  kml << "      </Document>" << endl ;
  kml << "      </kml>" << endl ;


  kml.close() ;
	}
	
list<unsigned long> patSample::getTripIds()  {
list<unsigned long> tripIds;
	for(set<patTraveler>::iterator travelerIter = travelerSet.begin();
							travelerIter !=travelerSet.end();
							++travelerIter){
		set<patObservation>* ao = const_cast<patTraveler*>(&(*travelerIter))->getAllObservations();
		
		for(set<patObservation>::iterator obIter = ao->begin();
							obIter !=ao->end();
							++obIter){
				tripIds.push_back( const_cast<patObservation*>(&(*obIter))->getId());			
			}
	}
	return tripIds;
}

patString patSample::getTripIdsStr(){
	stringstream stream;
	list<unsigned long> tripIds = getTripIds();
	list<unsigned long>::iterator iIter = tripIds.begin();
	stream<<*(iIter++);
	for(;
		iIter!=tripIds.end();
		++iIter){
		stream<<"+"<<*(iIter);
	}
	return stream.str();
}