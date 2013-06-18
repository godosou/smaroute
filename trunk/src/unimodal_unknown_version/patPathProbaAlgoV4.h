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

    double getPointSimpleDDR(unsigned long g);
    double firstPointProba(patString algoType);
    double run(patString type);
    double run_raw();
    double calDistance(unsigned long j);
    double calMapMatchingPoint(unsigned long j);
    double run_MapMatching();
    double run_dist();
    double run_ts(patString algoType);
    /**
     * Calculate probability for GPS point
     * @param j, the index of the GPS point
     * return probability value
     */
    double calPointProba(patString algoType, unsigned long j);


    /**
     * Calculate probability of point-arc
     *@param j, the GPS point index
     *@param arcDDR, the arc and its ddr value
     * return value, probabilty value
     */
    double calPointArcProba(patString algoType, unsigned long j, pair<patArc*, double> arcDDR);


    double calPathProba();

protected:
    vector<double> proba;
    vector<patGpsPoint*>* gpsSequence;

    vector<patGpsPoint>* originalGpsPoints;
    patNetwork* baseNetwork;
    patPathJ* path;
    double initArcProba;
    vector<map<patArc*, double> > pointArcProba;
    set<patArcTransition>* arcTranSet;
};


#endif

