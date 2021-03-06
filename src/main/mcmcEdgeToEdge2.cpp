#include "mcmcEdgeToEdge2.hpp"


enum parInd{INTCP_=0,TRTP_=1,TRTA_=2};



void EdgeToEdge2Samples::setMode(){
    intcpSet = DensityEst(intcp).max().second;
    trtPreSet = DensityEst(trtPre).max().second;
    trtActSet = DensityEst(trtAct).max().second;


    int j,k;
    betaSet.clear();
    betaInfSet.clear();
    for(j = 0; j < numCovar; ++j){
        std::vector<double> betaJ,betaInfJ;
        for(k = 0; k < numSamples; ++k){
            betaJ.push_back(beta.at(k*numCovar + j));
            betaInfJ.push_back(betaInf.at(k*numCovar + j));
        }
        betaSet.push_back(DensityEst(betaJ).max().second);
        betaInfSet.push_back(DensityEst(betaInfJ).max().second);
    }
}



void EdgeToEdge2Samples::setMean(){
    intcpSet = trtPreSet = trtActSet = 0.0;
    betaSet.resize(numCovar);
    std::fill(betaSet.begin(),betaSet.end(),0.0);
    betaInfSet.resize(numCovar);
    std::fill(betaInfSet.begin(),betaInfSet.end(),0.0);

    intcpSet = std::accumulate(intcp.begin(),intcp.end(),0.0);
    intcpSet /= double(numSamples);
    trtPreSet = std::accumulate(trtPre.begin(),trtPre.end(),0.0);
    trtPreSet /= double(numSamples);
    trtActSet = std::accumulate(trtAct.begin(),trtAct.end(),0.0);
    trtActSet /= double(numSamples);

    int j = 0;
    std::for_each(beta.begin(),beta.end(),
            [this,&j](const double & x){
                betaSet.at(j++ % numCovar) += x;
            });
    std::for_each(betaSet.begin(),betaSet.end(),
            [this](double & x){x /= double(numSamples);});

    j = 0;
    std::for_each(betaInf.begin(),betaInf.end(),
            [this,&j](const double & x){
                betaInfSet.at(j++ % numCovar) += x;
            });
    std::for_each(betaInfSet.begin(),betaInfSet.end(),
            [this](double & x){x /= double(numSamples);});
}


void EdgeToEdge2Samples::setRand(){
    intcpSet = trtPreSet = trtActSet = 0.0;
    betaSet.resize(numCovar);
    std::fill(betaSet.begin(),betaSet.end(),0.0);
    betaInfSet.resize(numCovar);
    std::fill(betaInfSet.begin(),betaInfSet.end(),0.0);

    int i = njm::runifInterv(0,numSamples);

    intcpSet = intcp.at(i);
    trtPreSet = trtPre.at(i);
    trtActSet = trtAct.at(i);

    int j = 0;
    std::for_each(betaSet.begin(),betaSet.end(),
            [this,&i,&j](double & x){
                x = beta.at(i*numCovar + j++);});
    j = 0;
    std::for_each(betaInfSet.begin(),betaInfSet.end(),
            [this,&i,&j](double & x){
                x = betaInf.at(i*numCovar + j++);});
}

void EdgeToEdge2Samples::setPar(const int i,const bool fromBurn){
    intcpSet = trtPreSet = trtActSet = 0.0;
    betaSet.resize(numCovar);
    std::fill(betaSet.begin(),betaSet.end(),0.0);
    betaInfSet.resize(numCovar);
    std::fill(betaInfSet.begin(),betaInfSet.end(),0.0);

    if(fromBurn){
        intcpSet = intcpBurn.at(i);
        trtPreSet = trtPreBurn.at(i);
        trtActSet = trtActBurn.at(i);

        int j = 0;
        std::for_each(betaSet.begin(),betaSet.end(),
                [this,&i,&j](double & x){
                    x = betaBurn.at(i*numCovar + j++);});
        j = 0;
        std::for_each(betaInfSet.begin(),betaInfSet.end(),
                [this,&i,&j](double & x){
                    x = betaInfBurn.at(i*numCovar + j++);});
    }
    else{
        intcpSet = intcp.at(i);
        trtPreSet = trtPre.at(i);
        trtActSet = trtAct.at(i);

        int j = 0;
        std::for_each(betaSet.begin(),betaSet.end(),
                [this,&i,&j](double & x){
                    x = beta.at(i*numCovar + j++);});
        j = 0;
        std::for_each(betaInfSet.begin(),betaInfSet.end(),
                [this,&i,&j](double & x){
                    x = betaInf.at(i*numCovar + j++);});
    }
}




