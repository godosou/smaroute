#ifndef patPathProbaAlgoV4_h
#define patPathProbaAlgoV4_h

#include "patType.h"
#include <map>
#include <vector>
class patPathJ;
class patGpsPoint;
class patNetwork;
class patArc;
class patArcTransition;

class patPathProbaAlgoV4 {
public:
    /**
     * Constructor
     * @param thePath, path to be parsed
     * @param theGpsSequence, sequence of GPS points
     * @param theNetwork, the base network
     */
    patPathProbaAlgoV4(patPathJ* thePath,
            vector<patGpsPoint*>* theGpsSequence,
            patNetwork* theNetwork,
            set<patArcTransition>* theArcTranSet,
            vector<patGpsPoint>* theOriginalGpsPoints);
    /**
     * parse the init arc, set probability of init arc as 1.
     */
    void initArc();

    patReal getPointSimpleDDR(patULong g);
    patReal firstPointProba(patString algoType);
    patReal run(patString type);
    patReal run_raw();
    patReal calDistance(patULong j);
    patReal calMapMatchingPoint(patULong j);
    patReal run_MapMatching();
    patReal run_dist();
    patReal run_ts(patString algoType);
    /**
     * Calculate probability for GPS point
     * @param j, the index of the GPS point
     * return probability value
     */
    patReal calPointProba(patString algoType, patULong j);


    /**
     * Calculate probability of point-arc
     *@param j, the GPS point index
     *@param arcDDR, the arc and its ddr value
     * return value, probabilty value
     */
    patReal calPointArcProba(patString algoType, patULong j, pair<patArc*, patReal> arcDDR);


    patReal calPathProba();

protected:
    vector<patReal> proba;
    vector<patGpsPoint*>* gpsSequence;

    vector<patGpsPoint>* originalGpsPoints;
    patNetwork* baseNetwork;
    patPathJ* path;
    patReal initArcProba;
    vector<map<patArc*, patReal> > pointArcProba;
    set<patArcTransition>* arcTranSet;
};


#endif

