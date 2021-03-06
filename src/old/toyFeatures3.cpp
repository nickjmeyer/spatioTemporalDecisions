#include "toyFeatures3.hpp"

std::vector<double> ToyFeatures3TuneParam::getPar() const{
  return std::vector<double>(0);
}


void ToyFeatures3TuneParam::putPar(const std::vector<double> & par){
  // do nothing!
}





template class ToyFeatures3<ModelGravityGDist>;

template class ToyFeatures3<ModelTimeGDist>;

template class ToyFeatures3<ModelTimeExpCavesGDist>;

template class ToyFeatures3<ModelRadius>;



template <class M>
void ToyFeatures3<M>::preCompData(const SimData & sD,
				  const TrtData & tD,
				  const FixedData & fD,
				  const DynamicData & dD,
				  M & m){
  // pre compute stuff

  // load estimated probabilities of infection
  m.modFill(sD,tD,fD,dD);
  m.setQuick(sD,tD,fD,dD);

  // extract subgraph connectiviy for not infected
  int i;
  subGraphNotInfec.resize(sD.numNotInfec);
  for(i=0; i<sD.numNotInfec; i++)
    subGraphNotInfec(i) = fD.subGraph.at(sD.notInfec.at(i));

  // obtain neighbors and probabilities not infected infects other not infected
  // initialize containers
  notNeigh.resize(sD.numNotInfec);
  notNeighOf.resize(sD.numNotInfec);
  std::fill(notNeigh.begin(),notNeigh.end(),
	    std::vector<std::pair<int,double> >(0));
  std::fill(notNeighOf.begin(),notNeighOf.end(),
	    std::vector<std::pair<int,int> >(0));

  notNeighNum.resize(sD.numNotInfec);
  notNeighOfNum.resize(sD.numNotInfec);
  std::fill(notNeighNum.begin(),notNeighNum.end(),0);
  std::fill(notNeighOfNum.begin(),notNeighOfNum.end(),0);
  
  
  std::vector<int>::const_iterator itD0,itD1,beg;
  int j;
  beg=sD.notInfec.begin();
  for(i=0,itD0=beg; i<sD.numNotInfec; i++,itD0++){
    for(j=0,itD1=beg; j<sD.numNotInfec; j++,itD1++){
      if(i!=j && fD.network.at((*itD0)*fD.numNodes + (*itD1))){
	// neighbors of i
	notNeigh.at(i).push_back(std::pair<int,double>
				 (j,m.oneOnOne(*itD1,*itD0,fD.numNodes)));
	
	// i is a neighbor of j
	notNeighOf.at(j).push_back(std::pair<int,int>(i,notNeighNum.at(i)));

	// increment totals
	notNeighNum.at(i)++;
	notNeighOfNum.at(j)++;
      }
    }
  }
}