std::vector<double> EdgeToEdge2Samples::getPar() const {
    std::vector<double> par {intcpSet};
    par.insert(par.end(),betaSet.begin(),betaSet.end());
    par.insert(par.end(),betaInfSet.begin(),betaInfSet.end());
    par.push_back(trtActSet);
    par.push_back(trtPreSet);

    return par;
}



void EdgeToEdge2Mcmc::load(const std::vector<std::vector<int> > & history,
        const std::vector<int> & status,
        const FixedData & fD){
    std::vector<std::vector<int> > all;
    all = history;
    all.push_back(status);
    load(all,fD);
}



void EdgeToEdge2Mcmc::load(const std::vector<std::vector<int> > & history,
        const FixedData & fD){
    numNodes=fD.numNodes;
    T=(int)history.size();
    numCovar=fD.numCovar;
    samples.numCovar = numCovar;
    net = fD.network;

    priorTrtMean = fD.priorTrtMean;

    infHist.resize(numNodes*T);
    trtPreHist.resize(numNodes*T);
    trtActHist.resize(numNodes*T);
    d = fD.gDist;
    cc.resize(numNodes*numNodes);
    covar = fD.covar;
    timeInf.resize(numNodes*T);
    int i,j;
    for(i = 0; i < numNodes; ++i){
        for(j = 0; j < T; ++j){// get the histories of infection and treatments
            infHist.at(i*T + j)=(history.at(j).at(i) < 2 ? 0 : 1);
            trtPreHist.at(i*T + j)=(history.at(j).at(i) == 1 ? 1 : 0);
            trtActHist.at(i*T + j)=(history.at(j).at(i) == 3 ? 1 : 0);
        }

        // while you're looping, get d and cc
        for(j = 0; j < numNodes; ++j){
            cc.at(i*numNodes + j)=fD.caves.at(i)*fD.caves.at(j);
        }
    }


    int val;
    for(i = 0; i < numNodes; ++i){
        val = 0;
        for(j = 0; j < T; ++j){
            if(infHist.at(i*T + j) == 1) // this should be 1
                ++val;
            timeInf.at(i*T + j) = val;
        }
    }

}


void EdgeToEdge2Mcmc::sample(int const numSamples, int const numBurn,
        const bool saveBurn){
    std::vector<double> beta (numCovar,0.0);
    std::vector<double> betaInf (numCovar,0.0);
    std::vector<double> par = {-3.0, // intcp
                               0.0, // trtAct
                               0.0}; // trtPre
    par.insert(par.begin()+1,beta.begin(),beta.end());
    par.insert(par.begin()+1,betaInf.begin(),betaInf.end());
    sample(numSamples,numBurn,par,saveBurn);
}


