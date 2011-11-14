//----------------------------------------------------------------
// File: patTripParser.cc
// Author:
// Creation: 17 March 2009
//----------------------------------------------------------------

#include "patType.h"
#include "patConst.h"
#include <fstream>
#include <time.h>
#include "patTripParser.h"
#include "patDisplay.h"
#include "patPathDDR.h"
#include "patGpsPoint.h"
#include "patNetwork.h"
#include "patObservation.h"
#include "patTraveler.h"
#include <vector>
#include <list>
#include "patArc.h"
#include "patSample.h"
#include "patPathJ.h"
#include "patOdJ.h"
#include "patError.h"
#include "patTripGraph.h"

patTripParser::patTripParser()
{
} 
patTripParser::patTripParser(patTraveler* theTraveler,
			   patULong& theTripId,
			   patULong& theStartTime):
			  generatedObservation(theTraveler,theTripId)
{
	generatedObservation.setStartTime(theStartTime);
} 

void patTripParser::setEnviroment(patSample* sample, patNetwork* network){
	theSample = sample;
	theNetwork = network;
}

void patTripParser::setMapBounds(patReal minLat, 
			      patReal maxLat, 
			      patReal minLon, 
			      patReal maxLon) {
				  
generatedObservation.setMapBounds( minLat, 
			       maxLat, 
			       minLon, 
			       maxLon);
}




patBoolean patTripParser::addPoint(const patGpsPoint theGpsPoint) {
	
	gpsSequence.push_back(theGpsPoint);

	return patTRUE;
}

void patTripParser::genPath(){
	set<patULong> theO = gpsSequence[0].getStartNodes(theNetwork);
	patTripGraph theTripGraph(theNetwork);
	theTripGraph.setOrigin(theO);
	theTripGraph.parseGpsPoint(&(gpsSequence[0]),NULL);
	for(int i=1;i<gpsSequence.size();++i){
		theTripGraph.parseGpsPoint(&(gpsSequence[i]),&(gpsSequence[i-1]));
	}
	for(int i=1;i<gpsSequence.size();++i){
		theTripGraph.enterMissingPart(&(gpsSequence[i]),&(gpsSequence[i-1]));
	}
	theTripGraph.computePaths();
	set<patPathJ>* generatedPathSet = &(theTripGraph.getPathSet());
	
	for(set<patPathJ>::iterator pathIter = generatedPathSet->begin();
						pathIter != generatedPathSet->begin();
						++pathIter
						){
		patError* err;
		patPathJ thePath = *pathIter;
		
		patOdJ od = thePath.generateOd( theNetwork, err);
		if(err!=NULL){
			continue;
		}
		patOdJ* theOd = theSample->addOd(od);

		thePath.assignOd(theOd);
		patPathJ* pathPointer = theOd->addPath(thePath);
		generatedObservation.pathDDRs[pathPointer] = patPathDDR(pathPointer,&gpsSequence);

		
	}

}

void patTripParser::endOfTrip(vector< list <  pair<patArc*,patULong> > >* adjList,
			patULong theEndTime){
			
	generatedObservation.setEndTime(theEndTime);
	
	DEBUG_MESSAGE("Prepare to generate DDR. " ) ;
	genDDR(adjList);
	DEBUG_MESSAGE("Prepare to generate OD . " ) ;
	genOd();
	DEBUG_MESSAGE("Prepare to generate OD DDR. " ) ;
	genOdDDR();
	DEBUG_MESSAGE("Prepare to generate paths. " ) ;
	genPath();
			DEBUG_MESSAGE("Prepare to generate path ddrs. " ) ;
	genPathDDR();
	DEBUG_MESSAGE("Prepare to write files. " ) ;
	patString userIdString;
	patString tripIdString;
	userIdString = generatedObservation.getTraveler()->getId();
	tripIdString = generatedObservation.getId();
	/*
	sprintf(userIdString,"%d",generatedObservation.getTraveler()->getId());
	sprintf(userTripString,"%d",generatedObservation.getId());
*/
	
	//writeToKML(userIdString+"-"+tripIdString);
	writeToKML(patString("output.kml"));
	DEBUG_MESSAGE("A trip have been created. " ) ;
	generatedObservation.getTraveler()->addObservation(generatedObservation);
}

void patTripParser::setOd(patOdJ* theOd){
	od = theOd;
}

