/* 
 * File:   patStreetSegment.h
 * Author: jchen
 *
 * Created on April 7, 2011, 11:44 AM
 */

#ifndef PATSTREETSEGMENT_H
#define	PATSTREETSEGMENT_H

#include <list>
#include "patNetwork.h"
#include "patArc.h"
#include "patNode.h"
#include "patError.h"
#include "patType.h"
class patStreetSegment{
  friend bool operator==(const patStreetSegment& path1, const patStreetSegment& path2) ;

public:
    patStreetSegment(patNetwork* aNetwork);
    patStreetSegment(patNetwork* aNetwork, list<patArc*> anArcList);
patStreetSegment(patNetwork* aNetwork, patArc* anArc, patError*& err);
    patNode* getStartNode(patError*& err);
    patNode* getEndNode(patError*& err);
    patArc* getStartArc();
    patArc* getEndArc();
    bool addArcToBack( patArc* anArc);
    bool addArcToFront( patArc* anArc);
    bool deleteArcFromFront();
    bool deleteArcFromBack();
    double getLength();
    bool checkUpwardSingleWay(patNode* aNode, patNode* nextNode);
 unsigned long size();

bool checkDownwardSingleWay(patNode* aNode, patNode* prevNode);

    void generateStreetFromArc( patArc* anArc, patError*& err);
    const list<patArc*>* getArcList() const;
protected:
    list<patArc*> ArcList;
    patNetwork* Network;
};

#endif	/* PATSTREETSEGMENT_H */