void EdgeToEdge2Mcmc::sample(int const numSamples, int const numBurn,
        const std::vector<double> & par,
        const bool saveBurn){
    samples.numSamples = numSamples - numBurn;
    samples.numBurn = numBurn;

    // priors
    int thin=1;
    double intcp_mean=0,intcp_var=100,
        beta_mean=0,beta_var=10,
        betaInf_mean=0,betaInf_var=10,
        trtPre_mean=priorTrtMean,trtPre_var=1,
        trtAct_mean=priorTrtMean,trtAct_var=1;


    int i,j;
    // set containers for current and candidate samples
    std::vector<double>::const_iterator it = par.begin();
    intcp_cur=intcp_can= *it++;

    beta_cur.clear();
    for(i = 0; i < numCovar; ++i)
        beta_cur.push_back(*it++);
    beta_can = beta_cur;

    betaInf_cur.clear();
    for(i = 0; i < numCovar; ++i)
        betaInf_cur.push_back(*it++);
    betaInf_can = betaInf_cur;

    trtAct_cur=trtAct_can= *it++;
    trtPre_cur=trtPre_can= *it++;

    // set containers for storing all non-burned samples
    samples.intcp.clear();
    samples.intcpBurn.clear();
    samples.intcp.reserve(numSamples-numBurn);
    samples.intcpBurn.reserve(numBurn);

    samples.beta.clear();
    samples.betaBurn.clear();
    samples.beta.reserve((numSamples-numBurn)*numCovar);
    samples.betaBurn.reserve((numBurn)*numCovar);

    samples.betaInf.clear();
    samples.betaInfBurn.clear();
    samples.betaInf.reserve((numSamples-numBurn)*numCovar);
    samples.betaInfBurn.reserve((numBurn)*numCovar);

    samples.trtPre.clear();
    samples.trtPreBurn.clear();
    samples.trtPre.reserve(numSamples-numBurn);
    samples.trtPreBurn.reserve(numBurn);

    samples.trtAct.clear();
    samples.trtActBurn.clear();
    samples.trtAct.reserve(numSamples-numBurn);
    samples.trtActBurn.reserve(numBurn);


    samples.ll.clear();
    samples.llBurn.clear();
    samples.ll.reserve(numSamples-numBurn);
    samples.llBurn.reserve(numBurn);


    covarBeta_cur.resize(numNodes);
    updateCovarBeta(covarBeta_cur,covar,beta_cur,numNodes,numCovar);
    covarBeta_can = covarBeta_cur;

    covarBetaInf_cur.resize(numNodes);
    updateCovarBeta(covarBetaInf_cur,covar,betaInf_cur,numNodes,numCovar);
    covarBetaInf_can = covarBetaInf_cur;

    // get the likelihood with the current parameters
    ll_cur=ll_can=ll();

    // set the MH tuning parameters
    acc=att= std::vector<int>(par.size(),0);
    mh=std::vector<double>(par.size(),0.5);
    // tau=std::vector<double>(numCovar+2,0.0);

    // mu=std::vector<double>(numCovar+2,0.0);
    // mu.at(numCovar+INTCP_) = -3;

    double upd;
    double R;

    int displayOn=1;
    int display=1;

    // do a bunch of nonsense...
    for(i=0; i<numSamples; ++i){
        if(display && i%displayOn==0){
            printf("McmcEdgeToEdge2...%6s: %6d\r","iter",i);
            fflush(stdout);
        }


        // sample intcp
        ++att.at(numCovar+INTCP_);
        upd=intcp_cur+mh.at(numCovar+INTCP_)*njm::rnorm01();
        intcp_can=upd;

        // get new likelihood
        ll_can=ll();


        R=ll_can + (-.5/intcp_var)*std::pow(intcp_can - intcp_mean,2.0)
            - ll_cur - (-.5/intcp_var)*std::pow(intcp_cur - intcp_mean,2.0);


        // accept?
        if(std::log(njm::runif01()) < R){
            ++acc.at(numCovar+INTCP_);
            intcp_cur=intcp_can;
            ll_cur=ll_can;
        }
        else{
            intcp_can=intcp_cur;
            ll_can=ll_cur;
        }

        // sample beta
        for(j = 0; j < numCovar; ++j){
            ++att.at(j);

            upd=beta_cur.at(j)+mh.at(j)*njm::rnorm01();
            beta_can.at(j)=upd;

            updateCovarBeta(covarBeta_can,covar,
                    beta_cur.at(j),beta_can.at(j),
                    j,numCovar);

            // get new likelihood
            ll_can=ll();

            R=ll_can + (-.5/beta_var)*std::pow(beta_can.at(j) - beta_mean,2.0)
                - ll_cur - (-.5/beta_var)*std::pow(beta_cur.at(j) - beta_mean,2.0);

            // accept?
            if(std::log(njm::runif01()) < R){
                ++acc.at(j);
                beta_cur.at(j)=beta_can.at(j);
                ll_cur=ll_can;
                covarBeta_cur=covarBeta_can;
            }
            else{
                beta_can.at(j)=beta_cur.at(j);
                covarBeta_can=covarBeta_cur;
                ll_can=ll_cur;
            }
        }


        // sample betaInf
        for(j = 0; j < numCovar; ++j){
            ++att.at(j);

            upd=betaInf_cur.at(j)+mh.at(j)*njm::rnorm01();
            betaInf_can.at(j)=upd;

            updateCovarBeta(covarBetaInf_can,covar,
                    betaInf_cur.at(j),betaInf_can.at(j),
                    j,numCovar);

            // get new likelihood
            ll_can=ll();

            R=ll_can + (-.5/betaInf_var)*std::pow(betaInf_can.at(j) - betaInf_mean,
                    2.0)
                - ll_cur - (-.5/betaInf_var)*std::pow(betaInf_cur.at(j) - betaInf_mean,
                        2.0);

            // accept?
            if(std::log(njm::runif01()) < R){
                ++acc.at(j);
                betaInf_cur.at(j)=betaInf_can.at(j);
                ll_cur=ll_can;
                covarBetaInf_cur=covarBetaInf_can;
            }
            else{
                betaInf_can.at(j)=betaInf_cur.at(j);
                covarBetaInf_can=covarBetaInf_cur;
                ll_can=ll_cur;
            }
        }


        // sample trtPre
        ++att.at(numCovar+TRTP_);
        upd=trtPre_cur+mh.at(numCovar+TRTP_)*njm::rnorm01();
        trtPre_can=upd;


        // get new likelihood
        ll_can=ll();

        R=ll_can + (-.5/trtPre_var)*std::pow(trtPre_can - trtPre_mean,2.0)
            - ll_cur - (-.5/trtPre_var)*std::pow(trtPre_cur - trtPre_mean,2.0);

        // accept?
        if(std::log(njm::runif01()) < R){
            ++acc.at(numCovar+TRTP_);
            trtPre_cur=trtPre_can;
            ll_cur=ll_can;
        }
        else{
            trtPre_can=trtPre_cur;
            ll_can=ll_cur;
        }



        // sample trtAct
        ++att.at(numCovar+TRTA_);
        upd=trtAct_cur+mh.at(numCovar+TRTA_)*njm::rnorm01();
        trtAct_can=upd;

        // get new likelihood
        ll_can=ll();

        R=ll_can + (-.5/trtAct_var)*std::pow(trtAct_can - trtAct_mean,2.0)
            - ll_cur - (-.5/trtAct_var)*std::pow(trtAct_cur - trtAct_mean,2.0);


        // accept?
        if(std::log(njm::runif01()) < R){
            ++acc.at(numCovar+TRTA_);
            trtAct_cur=trtAct_can;
            ll_cur=ll_can;
        }
        else{
            trtAct_can=trtAct_cur;
            ll_can=ll_cur;
        }



        if(i<numBurn){
            // time for tuning!
            int len=int(mh.size());
            double accRatio;
            for(j = 0; j < len; ++j){
                if(att.at(j) > 50){
                    accRatio=((double)acc.at(j))/((double)att.at(j));
                    if(accRatio < .3)
                        mh.at(j)*=.8;
                    else if(accRatio > .6)
                        mh.at(j)*=1.2;

                    acc.at(j)=0;
                    att.at(j)=0;
                }
            }
            if(saveBurn){
                samples.intcpBurn.push_back(intcp_cur);
                samples.betaBurn.insert(samples.betaBurn.end(),
                        beta_cur.begin(),
                        beta_cur.end());
                samples.betaInfBurn.insert(samples.betaInfBurn.end(),
                        betaInf_cur.begin(),
                        betaInf_cur.end());
                samples.trtPreBurn.push_back(trtPre_cur);
                samples.trtActBurn.push_back(trtAct_cur);
            }
        }
        else if(i%thin==0){
            // save the samples
            samples.intcp.push_back(intcp_cur);
            samples.beta.insert(samples.beta.end(),beta_cur.begin(),beta_cur.end());
            samples.betaInf.insert(samples.betaInf.end(),
                    betaInf_cur.begin(),betaInf_cur.end());
            samples.trtPre.push_back(trtPre_cur);
            samples.trtAct.push_back(trtAct_cur);

            samples.ll.push_back(ll_cur);
        }
    }

    // get likelihood evaluated at posterior mean
    samples.setMean();
    intcp_can = samples.intcpSet;
    beta_can = samples.betaSet;
    betaInf_can = samples.betaInfSet;
    trtPre_can = samples.trtPreSet;
    trtAct_can = samples.trtActSet;

    updateCovarBeta(covarBeta_can,covar,beta_can,numNodes,numCovar);
    updateCovarBeta(covarBetaInf_can,covar,betaInf_can,numNodes,numCovar);

    samples.llPt = ll();

    samples.Dbar=-2.0*std::accumulate(samples.ll.begin(),
            samples.ll.end(),
            0.0)/double(samples.numSamples);
    samples.pD=samples.Dbar - -2.0*samples.llPt;
    samples.DIC=samples.pD + samples.Dbar;


    if(display)
        printf("\33[2K\r");
}



