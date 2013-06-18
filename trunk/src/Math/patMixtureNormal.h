#ifndef patMixtureNormal_h
#define patMixtureNormal_h
#include "patError.h"

#include <vector>
using namespace std;
class patMixtureNormal{
public:
	patMixtureNormal(int& components,  vector<double>& w,  vector<double>& mu, vector<double>& sigma, patError *& err);
	double pdf(const double& value) const;
protected:
	int m_components;
	 vector<double> m_w;
	vector<double> m_mu;
	vector<double> m_sigma;
};

#endif
