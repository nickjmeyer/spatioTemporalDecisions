#ifndef RANK_AGENT_HPP__
#define RANK_AGENT_HPP__


#include <armadillo>
#include <vector>
#include <queue>
#include <limits>
#include "system.hpp"
#include "timer.hpp"
#include "agent.hpp"
#include "dataDepth.hpp"
#include "tuneParam.hpp"
#include "features.hpp"
#include "toyFeatures2.hpp"
#include "toyFeatures3.hpp"
#include "toyFeatures4.hpp"
#include "wnsFeatures0.hpp"
#include "wnsFeatures1.hpp"
#include "calcCentrality.hpp"


class RankTuneParam : public TuneParam {
 public:
  std::vector<double> getPar() const;
  void putPar(const std::vector<double> & par);

  arma::colvec weights_r;
  arma::colvec weights;

  double jitterScale;
};


template <class F, class M>
class RankAgent : public BaseAgent<M> {
 public:
  RankAgent();

  void reset();
  
  virtual void applyTrt(const SimData & sD,
			TrtData & tD,
			const FixedData & fD,
			const DynamicData & dD,
			M & m);

  F f;

  arma::colvec infRanks;
  arma::colvec notRanks;
  
  int numAct;
  int numPre;

  RankTuneParam tp;

  std::string name;
};



#endif
