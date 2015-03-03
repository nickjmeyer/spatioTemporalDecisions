#include "tuneGen.hpp"

double getDPow(const double & power, const double & alpha,
	       const std::vector<double> & caves){
  double meanCaves = std::accumulate(caves.begin(),caves.end(),0);
  meanCaves /= double(caves.size());

  return(std::log(std::log(1.25)*std::pow(meanCaves,2.0*power)/alpha + 1.0)/
	 std::log(2.0));
}


void rescaleD(const double & pastScale, const double & currScale,
	      std::vector<double> & d){
  double scale = currScale/pastScale;
  std::for_each(d.begin(),d.end(),[&scale](double & x){x=std::pow(x,scale);});
}

template <class S, class NT,class RN>
double TuneGenNT(S & s){
  NT nt;
  RN rn;

  double goal = 0.7;
  int numReps = 500;
  int numYears = s.fD.finalT;
  double tol = 0.001;

  std::vector<double> scaleD;
  njm::fromFile(scaleD, njm::sett.srcExt("rawD.txt"));
  double pastScale = 1.0;
  double currScale = getDPow(s.paramGen_r.power,s.paramGen_r.alpha,
			     s.fD.caves);

  rescaleD(pastScale,currScale,scaleD);
  s.fD.dist = scaleD;

  std::vector<double> par = s.paramGen_r.getPar();
  double power = s.paramGen_r.power;
  double val = rn.run(s,nt,numReps,numYears);
  double scale = 1.5, shrink = .975;
  int above = int(val > goal);
  int iter = 0;

  printf("Iter: %05d  >>>  Current value: %08.6f\r",
	 ++iter, val);

  while(std::abs(val - goal) > tol){
    if(val > goal){
      if(!above)
	scale*=shrink;
      
      std::for_each(par.begin(),par.end(),
		    [&scale](double & x){x*= 1.0 + scale;});
      
      above = 1;
    }
    else{
      if(above)
	scale*=shrink;

      std::for_each(par.begin(),par.end(),
		    [&scale](double & x){x*= 1.0/(1.0 + scale);});
      
      s.reset();
      
      above = 0;
    }

    s.paramGen_r.putPar(par);
    s.paramGen_r.power = power;
    s.paramEst_r.putPar(par);
    s.paramEst_r.power = power;

    s.reset();

    pastScale = currScale;
    currScale = getDPow(s.paramGen_r.power,s.paramGen_r.alpha,
			s.fD.caves);
    rescaleD(pastScale,currScale,scaleD);
    s.fD.dist = scaleD;


    val = rn.run(s,nt,numReps,numYears);
    printf("Iter: %05d  >>>  Current value: %08.6f\r", ++iter, val);
    fflush(stdout);
  }

  return(val);
}


template <class S, class PA, class RP>
double TuneGenPA(S & s){
  double trtSize = s.modelGen.tuneTrt(s.fD,s.paramGen);

  s.paramGen_r.trtPre = s.paramGen_r.trtAct = trtSize;
  s.paramEst_r.trtPre = s.paramEst_r.trtAct = trtSize;

  PA pa;
  RP rp;

  return rp.run(s,pa,500,s.fD.finalT);
}


