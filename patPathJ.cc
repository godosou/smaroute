#include "patPathJ.h"
#include "patConst.h"
#include "patDisplay.h"
#include "patPower.h"
#include "patErrMiscError.h"
#include "patErrNullPointer.h"
#include "patGpsPoint.h"
#include "patNBParameters.h"
#include <list>

#include <fstream>
#include <sstream>
patPathJ::patPathJ() :
name (patString("No name")),
Id(-1)
{
attributes.length=-1;

}

patPathJ::patPathJ(list<patArc* > theListOfArcs) :
name (patString("No name")),
Id(-1)
{
attributes.length=-1;
 listOfArcs = theListOfArcs;
 }
 
 patPathJ::patPathJ(list<patNode* > listOfNodes,patNetwork* theNetwork,patError*& err) :
name (patString("No name")),
Id(-1)
{
attributes.length=-1;
 patNode* upNode = listOfNodes.front();
 list<patNode*>::iterator nodeIter = listOfNodes.begin();
 nodeIter++;
 for(;
			nodeIter!=listOfNodes.end();
			++nodeIter){
	patNode* downNode = *nodeIter;
	patArc* arc = theNetwork->getArcFromNodesUserId(upNode->userId,downNode->userId);
	if(arc == NULL){
		err = new patErrNullPointer("patArc") ;
		WARNING(err->describe()) ;
		
		return;
	}
	listOfArcs.push_back(arc);
	
	upNode = downNode;
 
 }
 }
 
ostream& operator<<(ostream& str, const patPathJ& x)  {
	str<<"<"<<x.listOfArcs.front()->upNodeId;
	for(list<patArc*>::const_iterator arcIter = x.listOfArcs.begin();
						arcIter != x.listOfArcs.end();
						++arcIter){
		
		str<<"-"<<(*arcIter)->downNodeId;
	}
	str<<">";
	
	return str;
}


bool operator==(const patPathJ& aPath, const patPathJ& bPath) {
	
	//patBoolean equal =patTRUE;
	//compare path size
	if(aPath.listOfArcs.size()!=bPath.listOfArcs.size()){
		//equal = patFALSE;
		return false;
	}
	
	//compare from arc to arc
	list<patArc*>::const_iterator aIter = aPath.listOfArcs.begin();
	list<patArc*>::const_iterator bIter = bPath.listOfArcs.begin();
	while(aIter!=aPath.listOfArcs.end()&&bIter!=bPath.listOfArcs.end()){
		if (*aIter!=*bIter){
			return false;
		}
		++aIter;
		++bIter;
	}
	//DEBUG_MESSAGE("path equal");
	return true;
}

bool operator!=(const patPathJ& aPath, const patPathJ& bPath) {
		return !(aPath==bPath);
}


bool operator<(const patPathJ& aPath, const patPathJ& bPath) {
	
	if(const_cast<patPathJ*>(&aPath)->computePathLength()<const_cast<patPathJ*>(&bPath)->computePathLength()){
		return true;
	}
	if(const_cast<patPathJ*>(&aPath)->computePathLength()==const_cast<patPathJ*>(&bPath)->computePathLength()){
		if(aPath.listOfArcs.size()<aPath.listOfArcs.size()){
			return true;
		}
		
		list<patArc*>::const_iterator aIter = aPath.listOfArcs.begin();
		list<patArc*>::const_iterator bIter = bPath.listOfArcs.begin();
		while(aIter!=aPath.listOfArcs.end()&&bIter!=bPath.listOfArcs.end()){
			if ((*aIter)->userId>(*bIter)->userId){
				return false;
			}if ((*aIter)->userId<(*bIter)->userId){
				return true;
			}
			++aIter;
			++bIter;
		}

	}
	//DEBUG_MESSAGE("less");
	return false;
}
void patPathJ::addArcToFront(patArc* theArc){
	listOfArcs.push_front(theArc);
}
void patPathJ::addArcToBack(patArc* theArc){

	listOfArcs.insert(listOfArcs.end(),theArc);

}

