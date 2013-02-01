//----------------------------------------------------------------
// File: patTrip.cc
// Author:
// Creation: 17 March 2009
//----------------------------------------------------------------
#include <vector>

#include "patType.h"
#include "patConst.h"
#include <fstream>
#include <time.h>
#include "patTrip.h"
#include "patDisplay.h"
patTrip::patTrip(patNetwork* network,vector< list <  pair<patArc*,patULong> > >* theAdjList)
{
	theNetwork = network;
	adjList=theAdjList;
} 
void patTrip::setMapBounds(patReal minLat, 
			      patReal maxLat, 
			      patReal minLon, 
			      patReal maxLon) {
  minLatitude = minLat ;
  maxLatitude = maxLat ;
  minLongitude = minLon ;
  maxLongitude = maxLon ;
}
void patTrip::newTrip(patULong theUserId,
			  patULong theTipId,
			  patULong theStartTime){

	clean();
}
void patTrip::clean(){
	gpsSequence.clear();
	originDDR.clear();
	destinationDDR.clear();

   pathList.clear();
  pathDDRs.clear();

}

patBoolean patTrip::addPoint(const patGpsPoint theGpsPoint) {
	gpsSequence.push_back(theGpsPoint);

	return patTRUE;
}
void patTrip::endofTrip(patULong theEndTime){
	endTime=theEndTime;
	DEBUG_MESSAGE("Prepare to generate DDR. " ) ;
	genDDR();
	DEBUG_MESSAGE("Prepare to generate OD DDR. " ) ;
	genOdDDR();
	DEBUG_MESSAGE("Prepare to generate paths. " ) ;
	genPath();
	DEBUG_MESSAGE("Prepare to write files. " ) ;
	writeToFile();
	
	DEBUG_MESSAGE("A trip have been created. " ) ;
}
void patTrip::writeToFile(){
	ofstream outfile("output.out");
	DEBUG_MESSAGE("path number:"<<pathList.size());
	/*
	for (int i=0;i<pathList.size();++i){
			//DEBUG_MESSAGE(i);
		for (list<patArc*>::iterator iter= pathList[i].begin();iter!=pathList[i].end();++iter){
			outfile<<(*iter)->userId<<",";
					
		}
		outfile<<endl;

		for (int j=0;j<pathDDRs[i].size();++j){
			outfile<<pathDDRs[i][j]<<",";		
		}

		outfile<<endl;
	}
	*/
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
	}
}
void patTrip::writeToKML(patString fileName, patError*& err){
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
  for (map<pair<patNode*,patNode*>,vector<list<patArc*>  >::iterator iter = pathList.begin();iter != pathList.end();++iter){
	 kml<< "<Folder>";
	 kml<<"<name>"<<endl;
	 kml<<"Origin:"<<(*(iter->first->first))->userId<<",Destination:"<<(*(iter->first->first))<<endl;
	 kml<<"</name>"<<endl;
	for (i=0;i<iter->second.size();++i){
  kml<< "<Folder>";
   kml<< "<description>";
  patReal pathDDRValue=1;
  			for (int j=0;j<pathDDRs[*(iter->first)][i].size();++j){
			pathDDRValue*=pathDDRs[*(iter->first)][i][j];		
		}
		kml<<pathDDRValue;
  kml<< "</description>";
		for (list<patArc*>::iterator aIter= iter->second.begin();aIter!=iter->second.end();++aIter){
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
  		time_t rawtime = gpsSequence[i].timeStamp;
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
		<< gpsSequence[i].geoCoord.getKML() << ", 0</coordinates>" << endl ;
    	kml << "                  </Point>" << endl ;
  		kml<<"</Placemark>"<<endl;
  		
		for(map<patArc*,patReal>::iterator aIter = gpsSequence[i].linkDDR.begin();aIter!=gpsSequence[i].linkDDR.end();++aIter){
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
void patTrip::genOdDDR( ){
	DEBUG_MESSAGE("OK");
	map<patArc*,patReal> originLinkDDR = gpsSequence.front().linkDDR;
	patNode* currNode;
	patReal mostLikelyDistance = gpsSequence.front().accuracy/sqrt(2);
	patReal leastDistance = 10000;
	for(map<patArc*,patReal>::const_iterator iter = originLinkDDR.begin();iter!=originLinkDDR.end();++iter){

		currNode = theNetwork->getNodeFromUserId(iter->first->upNodeId);
		patGeoCoordinates* upGeo = &(currNode->geoCoord);
		patGeoCoordinates* downGeo = &(theNetwork->getNodeFromUserId(iter->first->downNodeId)->geoCoord);
		map<char*,patReal> distance = gpsSequence.front().distanceTo(upGeo,downGeo);
		if (distance["link"]<leastDistance){
			originNode == currNode->internalId;		
		}
		//map<patNode*,patReal>::iterator found = originDDR.find(currNode) ;	
		//originDDR[currNode] = (found==originDDR.end())?iter->second:(originDDR[currNode]+iter->second);
	}
	
	map<patArc*,patReal> destinationLinkDDR = gpsSequence.back().linkDDR;
	leastDistance = 10000;
	for(map<patArc*,patReal>::const_iterator iter = destinationLinkDDR.begin();iter!=destinationLinkDDR.end();++iter){
		currNode = theNetwork->getNodeFromUserId(iter->first->downNodeId);
		patGeoCoordinates* upGeo = &(currNode->geoCoord);
		patGeoCoordinates* downGeo = &(theNetwork->getNodeFromUserId(iter->first->downNodeId)->geoCoord);
		map<char*,patReal> distance = gpsSequence.back().distanceTo(upGeo,downGeo);
		if (distance["link"]<leastDistance){
			destinationNode == currNode->internalId;		
		}
		
		//DEBUG_MESSAGE("destination. "<< currNode->internalId<< ","<<currNode->geoCoord) ;
		//map<patNode*,patReal>::iterator found = originDDR.find(currNode) ;	
		//destinationDDR[currNode] = (found==originDDR.end())?iter->second:(originDDR[currNode]+iter->second);
	}
}
void patTrip::genDDR() {

	vector<patGpsPoint>::iterator iter = gpsSequence.begin();
	DEBUG_MESSAGE("first point. "<< gpsSequence[0].getTimeStamp() ) ;
	(*iter).genInitDDR(&(*(iter+1)),theNetwork);
	iter++;	

	for (iter;iter!=gpsSequence.end();++iter){
	DEBUG_MESSAGE("current point. "<< iter->getTimeStamp() ) ;
		(*iter).genSegmentDDR(&(*(iter-1)),theNetwork,adjList);
	}
	
}
void patTrip::genPath(){
	
	for(map<patNode*, patReal>::iterator destIter = destinationDDR.begin();destIter != destinationDDR.end();++destIter){
		list<list<patULong> > pathTemp;

		connectPoints(destIter->first->internalId,gpsSequence.size()-1,&pathTemp);	
	}
	
	//list<list<patULong> > pathTemp;
	//connectPoints(destinationNode,gpsSequence.size()-1,&pathTemp);	
	for (map<pair<patNode*,patNode*>,vector<list<patArc*>  >::iterator iter = pathList.begin();iter != pathList.end();++iter){
		for (i=0;i<pathList->second.size();++i){
			pairDDRs[*(iter->first)][i]= getPathDDR((*(iter->second))[i]);	
		}
		
	}

}
vector<patReal> patTrip::getPathDDR(list<patArc*> path){
	//DEBUG_MESSAGE("path"<<(*path.front()));
	vector<patReal> DDRValue;	
	DDRValue.assign(gpsSequence.size(),0);
	patReal pathLength = 0;
	for (list<patArc*>::iterator iter= path.begin();iter!=path.end();++iter){
		pathLength+=(*iter)->length;
	}
	for (int i=0;i<gpsSequence.size();++i){

		for (map<patArc*,patReal>::iterator iter=gpsSequence[i].linkDDR.begin();iter!=gpsSequence[i].linkDDR.end();++iter){
			for (list<patArc*>::iterator iter2= path.begin();iter2!=path.end();++iter2){
				if(*iter2 == iter->first){
					DDRValue[i]+=iter->second;			
				}
			}
			
		}	
		//DEBUG_MESSAGE(DDRValue[i]);
	}

	return DDRValue;
}
void patTrip::recordPath(list<list<patULong> >* pathTemp){
	list<patArc*> path;
	patNode* originNode = theNetwork->internalNodes[pathTemp->front().front()];
	patNode* destinationNode = theNetwork->internalNodes[pathTemp->back().back()];
	
	patULong upNodeId=theNetwork->internalNodes[pathTemp->front().front()]->userId;
	for(list<list<patULong> >::iterator iter1=pathTemp->begin();iter1!=pathTemp->end();++iter1){
		list<patULong>::iterator iter2=iter1->begin();		
		if (iter1==pathTemp->begin()){
			iter2++;
		}
		for(iter2;iter2!=iter1->end();++iter2){
				patULong downNodeId=theNetwork->internalNodes[*iter2]->userId;
				path.push_back(theNetwork->getArcFromNodesUserId(upNodeId,downNodeId));
				upNodeId = downNodeId;
		}
	}
	pair<patNode*,patNode*> OD = pair<patNode*,patNode*>(originNode,destinationNode);
	if(pathList.find(OD)==pathList.end()){
		pathDDRs[OD] = vector<vector<patReal> >();
		pathList[OD] = vector<list<patArc*>  >();
	}

	pathList[OD].push_back(path);
	pathDDRs[OD].push_back(0);
}
void patTrip::connectPoints(patULong endNodeId,patULong hierarchy,list<list<patULong> >* pathTemp) {

	for (int i=0;i<gpsSequence[hierarchy].predecessor[endNodeId].size();++i){
		pathTemp->push_front(gpsSequence[hierarchy].predecessor[endNodeId][i].second);
		if(hierarchy==0){

				recordPath(pathTemp);

		}else{
			connectPoints(gpsSequence[hierarchy].predecessor[endNodeId][i].first,hierarchy-1,pathTemp);
		}			
		pathTemp->pop_front();
	}
}