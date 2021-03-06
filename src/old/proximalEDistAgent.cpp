#include "proximalEDistAgent.hpp"

template class ProximalEDistAgent<ModelGravityEDist>;

template class ProximalEDistAgent<ModelTimeExpCavesEDist>;


template<class M>
std::string ProximalEDistAgent<M>::name = "proximal";

template <class M>
void ProximalEDistAgent<M>::applyTrt(const SimData & sD,
				TrtData & tD,
				const FixedData & fD,
				const DynamicData & dD,
				M & m){
  numPre = getNumPre(sD,tD,fD,dD);
  numAct = getNumAct(sD,tD,fD,dD);

  int i,j,node0,node1;
  double minDist,curDist,maxDist;

  maxDist = std::numeric_limits<double>::max();

  std::priority_queue<std::pair<double,int> > sortInfected,sortNotInfec;

  for(i=0; i<sD.numNotInfec; i++){
    node0=sD.notInfec.at(i);

    minDist=maxDist;
    for(j=0; j<sD.numInfected; j++){
      node1=sD.infected.at(j);
      curDist=fD.eDist.at(node0*fD.numNodes + node1);
      if(minDist > curDist)
	minDist = curDist;
    }

    sortNotInfec.push(std::pair<double,int>(-minDist,node0));
  }    

  
  for(i=0; i<sD.numInfected; i++){
    node0=sD.infected.at(i);

    minDist=maxDist;
    for(j=0; j<sD.numNotInfec; j++){
      node1=sD.notInfec.at(j);
      curDist=fD.eDist.at(node0*fD.numNodes + node1);
      if(minDist > curDist)
	minDist = curDist;
    }

    sortInfected.push(std::pair<double,int>(-minDist,node0));
  }    
	 

  for(i=0; i<numAct; i++){
    tD.a.at(sortInfected.top().second) = 1;
    sortInfected.pop();
  }

  for(i=0; i<numPre; i++){
    tD.p.at(sortNotInfec.top().second) = 1;
    sortNotInfec.pop();
  }
}

