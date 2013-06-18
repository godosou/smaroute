#include <sstream>

#include <fstream>
#include <xmlwrapp/xmlwrapp.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
	 #include "patSimulator.h"
#include "patGpsPoint.h"
#include "patGeoCoordinates.h"
#include <sstream>
#include "patDisplay.h"
#include "patArc.h"
#include "patNode.h"
#include "patPathJ.h"
#include "patNBParameters.h"
  patSimulator::patSimulator(patNetwork* aNetwork,patULong seed):
      ranNumUniform(seed),
	  theNetwork(aNetwork)
  
  {
	trueGps = vector<patGpsPoint>();
	ranNumNormal.setUniform(&ranNumUniform);
  }
  
  patSimulator::patSimulator(patULong seed):
  ranNumUniform(seed)
  
  {
	trueGps = vector<patGpsPoint>();
	ranNumNormal.setUniform(&ranNumUniform);
  }
  
  bool operator<(const synNode& aNode, const synNode& bNode){
	if (aNode.id<bNode.id){
		return true;
	} 
	return false;
  }
  
  synNode patSimulator::createNode(patReal lat, patReal lon, patString type, patULong id){
	synNode node = {patGeoCoordinates(lat,lon),type,id};
	
	return node;
  }
  
  bool operator<(const synArc& aArc, const synArc& bArc){
	if (aArc.id<bArc.id){
		return true;
	}
	return false;
  }
  
 patString patSimulator::genNetwork_grid(patULong nbrOfParArcs,
			patReal upDownDistance,
			patReal parLength,
			patReal changeat,
			 list<patULong>* listOfNodes){
	realPath = list<synArc*>();
	pathDistance = upDownDistance;
	trueGps = vector<patGpsPoint>();
	nodes = set<struct synNode>();
	arcs = set<struct synArc>();
	orig =NULL;
	dest = NULL;
	
	patULong arcId = 0;
	patReal parLengthInDegree = lengthToDegree(parLength);
	patReal distanceInDegree = lengthToDegree(upDownDistance);
	
	patReal lowLat = 1.00000000001;
	patReal highLat = lowLat + distanceInDegree;
	
	
	
	
	patReal origLon = 1.0000000001;
	synNode* prevLow = const_cast<synNode*>(&(*(nodes.insert(createNode(lowLat,origLon,"",1)).first)));
	synNode* prevHigh = const_cast<synNode*>(&(*(nodes.insert(createNode(highLat,origLon,"",2)).first)));
	
	orig = prevLow;
	
	arcs.insert(createArc(prevLow,prevHigh,++arcId));
	arcs.insert(createArc(prevHigh,prevLow,++arcId));
	
	set<struct synArc>::iterator arcIter;
	patULong change = nbrOfParArcs+1;
	if(changeat>=0.0){
		change = floor(nbrOfParArcs * 0.7);
	}
	for(patULong i = 0;i<nbrOfParArcs;++i){
		synNode* low = const_cast<synNode*>(&(*(nodes.insert(createNode(lowLat,
				prevLow->geoCoord.longitudeInDegrees + parLengthInDegree,"",2*i+3)).first)));
		synNode* high = const_cast<synNode*>(&(*(nodes.insert(createNode(highLat,
				prevHigh->geoCoord.longitudeInDegrees + parLengthInDegree,"",2*i+4)).first)));
		
			set<struct synArc>::iterator cIter =arcs.insert(createArc(low,high,++arcId)).first;
			arcs.insert(createArc(high,low,++arcId));

			arcIter = arcs.insert(createArc(prevLow,low,++arcId)).first;
			set<struct synArc>::iterator anoIter = arcs.insert(createArc(prevHigh,high,++arcId)).first;
			
			if(i> change){
				arcIter = anoIter;
			}
			realPath.push_back(const_cast<struct synArc*>(&(*arcIter)));
			if (i == change){
				realPath.push_back(const_cast<struct synArc*>(&(*cIter)));
			}
	
		prevLow = low;
		prevHigh = high;
	
	}
	
	
	dest = prevLow;
	DEBUG_MESSAGE("a network generated. Nodes:"<<nodes.size()<<", "<<"Arcs: "<<arcs.size());

	listOfNodes->push_back(realPath.front()->upNode->id);
	
	for(list<synArc*>::iterator arcIter = realPath.begin();
			arcIter!=realPath.end();
			++arcIter){
			
		listOfNodes->push_back((*arcIter)->downNode->id);
	}
	
	
	patString networkName;
	stringstream stream;
	stream <<"result/network/up down "<<upDownDistance
			<<" par acs "<<nbrOfParArcs
			<<" arc length "<<parLength
			<<".xml";
	
	patString rtnFileName = stream.str();
	finalizeNetwork(rtnFileName);

	return rtnFileName;
	
  }
  
  void patSimulator::finalizeNetwork(patString fileName){
  		calArcsLength();
	calArcsHeading();
	computePathLength();
	genMapBound();
		writeNetwork(fileName);

  }
  
  void patSimulator::genMapBound(){
  
	minLat = orig->geoCoord.latitudeInDegrees;
	minLon = orig->geoCoord.longitudeInDegrees;
	maxLat = orig->geoCoord.latitudeInDegrees;
	maxLon = orig->geoCoord.longitudeInDegrees;

	for(set<struct synNode>::iterator nodeIter = nodes.begin(); 
			nodeIter != nodes.end();
			++nodeIter){
			
		if(nodeIter->geoCoord.latitudeInDegrees > maxLat){
			maxLat = nodeIter->geoCoord.latitudeInDegrees;
		}
		if(nodeIter->geoCoord.latitudeInDegrees < minLat){
			minLat = nodeIter->geoCoord.latitudeInDegrees;
		}
		
		
		if(nodeIter->geoCoord.longitudeInDegrees > maxLon){
			maxLon = nodeIter->geoCoord.longitudeInDegrees;
		}
		if(nodeIter->geoCoord.longitudeInDegrees < minLon){
			minLon = nodeIter->geoCoord.longitudeInDegrees;
		}
		
	}
  }
  
  ostream& operator<<(ostream& str, const synArc& x) {
  str << "Arc " 
      << x.id <<"(" 
      << x.upNode->id 
      << "->" 
      << x.downNode->id 
      << ")" ;
	str << " length=" 
      << x.length << "m" ;
  return str ;
}

