
#ifndef patSample_h
#define patSample_h
#include "patGpsPoint.h"
#include "patError.h"
#include "patString.h"
#include "patType.h"
#include <map>
class patTraveler;
class patOdJ;
class patPathJ;
class patObservation;
class patSample{
	public:
		
		/*
		**Constructor
		**@param theName name of the sample
		*/
		patSample() ;

		void setMapBounds(patReal minLat, 
			      patReal maxLat, 
			      patReal minLon, 
			      patReal maxLon) ;
				  
		patTraveler* addTraveler(patTraveler theTraveler);
		patTraveler* addTraveler(const patULong& theTravelerId);
		
		patTraveler* findTraveler(const patULong& theTravelerId);
		void assignPathIds();
		
		patOdJ*  addOd( patOdJ theOd);
		patOdJ*  findOd( patOdJ theOd);
		set<patOdJ>* getAllOds();
		
		set<patTraveler>* getAllTravelers();
		patPathJ* findPath( patPathJ& thePath);
		
list<patULong>  getTripIds();
patString getTripIdsStr();
		patULong getNumberOfObservations() ;
		patString genDescription();
		void writeKML(patString fileName);
	protected:
		patString name;
	  	patReal minLatitude ;
 	 	patReal maxLatitude ;
 	 	patReal minLongitude ;
 		patReal maxLongitude ;
		set<patTraveler> travelerSet;
		set<patOdJ> odSet;
};
#endif
