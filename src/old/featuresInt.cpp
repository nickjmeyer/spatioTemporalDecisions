#include "featuresInt.hpp"

template class FeaturesInt<ToyFeatures2<ModelGravityGDist>,
			   ModelGravityGDist>;


template <class F, class M>
void FeaturesInt<F,M>::preCompData(const SimData & sD,
				   const TrtData & tD,
				   const FixedData & fD,
				   const DynamicData & dD,
				   M & m){
  f.preCompData(sD,tD,fD,dD,m);
}


template <class F, class M>
void FeaturesInt<F,M>::getFeatures(const SimData & sD,
				   const TrtData & tD,
				   const FixedData & fD,
				   const DynamicData & dD,
				   M & m){
  f.getFeatures(sD,tD,fD,dD,m);
  
  infFeat.ones(sD.numInfected,numFeatures);
  notFeat.ones(sD.numNotInfec,numFeatures);

  infFeat.submat(0,1,infFeat.n_rows-1,infFeat.n_cols-1) = f.infFeat;
  notFeat.submat(0,1,notFeat.n_rows-1,notFeat.n_cols-1) = f.notFeat;
}


template <class F, class M>
void
FeaturesInt<F,M>::updateFeatures(const SimData & sD,
				 const TrtData & tD,
				 const FixedData & fD,
				 const DynamicData & dD,
				 M & m){
  f.updateFeatures(sD,tD,fD,dD,m);
  
  infFeat.ones(sD.numInfected,numFeatures);
  notFeat.ones(sD.numNotInfec,numFeatures);

  infFeat.submat(0,1,infFeat.n_rows-1,infFeat.n_cols-1) = f.infFeat;
  notFeat.submat(0,1,notFeat.n_rows-1,notFeat.n_cols-1) = f.notFeat;
}

