#ifndef TOY_FEATURES_1_HPP__
#define TOY_FEATURES_1_HPP__

#include <vector>
#include <queue>
#include "features.hpp"
#include "tuneParam.hpp"
#include "calcCentrality.hpp"

class ToyFeatures1TuneParam : public TuneParam {
 public:
  virtual std::vector<double> getPar() const ;
  virtual void putPar(const std::vector<double> & par);

  int valReps;
};

template<class M>
class ToyFeatures1 : public BaseFeatures<M> {
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

  TrtData tDPre;

  arma::mat infFeat;
  arma::mat notFeat;

  static int numFeatures;

  ToyFeatures1TuneParam tp;

  std::string name;

};



#endif
