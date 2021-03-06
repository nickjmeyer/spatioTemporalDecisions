#ifndef WNS_FEATURES_0_HPP__
#define WNS_FEATURES_0_HPP__

#include <vector>
#include <queue>
#include "features.hpp"
#include "tuneParam.hpp"
#include "calcCentrality.hpp"

class WnsFeatures0TuneParam : public TuneParam {
 public:
  virtual std::vector<double> getPar() const ;
  virtual void putPar(const std::vector<double> & par);
};

template<class M>
class WnsFeatures0 : public BaseFeatures<M> {
 public:
  virtual void preCompData(const SimData & sD,
			   const TrtData & tD,
			   const FixedData & fD,
			   const DynamicData & dD,
			   M & m);

  virtual void getFeatures(const SimData & sD,
			   const TrtData & tD,
			   const FixedData & fD,
			   const DynamicData & dD,
			   M & m);
			   
  virtual void updateFeatures(const SimData & sD,
			      const TrtData & tD,
			      const FixedData & fD,
			      const DynamicData & dD,
			      M & m);
  
  // neighbors of not infected
  std::vector<std::vector<std::pair<int,double> > > notNeigh;
  std::vector<int> notNeighNum;

  // not infected are neighbors of
  std::vector<std::vector<std::pair<int,int> > > notNeighOf; 
  std::vector<int> notNeighOfNum;

  // halfplane data depth of not infec
  arma::colvec hpddNotInfec;

  TrtData tDPre;

  arma::mat infFeat;
  arma::mat notFeat;

  const static int numFeatures = 4;

  WnsFeatures0TuneParam tp;

  std::string name;

};



#endif