void patSimulator::genNetwork(patULong nbrOfParArcs, 
		patReal upDownDistance,
		patReal parLength){
	patULong arcId = 0;
	patReal parLengthInDegree = lengthToDegree(parLength);
	patReal upDownDistanceInDegree =lengthToDegree(upDownDistance);
	synNode aNode = {patGeoCoordinates(1.0,0.0),"",0};
	
	orig = const_cast<synNode*>(&(*(nodes.insert(aNode).first)));
	patReal upLat = 1.0 + upDownDistanceInDegree/2.0;
	patReal downLat = 1.0 - upLat;
	maxLat = upLat;
	minLat = downLat;
	minLon = 0.0;
	 synNode prevHigh = {patGeoCoordinates(upLat,orig->geoCoord.longitudeInDegrees + upDownDistanceInDegree/2.0),"normal",1};
	 synNode prevLow = {patGeoCoordinates(downLat, orig->geoCoord.longitudeInDegrees + upDownDistanceInDegree/2.0),"normal",2};
	
	
	set<struct synArc>::iterator arcIter = arcs.insert(createArc(orig,&prevHigh,++arcId)).first;
	realPath.push_back(const_cast<struct synArc*>(&(*arcIter)));
	
	arcs.insert(createArc(orig,&prevLow,++arcId));
	
	for(patULong i = 1;i<nbrOfParArcs;++i){
		 synNode high = {patGeoCoordinates(upLat,prevHigh.geoCoord.longitudeInDegrees+parLengthInDegree),"normal",i*2+1};
		 synNode low = {patGeoCoordinates(downLat,prevHigh.geoCoord.longitudeInDegrees+parLengthInDegree),"normal",i*2+2};;
		
		nodes.insert(high);
		nodes.insert(low);
		
		arcIter=arcs.insert(createArc(&prevHigh,&high,++arcId)).first;
		realPath.push_back(const_cast<struct synArc*>(&(*arcIter)));
		
		arcs.insert(createArc(&prevLow,&low,++arcId));
		arcs.insert(createArc(&high,&low,++arcId));
		arcs.insert(createArc(&low,&high,++arcId));

		prevHigh = high;
		prevLow = low;
	}
	 synNode bNode = {patGeoCoordinates(1.0,prevHigh.geoCoord.longitudeInDegrees+upDownDistanceInDegree/2.0),"normal",100};
	dest = const_cast<synNode*>(&(*(nodes.insert(bNode).first)));
	maxLon = dest->geoCoord.longitudeInDegrees;
	
	arcIter=arcs.insert(createArc(&prevHigh,dest,++arcId)).first;
	realPath.push_back(const_cast<struct synArc*>(&(*arcIter)));
	
	arcs.insert(createArc(&prevLow,dest,++arcId));
	calArcsLength();
	calArcsHeading();
	computePathLength();
	patString networkName;
	stringstream stream;
	stream <<"up down"<<upDownDistance
			<<"par acs"<<nbrOfParArcs
			<<"arc length"<<parLength;
	
	patString rtnFileName = stream.str();
	writeNetwork(rtnFileName);
	
  }
  
  synArc patSimulator::createArc(struct synNode* aNode, struct synNode* bNode,patULong id){
	struct synArc aArc = {aNode,bNode,patReal(0.0),patReal(-1.0),id};
	return aArc;
  }
  
  string patSimulator::numberToString(patULong number){
	stringstream stream;
	stream <<number;
	return stream.str();
  }
  
  
  string patSimulator::numberToString(patReal number){
	stringstream stream;
	stream <<number;
	return stream.str();
  }
  void patSimulator::writeNetwork(patString fileName){
	xml::document xmldoc("osm");
	xml::node &it = xmldoc.get_root_node();
	
	it.get_attributes().insert("version","0.5");
	it.get_attributes().insert("generator","bioroute simulator");
	xml::node::iterator xmlNode = it.insert(xml::node("bound"));
	xmlNode->get_attributes().insert("maxlat",numberToString(maxLat).c_str());
	xmlNode->get_attributes().insert("minlat",numberToString(minLat).c_str());
	xmlNode->get_attributes().insert("maxlon",numberToString(maxLon).c_str());
	xmlNode->get_attributes().insert("minlon",numberToString(minLon).c_str());
			
	for(set<struct synNode>::iterator nodeIter = nodes.begin(); 
			nodeIter != nodes.end();
			++nodeIter){
		xmlNode = it.insert(xml::node("node"));
		xmlNode->get_attributes().insert("id",numberToString(nodeIter->id).c_str());
		xmlNode->get_attributes().insert("lat",numberToString(nodeIter->geoCoord.latitudeInDegrees).c_str());
		xmlNode->get_attributes().insert("lon",numberToString(nodeIter->geoCoord.longitudeInDegrees).c_str());
		if(nodeIter->type!="normal"){
			xml::node::iterator tagIt = xmlNode->insert(xml::node("tag"));
			tagIt->get_attributes().insert("k","highway");
			tagIt->get_attributes().insert("v",nodeIter->type.c_str());
		}
	}
	for(set<struct synArc>::iterator arcIter = arcs.begin(); 
			arcIter != arcs.end();
			++arcIter){
		 xmlNode = it.insert(xml::node("way"));
		xmlNode->get_attributes().insert("id",numberToString(arcIter->id).c_str());

		xml::node::iterator tagIt = xmlNode->insert(xmlNode->end(),xml::node("nd"));
		tagIt->get_attributes().insert("ref",numberToString(arcIter->upNode->id).c_str());
		
		tagIt = xmlNode->insert(xmlNode->end(),xml::node("nd"));
		tagIt->get_attributes().insert("ref",numberToString(arcIter->downNode->id).c_str());
		tagIt = xmlNode->insert(xml::node("tag"));
		tagIt->get_attributes().insert("k","oneway");
		tagIt->get_attributes().insert("v","true");
	}
	
	xmldoc.set_is_standalone(true);
	xmldoc.set_encoding("UTF-8");
	ofstream xmlFile(fileName.c_str()) ;
	xmlFile<<xmldoc;
	xmlFile.close();
	DEBUG_MESSAGE("network written to file "<<fileName);
  }
  
  patReal patSimulator::assignSignal(){
	set<patULong> signals;
	//signals.insert(patULong(4));
	for(set<struct synNode>::iterator nodeIter = nodes.begin(); 
			nodeIter != nodes.end();
			++nodeIter){
		if(signals.find(nodeIter->id)!=signals.end()){
			const_cast<struct synNode *>(&(*nodeIter))->type = "traffic_signals";
		}	
	}
  }
  patReal patSimulator::computePathLength(){
  patReal rtn = 0.0;
  for(list<struct synArc*>::iterator arcIter = realPath.begin();
			arcIter!=realPath.end();
			++arcIter){
		rtn += (*arcIter)->length;
  }
  return rtn;
  }
 