patArc* patPathJ::front(){
		if(listOfArcs.empty()==true){
		return NULL;
	}
	
	return listOfArcs.front();

}

patArc* patPathJ::back(){
	
	if(listOfArcs.empty()==true){
		return NULL;
	}
	
	return listOfArcs.back();
}

patArc* patPathJ::back2(){
	
	if(listOfArcs.size()<2){
		return NULL;
	}
	list<patArc*>::iterator arcIter = listOfArcs.end();
	arcIter--;
	arcIter--;
	return const_cast<patArc*>(*arcIter);
}

patArc* patPathJ::deleteArcFromBack(){

	if(listOfArcs.empty()==true){
		return NULL;
	}
	patArc* backArc = listOfArcs.back();
	listOfArcs.pop_back();
	return backArc;
}

/*
void patPathJ::addArcSequenceToBack(list<patArc*> arcSequence){
	listOfArcs.insert(listOfArcs.end(),arcSequence.begin(),arcSequence.end());
}
*/
patOdJ patPathJ::generateOd(patNetwork* theNetwork,patError*& err){
	patNode* originNode = theNetwork->getNodeFromUserId(listOfArcs.front()->upNodeId);
	if(originNode==NULL){
		stringstream str ;
		str << "Origin node "<<listOfArcs.front()->upNodeId<<" does not exist"   ;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		return patOdJ();
	}
	patNode* destinationNode = theNetwork->getNodeFromUserId(listOfArcs.back()->downNodeId);
	if(originNode==NULL){
		stringstream str ;
		str << "Destination node "<<listOfArcs.back()->downNodeId<<" does not exist"   ;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		return patOdJ();
	}
	patOdJ theOd(originNode,destinationNode);
	return theOd;
}

void patPathJ::assignOd(patOdJ* theOd){
	od=theOd;
}

patBoolean patPathJ::isLinkInPath(patArc* theArc) const{

	patBoolean found=patFALSE;
	for(list<patArc*>::const_iterator arcIter = listOfArcs.begin();
			arcIter != listOfArcs.end();
			++arcIter){
		if(*arcIter == theArc){
			return patTRUE;		
		}		
	}

	return found;
}

patReal patPathJ::computePathLength(){
	attributes.length =0.0;
	for(list<patArc*>::iterator arcIter = listOfArcs.begin();
						arcIter!=listOfArcs.end();
						++arcIter){
						
		attributes.length+=(*arcIter)->getLength();
	}
	
	return attributes.length;
}

patReal patPathJ::getPathLength(){
	if(attributes.length<=0){
		//DEBUG_MESSAGE("compute path length");
		computePathLength();
	}
	
	return attributes.length;
}
patReal patPathJ::getPathLengthG(patReal tmpLength){
	patReal pathSizeGamma = 0.0;

	if (pathSizeGamma == 0.0) {
     return 1.0 ;
   }
   else if (pathSizeGamma == 1.0) {
     return tmpLength ; 
   }
   else {
     return patPower(tmpLength,pathSizeGamma) ;
   }
}


void patPathJ::assignId(const patULong theId){
	Id=theId;
}

patOdJ* patPathJ::getOd(){
	return od;
}

list<patArc*>* patPathJ::getArcList(){
	return &listOfArcs;
}

patULong patPathJ::nbrOfArcs(){
	return listOfArcs.size();
}

patArc* patPathJ::getArc(patULong iArc){
	list<patArc*>::iterator arcIter= listOfArcs.begin();
	
	for(patULong i=0;i<=iArc;++i){
		arcIter++;
	}
	
	return const_cast<patArc*>(*arcIter);
}
  
 	
	   
