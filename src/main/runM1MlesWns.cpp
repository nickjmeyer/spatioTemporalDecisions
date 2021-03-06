#include <gflags/gflags.h>
#include <glog/logging.h>
#include <iostream>
#include <iomanip>
#include "runM1MlesWns.hpp"

using namespace google;
using namespace gflags;


DEFINE_string(srcDir,"","Path to source directory");
DEFINE_bool(edgeToEdge,false,"Edge to edge transmission");
DEFINE_bool(dryRun,false,"Do not execute main");

int main(int argc, char ** argv){
    InitGoogleLogging(argv[0]);
    ParseCommandLineFlags(&argc,&argv,true);
    if(!FLAGS_dryRun) {
        njm::sett.setup(std::string(argv[0]),FLAGS_srcDir);

        njm::toFile(FLAGS_edgeToEdge,
                njm::sett.datExt("edgeToEdge_flag_",".txt"));

        if(FLAGS_edgeToEdge) {
            // typedef ModelTimeExpCavesGDistTrendPowCon MG;
            typedef Model2EdgeToEdge MG;

            typedef MG ME;

            typedef System<MG,ME> S;

            typedef NoTrt<ME> NT;
            typedef ProximalAgent<ME> PA;
            typedef MyopicAgent<ME> MA;
            typedef AllAgent<ME> AA;

            typedef WnsFeatures3<ME> F;
            typedef RankAgent<F,ME> RA;

            typedef M1SpOptim<S,RA,ME> SPO;

            typedef VanillaRunner<S,NT> R_NT;
            typedef VanillaRunner<S,PA> R_PA;
            typedef FitOnlyRunner<S,MA> R_MA;
            typedef VanillaRunner<S,AA> R_AA;
            typedef OptimRunner<S,RA,SPO> R_RA;


            // S s;
            S s("obsData.txt");
            s.setEdgeToEdge(FLAGS_edgeToEdge);
            s.modelGen_r.setType(MLES);
            s.modelEst_r.setType(MLES);

            int numReps = 100;
            Starts starts("startingLocations.txt");

            NT nt;
            PA pa;
            MA ma;
            AA aa;
            RA ra;

            pa.setEdgeToEdge(FLAGS_edgeToEdge);

            ra.tp.jitterScale = -1;
            ra.setEdgeToEdge(FLAGS_edgeToEdge);

            ma.setEdgeToEdge(FLAGS_edgeToEdge);

            SPO spo;
            spo.tp.fixSample = 1;

            R_NT r_nt;
            R_PA r_pa;
            R_MA r_ma;
            R_AA r_aa;
            R_RA r_ra;


            RunStats rs;

            // no treatment
            rs = r_nt.run(s,nt,numReps,s.fD.finalT,starts);
            njm::message("   No treatment: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("none, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            // proximal
            rs = r_pa.run(s,pa,numReps,s.fD.finalT,starts);
            njm::message("       Proximal: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("proximal, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            // myopic
            rs = r_ma.run(s,ma,numReps,s.fD.finalT,starts);
            njm::message("         Myopic: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("myopic, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            // all agent
            rs = r_aa.run(s,aa,numReps,s.fD.finalT,starts);
            njm::message("      All Agent: "
                    + njm::toString(rs.sMean(),"")
                    + " (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("all, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            // policy search
            rs = r_ra.run(s,ra,spo,numReps,s.fD.finalT,starts);
            njm::message("  Policy Search: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("ps, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

        } else {
            // typedef ModelTimeExpCavesGDistTrendPowCon MG;
            typedef Model2GravityEDist MG;

            typedef MG ME;

            typedef System<MG,ME> S;

            typedef NoTrt<ME> NT;
            typedef ProximalAgent<ME> PA;
            typedef MyopicAgent<ME> MA;
            typedef AllAgent<ME> AA;

            typedef WnsFeatures3<ME> F;
            typedef RankAgent<F,ME> RA;

            typedef M1SpOptim<S,RA,ME> SPO;

            typedef VanillaRunner<S,NT> R_NT;
            typedef VanillaRunner<S,PA> R_PA;
            typedef FitOnlyRunner<S,MA> R_MA;
            typedef VanillaRunner<S,AA> R_AA;
            typedef OptimRunner<S,RA,SPO> R_RA;


            // S s;
            S s("obsData.txt");
            s.setEdgeToEdge(FLAGS_edgeToEdge);
            s.modelGen_r.setType(MLES);
            s.modelEst_r.setType(MLES);

            int numReps = 100;
            Starts starts("startingLocations.txt");

            NT nt;
            PA pa;
            MA ma;
            AA aa;
            RA ra;

            pa.setEdgeToEdge(FLAGS_edgeToEdge);

            ra.tp.jitterScale = -1;
            ra.setEdgeToEdge(FLAGS_edgeToEdge);

            ma.setEdgeToEdge(FLAGS_edgeToEdge);

            SPO spo;
            spo.tp.fixSample = 1;

            R_NT r_nt;
            R_PA r_pa;
            R_MA r_ma;
            R_AA r_aa;
            R_RA r_ra;


            RunStats rs;

            // no treatment
            rs = r_nt.run(s,nt,numReps,s.fD.finalT,starts);
            njm::message("   No treatment: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("none, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            // proximal
            rs = r_pa.run(s,pa,numReps,s.fD.finalT,starts);
            njm::message("       Proximal: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("proximal, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            // myopic
            rs = r_ma.run(s,ma,numReps,s.fD.finalT,starts);
            njm::message("         Myopic: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("myopic, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            // all agent
            rs = r_aa.run(s,aa,numReps,s.fD.finalT,starts);
            njm::message("      All Agent: "
                    + njm::toString(rs.sMean(),"")
                    + " (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("all, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            // policy search
            rs = r_ra.run(s,ra,spo,numReps,s.fD.finalT,starts);
            njm::message("  Policy Search: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("ps, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);


        }
    }

    return 0;
}
