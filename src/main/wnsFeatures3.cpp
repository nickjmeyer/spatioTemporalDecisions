#include <glog/logging.h>
#include "wnsFeatures3.hpp"

std::vector<double> WnsFeatures3TuneParam::getPar() const{
    return std::vector<double>(0);
}


void WnsFeatures3TuneParam::putPar(const std::vector<double> & par){
    // do nothing!
}


template <class M>
void WnsFeatures3<M>::preCompData(const SimData & sD,
        const TrtData & tD,
        const FixedData & fD,
        const DynamicData & dD,
        M & m){
    CHECK_EQ(tp.getEdgeToEdge(),m.getEdgeToEdge());

    // pre compute stuff

    // load estimated probabilities of infection
    m.setFill(sD,tD,fD,dD);
    m.setQuick(sD,tD,fD,dD);

    // extract centrality for not infected
    int i;
    centralityNotInfec.resize(sD.numNotInfec);
    if(tp.getEdgeToEdge()) {
        const double minCentrality = * std::min_element(
                fD.subGraph.begin(),fD.subGraph.end());
        const double maxCentrality = * std::max_element(
                fD.subGraph.begin(),fD.subGraph.end());
        const double rngCentrality = maxCentrality - minCentrality;

        CHECK_GT(rngCentrality,1e-3);

        for(i=0; i<sD.numNotInfec; i++) {
            double val = fD.subGraph.at(sD.notInfec.at(i));
            val -= minCentrality;
            val /= rngCentrality;
            centralityNotInfec(i) = val;
        }
    } else {
        const double minCentrality = * std::min_element(
                fD.hpdd.begin(),fD.hpdd.end());
        const double maxCentrality = * std::max_element(
                fD.hpdd.begin(),fD.hpdd.end());
        const double rngCentrality = maxCentrality - minCentrality;

        CHECK_GT(rngCentrality,1e-3);

        for(i=0; i<sD.numNotInfec; i++){
            double val = fD.hpdd.at(sD.notInfec.at(i));
            val -= minCentrality;
            val /= maxCentrality;
            centralityNotInfec(i) = val;
        }
    }

    // obtain neighbors and probabilities not infected infects other not infected
    // initialize containers
    if(tp.getEdgeToEdge()) {
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
            for(j = 0,itD1=beg; j <sD.numNotInfec; ++j,++itD1){
                if(i!=j && fD.network.at((*itD0)*fD.numNodes + (*itD1))){
                    // neighbors of i
                    notNeigh.at(i).push_back(std::pair<int,double>
                            (j,m.oneOnOne(*itD1,*itD0,fD.numNodes)));

                    // i is a neighbor of j
                    notNeighOf.at(j).push_back(std::pair<int,int>(i,notNeighNum.at(i)));

                    ++notNeighNum.at(i);
                    ++notNeighOfNum.at(j);
                }
            }
        }
    }
}



