#ifndef patNBParameters_h
#define patNBParameters_h

#include "patType.h"
#include "patString.h"
#include "patError.h"
#include <map>
#include "dataStruct.h"

class patNBParameters {

public:
	patNBParameters();
	patNBParameters(patString fname);

	bool readFile(patString fname, patError*& err);
	static patNBParameters* the();
	bool setParam(patString name, patString value, patString type);

	bool getParam(patString name, double* value);
	bool getParam(patString name, unsigned long* value);
	bool getParam(patString name, patString* value);
	void showAll();
	bool setTrafficModelParam();

	void init(patError*& err);
	static patNBParameters* ins;

	double networkAccuracy;

	unsigned long maxNbrOfNodesOrig;
	unsigned long maxNbrOfNodesOrigin;

	patString dataDirectory;
	//patPathJ

	double uTurnAngle;
	double leftTurnAngle;
	double rightTurnAngle;
	double straightTurnAngle;
	//patTripParser
	double maxHorizonAccuracy;
	double maxHeadingAccuracy;
	double maxSpeedAccuracy;
	unsigned long minNbrOfGeneratedPath;
	unsigned long maxStrangeHeading;
	double calStrangeSpeedRatio;
	double calStrangeSpeedVarianceA;
	double calStrangeSpeedVarianceB;

	unsigned long doMapMatching;
	//patGpsPoint.cc
	//patGpsPoint.cc
	unsigned long maxDomainSize;
	double minNormalSpeed;
	double minAverageSpeedRatio;
	double maxNormalSpeedHeading;

	unsigned long maxDomainSizeOrig;
	unsigned long maxDomainSizeDest; //pattripParser
	double minNodeDDR;
	double minArcDDR;

	double maxHeadingGPSArc;
	double maxDistanceGPSArc;
	double maxDistanceGPSLoc;
	double zoneRadius;
	double zoneTime;
	double maxDistanceRatioSP;
	unsigned long selectDDRByDistance;

	//traffic model
	double maxMotorSpeed;
	double zeroSpeedProba0;
	double zeroSpeedProba1;
	double zeroSpeedProba2;
	double pZeroSpeedRatio;
	double pZeroLambda;

	//routeChoice_syn
	unsigned long pathSamplingAlgo;
	//patOd
	double kumaA;
	double kumaB;
	unsigned long maxNumberOfGeneratedPaths;
	unsigned long maxTrialsForRandomWalk;
	double pathSizeGamma;
	//patArcTransition
	double leftTurnPenalty;
	double goStraightPenalty;
	double rightTurnPenalty;
	double stopPenalty;
	unsigned long randomSeed;
	unsigned long nbrOfIntegrationCalls;
	patString integrationMethod;

	double tmcw;
	double tmclambda;
	double tmcmu;
	double tmcsigma;

	//patWriteBiogemeOutput
	patString lengthUnit;
	unsigned long biogemeEstimationDraws;

	//routeChoice_syn
	unsigned long simNumberOfHorizonArcs;
	unsigned long simNumberOfTrips;
	double simHorizonArcLength;
	double simTurnUp;
	double simSpeed;
	double simNetworkUpDownDistance;
	double simSpeedDeltaRatio;

	//patPathProbaAlgoV4
	double pointProbaPower;
	patString lowSpeedAlgo;

	//patSimulator
	unsigned long doSimulation;
	double latlngOffsetRange;
	double simDelayatIntersection;
	double simNetworkScale;
	//patTripGraph
	unsigned long selectBestPaths;
	unsigned long selectWorstPaths;
	unsigned long selectShortestPaths;
	double pathLengthCeil;
	//patPathDevelop
	double maxPathLengthRatio;
	double minPathInterLength;
	double maxPathBeginLength;
	patString selectPathCte;
	double selectPathInversePercent;
	double selectPathCdfThreshold;
	patString algoInSelection;
	unsigned long minGeneratedInterMediatePath;
	double selectImportantDDRCdf;
	unsigned long minDomainSize;
	double minDomainDDRCdf;

	//patMapMatching,patMapMatchingRoute

	double maxGapDistance;
	double maxGapTime;
	double initialSearchRadius;
	double searchRadiusIncrement;
	double arcFreeFlowSpeed;
	double routeJoiningQualityThreshold;
	unsigned long minNbrOfStartNodes;
	unsigned long maxNbrOfCandidates;
	unsigned long minNbrOfGpsPerSegment;

	//patArc,
	unsigned long stepsPriority;
	unsigned long footwayPriority;
	unsigned long cyclewayPriority;
	unsigned long primary_linkPriority;
	unsigned long trunk_linkPriority;
	unsigned long motorway_linkPriority;
	unsigned long bridlewayPriority;
	unsigned long residentialPriority;
	unsigned long unclassifiedPriority;
	unsigned long tertiaryPriority;
	unsigned long secondaryPriority;
	unsigned long primaryPriority;
	unsigned long trunkPriority;
	unsigned long motorwayPriority;
	unsigned long railwayPriority;
	unsigned long otherRoadTypePriority;

	unsigned long doSensitivityAnalysis;
	unsigned long doProbabilisticMapMatching;
	patString SAResultPath;
	patString SAType;
	patString SAPathFolder;
	//kml
	patString gpsIcon;
	patString gpsIconScale;
	patString gpsIconColor;
	patString pathLineColor;
	patString pathLineWidth;
	patString ddrLineColor;
	patString ddrLineWidth;
	unsigned long exportDDR;

	patString resultPath;

	patString osm_xpi_url;
	patString OsmNetworkFileName;

	double newGpsSamplingInterval;
	unsigned long newGpsSamplingIntervalTestBase;
	unsigned long repeatRuns;
	TrafficModelParam CAR_param;
	TrafficModelParam BUS_param;
	TrafficModelParam WALK_param;
	TrafficModelParam BIKE_param;
	TrafficModelParam TRAIN_param;
	TrafficModelParam METRO_param;



	double walkNetworkMinSpeed;
	double walkNetworkMaxSpeed;
	double trainNetworkMinSpeed;
	double trainNetworkMaxSpeed;
	double busNetworkMinSpeed;
	double busNetworkMaxSpeed;
	double carNetworkMinSpeed;
	double carNetworkMaxSpeed;
	double bikeNetworkMinSpeed;
	double bikeNetworkMaxSpeed;

	double minPathTravelTimeRatio;

	double minChangeLengthBackToTheSame;

	double boundingBoxLeftUpLatitude;
	double boundingBoxLeftUpLongitude;
	double boundingBoxRightBottumLatitude;
	double boundingBoxRightBottumLongitude;

private:

	map<patString, pair<patString, patString> > params;

};

#endif