vector<patGpsPoint> patSimulator::travel(patPathJ* thePath, patReal theSpeed){
	trueGps = vector<patGpsPoint>(); 
	vector<patGpsPoint> gpsSequence;
	speed = theSpeed; // km/h
	DEBUG_MESSAGE("travel speed"<<speed);
	patReal speedMS = speed/3.6;
	interval  = 10.0;
	list<patArc* >* realPath = thePath->getArcList();
	
	list<patArc* >::iterator onArc = realPath->begin();
	patReal position = 0.0;
	
	patReal pathLength = thePath->computePathLength();
	patReal depart = 0.0;
	patReal nextPosition;
	patReal positionOnArc = 0.0;
	patULong time = 0;
	patReal currSpeed = getSpeed();
	patReal timeLeft = 0.0;
	timeLeft = 1.0;	
	DEBUG_MESSAGE("speed"<<currSpeed<<"path length"<<pathLength);
	while(onArc!=realPath->end()){
		patReal positionOnArc = 0.0;
		patReal nextStep = timeLeft * currSpeed;
		
		while(positionOnArc + nextStep <= (*onArc)->getLength()){
			positionOnArc +=nextStep;
			gpsSequence.push_back(genGps_random(positionOnArc/(*onArc)->getLength(),const_cast<patArc*>(*onArc),time,currSpeed));
			time+=interval;
			timeLeft = interval;
			currSpeed = getSpeed();
			nextStep = currSpeed * timeLeft;;
		}
		patReal timeUsedForRest = ((*onArc)->length - positionOnArc)/currSpeed;
		timeLeft -= timeUsedForRest;
		DEBUG_MESSAGE("reach the end of arc"<<*(*onArc)<<"; time needed"<<timeUsedForRest<<"; time left"<<timeLeft);
		if(theNetwork->getNodeFromUserId((*onArc)->downNodeId)->attributes.type!="traffic_signals"){
			++onArc;
		}
		else{
			patReal timeLeftAtIntersection = patNBParameters::the()->simDelayatIntersection;
			while( timeLeft <= timeLeftAtIntersection){
				DEBUG_MESSAGE("stop at intersection"<<timeLeftAtIntersection<<","<<timeLeft);
				timeLeftAtIntersection -=timeLeft;
				timeLeft = interval;
				gpsSequence.push_back(genGps_random(1.0,const_cast<patArc*>(*onArc),time,0.0));
				time+=interval;
				currSpeed = getSpeed();
			
			}
			
			timeLeft = interval - timeLeftAtIntersection;
			DEBUG_MESSAGE("move on"<<timeLeft);
				
			++onArc;
		}
		
	}
	
	string kmlFileName;
	stringstream kmlFileNameStream(kmlFileName);
	kmlFileNameStream << "result/";
	kmlFileNameStream << "travel";
	kmlFileNameStream <<" -";
	kmlFileNameStream << theSpeed;
	kmlFileNameStream << ".kml";
	kmlFileName = kmlFileNameStream.str();
	DEBUG_MESSAGE("OK");
	writeTrack(kmlFileName);
	kmlFileNameStream << ".csv";
	return gpsSequence;
} 
vector<patGpsPoint> patSimulator::travel(patReal theSpeed){
	
	trueGps = vector<patGpsPoint>(); 
	vector<patGpsPoint> gpsSequence;
	speed = theSpeed; // km/h
	patReal speedMS = speed/3.6;
	interval  = 10.0;
	list<struct synArc*>::iterator onArc = realPath.begin();
	patReal position = 0.0;
	
	patReal pathLength = computePathLength();
	patReal depart = 0.0;
	patReal nextPosition;
	patReal positionOnArc = 0.0;
	patULong time = 0;
	patReal currSpeed = getSpeed();
	patReal timeLeft = 0.0;
	timeLeft = 1.0;	
	//DEBUG_MESSAGE("speed"<<currSpeed<<"path length"<<pathLength);
	while(onArc!=realPath.end()){
		patReal positionOnArc = 0.0;
		patReal nextStep = timeLeft * currSpeed;
		
		while(positionOnArc + nextStep <= (*onArc)->length){
			positionOnArc +=nextStep;
			gpsSequence.push_back(genGps_random(positionOnArc/(*onArc)->length,const_cast<struct synArc*>(*onArc),time,currSpeed));
			time+=interval;
			timeLeft = interval;
			currSpeed = getSpeed();
			nextStep = currSpeed * timeLeft;;
		}
		patReal timeUsedForRest = ((*onArc)->length - positionOnArc)/currSpeed;
		timeLeft -= timeUsedForRest;
		DEBUG_MESSAGE("reach the end of arc"<<*(*onArc)<<"; time needed"<<timeUsedForRest<<"; time left"<<timeLeft);
		
		if((*onArc)->downNode->type == ""){
			++onArc;
			continue;
		}
		else if((*onArc)->downNode->type == "traffic_signals"){
			
			if(++onArc == realPath.end()){
				break;
			}
			
			synArc* downArc = const_cast<struct synArc*>(*onArc);
			onArc--;
			synArc* currArc = const_cast<struct synArc*>(*onArc);
			patReal waiting = getWaiting(currArc,downArc);
			while (timeLeft < waiting){
				gpsSequence.push_back(genGps_random(1,const_cast<struct synArc*>(*onArc),time,0));
				time+=interval;
				waiting -=timeLeft;
				timeLeft = interval;
			}
			
			timeLeft -=waiting;
			++onArc;
			continue;
		}
		
	}
	
	string kmlFileName;
	stringstream kmlFileNameStream(kmlFileName);
	kmlFileNameStream << "result/";
	kmlFileNameStream << "travel";
	kmlFileNameStream <<" -";
	kmlFileNameStream << theSpeed;
	kmlFileNameStream << ".kml";
	kmlFileName = kmlFileNameStream.str();
	DEBUG_MESSAGE("OK");
	writeTrack(kmlFileName);
	kmlFileNameStream << ".csv";
//	writeCsv(kmlFileNameStream.str());
	DEBUG_MESSAGE("OK");
	return gpsSequence;
  }
  