template <class M>
void WnsFeatures3<M>::getFeatures(const SimData & sD,
        const TrtData & tD,
        const FixedData & fD,
        const DynamicData & dD,
        M & m){
    CHECK_EQ(tp.getEdgeToEdge(),m.getEdgeToEdge());

    // clear containers
    infFeat.zeros(sD.numInfected,numFeatures);
    notFeat.zeros(sD.numNotInfec,numFeatures);

    arma::mat weightMat(m.getQuick().data(),sD.numInfected,sD.numNotInfec,false);

    // start feature construction


    int i,j,featNum=0;
    std::vector<int>::const_iterator itD0,itD1,beg;


    // feature 0
    // probability of infection or infecting
    notFeat.col(featNum) = 1 - arma::prod(weightMat,0).t();
    // infFeat.col(featNum) = 1 - arma::prod(weightMat,1);
    for(i=0; i<sD.numInfected; i++){
        int node0 = sD.infected.at(i);

        if(tp.getEdgeToEdge()) {
            int count = 0;
            int total = 0;
            for(j=0; j<sD.numNotInfec; j++){
                if(fD.network.at(node0*fD.numNodes + sD.notInfec.at(j))){
                    total+=notFeat(j,0);
                    count++;
                }
            }
            if (count > 0) {
                infFeat(i,featNum) = total/((double)count);
            } else {
                infFeat(i,featNum) = 0.;
            }
        } else {
            double weightProb = 0;
            double weightTot = 0;
            for (j = 0; j < sD.numNotInfec; ++j) {
                double weight = fD.expDistWeight.at(
                        node0 * fD.numNodes + sD.notInfec.at(j));
                weightProb += weight * notFeat(j,0);
                weightTot += weight;
            }
            if (weightTot > 0) {
                infFeat(i,featNum) = weightProb/weightTot;
            } else {
                infFeat(i,featNum) = 0.;
            }
        }
    }



    featNum++;



    // feature 1
    // joint probability of infection between not infected and not infected
    // weighted average of joint probabilities
    int num;
    double modProbTot,modProb;
    std::pair<int,double> neigh;
    for(i = 0; i < sD.numNotInfec; i++){
        if(tp.getEdgeToEdge()) {
            modProbTot = 0;
            num = notNeighNum.at(i);
            for(j = 0; j < num; j++){
                neigh=notNeigh.at(i).at(j);

                modProb = 1.0 - notFeat(neigh.first,0);
                // should this be 1.0 - ... ?
                modProb *= 1.0 - 1.0/(1.0 + std::exp(neigh.second));
                // should this also be 1.0 - ... ?
                modProbTot += modProb;
            }
            notFeat(i,featNum) = modProbTot*notFeat(i,0);
        } else {
            double distWeightProb = 0;
            double sumWeight = 0;
            for (int j = 0; j < sD.numNotInfec; ++j) {
                if (j != i) {
                    // probability i infects j, weighted by distance
                    const int index = sD.notInfec.at(j) * fD.numNodes + sD.notInfec.at(i);
                    const double weight = fD.expDistWeight.at(index);

                    const double prob = 1.0 - 1.0/(1.0 + std::exp(m.oneOnOne(j,i,
                                            fD.numNodes)));
                    const double modProb = prob * (1.0 - notFeat(j,0));

                    distWeightProb += weight * modProb;
                    sumWeight += weight;
                }
            }

            // ues as a guard against division by zero
            if (sD.numNotInfec > 1) {
                notFeat(i,featNum) = notFeat(i,0) * (distWeightProb / sumWeight) ;
            } else {
                notFeat(i,featNum) = 0.0;
            }
        }
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
    // weighted half plane data depth
    notFeat.col(featNum) = notFeat.col(0) % centralityNotInfec;

    infFeat.col(featNum) = (1.0 - weightMat) * centralityNotInfec;


    featNum++;

    tDPre = tD;


    // const arma::colvec notMx = arma::max(notFeat,0).t();
    // const arma::colvec notMn = arma::min(notFeat,0).t();
    // const arma::colvec infMx = arma::max(infFeat,0).t();
    // const arma::colvec infMn = arma::min(infFeat,0).t();

    // for(i = 0; i < numFeatures; ++i){
    //   if((notMx(i) - notMn(i)) > 1e-15){
    //     notFeat.col(i) = (notFeat.col(i) - notMn(i))/(notMx(i) - notMn(i));
    //   }
    //   if((infMx(i) - infMn(i)) > 1e-15){
    //     infFeat.col(i) = (infFeat.col(i) - infMn(i))/(infMx(i) - infMn(i));
    //   }
    // }

    CHECK_EQ(featNum,numFeatures);
}



template <class M>
void WnsFeatures3<M>::updateFeatures(const SimData & sD,
        const TrtData & tD,
        const FixedData & fD,
        const DynamicData & dD,
        M & m){
    CHECK_EQ(tp.getEdgeToEdge(),m.getEdgeToEdge());

    int i,j,num;
    std::pair<int,int> neighOf;

    m.modFill(sD,tD,fD,dD);
    m.setQuick(sD,tD,fD,dD);

    // update neighbor probs
    if(tp.getEdgeToEdge()) {
        for(i = 0; i < sD.numNotInfec; i++){
            num=notNeighOfNum.at(i);
            for(j = 0; j < num; j++){
                neighOf = notNeighOf.at(i).at(j);

                notNeigh.at(neighOf.first).at(neighOf.second).second =
                    m.oneOnOne(sD.notInfec.at(i),sD.notInfec.at(neighOf.first),
                            fD.numNodes);
            }
        }
    }

    getFeatures(sD,tD,fD,dD,m);
}


template class WnsFeatures3<ModelGravityGDist>;

template class WnsFeatures3<Model2GravityGDist>;

template class WnsFeatures3<Model2GravityEDist>;

template class WnsFeatures3<ModelGDist>;

template class WnsFeatures3<ModelEDist>;

template class WnsFeatures3<ModelIntercept>;

template class WnsFeatures3<Model2EdgeToEdge>;