template <class M>
void ToyFeatures3<M>::getFeatures(const SimData & sD,
				  const TrtData & tD,
				  const FixedData & fD,
				  const DynamicData & dD,
				  M & m){
  // clear containers
  infFeat.zeros(sD.numInfected,numFeatures);
  notFeat.zeros(sD.numNotInfec,numFeatures);

  arma::mat weightMat(m.getQuick().data(),sD.numInfected,sD.numNotInfec,false);
  
  // start feature construction

  
  int i,j,featNum=0;
  std::vector<int>::const_iterator itD0,itD1,beg;

  
  // feature 0
  // probability of infection or infecting
  infFeat.col(featNum) = 1 - arma::prod(weightMat,1);
  notFeat.col(featNum) = 1 - arma::prod(weightMat,0).t();
  

  featNum++;

  
  
  // feature 1
  // joint probability of infection between not infected and not infected
  // weighted average of joint probabilities
  int num;
  double modProbTot,modProb;
  std::pair<int,double> neigh;
  for(i = 0; i < sD.numNotInfec; i++){
    modProbTot = 0;
    num = notNeighNum.at(i);
    for(j = 0; j < num; j++){
      neigh=notNeigh.at(i).at(j);
      
      modProb = 1.0 - notFeat(neigh.first,0);
      modProb *= 1.0 - 1.0/(1.0 + std::exp(neigh.second));
      modProbTot += modProb;
    }
    notFeat(i,featNum) = modProbTot*notFeat(i,0);
  }
  
  // arma::mat weightMat(sD.numInfected,sD.numNotInfec);
  // double val;
  // int node0;
  // for(i = 0; i < sD.numInfected; ++i){
  //   node0 = sD.infected.at(i);
  //   for(j = 0; j < sD.numNotInfec; ++j){
  //     val = m.oneOnOne(sD.notInfec.at(j),node0,fD.numNodes);
  //     weightMat(i,j) = 1.0 - 1.0/(1.0 + std::exp(val));
  //   }
  // }
				     
  infFeat.col(featNum) = (1.0 - weightMat) * notFeat.col(featNum);

  
  featNum++;

  
  // feature 2
  // weighted subgraph connectivity measures
  notFeat.col(featNum) = notFeat.col(0) % subGraphNotInfec;

  infFeat.col(featNum) = (1.0 - weightMat) * notFeat.col(0);

    
  featNum++;


  // feature 3
  // density estimate
  std::vector<int>::const_iterator itD2,itD3;
  std::priority_queue<double> p; 
  itD2 = sD.notInfec.begin();
  itD3 = sD.infected.begin();
  double totalDist;
  for(i = 0,itD0 = itD2; i < sD.numNotInfec; i++, itD0++){
    // density estimates for not infected
    totalDist=0;
    for(j = 0,itD1 = itD3; j < sD.numInfected; j++, itD1++)
      totalDist += fD.expGDistSD.at((*itD0)*fD.numNodes + *itD1);
    totalDist /= fD.numNodes*fD.gDistSD;
    notFeat(i,featNum) = std::log(1.0+totalDist);
  }

  for(i = 0,itD0 = itD3; i < sD.numInfected; i++, itD0++){
    // density estimates for infected
    totalDist=0;
    for(j = 0,itD1 = itD2; j < sD.numNotInfec; j++, itD1++)
      totalDist += fD.expGDistSD.at((*itD0)*fD.numNodes + *itD1);
    totalDist /= fD.numNodes*fD.gDistSD;
    infFeat(i,featNum) = std::log(1.0+totalDist);
  }

  featNum++;

  tDPre = tD;

  arma::colvec notMx = arma::max(notFeat,0);
  arma::colvec notMn = arma::min(notFeat,0);
  arma::colvec infMx = arma::max(infFeat,0);
  arma::colvec infMn = arma::min(infFeat,0);

  for(i = 0; i < numFeatures; ++i){
    if((notMx(i) - notMn(i)) > 1e-15){
      notFeat.col(i) = (notFeat.col(i) - notMn(i))/(notMx(i) - notMn(i));
    }
    if((infMx(i) - infMn(i)) > 1e-15){
      infFeat.col(i) = (infFeat.col(i) - infMn(i))/(infMx(i) - infMn(i));
    }
  }


#ifndef NJM_NO_DEBUG
  if(featNum != numFeatures){
    std::cout << "Error: in getFeatures: featNum != numFeatures"
	      << std::endl;
    throw 1;
  }
#endif
}



template <class M>
void ToyFeatures3<M>::updateFeatures(const SimData & sD,
				     const TrtData & tD,
				     const FixedData & fD,
				     const DynamicData & dD,
				     M & m){
  int i,j,num;
  std::pair<int,int> neighOf;

  m.modFill(sD,tD,fD,dD);
  m.setQuick(sD,tD,fD,dD);

  // update neighbor probs
  for(i = 0; i < sD.numNotInfec; i++){
    num=notNeighOfNum.at(i);
    for(j = 0; j < num; j++){
      neighOf = notNeighOf.at(i).at(j);

      notNeigh.at(neighOf.first).at(neighOf.second).second =
	m.oneOnOne(sD.notInfec.at(i),sD.notInfec.at(neighOf.first),
		   fD.numNodes);
    }
  }

  getFeatures(sD,tD,fD,dD,m);
}