patReal patSimulator::getWaiting(struct synArc* upArc, struct synArc* downArc){
	return 5;
  }

patReal patSimulator::getSpeed(){
	return speed/3.6;
  }

patReal patSimulator::calHeading(struct synArc* onArc) {

	  
  patGeoCoordinates startCoord = onArc->upNode->geoCoord;
  patGeoCoordinates nextCoord = onArc->downNode->geoCoord;
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
	onArc->heading = theArcHeading;
	return theArcHeading;
}

patReal patSimulator::nextPosition(patReal speed){
  
  }

void patSimulator::calArcsLength(){
	
	for(set<struct synArc>::iterator arcIter = arcs.begin(); 
			arcIter != arcs.end();
			++arcIter){
		struct synArc* arcPointer = const_cast<struct synArc*>(&(*arcIter));
		arcPointer->length = arcPointer->upNode->geoCoord.distanceTo(arcPointer->downNode->geoCoord);
	}
  }
  
void patSimulator::calArcsHeading(){
	
	for(set<struct synArc>::iterator arcIter = arcs.begin(); 
			arcIter != arcs.end();
			++arcIter){
		struct synArc* arcPointer = const_cast<struct synArc*>(&(*arcIter));
		arcPointer->heading = calHeading(arcPointer);
	}
  }
  
