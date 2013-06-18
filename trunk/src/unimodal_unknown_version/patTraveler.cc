#include "patTraveler.h"
#include "patType.h"
#include "patConst.h"
#include "patObservation.h"
#include <map>
#include "patDisplay.h"
patTraveler::patTraveler(const patULong& theId):
				id(theId)
{
	
}

patTraveler::patTraveler(const patULong& theId,
				const patString& theName):
				id(theId),
				name(theName){
				
}

patBoolean operator==(const patTraveler& aTraveler, const patTraveler& bTraveler) {
	
	if(aTraveler.id == bTraveler.id){
		return patTRUE;
	}
	else{
		return patFALSE;
	}
}
patBoolean operator<(const patTraveler& aTraveler, const patTraveler& bTraveler) {
	
	if(aTraveler.id < bTraveler.id){
		return patTRUE;
	}
	else{
		return patFALSE;
	}
}
void patTraveler::setAttributes(const patString& key, 
					const patReal& value){
	attributes[key] = value;
	
}
	

patReal patTraveler::getAttributes(const patString& key){
	return attributes[key];
}

patObservation* patTraveler::addObservation(patObservation& theObservation){
	pair<set<patObservation>::iterator,bool> insertResult = observationSet.insert(theObservation);
	if(insertResult.second!=true){
		WARNING("fail to add the observation");
	}
	return const_cast<patObservation*>(&(*(insertResult.first)));
}

patULong patTraveler::getNumberOfObservations(){
	return observationSet.size();
}

patULong patTraveler::getId(){
	return id;
}

	set<patObservation>* patTraveler::getAllObservations(){
		return &observationSet;
	}