list<patArc*> patPathJ::getSeg(patArc* aArc, patArc* bArc){
		list<patArc*> rtnList;

	if(aArc==bArc){
		rtnList.push_back(aArc);
		return rtnList;
	}
	list<patArc*>::iterator arcIter = listOfArcs.begin();
	
	while((*arcIter)!=aArc && arcIter!=listOfArcs.end()){
		++arcIter;
	}
	
	if(arcIter == listOfArcs.end()){
		return list<patArc*>();
	}
	
	rtnList.push_back(*arcIter);
	 ++arcIter;
		
	while((*arcIter)!=bArc && arcIter!=listOfArcs.end()){
		rtnList.push_back(*arcIter);
		++arcIter;
	}
	
	if(arcIter == listOfArcs.end()){
		return list<patArc*>();
	}
	rtnList.push_back(*arcIter);
	return rtnList;
}
/*
patBoolean patPathJ::isValidPath(vector<patGpsPoint>* gpsSequence){
	
	patBoolean validPath = patTRUE;
	for(patULong i = 1; i<gpsSequence->size();++i){
		map<patArc*, patReal>* currLinkDDR = gpsSequence->at(i).getLinkDDR();
		map<patArc*, patReal>* prevLinkDDR = gpsSequence->at(i-1).getLinkDDR();
		patBoolean flag1 = patFALSE;
		for(map<patArc*, patReal>::iterator currIter = currLinkDDR->begin();
			currIter != currLinkDDR->end();
			++currIter){
			if(isLinkInPath(currIter->first) ==  patFALSE){
				continue;
			}
			patBoolean flag2 = patFALSE;
			for(map<patArc*, patReal>::iterator prevIter = prevLinkDDR->begin();
				prevIter != prevLinkDDR->end();
				++prevIter){
				if(!(getSeg(prevIter->first, currIter->first).empty())){
					flag2 = patTRUE;
					break;
				}
			}
			if (flag2 == patTRUE){
				flag1 = patTRUE;
				break;
			}
			
		}
	
		if(flag1 == patFALSE){
			validPath = patFALSE;
			DEBUG_MESSAGE("no connection at point"<<i<<":"<<gpsSequence->at(i).getTimeStamp());
			break;
		}
	}
	
	return validPath;
}
*/
patBoolean patPathJ::isValidPath(vector<patGpsPoint>* gpsSequence){
	set<patArc*> prevValidArcs;
	for(list<patArc*>::iterator arcIter = listOfArcs.begin();
				arcIter != listOfArcs.end();
				++arcIter){
				
		if(gpsSequence->at(0).getLinkDDR()->find(*arcIter)!=gpsSequence->at(0).getLinkDDR()->end()){
			prevValidArcs.insert(*arcIter);
		}
	
	}
	if(prevValidArcs.empty()){
	//DEBUG_MESSAGE("no connection at point"<<0<<","<<gpsSequence->at(0).getTimeStamp()<<","<<*listOfArcs.front());
			
		return patFALSE;
	}
	for(patULong i = 1; i<gpsSequence->size();++i){
		set<patArc*> currValidArcs;
	
		map<patArc*,patReal>* currLinkDDR = gpsSequence->at(i).getLinkDDR();
		for(map<patArc*,patReal>::iterator currArcIter = currLinkDDR->begin();
						currArcIter != currLinkDDR->end();
						++currArcIter){
				
			for(set<patArc*>::iterator prevArcIter = prevValidArcs.begin();
					prevArcIter != prevValidArcs.end();
					++prevArcIter){		
					if(!(getSeg(*prevArcIter,currArcIter->first).empty())){
						currValidArcs.insert(currArcIter->first);
						
					}
			}	
			
		}
		if(currValidArcs.empty()){
			//DEBUG_MESSAGE("no connection at point"<<i<<","<<gpsSequence->at(i).getTimeStamp());
			return patFALSE;
			
		}
		prevValidArcs = 	currValidArcs;
	}
	if(prevValidArcs.empty()){
		return patFALSE;
	}
	return patTRUE;
}

