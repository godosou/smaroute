
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

		void setMapBounds(double minLat, 
			      double maxLat, 
			      double minLon, 
			      double maxLon) ;
				  
		patTraveler* addTraveler(patTraveler theTraveler);
		patTraveler* addTraveler(const unsigned long& theTravelerId);
		
		patTraveler* findTraveler(const unsigned long& theTravelerId);
		void assignPathIds();
		
		patOdJ*  addOd( patOdJ theOd);
		patOdJ*  findOd( patOdJ theOd);
		set<patOdJ>* getAllOds();
		
		set<patTraveler>* getAllTravelers();
		patPathJ* findPath( patPathJ& thePath);
		
list<unsigned long>  getTripIds();
patString getTripIdsStr();
		unsigned long getNumberOfObservations() ;
		patString genDescription();
		void writeKML(patString fileName);
	protected:
		patString name;
	  	double minLatitude ;
 	 	double maxLatitude ;
 	 	double minLongitude ;
 		double maxLongitude ;
		set<patTraveler> travelerSet;
		set<patOdJ> odSet;
};
#endif
