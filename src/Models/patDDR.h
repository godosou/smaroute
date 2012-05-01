#ifndef patDDR_h
#define patDDR_h
#include "patType.h"

class patDDR{
	public:
		patDDR();
		patDDR(const patReal& theAccuracy);
		patReal errPDFV2(const patReal& distance);

		virtual patReal errPDF(const patReal& distance);
		virtual patReal errCDF( patReal& a, 
			 patReal& b);
	protected:
		patReal accuracy;
};

#endif

