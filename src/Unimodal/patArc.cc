//----------------------------------------------------------------
// File: patArc.cc
// Author: Michel Bierlaire
// Creation: Thu Oct 30 10:53:34 2008
//----------------------------------------------------------------

#include "patArc.h"
#include "patNode.h"
#include "patError.h"

#include "patDisplay.h"
#include "patConst.h"
#include "patErrNullPointer.h"
#include "patNetwork.h"
#include "patGeoCoordinates.h"
#include "patNBParameters.h"

patArc::patArc(patULong theId, 
	       patNode* theUpNode, 
	       patNode* theDownNode, 
	       patString theName,
		   struct arc_attributes  theAttr,
	       patError*& err) :
  userId(theId),
  internalId(patBadId),
  name(theName),
  attributes(theAttr),
  length(0.0),
  generalizedCost(0.0)  {

  if ((theUpNode == NULL) || (theDownNode == NULL)) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return ;
  }
  upNodeId = theUpNode->userId ;
  downNodeId = theDownNode->userId ;

}
patArc::patArc(patULong theId, 
	       patNode* theUpNode, 
	       patNode* theDownNode, 
	       patString theName,
	       patError*& err) :
  userId(theId),
  internalId(patBadId),
  name(theName),
  length(0.0) {
  if ((theUpNode == NULL) || (theDownNode == NULL)) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return ;
  }
  upNodeId = theUpNode->userId ;
  downNodeId = theDownNode->userId ;
}
void patArc::calPriority(){
	if(attributes.type == "steps"){
		attributes.priority = patNBParameters::the()->stepsPriority  ;
	}
	else if(attributes.type == "footway"){
		attributes.priority = patNBParameters::the()->footwayPriority ;
	}
	else if(attributes.type == "cycleway"){
		attributes.priority = patNBParameters::the()->cyclewayPriority ;
	}
	else if(attributes.type == "primary_link"){
		attributes.priority = patNBParameters::the()->primary_linkPriority ;
	}
	else if(attributes.type == "trunk_link"){
		attributes.priority = patNBParameters::the()->trunk_linkPriority ;
	}
	else if(attributes.type == "motorway_link"){
		attributes.priority = patNBParameters::the()->motorway_linkPriority ;
	}
	else if(attributes.type == "bridleway"){
		attributes.priority = patNBParameters::the()->bridlewayPriority ;
	}
	else if(attributes.type == "residential"){
		attributes.priority = patNBParameters::the()->residentialPriority ;
	}
	else if(attributes.type == "unclassified"){
		attributes.priority =patNBParameters::the()->unclassifiedPriority ;
	}
	else if(attributes.type == "tertiary"){
		attributes.priority = patNBParameters::the()->tertiaryPriority ;
	}
	else if(attributes.type == "secondary"){
		attributes.priority = patNBParameters::the()->secondaryPriority ;
	}
	
	else if(attributes.type == "primary"){
		attributes.priority = patNBParameters::the()->primaryPriority ;
	}
	
	else if(attributes.type == "trunk"){
		attributes.priority = patNBParameters::the()->trunkPriority ;
	}
	
	else if(attributes.type == "motorway"){
		attributes.priority = patNBParameters::the()->motorwayPriority ;
	}
	
	else if(attributes.type == "railway"){
		attributes.priority = patNBParameters::the()->railwayPriority ;
	}
	else{
		attributes.priority = patNBParameters::the()->otherRoadTypePriority ;
	
	}
}
void patArc::setLength(patReal l) {
  length = l ;
  generalizedCost = l ;
}

patReal patArc::calHeading(patNetwork* theNetwork, patError*& err) {

	patNode* theUpNode = theNetwork->getNodeFromUserId(upNodeId);
	patNode* theDownNode = theNetwork->getNodeFromUserId(downNodeId);
	if ((theUpNode == NULL) || (theDownNode == NULL)) {
	
    err = new patErrNullPointer("patNode") ;
	DEBUG_MESSAGE("node"<<upNodeId<<","<<theUpNode<<"-"<<downNodeId<<","<<theDownNode);
    WARNING(err->describe()) ;
    return -1.0;
  }
  
  patGeoCoordinates startCoord = theUpNode->geoCoord;
  patGeoCoordinates nextCoord = theDownNode->geoCoord;
	patReal lng1=startCoord.longitudeInRadians;
	patReal lat1=startCoord.latitudeInRadians;
	patReal lng2=nextCoord.longitudeInRadians;
	patReal lat2=nextCoord.latitudeInRadians;
	
	patReal numerator = sin(lat1)*sin(lng2-lng1);
	patReal denumerator = sin(lat2)*cos(lat1)-cos(lat2)*sin(lat1)*cos(lng2-lng1);
	
	patReal theArcHeading = atan(numerator/denumerator)*180/pi;
	
	if(denumerator>0){
		theArcHeading+=360;
	}
	else{
		theArcHeading+=180;	
	}
	if (theArcHeading<0)
	{
		theArcHeading+=360;	
	}
	if(theArcHeading>=360){
		theArcHeading-=360;
	}
	attributes.heading = theArcHeading;
	return theArcHeading;
}


patReal patArc::getLength() const {
  return length ;
}

ostream& operator<<(ostream& str, const patArc& x) {
  str << "Arc " 
      << x.userId 
      << " [" 
      << x.name 
      << "](" 
      << x.upNodeId 
      << "->" 
      << x.downNodeId 
      << ")" ;
  str << " length=" 
      << x.length << "m; " ;
  return str ;
}
patULong patArc::computeTurn(patArc* downArc,patNetwork* theNetwork){
	patNode* intersection = theNetwork->getNodeFromUserId(downNodeId);
	if (intersection==NULL){
		return 100;
	}
	if (intersection->userId != downArc->upNodeId){
		return 100;
	}
	
	set<patULong>* successors = &(intersection->userSuccessors);
	map<patArc*, patReal> downHeading;
	patULong left=0;
	patULong right=0;
	patReal headingChange = 180.0-(downArc->attributes.heading- attributes.heading); 
	headingChange = (headingChange>360.0)?(headingChange-360.0):headingChange;
	headingChange = (headingChange<0.0)?(headingChange+360.0):headingChange;
		 
	for(set<patULong>::iterator iter1 = successors->begin();
			iter1 != successors->end();
			++iter1){
		
		patArc* downStream = theNetwork->getArcFromNodesUserId(intersection->userId, *iter1);
		if(downStream == NULL || downStream == downArc)
		{
			continue;
		}
		patReal h1 = 180.0-(downStream->attributes.heading - attributes.heading);
		h1 = (h1>360.0)?(h1-360.0):h1;
		h1 = (h1<0.0)?(h1+360.0):h1;
		
		if(fabs(h1)<10.0){
			continue;
		}
		
		if(h1>headingChange){
			right +=1;
		}
		if(h1<headingChange){
			left +=1;
		}
	}
	if (left+right==0){
		return 0;
	}
	else if(left+right==1){
		if(left==1){
			if(headingChange>=215.0){
				return 3;//right turn
			}
			else{
				return 2;
			}
			
		}
		else if (right==1){
			if(headingChange<=145.0){
				return 1;//left turn
			}
			else{
				return 2;//
			}
			
		}
		
	}
	else if(left+right>=2){
		if(right==0){
			return 3;
		}
		else if(right>=1&& left>=1){
			return 2;
		}
		else{
			return 1;
		}	
	}
}
