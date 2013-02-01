/* 
 * File:   patMapMatchingNetwork.h
 * Author: jchen
 *
 * Created on April 6, 2011, 3:49 PM
 */

#ifndef PATMAPMATCHINGNETWORK_H
#define	PATMAPMATCHINGNETWORK_H

class patMapMatchingNetwork{

protected:
  patString name ;
  map<patULong, patArc> theArcs ;
  map<patULong, patNode> theNodes ;
  vector<patArc*> internalArcs ;
  vector<patNode*> internalNodes ;
  set<patOd> theOds ;
  map<pair<patULong, patULong>, patULong > listOfArcsPerPairsOfNodes ;
  patReal minLatitude ;
  patReal maxLatitude ;
  patReal minLongitude ;
  patReal maxLongitude ;
  patReal minimumLabelForShortestPath ;
}

#endif	/* PATMAPMATCHINGNETWORK_H */

