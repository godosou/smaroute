/* 
 * File:   patCalculateProbaForPaths.h
 * Author: jchen
 *
 * Created on May 2, 2011, 3:28 PM
 */

#ifndef PATCALCULATEPROBAFORPATHS_H
#define	PATCALCULATEPROBAFORPATHS_H
#include "patString.h"
#include "patType.h"
#include "patPathJ.h"
#include "patNetwork.h"
#include "patError.h"
class patGpsPoint;
class patArcTransition;
class patCalculateProbaForPaths {
public:
patCalculateProbaForPaths(patNetwork* a_network,vector<patGpsPoint>* theOriginalGpsPoints);
    vector<patString> fromDirectory(patString directory_name,patError*& err);
    void fromFile(patString file_name,patError*& err);
    void fromPaths(set<patPathJ> a_path_set);
    set<patPathJ> getPaths();
     vector<double> calculateProbas();
    void genGpsDDRFromPaths(vector<patGpsPoint*>* valid_gps_sequence);
    patCalculateProbaForPaths(const patCalculateProbaForPaths& orig);
    virtual ~patCalculateProbaForPaths();
private:
    patNetwork* network;
        set<patPathJ> path_set;

vector<patGpsPoint>* gps_sequence;
};

#endif	/* PATCALCULATEPROBAFORPATHS_H */