void patPathJ::calTurns(patNetwork* theNetwork){
	attributes.leftTurn = 0;
	attributes.straightTurn = 0;
	attributes.rightTurn = 0;
	attributes.uTurn = 0;
	list<patArc*>::iterator arcIter = listOfArcs.begin();
	patArc* lastArc = const_cast<patArc*>(*arcIter);
	arcIter++;
	for( ;
				arcIter!=listOfArcs.end();
				++arcIter){
		patArc* currArc = *arcIter;
		patReal headingChange = 180.0-(currArc->attributes.heading- lastArc->attributes.heading); 
		headingChange = (headingChange>360.0)?(headingChange-360.0):headingChange;
		headingChange = (headingChange<0.0)?(headingChange+360.0):headingChange;
		lastArc = currArc;
		if(headingChange== 0.0 ||
			headingChange <= patNBParameters::the()->uTurnAngle ||
			headingChange > patNBParameters::the()->leftTurnAngle ){
			attributes.uTurn++;
		}
		else if(headingChange > patNBParameters::the()->uTurnAngle &&
				headingChange <= patNBParameters::the()->rightTurnAngle 
				){
			attributes.rightTurn++;
		}
		else if(headingChange > patNBParameters::the()->rightTurnAngle &&
				headingChange <= patNBParameters::the()->straightTurnAngle 
				){
			attributes.straightTurn++;
		}
		else if(headingChange > patNBParameters::the()->straightTurnAngle &&
				headingChange <= patNBParameters::the()->leftTurnAngle 
				){
			attributes.leftTurn++;
		}
	}
}

patULong patPathJ::getId(){
	return Id;	
}


patString patPathJ::genDescription(){
	stringstream ss;
	ss<<"id: "<<Id<<", ";
	ss<<"length: "<<attributes.length<<", ";
	ss<<"left turn: "<<attributes.leftTurn<<", ";
	ss<<"right turn: "<<attributes.rightTurn<<", ";
	ss<<"straight turn: "<<attributes.straightTurn<<", ";
	return ss.str();
	
}

patBoolean patPathJ::join(patPathJ bPath){
	if(back() != bPath.front()){
		return patFALSE;
	}
	
	list<patArc*>* bList = bPath.getArcList();
	if (bList==NULL||bList->empty()){
		return patTRUE;
	}
	list<patArc*>::iterator bIter = bList->begin();
	bIter++;
	for(;bIter!=bList->end();++bIter){
		listOfArcs.push_back(*bIter);
	}
	return patTRUE;
}

void patPathJ::computeTrafficSignals(patNetwork* theNetwork){
	attributes.nbrOfTrafficSignals = 0;
	if(listOfArcs.size()<=1){
		return;
	}
	
	list<patArc*>::iterator lastArc= listOfArcs.end();
	lastArc--;
	list<patArc*>::iterator arcIter = listOfArcs.begin();
	while(arcIter!=lastArc){
	
		patNode* intersection = theNetwork->getNodeFromUserId((*arcIter)->downNodeId);
	
		if(intersection != NULL && intersection->attributes.type == "traffic_signals"){
			attributes.nbrOfTrafficSignals++;
		}
		
		arcIter++;
	}
	return ;
}

patULong patPathJ::getTrafficSignals(){
	return attributes.nbrOfTrafficSignals;
}

patBoolean patPathJ::append(list<patArc*>* newSeg){
	if (!listOfArcs.empty()){
	if(newSeg == NULL){
		DEBUG_MESSAGE("null");
		return patFALSE;
	}
	else{
		if(newSeg->empty()){
		DEBUG_MESSAGE("empty");
		return patTRUE;
		}
	}
	
	}
	for(list<patArc*>::iterator arcIter = newSeg->begin();
				arcIter!=newSeg->end();
				++arcIter){
		//DEBUG_MESSAGE(**arcIter);
		listOfArcs.push_back(*arcIter);
	}
	computePathLength();
	return patTRUE;
}

