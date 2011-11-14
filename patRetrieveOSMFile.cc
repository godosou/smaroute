#include "patRetrieveOSMFile.h"

void patRetrieveOSMFile::genBoundingBoxFromGpsPoints(vector<patGpsPoint*> gps_sequence){
    top=-90;
    bottom=90;
    left=180;
    right=-180;
    for(vector<patGpsPoint*>::iterator gps_iter = gps_sequence.begin();
            gps_iter!=gps_sequence.end();
            ++gps_iter){
       patReal lat = (*gpsIter)-> getGeoCoord().latitudeInDegrees;
       patReal lon = (*gpsIter)-> getGeoCoord().longitudeInDegrees;
       top = (lat>top)?lat:top;
       bottom = (lat<bottom)?lat:bottom;

       left = (lon<left)?lon:left;
       right =(lon>right)?lon:right;
    }
}