int main(int argc, char ** argv){
  njm::sett.set(argc,argv);

  {
    typedef GravityModel GM;
    typedef GravityParam GP;
    typedef GM EM;
    typedef GP EP;

    typedef System<GM,GP,EM,EP> S;
    typedef NoTrt<EM,EP> NT;
    typedef ProximalAgent<EM,EP> PA;
    typedef MyopicAgent<EM,EP> MA;

    typedef ToyFeatures2<EM,EP> F;
    typedef RankAgent<F,EM,EP> RA;

    typedef VanillaRunnerNS<S,NT> RN;
    typedef VanillaRunnerNS<S,PA> RP;
    typedef VanillaRunnerNS<S,MA> RM;
    typedef VanillaRunnerNS<S,RA> RR;

    S s;
    s.paramEst_r = s.paramGen_r;
    s.reset();

    MA ma;
    RM rm;

    RA ra;
    RR rr;
    ra.reset();

    njm::message("Tuning Intercept");

    double valNT = TuneGenNT<S,NT,RN>(s);

    njm::message("Tuning Treatment");

    double valPA = TuneGenPA<S,PA,RP>(s);

    double valMA = rm.run(s,ma,500,s.fD.finalT);

    double valRA = rr.run(s,ra,500,s.fD.finalT);

    njm::message(" intcp: " + njm::toString(s.paramGen_r.intcp,"") +
		 "\n" +
		 " alpha: " + njm::toString(s.paramGen_r.alpha,"") +
		 "\n" +
		 " power: " + njm::toString(s.paramGen_r.power,"") +
		 "\n" +
		 "trtPre: " + njm::toString(s.paramGen_r.trtPre,"") +
		 "\n" +
		 "trtAct: " + njm::toString(s.paramGen_r.trtAct,"") +
		 "\n\n" +
		 " valNT: " + njm::toString(valNT,"") +
		 "\n" +
		 " valPA: " + njm::toString(valPA,"") +
		 "\n" +
		 " valMA: " + njm::toString(valMA,"") +
		 "\n" +
		 " valRA: " + njm::toString(valRA,""));

    s.paramGen_r.save();
  }

  
  {
    typedef GravityTimeInfModel GM;
    typedef GravityTimeInfParam GP;
    typedef GM EM;
    typedef GP EP;

    typedef System<GM,GP,EM,EP> S;
    typedef NoTrt<EM,EP> NT;
    typedef ProximalAgent<EM,EP> PA;
    typedef MyopicAgent<EM,EP> MA;

    typedef ToyFeatures2<EM,EP> F;
    typedef RankAgent<F,EM,EP> RA;

    typedef VanillaRunnerNS<S,NT> RN;
    typedef VanillaRunnerNS<S,PA> RP;
    typedef VanillaRunnerNS<S,MA> RM;
    typedef VanillaRunnerNS<S,RA> RR;

    S s;
    s.paramEst_r = s.paramGen_r;
    s.reset();

    MA ma;
    RM rm;

    RA ra;
    RR rr;
    ra.reset();

    njm::message("Tuning Intercept");

    double valNT = TuneGenNT<S,NT,RN>(s);

    njm::message("Tuning Treatment");

    double valPA = TuneGenPA<S,PA,RP>(s);

    double valMA = rm.run(s,ma,500,s.fD.finalT);

    double valRA = rr.run(s,ra,500,s.fD.finalT);

    njm::message(" intcp: " + njm::toString(s.paramGen_r.intcp,"") +
		 "\n" +
		 " alpha: " + njm::toString(s.paramGen_r.alpha,"") +
		 "\n" +
		 " power: " + njm::toString(s.paramGen_r.power,"") +
		 "\n" +
		 "    xi: " + njm::toString(s.paramGen_r.xi,"") +
		 "\n" +
		 "trtPre: " + njm::toString(s.paramGen_r.trtPre,"") +
		 "\n" +
		 "trtAct: " + njm::toString(s.paramGen_r.trtAct,"") +
		 "\n\n" +
		 " valNT: " + njm::toString(valNT,"") +
		 "\n" +
		 " valPA: " + njm::toString(valPA,"") +
		 "\n" +
		 " valMA: " + njm::toString(valMA,"") +
		 "\n" +
		 " valRA: " + njm::toString(valRA,""));


    s.paramGen_r.save();


    double priorMeanTrt = (s.paramGen_r.trtPre + s.paramGen_r.trtAct)/2.0;
    priorMeanTrt *= 4.0;

    // write new distance matrix to file
    njm::toFile(s.fD.dist,njm::sett.srcExt("d.txt"),
		std::ios_base::out,"\n","");
    // write prior mean of treatment effect
    njm::toFile(priorMeanTrt,njm::sett.srcExt("priorTrtMean.txt"),
		std::ios_base::out);
  }


  
  njm::sett.clean();
  
  return 0;
}
