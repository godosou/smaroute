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

	void init(patError*& err);
	static patNBParameters* ins;

	double networkAccuracy;

	patString observationDirectory;
	patString dataDirectory;
	//patPathJ
	unsigned long allowGetOffAnywhere;
	unsigned long walkOnTrack;
	double uTurnAngle;
	double leftTurnAngle;
	double rightTurnAngle;
	double straightTurnAngle;
	//patTripParser
	double maxHorizonAccuracy;
	double maxHeadingAccuracy;
	double maxSpeedAccuracy;

	unsigned long maxStrangeHeading;
	double calStrangeSpeedRatio;

	unsigned long doMapMatching;


	double minNormalSpeed;
	double minAverageSpeedRatio;
	double maxNormalSpeedHeading;

	double minNodeDDR;
	double minArcDDR;

	double maxHeadingGPSArc;
	double maxDistanceGPSArc;
	double maxDistanceGPSLoc;
	double zoneRadius;
	double zoneTime;
	double maxDistanceRatioSP;

	double proposeStopTime;
	unsigned long selectDDRByDistance;

	//routeChoice_syn
	unsigned long pathSamplingAlgo;
	//patOd
	double kumaA;
	double kumaB;
	unsigned long maxNumberOfGeneratedPaths;
	unsigned long maxTrialsForRandomWalk;
	double pathSizeGamma;
	//patArcTransition

	unsigned long randomSeed;
	unsigned long nbrOfIntegrationCalls;
	patString integrationMethod;


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
	unsigned long selectImportantDDRNumber;
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

	double minDistanceFromLastPoint;

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
	double metroNetworkMinSpeed;
	double metroNetworkMaxSpeed;
	double minPathTravelTimeRatio;

	double walkMaxSpeed;
	double bikeMaxSpeed;
	double minChangeLengthBackToTheSame;

unsigned long speedCapacityCheckPrevious;
	double boundingBoxLeftUpLatitude;
	double boundingBoxLeftUpLongitude;
	double boundingBoxRightBottumLatitude;
	double boundingBoxRightBottumLongitude;


	unsigned long samplePathsByMode;
	unsigned long samplePathsByChange;
	unsigned long samplePathsByDDR;
	string paramFolder;
	unsigned long sampleByDDRWithoutConstraint;
	unsigned long enableACCEL;
	unsigned long enableBT;

	unsigned long enableBusNetwork;
	unsigned long enableCarNetwork;
	unsigned long enableMetroNetwork;
	unsigned long enableTrainNetwork;
	unsigned long enableWalkNetwork;
	unsigned long enableBikeNetwork;

	double modeChangePenalty;

	string coordinatesSystem;

double discardGPSGap;
	string databaseHost;

	unsigned long printKMLEveryIteration;

	unsigned long usePreviousGPS;
	double stationaryPriorMu;


	//MHPathSampling
	unsigned long RANDOMSEED_ELEMENT;
	unsigned long MSGINTERVAL_ELEMENT;
	unsigned long TOTALITERATIONS_ELEMENT;
	unsigned long SAMPLEINTERVAL_ELEMENT;
	unsigned long WARMUP_ITERATIONS;
	unsigned long SAMPLE_COUNT;
	double CUTOFFPROBABILITY_ELEMENT;
	double RELATIVECOSTSCALE_ELEMENT;

	unsigned long samplingWithObs;
	string choiceSetFolder;



	double router_cost_link_scale;
	double router_cost_length_coef;
	double router_cost_sb_coef;

	double utility_link_scale;
	double utility_length_coef;
	double utility_ps_coef;
	double utility_sb_coef;

	double mh_obs_scale;
	double mh_link_scale;
	double mh_length_coef;
	double mh_sb_coef;
	double mh_ps_coef;

	unsigned long nbrOfThreads;

	unsigned long writeBiogemeModelFile;

	unsigned long nbrOfSimulatedErrorPaths;
	unsigned long overwriteSampleFile;
	double errorInSimulatedObservations;

	string choiceSetInBiogemeData;

	string pathSampleAlgorithm;

	unsigned long OriginId;
	unsigned long DestinationId;
private:

	map<patString, pair<patString, patString> > params;

};

#endif

