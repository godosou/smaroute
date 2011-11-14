#include <fstream>
#include <vector>
#include <list> 
#include "patArc.h"
#include "patPathJ.h"
#include "patOdJ.h"
#include <sstream>
#include "patWriteKML.h"
#include "patDisplay.h"
patWriteKML::patWriteKML(){

}
patString patWriteKML::writePath(patPathJ* path,patString desc){
stringstream kml;
  kml<< "<Folder>"<<endl;
   kml<< "<description>";
   kml<<desc;
  kml<< "</description>";
  list<patArc*>* listOfArcs = path->getArcList();
  //DEBUG_MESSAGE("number of arcs: "<<listOfArcs->size());
for (list<patArc*>::iterator aIter= listOfArcs->begin();
		aIter!=listOfArcs->end();
		++aIter){
	kml << "            <Placemark>" << ' ' ;
    patString theName((*aIter)->name) ;
    replaceAll(&theName,patString("&"),patString("and")) ;
    kml << "                  <name>"<< theName <<"</name>" << ' ' ;
    kml << "                  <description>Arc "<< (*aIter)->userId 
	<< " from node "
	<< (*aIter)->upNodeId 
	<< " to node "
	<< (*aIter)->downNodeId
	<< "</description>" << ' '<<endl ;
    kml << " " << ' ' ;
    kml << "                  <Style>" << endl ;
    kml << "                        <LineStyle>" << endl ;
    kml << "                              <color>4fff0000</color>" << endl ;
    kml << "                              <width>5</width>" << endl ;
    kml << "                        </LineStyle>" << endl ;
    kml << "                  </Style>" << endl ;
    kml << " " << ' ' <<endl;
    kml << "                  <LineString>" << ' ' <<endl;
    kml << "                        <coordinates>" << ' '<<endl ;
    
    list<patGeoCoordinates>::iterator gIter((*aIter)->polyline.begin()) ;
    list<patGeoCoordinates>::iterator hIter((*aIter)->polyline.begin()) ;
    ++hIter ;
    for ( ; hIter != (*aIter)->polyline.end() ; ++gIter, ++hIter) {
      
      patReal a1 = gIter->longitudeInDegrees ;
      patReal a2 = gIter->latitudeInDegrees ;
      patReal b1 = hIter->longitudeInDegrees ;
      patReal b2 = hIter->latitudeInDegrees ;

      kml << a1 <<"," << a2 << ",0 "  
	  << b1 <<"," << b2 << ",0" << ' ' ;
    }

		
		    kml << "</coordinates>	" << ' ' ;
    kml << "                  </LineString>" << ' ' ;
    kml << "            </Placemark>" << ' ' ;
  }
  kml<< "</Folder>"<<' '<<endl;
  
 return kml.str();
}

patString patWriteKML::writeODSample(patOdJ* od){
	
}