#ifndef patNBParameters_h
#define patNBParameters_h
  
#include "patType.h"
#include "patString.h"
#include "patError.h"
#include <map> 
class patNBParameters{
	
	public:
		patNBParameters();
		patNBParameters(patString fname);
		
		patBoolean readFile(patString fname,patError*& err);
		static patNBParameters* the() ;
		patBoolean setParam(patString name,patString value,patString type);
		
		patBoolean getParam(patString name, patReal* value);
		patBoolean getParam(patString name, patULong* value);
		patBoolean getParam(patString name, patString* value);
		void showAll();
		
		void init(patError*& err);
	static patNBParameters* ins;
	
	patReal networkAccuracy;
	
		
		patULong maxNbrOfNodesOrig;
		patULong maxNbrOfNodesOrigin;
		
		patString dataDirectory;
		//patPathJ
		
		patReal uTurnAngle;
		patReal leftTurnAngle;
		patReal rightTurnAngle;
		patReal straightTurnAngle;
		//patTripParser
		patReal maxHorizonAccuracy;
		patReal maxHeadingAccuracy;
		patReal maxSpeedAccuracy;
		patULong minNbrOfGeneratedPath;
		patULong maxStrangeHeading;
		patReal calStrangeSpeedRatio;
		patReal calStrangeSpeedVarianceA;
		patReal calStrangeSpeedVarianceB;
		
		patULong doMapMatching;
		//patGpsPoint.cc
		//patGpsPoint.cc
		patULong maxDomainSize;
		patReal minNormalSpeed;
		patReal minAverageSpeedRatio;
		patReal maxNormalSpeedHeading;
		
		patULong maxDomainSizeOrig;
		patULong maxDomainSizeDest;//pattripParser
		patReal minNodeDDR;
		patReal minArcDDR;
		
		patReal maxHeadingGPSArc;
	patReal maxDistanceGPSArc;
	patReal maxDistanceGPSLoc;
		patReal zoneRadius;
		patReal zoneTime;
		patReal maxDistanceRatioSP;
		patULong selectDDRByDistance;
		
	//traffic model
	patReal maxMotorSpeed;
	patReal zeroSpeedProba0;
	patReal zeroSpeedProba1;
	patReal zeroSpeedProba2;
	patReal pZeroSpeedRatio;
	patReal pZeroLambda;
	
		//routeChoice_syn
		patULong pathSamplingAlgo;
		//patOd
		patReal kumaA;
		patReal kumaB;
		patULong maxNumberOfGeneratedPaths;
		patULong maxTrialsForRandomWalk;
		patReal pathSizeGamma;
		//patArcTransition
		patReal leftTurnPenalty;
		patReal goStraightPenalty;
		patReal rightTurnPenalty;
		patReal stopPenalty;
		patULong randomSeed;
		patULong nbrOfIntegrationCalls;
		patString integrationMethod;
	
	patReal tmcw;
	patReal tmclambda;
	patReal tmcmu;
	patReal tmcsigma;
	
		//patWriteBiogemeOutput
		patString lengthUnit;
		patULong biogemeEstimationDraws;
		
		//routeChoice_syn
		patULong simNumberOfHorizonArcs;
		patULong simNumberOfTrips;
		patReal simHorizonArcLength;
		patReal simTurnUp;
		patReal simSpeed;
		patReal simNetworkUpDownDistance;
		patReal simSpeedDeltaRatio;
		
		//patPathProbaAlgoV4
		patReal pointProbaPower;
		patString lowSpeedAlgo;
		
		//patSimulator
		patULong doSimulation;
		patReal latlngOffsetRange;
		patReal simDelayatIntersection;
		patReal simNetworkScale;
		//patTripGraph
		patULong selectBestPaths;
		patULong selectWorstPaths;
		patULong selectShortestPaths;
		patReal pathLengthCeil;
		//patPathDevelop
		patReal maxPathLengthRatio;
		patReal minPathInterLength;
		patReal maxPathBeginLength;
		patString selectPathCte;
		patReal selectPathInversePercent;
		patReal selectPathCdfThreshold;
		patString algoInSelection;
		patULong minGeneratedInterMediatePath;
		patReal selectImportantDDRCdf;
		patULong minDomainSize;
		patReal minDomainDDRCdf;
		
		//patMapMatching,patMapMatchingRoute
		
		patReal maxGapDistance;
		patReal maxGapTime;
		patReal initialSearchRadius;
		patReal searchRadiusIncrement;
		patReal arcFreeFlowSpeed;
		patReal routeJoiningQualityThreshold;
		patULong minNbrOfStartNodes;
		patULong maxNbrOfCandidates;
		patULong minNbrOfGpsPerSegment;
		
		//patArc,
		patULong stepsPriority;
		patULong footwayPriority;
		patULong cyclewayPriority;
		patULong primary_linkPriority;
		patULong trunk_linkPriority;
		patULong motorway_linkPriority;
		patULong bridlewayPriority;
		patULong residentialPriority;
		patULong unclassifiedPriority;
		patULong tertiaryPriority;
		patULong secondaryPriority;
		patULong primaryPriority;
		patULong trunkPriority;
		patULong motorwayPriority;
		patULong railwayPriority;
		patULong otherRoadTypePriority;

                patULong doSensitivityAnalysis;
                patULong doProbabilisticMapMatching;
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
	patULong exportDDR;

        patString resultPath;

        patString osm_xpi_url;
                patString OsmNetworkFileName;

                patReal newGpsSamplingInterval;
                patULong newGpsSamplingIntervalTestBase;
                patULong repeatRuns;
		private:
		
		 map<patString,pair<patString, patString> >  params;
		
};

#endif
