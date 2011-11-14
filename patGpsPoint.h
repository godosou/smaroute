//----------------------------------------------------------------
// File: patGpsPoint.h
// Author:
// Creation: Thu March 17
//----------------------------------------------------------------

#ifndef patGpsPoint_h
#define patGpsPoint_h

#include "patGeoCoordinates.h"
#include "patType.h"
#include "patString.h"
#include "patArc.h"
#include "patNode.h"
#include "patNetwork.h"
#include "dataStruct.h"
class patArcTransition;
class patPathDevelop;
class patPathJ;
class patTrafficModel;
class patMeasurementModel;

struct base_params{
	struct gps_params gps;
	struct network_params network;
	patTrafficModel* TM;
	patMeasurementModel* MM;
};

class patTripGraph;
class patGpsPoint {

  friend class patTripParser ;
  friend class patShortestPathAlgoRange ;
  
friend bool operator<(const patGpsPoint& g1, const patGpsPoint& g2);
  friend ostream& operator<<(ostream& str, const patGpsPoint& x) ;

 public:
  /**
     @param theTimestamp Unix timestamp of the GPS point
     @param lat Latitude of the GPS point
     @param lon Longitude of the GPS point
     @param theSpeed Speed of the GPS point
		@param theAccuracy Horizontal accuracy of the GPS point

   */
	 patGpsPoint(patULong theUserId,
		patULong theTimeStamp, 
 		patReal lat,
 		patReal lon,
 		patReal theSpeed,
 		patReal theAccuracy,
 		patReal theHeading,
		patReal theVerticalAccuracy,
		patReal theSpeedAccuracy,
		patReal theHeadingAccuracy,
        map<patString, patReal>* theAlgoParams) ;
	 patReal distanceTo( const  patNode* theNode)  ;
	 patULong getTimeStamp() const;

	map<char*, patReal> distanceTo(  patGeoCoordinates* startCoord, patGeoCoordinates* nextCoord) ;
	void genLinkDDR( patGpsPoint* prevGpsPoint, patNetwork* theNetwork)  ; 
	void genLinkDDR( patNetwork* theNetwork)  ; 
	void genNodeDDR( patNetwork* theNetwork)  ; 
	void genInitDDR(patGpsPoint* nextGpsPoint,patNetwork* theNetwork); 
	patReal calHeading(const patGeoCoordinates* startCoord,const patGeoCoordinates* nextCoord);
	map<patArc*,patReal>*  getLinkDDR();
	map<patNode*,patReal>* getNodeDDR();
	patReal getMaxSpeed(patGpsPoint* prevGpsPoint);
	patReal getDistanceCeilFromPrevDDR(patGpsPoint* prevGpsPoint);
	set<patULong> getStartNodes(patNetwork* theNetwork) ;
	set<patULong> getEndNodes(patNetwork* theNetwork) ;
	patReal measureDDR( map<char*,patReal> theDistance,struct link_ddr_range ldr) ;
	patReal measureDDR( patReal theDistance) ;
	patReal arcHeading(const patGeoCoordinates* startCoord,const patGeoCoordinates* nextCoord) ;
	patReal getSpeed() ;
	patBoolean detLinkDDR(patArc* theArc, patNode* upNode, patNode* downNode);
	patBoolean detLinkDDR(patArc* theArc, patULong upNodeId,patULong downNodeId,patNetwork* theNetwork);
	patBoolean detLinkDDR(patArc* theArc,patNetwork* theNetwork);
	void genSegmentDDR(patGpsPoint* prevGpsPoint,patNetwork* theNetwork, vector< list <  pair<patArc*,patULong> > >* adjList);
	patGeoCoordinates getGeoCoord() const;
	patReal getHorizonAccuracy();
	patGeoCoordinates* getGeoCoord();
	map<char*, patReal>  distanceTo(patNetwork* theNetwork, patArc* theArc);
void selectInitByDDR(patReal lowerBound, patReal upperBound);
	patReal calDDRSum();
void selectInitByName(set<patString>* arcName);
	patReal getDDRSum();
void connectDDRArcs(patTripGraph* theTripGraph);
void selectInitBySortDDR(patULong k);


void selectInitByOrigNode(set<patULong> origId);
	void genInitDDRV2(patGpsPoint* nextGpsPoint,patNetwork* theNetwork,patTripGraph* theTripGraph);
	
