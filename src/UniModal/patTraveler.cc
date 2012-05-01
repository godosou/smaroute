#include "patTraveler.h"
#include "patType.h"
#include "patConst.h"
#include "patObservation.h"
#include <map>
#include "patDisplay.h"
patTraveler::patTraveler(const unsigned long& theId):
				id(theId)
{
	
}

patTraveler::patTraveler(const unsigned long& theId,
				const patString& theName):
				id(theId),
				name(theName){
				
}

bool operator==(const patTraveler& aTraveler, const patTraveler& bTraveler) {
	
	if(aTraveler.id == bTraveler.id){
		return true;
	}
	else{
		return false;
	}
}
bool operator<(const patTraveler& aTraveler, const patTraveler& bTraveler) {
	
	if(aTraveler.id < bTraveler.id){
		return true;
	}
	else{
		return false;
	}
}
void patTraveler::setAttributes(const patString& key, 
					const double& value){
	attributes[key] = value;
	
}
	

double patTraveler::getAttributes(const patString& key){
	return attributes[key];
}

patObservation* patTraveler::addObservation(patObservation& theObservation){
	pair<set<patObservation>::iterator,bool> insertResult = observationSet.insert(theObservation);
	if(insertResult.second!=true){
		WARNING("fail to add the observation");
	}
	return const_cast<patObservation*>(&(*(insertResult.first)));
}

unsigned long patTraveler::getNumberOfObservations(){
	return observationSet.size();
}

unsigned long patTraveler::getId(){
	return id;
}

	set<patObservation>* patTraveler::getAllObservations(){
		return &observationSet;
	}