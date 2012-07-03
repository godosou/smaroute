/*
 * File:   patReadPathFromKML.h
 * Author: jchen
 *
 * Created on May 2, 2011, 11:29 AM
 */

#ifndef PATREADPATHFROMKML_H
#define	PATREADPATHFROMKML_H
#include "patNetwork.h"
#include "patType.h"
#include "patString.h"
#include "patError.h"
#include <set>
#include <vector>
#include <map>
#include <list>
#include "patPathJ.h"

class patReadObservationFromKML {
public:
    patReadPathFromKML(string file_name,patNetworkElements* network);

    bool parseFile(patError*& err);

    set<patPathJ> getPathSet();
    virtual ~patReadObservationFromKML();
    set<patPathJ> path_set;
private:
    patNetworkElements* m_network;
    string m_file_name;

};

#endif	/* PATREADPATHFROMKML_H */