	void genSegmentDDRV2(patGpsPoint* prevGpsPoint,
			patNetwork* theNetwork, 
			vector< list <  pair<patArc*,patULong> > >* adjList,
			patTripGraph* theTripGraph,
			set<patArcTransition>* arcTranSet,
			vector<patGpsPoint>* gpsSequence,
					set<patArc*>* interArcs,
					patBoolean stopGps);
	patBoolean detReverseArc(patArc* aArc, patArc* bArc);
	patArc* getReverseArc(patArc* aArc,patNetwork* thNetwork);
	patBoolean inReverseArc(patArc* aArc,patGpsPoint* prevGpsPoint,patNetwork* thNetwork);
	patReal getDDRValue(patArc* aArc);
void traceBackDDRArcs_V1(patGpsPoint* prevGpsPoint,
				patNetwork* theNetwork,
					vector< list <  pair<patArc*,patULong> > >* adjList,
						set<patArc*> inherentDDR

				);
set<patArc*> detInherentDDR(patGpsPoint* prevGpsPoint,patNetwork* theNetwork);
map<patULong,set<patArc*> > detAssocDownNodes(map<patArc*,patReal> ddrMap);
void genSegmentDDRV2_2(patGpsPoint* prevGpsPoint,
					patNetwork* theNetwork, 
					vector< list <  pair<patArc*,patULong> > >* adjList,
					patTripGraph* theTripGraph,
					set<patArc*>* interArcs);
					
void genDDRFromPaths(set<patPathJ>* pathSet,patNetwork* theNetwork);					
void genSegmentDDRV3(patGpsPoint* prevGpsPoint,
					patNetwork* theNetwork, 
					vector< list <  pair<patArc*,patULong> > >* adjList,
					patPathDevelop* thePathDevelop,
					vector<patGpsPoint>* gpsSequence,
	vector<patGpsPoint*>* lowSpeedGpsPoints);
set<patArc*>  getDomainSet();
//patBoolean isInArcDomain(patArc* aArc);
pair<patArc*,patReal> isInArcDomain(patArc* aArc);
void selectInitById(set<patULong>* arcId);
patReal calLinkDDR(patArc* theArc,patNetwork* theNetwork);
patReal getArcDDRValue(patArc* theArc,patNetwork* theNetwork);
patReal getSpeedAccuracy() ;
patReal getSpeedMS() ;
patReal getSpeedAccuracyMS() ;
patReal getHeading();
patReal getHeadingAccuracy();
void lowSpeedDDR(set<patArc*>* arcSet,patNetwork* theNetwork);
pair<patBoolean,patReal> isGpsPointInZone(patGpsPoint* prevGpsPoint, patGpsPoint* aGpsPoint);
map<patGpsPoint*,patReal> detGpsPointsInZone(patGpsPoint* prevGpsPoint,vector<patGpsPoint>* gpsSequence);
pair<patReal, patReal> calSpeedInZone(patGpsPoint* prevGpsPoint, vector<patGpsPoint>* gpsSequence);
void setGpsParams(struct gps_params * p,patGpsPoint* prevGpsPoint, vector<patGpsPoint>* gpsSequence);
set<patArc*> getDDRArcSet();
patReal calPerpendicularFootOnArc(map<char*, patReal> distanceToArc);
void setType(patString theType);
patString getType();
patReal distanceTo(patGpsPoint* aGpsPoint);

void selectDomainByNumber(patULong n); 
patBoolean emptyDomain();
set<patArc*> selectDomainByCdf(patNetwork* theNetwork);
set<patArc*> selectDomainByDistance(patNetwork* theNetwork);
void finishDDR(patULong k);

patReal getLatitude(); 
map<patReal,set<patArc*> > sortDDRByValue();

void updateLinkDDR(set<patArc*>* newDomain);
patReal getLongitude();

patBoolean isArcInDomain(patArc* theArc);
void setHeading(patReal v);
void setSpeed(patReal v);
patReal calHeading(const patGpsPoint* nextGpsPoint);
patReal calHeading(patGpsPoint* prevGpsPoint, patGpsPoint* nextGpsPoint);
	struct link_ddr_range detLinkDDR_Range(map<char*, patReal> distanceLink);
void setSpeedAccuracy(patReal v);
	struct link_ddr_range getLinkDDRRange(patArc* arc);
	map<patArc*, struct link_ddr_range> getLinkDDRRange();
        void setAlgoParams(map<patString, patReal>* theAlgoParams);
        void setAlgoParams(patString paramName, patReal paramValue);
void setSpeedType();

 protected:
 patULong userId;
  patULong timeStamp ;
  patReal speed ;
 patString type;
 patReal networkAccuracy;
    patReal    minPointDDR;
	patReal horizonAccuracy;
        patReal coordsHorizontalAccuracy;
	patReal speedAccuracy;
	patReal heading;
	patReal headingAccuracy;
	patReal verticalAccuracy;
	patReal distanceThreshold;
	patReal distanceThreshold_Loc;
        patReal minNormalSpeed;
        patReal maxNormalSpeedHeading;
 patGeoCoordinates geoCoord ;
map<patArc*,patReal> linkDDR;
	map<patArc*,struct link_ddr_range> linkDDR_Range;
map<patNode*,patReal> nodeDDR;
patReal DDRSum;
	map<patULong,vector<pair<patULong,list<patULong> > > > predecessor;
	patReal ddrDecreaseFactor;
} ;


#endif