patBoolean patPathJ::append(patPathJ* newSeg){
	return append(newSeg->getArcList());
}

patReal patPathJ::computePointDDRRaw(map<patArc*,patReal>* currLinkDDR){
patReal rtnValue = 0.0;
if(currLinkDDR->empty()){
	DEBUG_MESSAGE("empty domain");
	return rtnValue;
}

for(map<patArc*,patReal>::iterator arcIter= currLinkDDR->begin();
	arcIter!=currLinkDDR->end();
	++arcIter){
	if(isLinkInPath(arcIter->first)){
		rtnValue+=arcIter->second;
	}
	}
	
	return rtnValue;
}

patULong patPathJ::endNodeUserId(){
	if(listOfArcs.empty()){
		return -1;
		}
	else{
		return listOfArcs.back()->downNodeId;
	}
}

patBoolean patPathJ::containLoop(){
	set<patULong> nodeSet;
	for(list<patArc*>::iterator arcIter = listOfArcs.begin();
						arcIter!=listOfArcs.end();
						++arcIter){
		if(nodeSet.find((*arcIter)->upNodeId)==nodeSet.end()){
			nodeSet.insert((*arcIter)->upNodeId);
		}				
		else{
			return patTRUE;
		}
	}
	if(nodeSet.find(listOfArcs.back()->downNodeId)==nodeSet.end()){
		return patFALSE;
		}				
		else{
			return patTRUE;
		}
}

patULong patPathJ::getLeftTurns(){
	return attributes.leftTurn;
}


patULong patPathJ::getRightTurns(){
	return attributes.rightTurn;
}


patULong patPathJ::getStraightTurns(){
	return attributes.straightTurn;
}


patULong patPathJ::getUTurns(){
	return attributes.uTurn;
}

patReal patPathJ::getPerPrimaryLinkRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "primary_link"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();

}
patReal patPathJ::getPerTrunkLinkRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "trunk_link"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}
patReal patPathJ::getPerMotorwayLinkRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "motorway_link"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}
patReal patPathJ::getPerBridleRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "bridleway"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}
patReal patPathJ::getPerResidentialRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "residential"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}
patReal patPathJ::getPerUnclassifiedRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "unclassified"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}
patReal patPathJ::getPerTertiaryRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "tertiary"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}
patReal patPathJ::getPerSecondaryRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "secondary"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}
patReal patPathJ::getPerPrimaryRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "primary"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}
patReal patPathJ::getPerTrunkRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "trunk"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}
patReal patPathJ::getPerMotorwayRd(){
	patReal rtn=0.0;
	for (list<patArc*>::iterator arcIter=listOfArcs.begin(); arcIter!=listOfArcs.end(); ++arcIter) {
		if((*arcIter)->attributes.type == "motorway"){
			rtn+=(*arcIter)->getLength();
		}
		
	}
	return rtn/getPathLength();
	
}

 patULong patPathJ::getSubPath(patPathJ* newPath, patNode* startNode, patNode* endNode){
		patPathJ tmpPath;
		list<patArc*>::iterator arcIter = listOfArcs.begin();
		while(arcIter!= listOfArcs.end() && startNode->userId != (*arcIter)->upNodeId){
			
			arcIter++;
		}
		
		if(arcIter == listOfArcs.end()){
			return 1; //can't get the start node;
		}
		
		while(arcIter != listOfArcs.end()){
			newPath->addArcToBack(*arcIter);
			if(endNode->userId == (*arcIter)->downNodeId){
				break;
			}
			arcIter++;
			
		}
		if(arcIter == listOfArcs.end()){
			return 2; //can't get the 2end node;
		}
		
		return 3;
}
set<patArc*> patPathJ::getDistinctArcs(){
	set<patArc*> arcSet;
	for(list<patArc*>::iterator arcIter = listOfArcs.begin();
			arcIter!=listOfArcs.end();
			++arcIter){
				arcSet.insert(*arcIter);		
		
	}
	return arcSet;
}