patReal patSimulator::genDelta(patReal downBound,patReal upBound){
	   const gsl_rng_type * T;
       gsl_rng * r;
     
       //int i, n = 10;
     
       gsl_rng_env_setup();
     
       T = gsl_rng_default;
       r = gsl_rng_alloc (T);
     
        patReal u = gsl_ran_flat  (r,downBound,upBound);
       gsl_rng_free (r);
     
       return u;

  }
  
patReal patSimulator::randomNumber_Normal(patReal delta){
  	   const gsl_rng_type * T;
       gsl_rng * r;
     
     
       gsl_rng_env_setup();
     
       T = gsl_rng_default;
       r = gsl_rng_alloc (T);
     
           patReal u = gsl_ran_gaussian (r,delta);
     
       gsl_rng_free (r);
	
	return u;
  }
  

  
patReal patSimulator::lengthToDegree(patReal length){
	patReal earthRadius = 6372000.7976;
	
	patReal oneDegreeRadius = earthRadius * sin(89.0);
	
	return 180.0 * length /(pi * oneDegreeRadius);
 }
  
patReal patSimulator::lengthToDegree_Equator(patReal length){
	patReal earthRadius = 6372000.7976;
	
	
	return 180.0 * length /(pi * earthRadius);
 }
  
patGpsPoint patSimulator::genGps(patReal position,struct synArc* onArc,patULong time,patReal currSpeed){
	
	patReal lat = onArc->upNode->geoCoord.latitudeInDegrees +\
					position * (onArc->downNode->geoCoord.latitudeInDegrees-\
							onArc->upNode->geoCoord.latitudeInDegrees);
							
	patReal lon = onArc->upNode->geoCoord.longitudeInDegrees +\
					position * (onArc->downNode->geoCoord.longitudeInDegrees-\
							onArc->upNode->geoCoord.longitudeInDegrees);
	patError* err;						
	patReal deltaLat = genDelta(0.0,100.0);
	patReal deltaLon = genDelta(0.0,100.0);
	
	patReal offsetLat = lengthToDegree(randomNumber_Normal(deltaLat));
	patReal offsetLon = lengthToDegree(randomNumber_Normal(deltaLon));
	
	DEBUG_MESSAGE("offset lat "<< offsetLat<<"; offset lon"<<offsetLon);
	patReal gpsLat = lat + offsetLat;
	patReal gpsLon = lon + offsetLon;
	
	//gpsLat = lat;
	//gpsLon = lon;
	patReal deltaCoord = sqrt(deltaLat * deltaLat + deltaLon * deltaLon);
	
	currSpeed *=3.6;
	//patReal deltaSpeed = genDelta(0,currSpeed>1.0?currSpeed * 0.3:2);
	patReal deltaSpeed = currSpeed>1.0?speedDelta:6;
	
	patReal offsetSpeed = ranNumNormal.getNextValue(err)*deltaSpeed;
	//offsetSpeed = 0.0;
	patReal gpsSpeed = currSpeed + offsetSpeed;
		DEBUG_MESSAGE("currSpeed "<< currSpeed<<"; offset speed"<<offsetSpeed);

	while(gpsSpeed < 0.0){
		offsetSpeed = ranNumNormal.getNextValue(err)*deltaSpeed;
		gpsSpeed = currSpeed + offsetSpeed;
	}
	
	patReal deltaHeading = genDelta(0,30);
	patReal offsetHeading = ranNumNormal.getNextValue(err)*deltaHeading;
	offsetHeading = 0.0;
	patReal gpsHeading = onArc->heading + offsetHeading;
	DEBUG_MESSAGE("arc heading "<< onArc->heading<<"; offset heading"<<offsetHeading);

	gpsHeading = (gpsHeading<360.0)?gpsHeading:(gpsHeading-360.0);
	gpsHeading = (gpsHeading>0.0)?gpsHeading:(gpsHeading+360.0);
	DEBUG_MESSAGE("a gps generated on arc "<<onArc->id<<"("<<onArc->upNode->id<<"->"<<onArc->downNode->id<<" "<<position<<")"<<": time "<<time<<", lat "<<gpsLat<<", lon "<<gpsLon<<
			", horiz accu "<<deltaCoord<<", speed "<<currSpeed<<"-"<<gpsSpeed<<", speed accu "<<deltaSpeed
			<<", heading "<<gpsHeading<<", heading accu"<<deltaHeading);
	return patGpsPoint(
		patULong(1),
		time,
		gpsLat,
		gpsLon,
		gpsSpeed,
		deltaCoord,
		gpsHeading,
		patReal(1.0),
		deltaSpeed,
		deltaHeading
	);
  }
  
