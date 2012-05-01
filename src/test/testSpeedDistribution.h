#include <cxxtest/TestSuite.h>
#include <vector>
#include "patNBParameters.h"
#include "patTransportMode.h"
#include "patError.h"
#include "patDisplay.h"
#include <stdio.h> 
#include "patSpeedDistributions.h"
class MyTestSpeedDistribution : public CxxTest::TestSuite{
public:

	void testSpeedDistribution (void){
		patError* err(NULL);
		patNBParameters::the()->readFile("/Users/jchen/Documents/Project/biogeme/newbioroute/config.xml", err);
		patNBParameters::the()->init(err);
		TS_ASSERT_EQUALS(err,(void*)0);
		patSpeedDistributions::readParams(err);
		TS_ASSERT_EQUALS(err,(void*)0);
		double lower_bound=0.0;
		double upper_bound=300.0;
		double step_size=0.01;

		int step = (int) (upper_bound-lower_bound)/step_size;

		map<TransportMode,double> modes;
		modes[CAR]=0.0;
		modes[BIKE]=0.0;
		modes[METRO]=0.0;
		modes[BUS]=0.0;
		modes[WALK]=0.0;
		FILE *f = fopen("speedpdf.csv", "w");	

		double last_value=lower_bound;	
		for(int i =0;i<=step;++i){
			fprintf(f,"%.2f",last_value);
			for (std::map<TransportMode,double>::iterator mode_iter = modes.begin();mode_iter!=modes.end();++mode_iter){

				double proba = patSpeedDistributions::the()->pdf(last_value,mode_iter->first);
				fprintf(f,",%.5f",proba);
				mode_iter->second+=proba*step_size;

			}
			fprintf(f, "\n");
			last_value+=step_size;
		}
		fclose(f);
		
		for (std::map<TransportMode,double>::iterator mode_iter = modes.begin();mode_iter!=modes.end();++mode_iter){

			TS_ASSERT_DELTA(mode_iter->second,1.0,1e-3);

		}
	
	}
};