#ifndef patSimulator_h
#define patSimulator_h

#include "patType.h"
#include <set>
#include <list>
#include "patGeoCoordinates.h"

#include "patUniform.h" 
#include "patParameters.h"
#include "patRandomNumberGenerator.h"
#include "patNormalWichura.h"
#include "patUnixUniform.h"

class patGpsPoint;
class patNetwork;
class patArc;
class patPathJ;

//class patGeoCoordinates;

struct synNode{
	patGeoCoordinates geoCoord;
	patString type;
	patULong id;
};

struct synArc{
	struct synNode* upNode;
	struct synNode* downNode;
	patReal length;
	patReal heading;
	patReal id;
};

struct observation{
	patReal x;
	patReal y;
	patReal ox;
	patReal oy;
};
class patSimulator{
friend   ostream& operator<<(ostream& str, const synArc& x);

public:
  patSimulator(patULong seed);

  patSimulator(patNetwork* aNetwork,patULong seed);
  void genNetwork(patULong nbrOfParArcs, 
		patReal upDownDistance,
		patReal parLength);
		  patReal lengthToDegree_Equator(patReal length);
	  patString genNetwork_grid(patULong nbrOfParArcs,
			patReal upDownDistance,
			patReal parLength,
			patReal changeat,
			 list<patULong>* listOfNodes);
  synNode createNode(patReal lat, patReal lon, patString type, patULong id);
  string numberToString(patULong number);
    string numberToString(patReal number);
  void finalizeNetwork(patString fielNameup
  );
    void genMapBound();
  void writeNetwork(patString fielName);
  patReal assignSignal();
  patReal computePathLength();
 vector<patGpsPoint> travel(patReal theSpeed);
  patReal getWaiting(struct synArc* upArc, struct synArc* downArc);
  patReal getSpeed();
patReal calHeading(struct synArc* onArc) ;
	void setSpeedDelta(patReal aValue);

  patReal nextPosition(patReal speed);
  void calArcsLength();
    void calArcsHeading();
  patReal genDelta(patReal downBound,patReal upBound);
  patReal randomNumber_Normal(patReal delta);
 patReal lengthToDegree(patReal length);
  patGpsPoint genGps(patReal position,struct synArc* onArc,patULong time,patReal currSpeed);
    patGpsPoint genGps_random(patReal position,struct synArc* onArc,patULong time,patReal currSpeed);
	      patReal randomNumber_Normal_biogeme(patReal delta);
	void writeTrack(patString fileName);
  synArc createArc(struct synNode* aNode, struct synNode* bNode,patULong id);
  
	void writeCsv(vector<patGpsPoint>* gpsSequence, patString filename);
	
	
	patGpsPoint genGps_random(patReal position,patArc* onArc,patULong time,patReal currSpeed);
vector<patGpsPoint> travel(patPathJ* thePath, patReal theSpeed);
protected:
set<struct synNode> nodes;
set<struct synArc> arcs;
synNode* orig;

vector<struct observation> gpsRecord;
synNode* dest;

patReal pathDistance;
patReal maxLat;
patReal minLat;
patReal maxLon;
patReal minLon;
list<synArc*> realPath;
patReal deltaUp;
patReal deltaDown;
patReal speed;
patReal speedDelta;
patReal interval;
patUnixUniform ranNumUniform;
patNormalWichura ranNumNormal;
vector<patGpsPoint > trueGps;
patNetwork* theNetwork;
};
#endif

