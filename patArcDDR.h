#ifndef patArcDDR_h
#define patArcDDR_h

#include "patDDR.h"
#include "patType.h"
#include "patGpsPoint.h"

struct arc_ddr_params{
patReal d;
patReal l;
patReal e_d;
patReal std_x;
struct link_ddr_range ldr;
	
};
patReal errPDF(arc_ddr_params param);
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
		patArcDDR( patReal theAccuracy);
		patReal errCDF(arc_ddr_params param);

		/**
		*Calculate error density probability
		*@param distance the distance from gps point to arc
		*@param position the postion of point on arc
		*@return the probability that error distance = distance between gps point and a point on arc
		*/
		 patReal errPDF( patReal distance, 
								 patReal position);
								
		/**
		*Calculate error cumulative probability
		*@param distance the distance from gps point to the arc
		*@param position1 terminal point of the arc
		*@param position2 the other terminal of the arc
		*@return probability for the arc observes the gps point
		*/
		 patReal errCDF( patReal distance,
								 patReal position1, 
								 patReal position2);
	protected:
	
};

#endif
