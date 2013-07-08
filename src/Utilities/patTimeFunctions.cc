#include "patTimeFunctions.h"

double getMillisecond(){
	   struct timeval tv;   // see gettimeofday(2)
	   gettimeofday(&tv, NULL);
	   double t = (double) tv.tv_sec + (double) 1e-6 * tv.tv_usec;
	   return t;
}