void patTripParser::writeToFile(){
	ofstream outfile("output.out");
	/*
	DEBUG_MESSAGE("path number:"<<pathList.size());
	
	for (int i=0;i<pathList.size();++i){
			//DEBUG_MESSAGE(i);
		for (list<patArc*>::iterator iter= pathList[i].begin();iter!=pathList[i].end();++iter){
			outfile<<(*iter)->userId<<",";
					
		}
		outfile<<endl;

		for (int j=0;j<generatedObservation.pathDDRs[i].size();++j){
			outfile<<generatedObservation.pathDDRs[i][j]<<",";		
		}

		outfile<<endl;
	}
	
	for (int i=0;i<gpsSequence.size();++i){
		outfile<<i<<":"<<gpsSequence[i];
		for(map<patULong,vector<pair<patULong,list<patULong> > > >::iterator iter = gpsSequence[i].predecessor.begin();iter!=gpsSequence[i].predecessor.end();++iter){
			outfile<<"down:"<<theNetwork->internalNodes[iter->first]->userId<<"("<<iter->second.size()<<"):"<<endl;
			for(vector<pair<patULong,list<patULong> > >::iterator iter2 = iter->second.begin();iter2 != iter->second.end();++iter2){
				outfile<<"pre:"<<theNetwork->internalNodes[iter2->first]->userId<<"("<<iter2->second.size()<<"):";
				for(list<patULong>::iterator iter3 = iter2->second.begin();iter3 != iter2->second.end();++iter3){
					outfile<<theNetwork->internalNodes[*iter3]->userId<<",";				
				}			
				outfile<<endl;
			}
		}
		outfile<<endl;
	}
	for (int i=0;i<gpsSequence.size();++i){
		outfile<<i<<":"<<gpsSequence[i];
		for(map<patArc*,patReal>::iterator iter = gpsSequence[i].linkDDR.begin();iter!=gpsSequence[i].linkDDR.end();++iter){
			outfile<<*(iter->first)<<":"<<iter->second<<endl;
			for (map<patString,patReal>::iterator iter2 = (iter->first)->attributes.begin();iter2 != (iter->first)->attributes.end();++iter2){
				outfile<<iter2->first<<":"<<iter2->second;
			}
			outfile<<endl;
		}
		outfile<<endl;
	}*/
}

