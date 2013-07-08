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

class patReadPathFromKML {
public:
    patReadPathFromKML(patString a_file_name,patNetwork* a_network);

    patBoolean parseFile(patError*& err);

    set<patPathJ> getPathSet();
    
    patReadPathFromKML(const patReadPathFromKML& orig);
    virtual ~patReadPathFromKML();
    set<patPathJ> path_set;
private:
    patNetwork* network;
    patString file_name;

};

#endif	/* PATREADPATHFROMKML_H */