patReal patSimulator::randomNumber_Normal_biogeme(patReal delta){
  	   const gsl_rng_type * T;
       gsl_rng * r;
     
     
       gsl_rng_env_setup();
     
       T = gsl_rng_default;
       r = gsl_rng_alloc (T);
     
           patReal u = gsl_ran_gaussian (r,delta);
     
       gsl_rng_free (r);
	
	return u;
  }
	
void patSimulator::setSpeedDelta(patReal aValue){
	speedDelta = aValue;
}
	
	void patSimulator::writeCsv(vector<patGpsPoint>* gpsSequence, patString filename){
	DEBUG_MESSAGE("gps points"<<gpsSequence->size());
		
		ofstream csv(filename.c_str()) ; 
		csv<<"1111"<<endl;
		for (int i=0;i<gpsSequence->size();++i){
		csv<<"11";//trip name
		csv<<","<<gpsSequence->at(i).getTimeStamp();//time
		csv<<","<<gpsSequence->at(i).getLongitude();
		csv<<","<<gpsSequence->at(i).getLatitude();
		csv<<","<<"10.0";
		csv<<","<<gpsSequence->at(i).getSpeed();
		csv<<","<<gpsSequence->at(i).getHeading();
		csv<<","<<gpsSequence->at(i).getHorizonAccuracy();
		csv<<","<<"10.0";
		csv<<","<<gpsSequence->at(i).getSpeedAccuracy();
		csv<<","<<gpsSequence->at(i).getHeadingAccuracy();
		csv<<",";
		csv<<",";
		csv<<",";
		csv<<endl;
		}
	csv.close();
	}
	void patSimulator::writeTrack(patString fileName){
	DEBUG_MESSAGE("gps points"<<trueGps.size());
		  ofstream kml(fileName.c_str()) ;
	  kml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl ;
  	kml << "      <kml xmlns=\"http://earth.google.com/kml/2.0\">" << endl ;
  kml << "      <Document>" << endl ;

  patString theName(fileName) ;
  replaceAll(&theName,patString("&"),patString("and")) ;
  
  kml << "            <name>" << theName <<"</name>" << endl ;
  kml << "            <description>File created by bioroute simulation</description>" << endl ;
   	kml<<"<Folder>"<<endl;
  	kml<<"<name>GPS Points</name>"<<endl;
  	for (int i=0;i<trueGps.size();++i){
	//DEBUG_MESSAGE("i"<<i);
  		kml<<"<Folder>"<<endl;
  		kml<<"<name>GPS:"<<i+1<<"</name>"<<endl;
  		kml<<"<Placemark>"<<endl;
  		  kml<<"<Style >"<<endl;
	kml<<"<IconStyle>"<<endl ;
  kml<<"<color>ff00ff00</color>"<<endl;
	kml<<"<scale>0.7</scale>"<<endl;
/* kml<<"<heading>"<<endl;
 kml<<gpsSequence[i].heading;
 kml<<"</heading>"<<endl;
 */
  kml<<"<Icon>"<<endl;
  kml<<"<href>http://maps.google.com/mapfiles/kml/shapes/cycling.png</href>"<<endl;
  kml<<"</Icon>"<<endl;
  kml<<"</IconStyle>"<<endl;
  kml<<"</Style>"<<endl;
  		kml<<"<TimeStamp>"<<endl;
  		kml<<"	<when>"<<endl;
  		time_t rawtime = trueGps[i].getTimeStamp();
		
	   tm* ptm = gmtime ( &rawtime );
	   char buffer[50];
	   sprintf(buffer,"%4d-%02d-%02dT%02d:%02d:%02dZ",(ptm->tm_year+1900), (ptm->tm_mon+1), ptm->tm_mday, (ptm->tm_hour)%24, ptm->tm_min, ptm->tm_sec);
	   kml<<(buffer )<<endl;
  		kml<<"	</when>"<<endl;
  		kml<<"</TimeStamp>"<<endl;
  		//kml<<"<name>GPS:"<<i+1<<"</name>"<<endl;
    	kml << "                  <description> "<< trueGps[i] <<"</description>" << endl ;
    	kml << "                  <Point>" << endl ;
    	kml << "                        <coordinates>" 
		<< trueGps[i].getGeoCoord()->getKML() << ", 0</coordinates>" << endl ;
    	kml << "                  </Point>" << endl ;
  		kml<<"</Placemark>"<<endl;
		kml<<"</Folder>"<<endl;
		//DEBUG_MESSAGE("OK"<<i);
	}
  	
	
	kml<<"</Folder>"<<endl;
    kml << " " << endl ;
  kml << "      </Document>" << endl ;
  kml << "      </kml>" << endl ;


  kml.close() ;
	DEBUG_MESSAGE("writen gps");
	  time_t timeSeed = time(NULL); 

			stringstream stream;
	stream <<"result/"
			<<timeSeed
			<<".txt";
	
	ofstream gps( stream.str().c_str()) ;
	for(patULong i=0;i<gpsRecord.size();++i){
		gps<<i+1<<","<<gpsRecord[i].ox
			<<","<<gpsRecord[i].oy<<"\n";
	}
	gps.close();
	return ;
	}
  patGpsPoint patSimulator::genGps_random(patReal position,struct synArc* onArc,patULong time,patReal currSpeed){
	observation aGps;
	
	patReal lat = onArc->upNode->geoCoord.latitudeInDegrees +\
					position * (onArc->downNode->geoCoord.latitudeInDegrees-\
							onArc->upNode->geoCoord.latitudeInDegrees);
							
	patReal lon = onArc->upNode->geoCoord.longitudeInDegrees +\
					position * (onArc->downNode->geoCoord.longitudeInDegrees-\
							onArc->upNode->geoCoord.longitudeInDegrees);
	trueGps.push_back(patGpsPoint(
		patULong(1),
		time,
		lat,
		lon,
		currSpeed,
		0.0,
		onArc->heading,
		patReal(1.0),
		0.0,
		0.0
	));
	
	patError* err;
	ranNumUniform.getUniform(err);						
	ranNumUniform.getUniform(err);
	
	patReal deltaLat = fabs(ranNumNormal.getNextValue(err))*30.0;
	
	patReal deltaLon = fabs(ranNumNormal.getNextValue(err))*30.0;
	
	//ranNumNormal.getZeroOneDraw(err);
	DEBUG_MESSAGE(deltaLat<<","<<deltaLon);
	//patReal a = ranNumNormal.getNextValue(err) ;
	//DEBUG_MESSAGE(a);
	patReal offsetLat = ranNumNormal.getNextValue(err) * deltaLat; 
	patReal offsetLon = ranNumNormal.getNextValue(err) * deltaLon;
	
	
	if (lat-1.0 >= 0.0001){
	
		while(offsetLat<=-30 || offsetLat>13.0){
			offsetLat = ranNumNormal.getNextValue(err) * deltaLat; 
	
		}

	}
	else{
		while(offsetLat >= 30||offsetLat<=-13.0){ 
			offsetLat = ranNumNormal.getNextValue(err) * deltaLat; 
	
		}
	}
	
	while(fabs(offsetLon)  >= 20.0){
		offsetLon = ranNumNormal.getNextValue(err) * deltaLon; 
	
	}
	aGps.ox = offsetLon;
	aGps.oy = offsetLat;
	offsetLat = lengthToDegree(offsetLat);
	offsetLon = lengthToDegree(offsetLon);
	
	DEBUG_MESSAGE("offset lat "<< offsetLat<<"; offset lon"<<offsetLon);
	patReal gpsLat = lat + offsetLat;
	patReal gpsLon = lon + offsetLon;
	patReal deltaCoord = sqrt(1.5 * 1.5 * deltaLat * deltaLat + deltaLon * deltaLon);
	
	currSpeed *=3.6;
	//patReal deltaSpeed = ranNumUniform.getUniform(err) * (currSpeed>1.0?(currSpeed * 0.3):2);
	patReal deltaSpeed = currSpeed>1.0?speedDelta:6.0;

	patReal offsetSpeed = ranNumNormal.getNextValue(err) * deltaSpeed;
	//offsetSpeed = 0.0;
	patReal gpsSpeed = currSpeed + offsetSpeed;
		DEBUG_MESSAGE("currSpeed "<< currSpeed<<"; offset speed"<<offsetSpeed);

	while(gpsSpeed < 0.0){
		offsetSpeed = ranNumNormal.getNextValue(err) * deltaSpeed;
		gpsSpeed = currSpeed + offsetSpeed;
	}
	
	patReal deltaHeading =  ranNumUniform.getUniform(err) * 20.0;
	patReal offsetHeading = ranNumNormal.getNextValue(err) * deltaHeading;
	offsetHeading = 0.0;
	patReal gpsHeading = onArc->heading + offsetHeading;
	DEBUG_MESSAGE("arc heading "<< onArc->heading<<"; offset heading"<<offsetHeading);

	gpsHeading = (gpsHeading<360.0)?gpsHeading:(gpsHeading-360.0);
	gpsHeading = (gpsHeading>0.0)?gpsHeading:(gpsHeading+360.0);
	DEBUG_MESSAGE("a gps generated on arc "<<onArc->id<<"("<<onArc->upNode->id<<"->"<<onArc->downNode->id<<" "<<position<<")"<<": time "<<time<<", lat "<<gpsLat<<", lon "<<gpsLon<<
			", horiz accu "<<deltaCoord<<", speed "<<currSpeed<<"-"<<gpsSpeed<<", speed accu "<<deltaSpeed
			<<", heading "<<gpsHeading<<", heading accu"<<deltaHeading);
			
			gpsRecord.push_back(aGps);
	return patGpsPoint(
		patULong(1),
		time,
		gpsLat,
		gpsLon,
		gpsSpeed,
		deltaCoord,
		gpsHeading,
		patReal(1.0),
		deltaSpeed,
		deltaHeading
	);
  }
  
  patGpsPoint patSimulator::genGps_random(patReal position,patArc* onArc,patULong time,patReal currSpeed){
	observation aGps;
	patNode* upNode = theNetwork->getNodeFromUserId(onArc->upNodeId);
	patNode* downNode = theNetwork->getNodeFromUserId(onArc->downNodeId);
	
	patReal lat = upNode->geoCoord.latitudeInDegrees +\
					position * (downNode->geoCoord.latitudeInDegrees-\
							upNode->geoCoord.latitudeInDegrees);
							
	patReal lon = upNode->geoCoord.longitudeInDegrees +\
					position * (downNode->geoCoord.longitudeInDegrees-\
							upNode->geoCoord.longitudeInDegrees);
	trueGps.push_back(patGpsPoint(
		patULong(1),
		time,
		lat,
		lon,
		currSpeed,
		0.0, 
		onArc->attributes.heading,
		patReal(1.0),
		0.0,
		0.0
	));
	
	patError* err;
	//ranNumUniform.getUniform(err);						
	ranNumUniform.getUniform(err);
	
	patReal deltaLat = fabs(ranNumNormal.getNextValue(err))* patNBParameters::the()->latlngOffsetRange;
	
	patReal deltaLon = fabs(ranNumNormal.getNextValue(err))* patNBParameters::the()->latlngOffsetRange;
	
	//ranNumNormal.getZeroOneDraw(err);
	DEBUG_MESSAGE(deltaLat<<","<<deltaLon);
	//patReal a = ranNumNormal.getNextValue(err) ;
	//DEBUG_MESSAGE(a);
	patReal offsetLat = ranNumNormal.getNextValue(err) * deltaLat; 
	patReal offsetLon = ranNumNormal.getNextValue(err) * deltaLon;

	aGps.ox = offsetLon;
	aGps.oy = offsetLat;
	offsetLat = lengthToDegree(offsetLat);
	offsetLon = lengthToDegree(offsetLon);
	if (currSpeed < 0.5){
	offsetLat*=0.3;
	offsetLon*=0.3;
	}
	
	DEBUG_MESSAGE("offset lat "<< offsetLat<<"; offset lon"<<offsetLon);
	patReal gpsLat = lat + offsetLat;
	patReal gpsLon = lon + offsetLon;
	patReal deltaCoord = sqrt(1.5 * 1.5 * deltaLat * deltaLat + deltaLon * deltaLon);
	
	currSpeed *=3.6;
	patReal deltaSpeed = ranNumUniform.getUniform(err) * speedDelta;
	while(deltaSpeed< 0.0){
		deltaSpeed = ranNumUniform.getUniform(err) * speedDelta;
//		DEBUG_MESSAGE("speed"<<currSpeed<<","<<deltaSpeed<<","<<speedDelta);
	}
	//patReal deltaSpeed = currSpeed>1.0?speedDelta:6.0;

	patReal offsetSpeed = ranNumUniform.getUniform(err) * deltaSpeed;
	//offsetSpeed = 0.0;
	patReal gpsSpeed = currSpeed + offsetSpeed;
		DEBUG_MESSAGE("currSpeed "<< currSpeed<<"; offset speed"<<offsetSpeed);

	while(gpsSpeed < 0.0){
		offsetSpeed = ranNumNormal.getNextValue(err) * deltaSpeed;
		gpsSpeed = currSpeed + offsetSpeed;
		DEBUG_MESSAGE("speed:"<<gpsSpeed<<"="<<currSpeed<<"+"<<offsetSpeed<<". delta:"<<deltaSpeed);
	}
	
	patReal deltaHeading =  ranNumUniform.getUniform(err) * 20.0;
	patReal offsetHeading = ranNumNormal.getNextValue(err) * deltaHeading;
	patReal gpsHeading = onArc->attributes.heading+ offsetHeading;
	DEBUG_MESSAGE("arc heading "<< onArc->attributes.heading<<"; offset heading"<<offsetHeading);

	gpsHeading = (gpsHeading<360.0)?gpsHeading:(gpsHeading-360.0);
	gpsHeading = (gpsHeading>0.0)?gpsHeading:(gpsHeading+360.0);
	DEBUG_MESSAGE("a gps generated on arc "<<onArc->userId<<"("<<upNode->userId<<"->"<<downNode->userId<<" "<<position<<")"<<": time "<<time<<", lat "<<gpsLat<<", lon "<<gpsLon<<
			", horiz accu "<<deltaCoord<<", speed "<<currSpeed<<"-"<<gpsSpeed<<", speed accu "<<deltaSpeed
			<<", heading "<<gpsHeading<<", heading accu"<<deltaHeading);
			
			gpsRecord.push_back(aGps);
	return patGpsPoint(
		patULong(1),
		time,
		gpsLat,
		gpsLon,
		gpsSpeed,
		deltaCoord,
		gpsHeading,
		patReal(1.0),
		deltaSpeed * 5,
		deltaHeading
	);
  }
  
  