void patTripParser::writeToKML(patString fileName){
	  ofstream kml(fileName.c_str()) ;
	map<patOdJ*,map<patPathJ*,patPathDDR> > pathDDRsOrderByOd = generatedObservation.getPathDDRsOrderByOd();
	  kml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl ;
  	kml << "      <kml xmlns=\"http://earth.google.com/kml/2.0\">" << endl ;
  kml << "      <Document>" << endl ;

  patString theName(fileName) ;
  replaceAll(&theName,patString("&"),patString("and")) ;
  
  kml << "            <name>" << theName <<"</name>" << endl ;
  kml << "            <description>File created by bioroute</description>" << endl ;
   	kml<<"<Folder>";
   	kml << "            <name>paths</name>" << endl ;
  for(map<patOdJ*,map<patPathJ*,patPathDDR> >::iterator odIter = pathDDRsOrderByOd.begin();
					odIter != pathDDRsOrderByOd.end();
					++odIter){
	 kml<< "<Folder>";
	 kml<<"<name>"<<endl;
	 kml<<"OD:"<<*(odIter->first)<<endl;
	 kml<<"</name>"<<endl;
	for (map<patPathJ*,patPathDDR>::iterator pathIter =odIter->second.begin();
			pathIter !=odIter->second.end();
			++pathIter){
  kml<< "<Folder>";
   kml<< "<description>";
 
	kml<<pathIter->second;
  kml<< "</description>";
  list<patArc*>* listOfArcs = pathIter->first->getArcList();
		for (list<patArc*>::iterator aIter= listOfArcs->begin();aIter!=listOfArcs->end();++aIter){
		    kml << "            <Placemark>" << endl ;
    patString theName((*aIter)->name) ;
    replaceAll(&theName,patString("&"),patString("and")) ;
    kml << "                  <name>"<< theName <<"</name>" << endl ;
    kml << "                  <description>Arc "<< (*aIter)->userId 
	<< " from node "
	<< (*aIter)->upNodeId 
	<< " to node "
	<< (*aIter)->downNodeId
	<< "</description>" << endl ;
    kml << " " << endl ;
    kml << "                  <Style>" << endl ;
    kml << "                        <LineStyle>" << endl ;
    kml << "                              <color>4fff0000</color>" << endl ;
    kml << "                              <width>8</width>" << endl ;
    kml << "                        </LineStyle>" << endl ;
    kml << "                  </Style>" << endl ;
    kml << " " << endl ;
    kml << "                  <LineString>" << endl ;
    kml << "                        <coordinates>" << endl ;
    
    list<patGeoCoordinates>::iterator gIter((*aIter)->polyline.begin()) ;
    list<patGeoCoordinates>::iterator hIter((*aIter)->polyline.begin()) ;
    ++hIter ;
    for ( ; hIter != (*aIter)->polyline.end() ; ++gIter, ++hIter) {
      
      patReal a1 = gIter->longitudeInDegrees ;
      patReal a2 = gIter->latitudeInDegrees ;
      patReal b1 = hIter->longitudeInDegrees ;
      patReal b2 = hIter->latitudeInDegrees ;

      kml << a1 <<"," << a2 << ",0 "  
	  << b1 <<"," << b2 << ",0" << endl ;
    }

		
		    kml << "</coordinates>	" << endl ;
    kml << "                  </LineString>" << endl ;
    kml << "            </Placemark>" << endl ;
  }
  kml<< "</Folder>"<<endl;
  }
  kml<<"</Folder>"<<endl;
  }
  	kml<<"</Folder>"<<endl;
  	
  	kml<<"<Folder>"<<endl;
  	kml<<"<name>GPS Points</name>"<<endl;
  	
  	for (int i=0;i<gpsSequence.size();++i){
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
  		time_t rawtime = gpsSequence[i].getTimeStamp();
	   tm* ptm = gmtime ( &rawtime );
	   char buffer[50];
	   sprintf(buffer,"%4d-%02d-%02dT%02d:%02d:%02dZ",(ptm->tm_year+1900), (ptm->tm_mon+1), ptm->tm_mday, (ptm->tm_hour)%24, ptm->tm_min, ptm->tm_sec);
	   kml<<(buffer )<<endl;
  		kml<<"	</when>"<<endl;
  		kml<<"</TimeStamp>"<<endl;
  		//kml<<"<name>GPS:"<<i+1<<"</name>"<<endl;
    	kml << "                  <description> "<< gpsSequence[i] <<"</description>" << endl ;
    	kml << "                  <Point>" << endl ;
    	kml << "                        <coordinates>" 
		<< gpsSequence[i].getGeoCoord()->getKML() << ", 0</coordinates>" << endl ;
    	kml << "                  </Point>" << endl ;
  		kml<<"</Placemark>"<<endl;
  		map<patArc*,patReal>* linkDDR = gpsSequence[i].getLinkDDR();
		for(map<patArc*,patReal>::iterator aIter = linkDDR->begin();aIter!=linkDDR->end();++aIter){
			 kml << "            <Placemark>" << endl ;
			   		kml<<"<TimeStamp>"<<endl;
  		kml<<"	<when>"<<endl;

	   sprintf(buffer,"%4d-%02d-%02dT%02d:%02d:%02dZ",(ptm->tm_year+1900), (ptm->tm_mon+1), ptm->tm_mday, (ptm->tm_hour)%24, ptm->tm_min, ptm->tm_sec);
	   kml<<(buffer )<<endl;
  		kml<<"	</when>"<<endl;
  		kml<<"</TimeStamp>"<<endl;
    patString theName(aIter->first->name) ;
    replaceAll(&theName,patString("&"),patString("and")) ;
    kml << "                  <name>"<< theName <<"</name>" << endl ;
    kml << "                  <description>Arc "<< aIter->first->userId
    <<"DDR Value:"
    << aIter->second
        <<"heading:"
    << aIter->first->attributes["heading"]
	<< " from node "
	<< aIter->first->upNodeId 
	<< " to node "
	<< aIter->first->downNodeId
	<< "</description>" << endl ;
    kml << " " << endl ;
    kml << "                  <Style>" << endl ;
    kml << "                        <LineStyle>" << endl ;
    kml << "                              <color>ff00ffff</color>" << endl ;
    kml << "                              <width>4</width>" << endl ;
    kml << "                        </LineStyle>" << endl ;
    kml << "                  </Style>" << endl ;
    kml << " " << endl ;
    kml << "                  <LineString>" << endl ;
    kml << "                        <coordinates>" << endl ;
    
    list<patGeoCoordinates>::iterator gIter(aIter->first->polyline.begin()) ;
    list<patGeoCoordinates>::iterator hIter(aIter->first->polyline.begin()) ;
    ++hIter ;
    for ( ; hIter != aIter->first->polyline.end() ; ++gIter, ++hIter) {
      
      patReal a1 = gIter->longitudeInDegrees ;
      patReal a2 = gIter->latitudeInDegrees ;
      patReal b1 = hIter->longitudeInDegrees ;
      patReal b2 = hIter->latitudeInDegrees ;

      kml << a1 <<"," << a2 << ",0 "  
	  << b1 <<"," << b2 << ",0" << endl ;
    }

		
		    kml << "</coordinates>	" << endl ;
    kml << "                  </LineString>" << endl ;
    kml << "            </Placemark>" << endl ;
		}
  		kml<<"</Folder>"<<endl;
	}
  		kml<<"</Folder>"<<endl;
    kml << " " << endl ;
  kml << "      </Document>" << endl ;
  kml << "      </kml>" << endl ;


  kml.close() ;
}

