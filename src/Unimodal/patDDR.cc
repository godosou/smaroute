#include "patDDR.h"
patDDR::patDDR ()
{

}

patDDR::patDDR(const patReal& theAccuracy):
	accuracy(theAccuracy)
{

}

patReal patDDR::errPDF(const patReal& distance){

	/*patReal threshold = 0.95;
	if(distance > accuracy*sqrt(-log(1-threshold))){
		return 0.0;
	}
	*/
	//return distance*exp(-distance*distance/(accuracy*accuracy))/(0.5*accuracy*accuracy);
	return exp(-distance*distance/(accuracy*accuracy));

}
patReal patDDR::errCDF( patReal& a, 
			 patReal& b){
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
    s = errPDF(a)+errPDF(b);
    do
    {
        s3 = s2;
        h = h/2;
        s1 = 0;
        x = a+h;
        do
        {
            s1 = s1+2*errPDF(x);
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

