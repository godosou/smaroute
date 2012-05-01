#ifndef patDDR_h
#define patDDR_h
#include "patType.h"

class patDDR{
	public:
		patDDR();
		patDDR(const double& theAccuracy);
		double errPDFV2(const double& distance);

		virtual double errPDF(const double& distance);
		virtual double errCDF( double& a, 
			 double& b);
	protected:
		double accuracy;
};

#endif