void patTripParser::genOd(){
	DEBUG_MESSAGE("OK");
	/*map<patArc*,patReal>* originLinkDDR = gpsSequence.front().getLinkDDR();
	patNode* currNode;
	patNode* originNode;
	patNode* destinationNode;
	patReal mostLikelyDistance = gpsSequence.front().getHorizonAccuracy()/sqrt(2);
	patReal leastDistance = 10000;
	for(map<patArc*,patReal>::const_iterator iter = originLinkDDR->begin();iter!=originLinkDDR->end();++iter){

		currNode = theNetwork->getNodeFromUserId(iter->first->upNodeId);
		patGeoCoordinates* upGeo = &(currNode->geoCoord);
		patGeoCoordinates* downGeo = &(theNetwork->getNodeFromUserId(iter->first->downNodeId)->geoCoord);
		map<char*,patReal> distance = gpsSequence.front().distanceTo(upGeo,downGeo);
		if (distance["link"]<leastDistance){
			originNode = currNode;		
		}
		//map<patNode*,patReal>::iterator found = originDDR.find(currNode) ;	
		//originDDR[currNode] = (found==originDDR.end())?iter->second:(originDDR[currNode]+iter->second);
	}
	*/
	map<patArc*,patReal>* destinationLinkDDR = gpsSequence.back().getLinkDDR();
	//leastDistance = 10000;
	for(map<patArc*,patReal>::const_iterator iter = destinationLinkDDR->begin();iter!=destinationLinkDDR->end();++iter){
		patNode* currNode = theNetwork->getNodeFromUserId(iter->first->downNodeId);
		
		/*patGeoCoordinates* upGeo = &(currNode->geoCoord);
		patGeoCoordinates* downGeo = &(theNetwork->getNodeFromUserId(iter->first->downNodeId)->geoCoord);
		map<char*,patReal> distance = gpsSequence.back().distanceTo(upGeo,downGeo);
		if (distance["link"]<leastDistance){
			destinationNode = currNode;		
		}
		*/
		DEBUG_MESSAGE("destination. "<< currNode->internalId<< ","<<currNode->geoCoord) ;
		map<patNode*,patReal>::iterator found = originDDR.find(currNode) ;	
		destinationDDR[currNode] = (found==originDDR.end())?iter->second:(originDDR[currNode]+iter->second);
	}
	/*
	od = theSample->findOd(patOdJ(originNode,destinationNode));
	if(od == NULL){
			od = theSample->addOd(patOdJ(originNode,destinationNode));
	}
*/

}

void patTripParser::genOdDDR(){
	map<patPathJ*,patPathDDR>* pathDDRs = generatedObservation.getPathDDRs();
	for(map<patPathJ*,patPathDDR>::iterator pathDDRIter = pathDDRs->begin();
					pathDDRIter != pathDDRs->end();
					++pathDDRIter){
		
		patOdJ* odFound = generatedObservation.odDDRs.find(pathDDRIter->first->getOd())->first;
		if (odFound == NULL){
			generatedObservation.odDDRs[odFound] = pathDDRIter->second.getPathValue();
		}
		else{
			generatedObservation.odDDRs[odFound] += pathDDRIter->second.getPathValue();
		}
	}
}

void patTripParser::genDDR(vector< list <  pair<patArc*,patULong> > >* adjList) {

	vector<patGpsPoint>::iterator iter = gpsSequence.begin();
	DEBUG_MESSAGE("first point. "<< gpsSequence[0].getTimeStamp() ) ;
	(*iter).genInitDDR(&(*(iter+1)),theNetwork);
	iter++;	

	for (iter;iter!=gpsSequence.end();++iter){
	DEBUG_MESSAGE("current point. "<< iter->getTimeStamp() ) ;
		(*iter).genSegmentDDR(&(*(iter-1)),theNetwork,adjList);
	}
	
}


vector<patReal> patTripParser::genPathDDR(){
	//DEBUG_MESSAGE("path"<<(*path.front()));
	vector<patReal> DDRValue;	
	DDRValue.assign(gpsSequence.size(),0);

	map<patPathJ*,patPathDDR>* pathDDRs = generatedObservation.getPathDDRs();
	DEBUG_MESSAGE("Path number"<<pathDDRs->size());
	for(map<patPathJ*,patPathDDR>::iterator pathDDRIter = pathDDRs->begin();
					pathDDRIter != pathDDRs->end();
					++pathDDRIter){
		patPathDDR thePathDDR(pathDDRIter->first,&gpsSequence);
			DEBUG_MESSAGE("generate path ddr using time space algorithm" ) ;
		thePathDDR.timeSpaceAlgorithm(theNetwork);
		pathDDRIter->second = thePathDDR;
	}

	return DDRValue;
}

patObservation patTripParser::rtnObservation(){
	return generatedObservation;
}