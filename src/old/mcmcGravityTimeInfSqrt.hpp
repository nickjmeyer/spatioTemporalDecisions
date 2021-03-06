#ifndef GRAVITY_TIME_INF_SQRT_MCMC_HPP__
#define GRAVITY_TIME_INF_SQRT_MCMC_HPP__

#include <vector>
#include <algorithm>
#include "rand.hpp"
#include "data.hpp"


class GravityTimeInfSqrtSamples{
 public:
  int numSamples;
  int numCovar;
  
  std::vector<double> intcp,beta,alpha,power,xi,trtPre,trtAct;

  double intcpSet;
  std::vector<double> betaSet;
  double alphaSet;
  double powerSet;
  double xiSet;
  double trtPreSet;
  double trtActSet;

  std::vector<double> ll;
  double llPt,pD,Dbar,DIC;

  void setMean();
  void setRand();

  void setPar(const int i);

  std::vector<double> getPar() const;
};


class GravityTimeInfSqrtMcmc{
 public:

  void load(const std::vector<std::vector<int> > & history,
	    const std::vector<int> & status,
	    const FixedData & fD);
  void load(const std::vector<std::vector<int> > & history,
	    const FixedData & fD);

  double priorTrtMean;
  
  // MCMC samples
  GravityTimeInfSqrtSamples samples;

  // history information of simulation
  int numNodes,T;
  std::vector<int> infHist;
  std::vector<int> trtPreHist;
  std::vector<int> trtActHist;
  std::vector<double> timeInfSqrt; // time infected valued

  // information about each county
  std::vector<double> d;
  std::vector<double> cc;
  std::vector<double> covar;
  int numCovar;
  
  std::vector<double> covarBeta_cur;
  std::vector<double> covarBeta_can;
  std::vector<double> alphaW_cur;
  std::vector<double> alphaW_can;
  std::vector<double> xiTimeInfSqrt_cur;
  std::vector<double> xiTimeInfSqrt_can;

  // current iteration of the parameters
  double intcp_cur;
  std::vector<double> beta_cur;
  double alpha_cur;
  double power_cur;
  double xi_cur;
  double trtPre_cur;
  double trtAct_cur;

  double intcp_can;
  std::vector<double> beta_can;
  double alpha_can;
  double power_can;
  double xi_can;
  double trtPre_can;
  double trtAct_can;

  // likelihood values
  double ll_cur;
  double ll_can;

  // MH step
  std::vector<double> mh;
  std::vector<int> acc;
  std::vector<int> att;
  // std::vector<double> tau;

  // // variables used for priors and such
  // std::vector<double> mu;

  //functions
  void sample(int const numSamples, int const numBurn);
  void sample(int const numSamples, int const numBurn,
	      const std::vector<double> & par);
  double ll();

  inline static void updateAlphaW(std::vector<double> & alphaW,
				  const double & alphaOld,
				  const double & alphaNew,
				  const int numNodes);
  inline static void updateAlphaW(std::vector<double> & alphaW,
				  const std::vector<double> & d,
				  const std::vector<double> & cc,
				  const double & alpha,
				  const double & powerNew,
				  const int numNodes);
  inline static void updateCovarBeta(std::vector<double> & covarBeta,
				     const std::vector<double> & covar,
				     const std::vector<double> & beta,
				     const int numNodes,
				     const int numCovar);
  inline static void updateCovarBeta(std::vector<double> & covarBeta,
				     const std::vector<double> & covar,
				     const double & betaOld,
				     const double & betaNew,
				     const int covarInd,
				     const int numCovar);
  inline static void updateXiTimeInfSqrt(std::vector<double> & xiTimeInfSqrt,
					 const double & scale);
};



inline void GravityTimeInfSqrtMcmc::updateAlphaW(std::vector<double> & alphaW,
						 const double & alphaOld,
						 const double & alphaNew,
						 const int numNodes){
  double scale = alphaNew/alphaOld;
  int i,j;
  for(i = 0; i < numNodes; ++i)
    for(j = i; j < numNodes; ++j)
      alphaW.at(i*numNodes + j) *= scale;
}


inline void GravityTimeInfSqrtMcmc::updateAlphaW(std::vector<double> & alphaW,
						 const std::vector<double> & d,
						 const std::vector<double> & cc,
						 const double & alpha,
						 const double & powerNew,
						 const int numNodes){
  int i,j;
  for(i = 0; i < numNodes; ++i)
    for(j = i; j < numNodes; ++j)
      alphaW.at(i*numNodes + j) = alpha * d.at(i*numNodes + j)/
	std::pow(cc.at(i*numNodes + j),powerNew);
}

// add intercept into covarBeta
inline
void GravityTimeInfSqrtMcmc::updateCovarBeta(std::vector<double> & covarBeta,
					     const std::vector<double> & covar,
					     const std::vector<double> & beta,
					     const int numNodes,
					     const int numCovar){
  int i,j;
  double prod;
  for(i = 0; i < numNodes; ++i){
    prod = 0;
    for(j = 0; j < numCovar; ++j){
      prod += covar.at(i*numCovar + j) * beta.at(j);
    }
    covarBeta.at(i) = prod;
  }
}


inline
void GravityTimeInfSqrtMcmc::updateCovarBeta(std::vector<double> & covarBeta,
					     const std::vector<double> & covar,
					     const double & betaOld,
					     const double & betaNew,
					     const int covarInd,
					     const int numCovar){
  int i = 0;
  double diff = betaNew - betaOld;
  std::for_each(covarBeta.begin(),covarBeta.end(),
		[&covar,&numCovar,&covarInd,&i,&diff](double & x){
		  x += covar.at(i*numCovar + covarInd)*diff;
		  ++i;
		});
}



inline
void GravityTimeInfSqrtMcmc
::updateXiTimeInfSqrt(std::vector<double> & xiTimeInfSqrt,
		      const double & scale){
  std::for_each(xiTimeInfSqrt.begin(),xiTimeInfSqrt.end(),
		[&scale](double & x){x *= scale;});
}


#endif
