
#include "patArcDDR.h"
#include <math.h>
	     #include <gsl/gsl_integration.h>
	#include "patConst.h"

	#include <gsl/gsl_rng.h>
	#include <gsl/gsl_randist.h>
	#include <gsl/gsl_math.h>
	#include <gsl/gsl_monte.h>
	#include <gsl/gsl_monte_plain.h>
	#include <gsl/gsl_monte_miser.h>
	#include <gsl/gsl_monte_vegas.h>
#include "patDisplay.h"
#include "patNBParameters.h"

/**
*
*@param k
**[0] e_a
*/
patReal arcPDF(patReal* k, size_t dim, void* params){
	struct arc_ddr_params * fp = (struct arc_ddr_params *)params;
	patReal d_curr_2 = pow(fp->d,2)+ pow((k[0] - fp->e_d) * fp->l,2);
		
		return exp(-(d_curr_2/ ( 2.0 * pow(fp->std_x,2))));//cdf rayleigh
				//DEBUG_MESSAGE("delta "<<fp->gps.std_x_curr<<"rtn"<<rtn);
}

patReal patArcDDR::errCDF(arc_ddr_params param){

	
	if (!param.ldr.upper_bound>param.ldr.lower_bound){
		DEBUG_MESSAGE('lower bound, upper bound:'<<param.ldr.lower_bound<<','<<param.ldr.upper_bound);
	}
	patReal *xl;
	patReal *xu;
	gsl_monte_function G ;
	G.f = &arcPDF;
	G.params=&param;
	xl = new patReal[1];
	xu = new patReal[1];
	xl[0]=param.ldr.lower_bound;
	xu[0]=param.ldr.upper_bound;
	
	G.dim = 1;
	patReal dimension=1;
	patReal res;
	patReal err;
	const gsl_rng_type *T;
	gsl_rng *r;
	
		size_t calls = patNBParameters::the()->nbrOfIntegrationCalls;

		gsl_rng_env_setup ();
		//DEBUG_MESSAGE("setup o k");

		T = gsl_rng_default;
		r = gsl_rng_alloc (T);
		if(patNBParameters::the()->integrationMethod == "miser")		{
			gsl_monte_miser_state *s = gsl_monte_miser_alloc (dimension);
			gsl_monte_miser_integrate (&G, xl, xu, dimension, calls, r, s,
	                                    &res, &err);
			gsl_monte_miser_free (s);
		}
		else if(patNBParameters::the()->integrationMethod == "vegas")
		{
			   gsl_monte_vegas_state *s = gsl_monte_vegas_alloc (dimension);

	         gsl_monte_vegas_integrate (&G, xl, xu, dimension, calls/4, r, s,
	                                    &res, &err);


	         do
	           {
	             gsl_monte_vegas_integrate (&G, xl, xu, dimension, calls/2, r, s,
	                                        &res, &err);
	           }
	         while (fabs (gsl_monte_vegas_chisq (s) - 1.0) > 0.5);


	         gsl_monte_vegas_free (s);


		}
		else{
			gsl_monte_plain_state *s = gsl_monte_plain_alloc (dimension);
	         gsl_monte_plain_integrate (&G, xl, xu, dimension, calls, r, s, 
	                                    &res, &err);
	         gsl_monte_plain_free (s);
		}
		gsl_rng_free (r);
		     
       return res;//cdf rayleigh
	

}

patArcDDR::patArcDDR(){

}

patArcDDR::patArcDDR ( patReal theAccuracy):
	patDDR(theAccuracy)
{

}

patReal patArcDDR::errPDF( patReal distance, 
			 patReal position){
	patReal distanceToPoint = sqrt(distance*distance+position*position);
	
	return patDDR::errPDF(distanceToPoint);
}

patReal patArcDDR::errCDF( patReal distance,
			 patReal a, 
			 patReal b){
	//from simpsonq
	patReal epsilon = 0.00001;
	patReal result;
    int i;
    int n;
    patReal h;
    patReal s;
    patReal s1;
    patReal s2;
    patReal s3;
    patReal x;
	 
	 patReal c;
	 if (a>b){
		c=a;
		a=b;
		b=c;	 
	 }
	 
    s2 = 1;
    h = b-a;
    s = errPDF(distance,a)+errPDF(distance,b);
    do
    {
        s3 = s2;
        h = h/2;
        s1 = 0;
        x = a+h;
        do
        {
            s1 = s1+2*errPDF(distance,x);
            x = x+2*h;
        }
        while(x<b);
        s = s+s1;
        s2 = (s+s1)*h/3;
        x = fabs(s3-s2)/15;
    }
    while(x>epsilon);
    result = s2;
    return result;


}
	