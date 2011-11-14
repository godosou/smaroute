/* 
 * File:   patRetrieveOSMFile.h
 * Author: jchen
 *
 * Created on April 14, 2011, 9:22 PM
 */

#ifndef PATRETRIEVEOSMFILE_H
#define	PATRETRIEVEOSMFILE_H

#include "patType.h"
#include "patGpsPoint.h"
class patRetrieveOSMFile{
public:
    void genBoundingBoxFromGpsPoints(vector<patGpsPoint*> gps_sequence);
protected:
    patString osm_file_name;
    patReal top;
    patReal bottom;
    patReal left;
    patReal right;
}
;

#endif	/* PATRETRIEVEOSMFILE_H */

