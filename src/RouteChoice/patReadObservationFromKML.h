/*
 * File:   patReadPathFromKML.h
 * Author: jchen
 *
 * Created on May 2, 2011, 11:29 AM
 */

#ifndef PATREADOBSERVATIONFROMKML_H
#define	PATREADOBSERVATIONFROMKML_H

#include "patNetworkElements.h"
#include "patObservation.h"

class patReadObservationFromKML {
public:
    patReadObservationFromKML(const patNetworkElements* a_network);

    void parseFile(string file_name, patObservation* observation);

    virtual ~patReadObservationFromKML();
private:
   const patNetworkElements* m_network;

};
#endif	/* PATREADPATHFROMKML_H */

