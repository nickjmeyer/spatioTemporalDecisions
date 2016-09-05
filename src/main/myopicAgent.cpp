#include "myopicAgent.hpp"

template class MyopicAgent<ModelGravityGDist>;

template class MyopicAgent<Model2GravityGDist>;

template class MyopicAgent<Model2GPowGDist>;

template class MyopicAgent<ModelGDist>;


template <class M>
std::string MyopicAgent<M>::name = "myopic";

template <class M>
void MyopicAgent<M>::applyTrt(const SimData & sD,
			      TrtData & tD,
			      const FixedData & fD,
			      const DynamicData & dD,
			      M & m){
  numPre = getNumPre(sD,tD,fD,dD);
  numAct = getNumAct(sD,tD,fD,dD);

  int i,j,node0,total,count;

  m.modFill(sD,tD,fD,dD);
  m.infProbs(sD,tD,fD,dD);

  notFeat = arma::conv_to<arma::colvec>::from(m.infProbs());

  infFeat.zeros(sD.numInfected);
  for(i=0; i<sD.numInfected; i++){
    node0 = sD.infected.at(i);
    count=0;
    total=0;
    for(j=0; j<sD.numNotInfec; j++){
      if(fD.network.at(node0*fD.numNodes + sD.notInfec.at(j))){
	total+=notFeat(j);
	count++;
      }
    }
    infFeat(i) = total/((double)count);
  }

  std::priority_queue<std::pair<double,int> > sortInfected,sortNotInfec;

  for(i=0; i<sD.numInfected; i++)
    sortInfected.push(std::pair<double,int>(infFeat(i),sD.infected.at(i)));
  for(i=0; i<sD.numNotInfec; i++)
    sortNotInfec.push(std::pair<double,int>(notFeat(i),sD.notInfec.at(i)));

  for(i=0; i<numAct; i++){
    tD.a.at(sortInfected.top().second) = 1;
    sortInfected.pop();
  }

  for(i=0; i<numPre; i++){
    tD.p.at(sortNotInfec.top().second) = 1;
    sortNotInfec.pop();
  }
}