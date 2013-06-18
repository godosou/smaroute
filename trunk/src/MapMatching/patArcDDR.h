#ifndef patArcDDR_h
#define patArcDDR_h

#include "patDDR.h"
#include "patType.h"
#include "dataStruct.h"
struct arc_ddr_params{
double d;
double l;
double e_d;
double std_x;
struct link_ddr_range ldr;
	
};
double errPDF(arc_ddr_params param);
class patArcDDR : public patDDR{
	public:
	
		/**
		*Constructor
		*/
		patArcDDR();
		
		/**
		*Constructor
		*@param theAccuracy accuracy value of arc DDR(data from gps point)
		*/
		patArcDDR( double theAccuracy);
		double errCDF(arc_ddr_params param);

		/**
		*Calculate error density probability
		*@param distance the distance from gps point to arc
		*@param position the postion of point on arc
		*@return the probability that error distance = distance between gps point and a point on arc
		*/
		 double errPDF( double distance, 
								 double position);
								
		/**
		*Calculate error cumulative probability
		*@param distance the distance from gps point to the arc
		*@param position1 terminal point of the arc
		*@param position2 the other terminal of the arc
		*@return probability for the arc observes the gps point
		*/
		 double errCDF( double distance,
								 double position1, 
								 double position2);
	protected:
	
};

#endif