double EdgeToEdge2Mcmc::ll(){
    int i,j,k;
    double llVal,wontProb,prob,expProb,baseProb,baseProbInit;

    llVal = 0.0;
    for(i=1; i<T; i++){// loop over time interval that has changed
        for(j=0; j<numNodes; j++){
            if(infHist.at(j*T + i-1)==0){// if county is susceptible get infProb
                wontProb=1.0;
                // set a base number to decrease floating point operations
                if(trtPreHist.at(j*T + i-1)==0)
                    baseProbInit=intcp_can+covarBeta_can.at(j);
                else
                    baseProbInit=intcp_can+covarBeta_can.at(j) - trtPre_can;

                for(k=0; k<numNodes; k++){
                    // if county is infected it affects the infProb only if it
                    // is a neighbor
                    if(infHist.at(k*T + i-1)==1 && net.at(j*numNodes + k)){
                        // calculate infProb
                        baseProb=baseProbInit;

                        baseProb += covarBetaInf_can.at(k);

                        if(trtActHist.at(k*T + i-1)==1)
                            baseProb -= trtAct_can;

                        expProb=std::exp(baseProb);

                        wontProb*=1.0/(1.0+expProb);
                    }
                }

                prob=1.0-wontProb;

                if(!(prob > 0.0))
                    prob=std::exp(-30.0);
                else if(!(prob < 1.0))
                    prob=1.0 - std::exp(-30.0);

                if(infHist.at(j*T + i)==0)
                    llVal+=std::log(1-prob);
                else
                    llVal+=std::log(prob);
            }
        }
    }

    return llVal;
}
