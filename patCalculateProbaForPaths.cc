/* 
 * File:   patCalculateProbaForPaths.cc
 * Author: jchen
 * 
 * Created on May 2, 2011, 3:28 PM
 */

#include "patCalculateProbaForPaths.h"

#include<sstream>
#include <dirent.h>

#include "patErrMiscError.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patReadPathFromKML.h"
#include "patPathProbaAlgoV4.h"
#include "patArcTransition.h"
#include "patGpsPoint.h"
vector<patString> patCalculateProbaForPaths::fromDirectory(patString directory_name,
        patError*& err) {

    const char* dirName = directory_name.c_str();
    DIR *dip;
    struct dirent *dit;

    DEBUG_MESSAGE("Try to open directory " << dirName);
    if ((dip = opendir(dirName)) == NULL) {
        stringstream str;
        str << "Directory " << dirName << " doesn't exist or no permission to read.";
        err = new patErrMiscError(str.str());
        WARNING(err->describe());
        return vector<patString > ();
    }
    DEBUG_MESSAGE("Direcotry " << dirName << " is now open");
    vector<patString> dirContent;
    unsigned char isFile = 0x8;
    patString kmlext("kml");

    while ((dit = readdir(dip)) != NULL) {
        if (dit->d_type == isFile) {
            patString fileName(dit->d_name);
            if (fileName.find(kmlext) != string::npos) {
                DEBUG_MESSAGE("Found file:" << fileName);
                dirContent.push_back(fileName);
            }

        }
    }


    closedir(dip);
    return dirContent;
}

void patCalculateProbaForPaths::fromFile(patString file_name, patError*& err) {
    patReadPathFromKML* read_paths = new patReadPathFromKML(file_name, network);
    if (read_paths->parseFile(err) != patTRUE) {
        path_set = read_paths->path_set;
    }

    delete read_paths;
    read_paths = NULL;
    return;
}

set<patPathJ> patCalculateProbaForPaths::getPaths(){
	return path_set;
}
void patCalculateProbaForPaths::fromPaths(set<patPathJ> a_path_set){
	path_set=a_path_set;
}

 vector<patReal> patCalculateProbaForPaths::calculateProbas() {
    vector<patReal> proba_results;
    set<patArcTransition>* arc_transition_set = new set<patArcTransition>;
    vector<patGpsPoint*> valid_gps_sequence;
    genGpsDDRFromPaths(&valid_gps_sequence);

    for (set<patPathJ>::iterator iter = path_set.begin();
            iter != path_set.end();
            ++iter) {
        patPathProbaAlgoV4 path_proba(const_cast<patPathJ*> (&(*iter)), &valid_gps_sequence, network, arc_transition_set, gps_sequence);
        patReal proba_value =path_proba.run("complex");
        //DEBUG_MESSAGE(proba_value);
        proba_results.push_back(proba_value);
    }
    delete arc_transition_set;
    arc_transition_set = NULL;
    return proba_results;
}

void patCalculateProbaForPaths::genGpsDDRFromPaths(vector<patGpsPoint*>* valid_gps_sequence) {
    patULong i = 0;
    patULong gps_points_count = gps_sequence->size();
    while (i < gps_points_count) {
        gps_sequence->at(i).genDDRFromPaths(&path_set, network);
        if (!gps_sequence->at(i).emptyDomain()) {
            valid_gps_sequence->push_back(&gps_sequence->at(i));

        }
        i++;
    }
}

patCalculateProbaForPaths::patCalculateProbaForPaths(patNetwork* a_network,
        vector<patGpsPoint>* theOriginalGpsPoints) :
network(a_network),
gps_sequence(theOriginalGpsPoints) {

}

patCalculateProbaForPaths::patCalculateProbaForPaths(const patCalculateProbaForPaths& orig) {
}

patCalculateProbaForPaths::~patCalculateProbaForPaths() {
}

