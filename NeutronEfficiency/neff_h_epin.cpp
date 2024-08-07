#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <typeinfo>

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLine.h>

#include "clas12reader.h"
#include "HipoChain.h"
#include "eventcut/eventcut.h"
#include "eventcut/functions.h"
#include "clas12ana.h"

using namespace std;
using namespace clas12;

const double c = 29.9792458;
const double mp = 0.938272;
const double m_piplus = 0.13957039;

void printProgress(double percentage);
double get_pin_mmiss(TVector3 p_b, TVector3 p_e, TVector3 ppi);
Double_t background_poly(Double_t *x, Double_t *par);
Double_t signal(Double_t *x, Double_t *par); 
Double_t mmiss_signal_gauss(Double_t *x, Double_t *par);
Double_t mmiss_signal_poly(Double_t *x, Double_t *par);
double * hist_projections(TCanvas * can, TH2D * hist2d, int num_hist, char v, double Mlow, double Mhigh);

// theta ranges
/*double t1 = 40;
double t2 = 50;
double t3 = 60;
double t4 = 70;
double t5 = 80;*/
double t1 = 45;
double t2 = 50;
double t3 = 55;
double t4 = 60;


// misc
int neff_pbins = 16;
int neff_tbins = 16;
int pgrid_x = ceil(sqrt(neff_pbins));
int pgrid_y = ceil((double)neff_pbins/(double)pgrid_x);
int tgrid_x = ceil(sqrt(neff_tbins));
int tgrid_y = ceil((double)neff_tbins/(double)tgrid_x);
double p_lo = 0.25;
double p_hi = 1.1;
double t_lo = 40;
double t_hi = 65;

void Usage()
{
  std::cerr << "Usage: ./code <MC=1,Data=0> <rgk cuts=1, no=0> <Ebeam(GeV)> <path/to/output.root> <path/to/output.pdf> <path/to/input.hipo> \n";
}


int main(int argc, char ** argv)
{

  if(argc < 6)
    {
      std::cerr<<"Wrong number of arguments.\n";
      Usage();
      return -1;
    }

  /////////////////////////////////////
 
  bool isMC = false;
  if(atoi(argv[1]) == 1){isMC=true;}

  bool is_rgk = false;
  if(atoi(argv[2]) == 1){is_rgk=true;}

  double Mlow = (is_rgk ? 0.7 : 0.85);
  double Mhigh = (is_rgk ? 1 : 1.05);
  double edep_cut = (is_rgk ? 2.5 : 5.0);
 
  double Ebeam = atof(argv[3]);

  
  // output file names
  TFile * outFile = new TFile(argv[4],"RECREATE");
  char * pdfFile = argv[5];

  char * basename = argv[4];
  basename[strlen(basename)-5] = '\0';
  string theta_name(string(basename) + "_theta.txt");
  theta_name.c_str();
  string p_name(string(basename) + "_p.txt");
  p_name.c_str();



  clas12root::HipoChain chain;
  for(int k = 6; k < argc; k++){
    cout<<"Input file "<<argv[k]<<endl;
    chain.Add(argv[k]);
  }
  auto config_c12=chain.GetC12Reader();
  chain.SetReaderTags({0});
  const std::unique_ptr<clas12::clas12reader>& c12=chain.C12ref();
  chain.db()->turnOffQADB();


  // create instance of clas12ana
  clas12ana clasAna;

  clasAna.readEcalSFPar("/w/hallb-scshelf2102/clas12/users/esteejus/rgm/Ana/cutFiles/paramsSF_LD2_x2.dat");
  clasAna.readEcalPPar("/w/hallb-scshelf2102/clas12/users/esteejus/rgm/Ana/cutFiles/paramsPI_LD2_x2.dat");

  clasAna.printParams();

        
  /////////////////////////////////////
  //Prepare histograms
  /////////////////////////////////////
  vector<TH1*> hist_list_1;
  vector<TH2*> hist_list_2;

  gStyle->SetTitleXSize(0.05);
  gStyle->SetTitleYSize(0.05);

  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleYOffset(0.8);

  char temp_name[100];
  char temp_title[100];



  /////////////////////////////////////
  //Histos: basic event selection
  /////////////////////////////////////
  TH1D * h_pid = new TH1D("particle pid","PID (All Particles)",1000,-3000,3000);
  hist_list_1.push_back(h_pid);

  /////////////////////////////////////
  //Histos: pions
  /////////////////////////////////////
  TH1D * h_pivertex = new TH1D("pi vertex","Pion vertex - electron vertex;v_{pi+} - v_{e} (cm)",50,-5,5);
  hist_list_1.push_back(h_pivertex);
  TH2D * h_dbetap = new TH2D("dbeta_p","#Delta #beta vs Momentum;Momentum (GeV/c);#beta_{meas} - p/sqrt(p^{2}+m^{2})",100,0,4,100,-0.2,0.2);
  hist_list_2.push_back(h_dbetap);
  TH1D * h_pitheta = new TH1D("pitheta","Pion #theta;#theta (degrees);Counts",180,0,180);
  hist_list_1.push_back(h_pitheta);


  /////////////////////////////////////
  //Histos: pmiss
  /////////////////////////////////////
  TH1D * h_thetamiss = new TH1D("thetamiss","Predicted Momentum Polar Angle;#theta_{pred} (degrees);Counts",50,40,90);
  hist_list_1.push_back(h_thetamiss);


  /////////////////////////////////////
  //Histos: candidates
  /////////////////////////////////////
  TH1D * h_mmiss_cand = new TH1D("mmiss_cand","Missing Mass p(e,e'#pi^{+})n;Missing Mass (GeV/c^{2});Events",100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_cand);
  TH1D * h_neff_pmiss_denom = new TH1D("neff_pmiss_denom","Neutron Candidates;p_{pred} (GeV/c);Counts",16,0,1.6);
  hist_list_1.push_back(h_neff_pmiss_denom);
  // efficiency angular dependence
  TH1D * h_neff_thetamiss_denom = new TH1D("neff_thetamiss_denom","#theta dependence of efficiency;#theta_{pred} (deg);efficiency",30,30,90);
  hist_list_1.push_back(h_neff_thetamiss_denom);
  TH1D * h_neff_phimiss_denom = new TH1D("neff_phimiss_denom","#phi dependence of efficiency;#phi;efficiency",36,-180,180);
  hist_list_1.push_back(h_neff_phimiss_denom);
  TH1D * h_nsize = new TH1D("nsize","Number of Reconstructed Neutrons in Event;Neutron number;Counts",10,0,10);
  hist_list_1.push_back(h_nsize);





  /////////////////////////////////////
  //Histos: mmiss vs pmiss by angle int
  /////////////////////////////////////
  // denominator
  TH2D * h_mmiss_pmiss_allt_denom = new TH2D("mmiss_pmiss_allt_denom","Neutron Candidates (all angles);p_{pred} (GeV/c);Missing Mass (GeV/c^{2})",100,p_lo,p_hi,100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_pmiss_allt_denom);
  TH2D * h_mmiss_pmiss_int1_denom = new TH2D("mmiss_pmiss_int1_denom","Neutron Candidates (int1);p_{pred} (GeV/c);M_{miss} (GeV/c^{2})",100,p_lo,p_hi,100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_pmiss_int1_denom);
  TH2D * h_mmiss_pmiss_int2_denom = new TH2D("mmiss_pmiss_int2_denom","Neutron Candidates (int2);p_{pred} (GeV/c);M_{miss} (GeV/c^{2})",100,p_lo,p_hi,100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_pmiss_int2_denom);
  TH2D * h_mmiss_pmiss_int3_denom = new TH2D("mmiss_pmiss_int3_denom","Neutron Candidates (int3);p_{pred} (GeV/c);M_{miss} (GeV/c^{2})",100,p_lo,p_hi,100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_pmiss_int3_denom);
  // numerator
  TH2D * h_mmiss_pmiss_allt_numer = new TH2D("mmiss_pmiss_allt_numer","Neutron Candidates (all angles);p_{pred} (GeV/c);Missing Mass (GeV/c^{2})",100,p_lo,p_hi,100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_pmiss_allt_numer);
  TH2D * h_mmiss_pmiss_int1_numer = new TH2D("mmiss_pmiss_int1_numer","Neutron Candidates (int1);p_{pred} (GeV/c);M_{miss} (GeV/c^{2})",100,p_lo,p_hi,100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_pmiss_int1_numer);
  TH2D * h_mmiss_pmiss_int2_numer = new TH2D("mmiss_pmiss_int2_numer","Neutron Candidates (int2);p_{pred} (GeV/c);M_{miss} (GeV/c^{2})",100,p_lo,p_hi,100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_pmiss_int2_numer);
  TH2D * h_mmiss_pmiss_int3_numer = new TH2D("mmiss_pmiss_int3_numer","Neutron Candidates (int3);p_{pred} (GeV/c);M_{miss} (GeV/c^{2})",100,p_lo,p_hi,100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_pmiss_int3_numer);

  /////////////////////////////////////
  //Histos: pmiss by angle int
  /////////////////////////////////////
  // denominator
  TH1D * h_pmiss_allt_denomD = new TH1D("pmiss_allt_denom","Neutron Candidates (all angles);p_{pred} (GeV/c);Efficiency",neff_pbins,p_lo,p_hi);
  hist_list_1.push_back(h_pmiss_allt_denomD);
  TH1D * h_pmiss_int1_denomD = new TH1D("pmiss_int1_denom","Neutron Candidates (int1);p_{pred} (GeV/c);Efficiency",neff_pbins,p_lo,p_hi);
  hist_list_1.push_back(h_pmiss_int1_denomD);
  TH1D * h_pmiss_int2_denomD = new TH1D("pmiss_int2_denom","Neutron Candidates (int2);p_{pred} (GeV/c);Efficiency",neff_pbins,p_lo,p_hi);
  hist_list_1.push_back(h_pmiss_int2_denomD);
  TH1D * h_pmiss_int3_denomD = new TH1D("pmiss_int3_denom","Neutron Candidates (int3);p_{pred} (GeV/c);Efficiency",neff_pbins,p_lo,p_hi);
  hist_list_1.push_back(h_pmiss_int3_denomD);
  // numerator
  TH1D * h_pmiss_allt_numerD = new TH1D("pmiss_allt_numer","Neutrons (all angles);p_{pred} (GeV/c);Efficiency",neff_pbins,p_lo,p_hi);
  hist_list_1.push_back(h_pmiss_allt_numerD);
  TH1D * h_pmiss_int1_numerD = new TH1D("pmiss_int1_numer","Neutrons (int1);p_{pred} (GeV/c);Efficiency",neff_pbins,p_lo,p_hi);
  hist_list_1.push_back(h_pmiss_int1_numerD);
  TH1D * h_pmiss_int2_numerD = new TH1D("pmiss_int2_numer","Neutrons (int2);p_{pred} (GeV/c);Efficiency",neff_pbins,p_lo,p_hi);
  hist_list_1.push_back(h_pmiss_int2_numerD);
  TH1D * h_pmiss_int3_numerD = new TH1D("pmiss_int3_numer","Neutrons (int3);p_{pred} (GeV/c);Efficiency",neff_pbins,p_lo,p_hi);
  hist_list_1.push_back(h_pmiss_int3_numerD);



  /////////////////////////////////////
  //Histos: neutron required
  /////////////////////////////////////
  TH1D * h_tof = new TH1D("tof_all","Time of Flight;TOF (ns);Counts",200,-10,20);
  hist_list_1.push_back(h_tof);
   TH2D * h_nangles = new TH2D("n angles","Neutron Angular Distribution;Phi;Theta",48,-180,180,110,35,100);
  hist_list_2.push_back(h_nangles);
  TH2D * h_mmiss_beta = new TH2D("mmiss_beta","Missing Mass vs Beta p(e,e'#pi^{+})n;Beta;Missing Mass (GeV/c^{2})",100,0,0.9,100,0,1.5);
  hist_list_2.push_back(h_mmiss_beta);
  TH1D * h_energy = new TH1D("energy","Neutron Energy Deposition",100,0,100);
  hist_list_1.push_back(h_energy);
  TH2D * h_compare = new TH2D("compare","Comparison between p_{n} and p_{pred};p_{pred}-p_{n};Angle between p_{pred} and p_{n}",100,-1,1,100,0,50);
  hist_list_2.push_back(h_compare);

  /////////////////////////////////////
  //Histos: good neutrons
  /////////////////////////////////////
  TH2D * h_pmiss_pn = new TH2D("pmiss_pn","Predicted Momentum vs Neutron Momentum;p_{neutron} (GeV/c);p_{pred} (GeV/c)",100,0,1.4,100,0,1.4);
  hist_list_2.push_back(h_pmiss_pn);
  TH1D * h_cos0 = new TH1D("cos0","cos #theta_{pred,n};cos #theta;Counts",30,-1.05,1.05);
  hist_list_1.push_back(h_cos0);
  TH1D * h_dphi = new TH1D("dphi","#Delta #phi = #phi_{pred} - #phi_{n};#Delta #phi;Counts",48,-180,180);
  hist_list_1.push_back(h_dphi);


  /////////////////////////////////////
  //Histos: efficiency
  /////////////////////////////////////
  TH2D * h_mmiss_theta_denom = new TH2D("mmiss_theta_denom","Missing Mass vs #theta_{pred} (denominator);#theta_{pred} (deg);Missing Mass (GeV/c^{2})",100,t_lo,t_hi,100,0.5,1.5);
  TH2D * h_mmiss_theta_numer = new TH2D("mmiss_theta_numer","Missing Mass vs #theta_{pred} (numerator);#theta_{pred} (deg);Missing Mass (GeV/c^{2})",100,t_lo,t_hi,100,0.5,1.5);



  /////////////////////////////////////
  //Histos: final neutron selection
  /////////////////////////////////////
  TH2D * h_p_theta = new TH2D("p theta","Momentum vs Theta (Neutrons);Theta;Momentum (GeV/c)",180,30,150,100,0,1.5);
  hist_list_2.push_back(h_p_theta);
  TH2D * h_p_phi = new TH2D("p phi","Momentum vs Phi (Neutrons);Phi;Momentum (GeV/c)",48,-180,180,100,0,2);
  hist_list_2.push_back(h_p_phi);
  TH2D * h_pmiss_thetamiss = new TH2D("pmiss_thetamiss","Predicted Momentum vs p_{pred} Polar Angle;#theta_{pred} (degrees);p_{pred} (GeV/c)",100,30,150,100,0,1.5);
  hist_list_2.push_back(h_pmiss_thetamiss);
  TH2D * h_pmiss_pn_cut = new TH2D("pmiss_pn_cut","Predicted Momentum vs Neutron Momentum;p_{neutron};p_{pred}",100,0,1.4,100,0,1.4);
  hist_list_2.push_back(h_pmiss_pn_cut);
  TH1D * h_mmiss_withn = new TH1D("mmiss_withn","Missing Mass p(e,e'#pi^{+}n);Missing Mass (GeV/c^{2})",100,0.5,1.5);
  hist_list_1.push_back(h_mmiss_withn);
  TH1D * h_neff_pmiss_numer = new TH1D("neff_pmiss_numer","Neutrons;p_{pred} (GeV/c);Counts",16,0,1.6);
  hist_list_1.push_back(h_neff_pmiss_numer);
  // efficiency angular dependence
  TH1D * h_neff_thetamiss_numer = new TH1D("neff_thetamiss_numer","CND Neutron Detection Efficiency;Polar Angle #theta (deg);Detection Efficiency",30,30,90);
  hist_list_1.push_back(h_neff_thetamiss_numer);
  TH1D * h_neff_phimiss_numer = new TH1D("neff_phimiss_numer","#phi dependence of efficiency;#phi;efficiency",36,-180,180);
  hist_list_1.push_back(h_neff_phimiss_numer);


  /////////////////////////////////////
  //Histos: 2d efficiency stuff
  /////////////////////////////////////
  TH2D * h_cand2d = new TH2D("cand2d","Neutron Candidates;p_{pred} (GeV/c);#theta_{pred} (degrees)",10,0.2,1.2,6,40,70);
  hist_list_2.push_back(h_cand2d);
  h_cand2d->SetStats(0);
  TH2D * h_det2d = new TH2D("det2d","Detected Neutrons;p_{pred} (GeV/c);#theta_{pred} (degrees)",10,0.2,1.2,6,40,70);
  hist_list_2.push_back(h_det2d);
  h_det2d->SetStats(0);






  for(int i=0; i<hist_list_1.size(); i++){
    hist_list_1[i]->Sumw2();
    hist_list_1[i]->GetXaxis()->CenterTitle();
    hist_list_1[i]->GetYaxis()->CenterTitle();
  }
  for(int i=0; i<hist_list_2.size(); i++){
    hist_list_2[i]->Sumw2();
    hist_list_2[i]->GetXaxis()->CenterTitle();
    hist_list_2[i]->GetYaxis()->CenterTitle();
  }


  int counter = 0;



  //Define cut class
  while(chain.Next()==true){

    //Display completed  
    counter++;
    if((counter%1000000) == 0){
      cerr << "\n" <<counter/1000000 <<" million completed";
    }    

    if((counter%100000) == 0){
      cerr << ".";
    }    

    // get particles by type
    clasAna.Run(c12);
    auto elec = clasAna.getByPid(11);
    auto prot = clasAna.getByPid(2212);
    auto neut = clasAna.getByPid(2112);
    auto pip = clasAna.getByPid(211);
    auto allParticles=c12->getDetParticles();
    double weight = 1;
    if(isMC){weight=c12->mcevent()->getWeight();}

    double ts = c12->event()->getStartTime(); // event start time



    // GENERAL EVENT SELECTION
    if (prot.size() != 0) {continue;} // before p>0 // hen !=0
    if (pip.size() != 1) {continue;} 
    if (elec.size() != 1) {continue;} // before e>1 // then !=1




    // ELECTRONS
    double vze = elec[0]->par()->getVz();
    double e_theta = elec[0]->getTheta()*180./M_PI;
    if (e_theta>35.) {continue;}


    // PIONS +
    TVector3 p_pip;
    p_pip.SetMagThetaPhi(pip[0]->getP(),pip[0]->getTheta(),pip[0]->getPhi());
    double vzpi = pip[0]->par()->getVz();
    double dbeta = pip[0]->par()->getBeta() - p_pip.Mag()/sqrt(p_pip.Mag2()+m_piplus*m_piplus);
    double pitheta = pip[0]->getTheta()*180./M_PI;

    // reject particles with the wrong PID
    bool trash = 0;
    for (int i=0; i<allParticles.size(); i++)
      {
      int pid = allParticles[i]->par()->getPid();
      h_pid->Fill(pid);
      if (pid!=2112 && pid!=11 && pid!=211 && pid!=22 && pid!=0) {trash=1;}
      }
    if (trash==1) {continue;}

    // pion cuts
    h_pivertex->Fill(vzpi-vze,weight);
    if ((vzpi-vze)<-4. || (vzpi-vze)>2.) {continue;}
    h_dbetap->Fill(p_pip.Mag(),dbeta,weight);
    if (dbeta<-0.03 || dbeta>0.03) {continue;}   
    if (p_pip.Mag() < 0.4) {continue;}
    if (p_pip.Mag() > 3.) {continue;}
    h_pitheta->Fill(pitheta,weight);
    if (pitheta>35.) {continue;}


    // Missing Mass and missing momentum
    TVector3 p_e;
    TVector3 p_b(0,0,Ebeam);
    p_e.SetMagThetaPhi(elec[0]->getP(),elec[0]->getTheta(),elec[0]->getPhi());
    double mmiss = get_pin_mmiss(p_b,p_e,p_pip);
    TVector3 pmiss = p_b - p_e - p_pip;
    double thetamiss = pmiss.Theta()*180/M_PI;

    // thetamiss histo
    h_thetamiss->Fill(thetamiss,weight);

    // missing momentum cuts
    if (thetamiss<40.) {continue;}
    if (thetamiss>140.) {continue;}
    if (pmiss.Mag() < 0.25) {continue;} // beta=0.2  // previously 0.1918
    if (pmiss.Mag() > 1.25) {continue;} // beta=0.8  // previoulsy 1.2528

    h_mmiss_theta_denom->Fill(thetamiss,mmiss,weight);
    h_mmiss_cand->Fill(mmiss,weight);   



    if (mmiss>Mlow && mmiss<Mhigh)
    {
      h_neff_thetamiss_denom->Fill(thetamiss,weight);
      h_neff_phimiss_denom->Fill(pmiss.Phi()*180/M_PI);
      h_neff_pmiss_denom->Fill(pmiss.Mag(),weight);
      h_pmiss_allt_denomD->Fill(pmiss.Mag(),weight);
      if (thetamiss>t1 && thetamiss<t2) {h_pmiss_int1_denomD->Fill(pmiss.Mag(),weight);}
      else if (thetamiss>t2 && thetamiss<t3) {h_pmiss_int2_denomD->Fill(pmiss.Mag(),weight);}
      else if (thetamiss>t3 && thetamiss<t4) {h_pmiss_int3_denomD->Fill(pmiss.Mag(),weight);}

      h_cand2d->Fill(pmiss.Mag(),thetamiss,weight);

    }

    // does not need mmiss cut - for demonstration only (not eff calculation)
    h_mmiss_pmiss_allt_denom->Fill(pmiss.Mag(),mmiss,weight);
    if (thetamiss>t1 && thetamiss<t2) {h_mmiss_pmiss_int1_denom->Fill(pmiss.Mag(),mmiss,weight);}
    else if (thetamiss>t2 && thetamiss<t3) {h_mmiss_pmiss_int2_denom->Fill(pmiss.Mag(),mmiss,weight);}
    else if (thetamiss>t3 && thetamiss<t4) {h_mmiss_pmiss_int3_denom->Fill(pmiss.Mag(),mmiss,weight);}


    // REQUIRE A NEUTRON HERE


    // NEUTRON NUMBER
    double sz = neut.size();
    h_nsize->Fill(sz);


    int pick = -1;
    if (neut.size() < 1) {continue;}
    else
    {
      double lowest_dphi = 180;
      for (int i=0; i<neut.size(); i++)
      {
        // in CND? if no - skip to next neutron in event
        bool is_CND1 = neut[i]->sci(CND1)->getDetector()==3;
        bool is_CND2 = neut[i]->sci(CND2)->getDetector()==3;
        bool is_CND3 = neut[i]->sci(CND3)->getDetector()==3;
        bool is_CTOF = neut[i]->sci(CTOF)->getDetector()==4;
        if (is_rgk && !is_CND1 && !is_CND2 && !is_CND3) {continue;}
        if (!is_rgk && !is_CND1 && !is_CND2 && !is_CND3 && !is_CTOF) {continue;}
  
        // in expected theta range? if no - skip to next neutron in event
        double n_theta = neut[i]->getTheta()*180./M_PI;
        if (n_theta==0) {continue;}
        if (n_theta<40) {continue;}
        if (n_theta>140) {continue;}
  
        // is beta high enough? if no - skip to next neutron in event
        double beta_n = neut[i]->par()->getBeta();
        if (beta_n<0.25) {continue;}
  
        // pick neutron with lowest dphi
        double this_dphi = std::abs( neut[i]->getPhi()*180./M_PI - pmiss.Phi()*180./M_PI );
        if (this_dphi < lowest_dphi)
        {
          pick = i;
          lowest_dphi = this_dphi;
        }
      }
    }

    if (pick==-1) {continue;}


    // get neutron information
    bool is_CND1 = neut[pick]->sci(CND1)->getDetector()==3;
    bool is_CND2 = neut[pick]->sci(CND2)->getDetector()==3;
    bool is_CND3 = neut[pick]->sci(CND3)->getDetector()==3;
    bool is_CTOF = neut[pick]->sci(CTOF)->getDetector()==4;
    double n_theta = neut[pick]->getTheta()*180./M_PI;
    double n_phi = neut[pick]->getPhi()*180./M_PI;
    double beta_n = neut[pick]->par()->getBeta();
    double tof_n = 0;
    double path = 0;
    double energy = neut[pick]->sci(CND1)->getEnergy() + neut[pick]->sci(CND2)->getEnergy() + neut[pick]->sci(CND3)->getEnergy() + neut[pick]->sci(CTOF)->getEnergy();
    
    if (!is_rgk && is_CTOF)
    {
      tof_n = neut[pick]->sci(CTOF)->getTime() - ts;
      path = neut[pick]->sci(CTOF)->getPath();
    }
    else if (is_CND1)
    {
      tof_n = neut[pick]->sci(CND1)->getTime() - ts;
      path = neut[pick]->sci(CND1)->getPath();
    }
    else if (is_CND2)
    {
      tof_n = neut[pick]->sci(CND2)->getTime() - ts;
      path = neut[pick]->sci(CND2)->getPath();
    }
    else if (is_CND3)
    {
      tof_n = neut[pick]->sci(CND3)->getTime() - ts;
      path = neut[pick]->sci(CND3)->getPath();
    }

    // tof and energy cuts
    h_tof->Fill(tof_n,weight);
    h_energy->Fill(energy,weight);
    //if (tof_n<0) {continue;}
    if (energy<edep_cut) {continue;}


    TVector3 pn;
    pn.SetMagThetaPhi(neut[pick]->getP(),neut[pick]->getTheta(),neut[pick]->getPhi());
    double dp = (pmiss.Mag()-pn.Mag());
    double dpp = dp/pmiss.Mag();

    h_compare->Fill(dp,pn.Angle(pmiss)*180./M_PI);

    //if (pn.Mag()<0.25) {continue;}
    //if (pn.Mag()>1.25) {continue;}
    if (!is_rgk && (dp<-0.2 || dp>0.2)) {continue;}
    if (!is_rgk && pn.Angle(pmiss)*180./M_PI>25) {continue;}
    if (is_rgk && abs(pn.Phi()*180./M_PI-pmiss.Phi()*180./M_PI)>20) {continue;}

    TVector3 vecX( neut[pick]->par()->getPx(), neut[pick]->par()->getPy(), neut[pick]->par()->getPz() );
    double cos0 = pmiss.Dot(vecX) / (pmiss.Mag() * vecX.Mag() );
    double dphi = pmiss.Phi()*180./M_PI - neut[pick]->getPhi()*180./M_PI;

    // neutron histos

    h_nangles->Fill(n_phi,n_theta,weight);
    h_mmiss_beta->Fill(beta_n,mmiss,weight);

    // more neutron histos
    h_pmiss_pn->Fill(pn.Mag(),pmiss.Mag(),weight);
    h_cos0->Fill(cos0,weight);
    h_dphi->Fill(dphi,weight);

    // exclusive cuts (requiring info from other final state particles)
    //if (cos0 < 0.9) {continue;}
    //if (std::abs(dphi)>20.) {continue;}


    // histos after requiring pn along pmiss
    

    h_mmiss_theta_numer->Fill(thetamiss,mmiss,weight);


    h_p_theta->Fill(n_theta,pn.Mag(),weight);
    h_p_phi->Fill(n_phi,pn.Mag(),weight);
    //h_pmiss_theta->Fill(neut[pick]->getTheta()*180./M_PI,pmiss.Mag(),weight);
    h_pmiss_thetamiss->Fill(thetamiss,pmiss.Mag(),weight);

    h_pmiss_pn_cut->Fill(pn.Mag(),pmiss.Mag(),weight);
    h_mmiss_withn->Fill(mmiss,weight);


    if (mmiss>Mlow && mmiss<Mhigh)
    {
      h_det2d->Fill(pmiss.Mag(),thetamiss,weight); // actually detected neutrons

      h_neff_thetamiss_numer->Fill(thetamiss,weight);
      h_neff_phimiss_numer->Fill(pmiss.Phi()*180/M_PI);
      h_neff_pmiss_numer->Fill(pmiss.Mag(),weight);
      h_pmiss_allt_numerD->Fill(pmiss.Mag(),weight);
      if (thetamiss>t1 && thetamiss<t2) {h_pmiss_int1_numerD->Fill(pmiss.Mag(),weight);}
      else if (thetamiss>t2 && thetamiss<t3) {h_pmiss_int2_numerD->Fill(pmiss.Mag(),weight);}
      else if (thetamiss>t3 && thetamiss<t4) {h_pmiss_int3_numerD->Fill(pmiss.Mag(),weight);}
    }

    // does not need mmiss cut - for demonstration only (not eff calculation)
    h_mmiss_pmiss_allt_numer->Fill(pmiss.Mag(),mmiss,weight);
    if (thetamiss>t1 && thetamiss<t2) {h_mmiss_pmiss_int1_numer->Fill(pmiss.Mag(),mmiss,weight);}
    else if (thetamiss>t2 && thetamiss<t3) {h_mmiss_pmiss_int2_numer->Fill(pmiss.Mag(),mmiss,weight);}
    else if (thetamiss>t3 && thetamiss<t4) {h_mmiss_pmiss_int3_numer->Fill(pmiss.Mag(),mmiss,weight);}

  }



  cout<<counter<<endl;

  outFile->cd();
  for(int i=0; i<hist_list_1.size(); i++){
    hist_list_1[i]->Write();
  }
  for(int i=0; i<hist_list_2.size(); i++){
    hist_list_2[i]->Write();
  }








  /////////////////////////////////////////////////////
  //Now create the output PDFs
  /////////////////////////////////////////////////////
  int pixelx = 1980;
  int pixely = 1530;
  TCanvas * myCanvas = new TCanvas("myPage","myPage",pixelx,pixely);
  TCanvas * myText = new TCanvas("myText","myText",pixelx,pixely);
  TLatex text;
  text.SetTextSize(0.05);

  char fileName[100];
  sprintf(fileName,"%s[",pdfFile);
  myText->SaveAs(fileName);
  sprintf(fileName,"%s",pdfFile);



  /////////////////////////////////////////////////////
  //Histos
  /////////////////////////////////////////////////////
 

  // pions
  myText->cd();
  text.DrawLatex(0.2,0.9,"Basic electron cuts");
  text.DrawLatex(0.2,0.8,"Hydrogen");
  text.DrawLatex(0.2,0.7,"p(e,e'#pi^{+})n");
  text.DrawLatex(0.2,0.6,"final state: 0p, 1pi+, 1e");
  text.DrawLatex(0.2,0.5,"#theta_{e} < 35");
  myText->Print(fileName,"pdf");
  myText->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pid->Draw();
  myCanvas->cd(2);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear(); 
 
  // pions
  myText->cd();
  text.DrawLatex(0.2,0.9,"Basic electron cuts");
  text.DrawLatex(0.2,0.8,"Hydrogen");
  text.DrawLatex(0.2,0.7,"p(e,e'#pi^{+})n");
  text.DrawLatex(0.2,0.6,"allow only PID=0,11,22,211,2112");
  myText->Print(fileName,"pdf");
  myText->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pivertex->Draw();
  TLine * l_pivertex1 = new TLine(-4,0,-4,h_pivertex->GetMaximum());
  l_pivertex1->SetLineColor(kRed);
  l_pivertex1->SetLineWidth(3);
  l_pivertex1->Draw("same");
  TLine * l_pivertex2 = new TLine(2,0,2,h_pivertex->GetMaximum());
  l_pivertex2->SetLineColor(kRed);
  l_pivertex2->SetLineWidth(3);
  l_pivertex2->Draw("same");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_dbetap->Draw("colz");
  TLine * l_dbetap1 = new TLine(0.4,-0.2,0.4,0.2);
  l_dbetap1->SetLineColor(kRed);
  l_dbetap1->SetLineWidth(3);
  l_dbetap1->Draw("same");
  TLine * l_dbetap2 = new TLine(3,-0.2,3,0.2);
  l_dbetap2->SetLineColor(kRed);
  l_dbetap2->SetLineWidth(3);
  l_dbetap2->Draw("same");
  TLine * l_dbetap3 = new TLine(0,-0.03,4,-0.03);
  l_dbetap3->SetLineColor(kRed);
  l_dbetap3->SetLineWidth(3);
  l_dbetap3->Draw("same");
  TLine * l_dbetap4 = new TLine(0,0.03,4,0.03);
  l_dbetap4->SetLineColor(kRed);
  l_dbetap4->SetLineWidth(3);
  l_dbetap4->Draw("same");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear(); 

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pitheta->Draw("colz");
  TLine * l_pitheta = new TLine(35,0,35,h_pitheta->GetMaximum());
  l_pitheta->SetLineColor(kRed);
  l_pitheta->SetLineWidth(3);
  l_pitheta->Draw("same");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear(); 
  
  
  // all photons and neutrons
  myText->cd();
  text.DrawLatex(0.2,0.9,"Basic electron cuts");
  text.DrawLatex(0.2,0.8,"Hydrogen run 015017");
  text.DrawLatex(0.2,0.7,"p(e,e'#pi^{+})n");
  text.DrawLatex(0.2,0.6,"-4 cm < v_{#pi+}-v_{e} < 2 cm");
  text.DrawLatex(0.2,0.5,"-0.03 < #Delta #beta < 0.03");
  text.DrawLatex(0.2,0.4,"p_{#pi+} > 0.4 GeV/c");
  text.DrawLatex(0.2,0.3,"#theta_{#pi} < 35");
  myText->Print(fileName,"pdf");
  myText->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_thetamiss->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();



  myText->cd();
  text.DrawLatex(0.2,0.9,"Basic electron cuts");
  text.DrawLatex(0.2,0.8,"Hydrogen run 015017");
  text.DrawLatex(0.2,0.7,"p(e,e'#pi^{+})n");
  text.DrawLatex(0.2,0.6,"pion cuts");
  text.DrawLatex(0.2,0.5,"40 deg < #theta_{miss} < 140 deg");
  text.DrawLatex(0.2,0.4,"0.094 GeV/c < p_{miss} < 1.25 GeV/c");
  myText->Print(fileName,"pdf");
  myText->Clear();


  ///////////////////////////
  // Denom: momentum dependence
  ///////////////////////////
  myText->cd();
  text.DrawLatex(0.2,0.9,"Get n_{eff} vs p (denominator)");
  myText->Print(fileName,"pdf");
  myText->Clear();

  // all angles
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_pmiss_allt_denom->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  // all angles
  myCanvas->Divide(1,1);
  double * justhereforthehist0 = hist_projections(myCanvas,h_mmiss_pmiss_allt_denom,neff_pbins, 'p', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  // theta interval 1
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_pmiss_int1_denom->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  double * sigd_int1 = hist_projections(myCanvas,h_mmiss_pmiss_int1_denom,neff_pbins, 'p', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  TH1D * h_pmiss_int1_denom = new TH1D("pmiss_int1_denom","Efficiency denominator (int1);p_{pred} (GeV/c)",neff_pbins,p_lo,p_hi);
  for (int i=0; i<neff_pbins; i++)
  {
    h_pmiss_int1_denom->SetBinContent(i,*(sigd_int1+i));
//std::cout << "int 1, val " << i << " = " << *(sigd_int1+i) << '\n';
    h_pmiss_int1_denom->SetBinError(i,sqrt(*(sigd_int1+i)));
//std::cout << *(sigd_int1+i);
//std::cout
  }

  // theta interval 2
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_pmiss_int2_denom->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  double * sigd_int2 = hist_projections(myCanvas,h_mmiss_pmiss_int2_denom,neff_pbins, 'p', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  TH1D * h_pmiss_int2_denom = new TH1D("pmiss_int2_denom","Efficiency denominator (int2);p_{pred} (GeV/c)",neff_pbins,p_lo,p_hi);
  for (int i=0; i<neff_pbins; i++)
  {
    h_pmiss_int2_denom->SetBinContent(i,*(sigd_int2+i));
//std::cout << "int 2, val " << i << " = " << *(sigd_int2+i) << '\n';
    h_pmiss_int2_denom->SetBinError(i,sqrt(*(sigd_int2+i)));
  }

  // theta interval 3
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_pmiss_int3_denom->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  double * sigd_int3 = hist_projections(myCanvas,h_mmiss_pmiss_int3_denom,neff_pbins, 'p', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  TH1D * h_pmiss_int3_denom = new TH1D("pmiss_int3_denom","Efficiency denominator (int3);p_{pred} (GeV/c)",neff_pbins,p_lo,p_hi);
  for (int i=0; i<neff_pbins; i++)
  {
    h_pmiss_int3_denom->SetBinContent(i,*(sigd_int3+i));
//std::cout << "int 3, val " << i << " = " << *(sigd_int3+i) << '\n';
    h_pmiss_int3_denom->SetBinError(i,sqrt(*(sigd_int3+i)));
  }

  // denominator - all theta intervals
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pmiss_int1_denom->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pmiss_int2_denom->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();
  
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pmiss_int3_denom->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();



  ///////////////////////////
  // Denom: theta dependence
  ///////////////////////////
  myText->cd();
  text.DrawLatex(0.2,0.9,"Get n_{eff} vs #theta (denominator)");
  myText->Print(fileName,"pdf");
  myText->Clear();

  // theta denominator
  myText->cd();
  text.DrawLatex(0.2,0.9,"Theta denominator");
  myText->Print(fileName,"pdf");
  myText->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_theta_denom->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  double * sigd_t = hist_projections(myCanvas,h_mmiss_theta_denom,neff_tbins, 't', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();







  // histos requiring a neutron start here
  
  myText->cd();
  text.DrawLatex(0.2,0.5,"p(e,e'#pi^{+}n)");
  myText->Print(fileName,"pdf");
  myText->Clear();


  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_nsize->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  myText->cd();
  text.DrawLatex(0.2,0.9,"Basic electron cuts");
  text.DrawLatex(0.2,0.8,"Hydrogen run 015017");
  text.DrawLatex(0.2,0.7,"p(e,e'#pi^{+})n");
  text.DrawLatex(0.2,0.6,"pion cuts");
  text.DrawLatex(0.2,0.5,"p_{miss}, M_{miss} cuts");
  text.DrawLatex(0.2,0.4,"Require at least 1 neutron in CND");
  text.DrawLatex(0.2,0.3,"Neutron in at least 1 lever of CND");
  text.DrawLatex(0.2,0.2,"Pick neutron closest to p_{miss} in #phi");
  myText->Print(fileName,"pdf");
  myText->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_compare->Draw("colz");
  h_compare->SetStats(0);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_nangles->Draw("colz");
  h_nangles->SetStats(0);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_beta->Draw("colz");
  h_mmiss_beta->SetStats(0);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_tof->Draw();
  h_tof->SetStats(0);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_energy->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  // all neutrons
  myText->cd();
  text.DrawLatex(0.2,0.9,"Basic electron cuts");
  text.DrawLatex(0.2,0.8,"Hydrogen run 015017");
  text.DrawLatex(0.2,0.7,"p(e,e'#pi^{+})n");
  text.DrawLatex(0.2,0.6,"pion cuts");
  text.DrawLatex(0.2,0.5,"p_{miss}, M_{miss} cuts");
  text.DrawLatex(0.2,0.4,"Require 1 neutron in CND");
  text.DrawLatex(0.2,0.3,"exclude #theta_{n}=0, #phi_{n}=0");
  text.DrawLatex(0.2,0.2,"40 deg < #theta_{n} < 140 deg");
  text.DrawLatex(0.2,0.1,"0.1 < #beta_{n} < 0.8");
  myText->Print(fileName,"pdf");
  myText->Clear();


  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pmiss_pn->Draw("colz");
  TF1 * line = new TF1("line","x",0,2);
  line->Draw("same");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();



  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_cos0->Draw();
  h_cos0->SetStats(0);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_dphi->Draw();
  h_dphi->SetStats(0);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();



  // neutrons in direction of pmiss
  myText->cd();
  text.DrawLatex(0.2,0.9,"Basic electron cuts");
  text.DrawLatex(0.2,0.8,"Hydrogen run 015017");
  text.DrawLatex(0.2,0.7,"p(e,e'#pi^{+})n");
  text.DrawLatex(0.2,0.6,"pion cuts");
  text.DrawLatex(0.2,0.5,"p_{miss}, M_{miss} cuts");
  text.DrawLatex(0.2,0.4,"exclude #theta_{n}=0, #phi_{n}=0");
  text.DrawLatex(0.2,0.3,"40 deg < #theta_{n} < 140 deg");
  text.DrawLatex(0.2,0.2,"0.1 < #beta_{n} < 0.8");
  text.DrawLatex(0.2,0.1,"cos #theta_{pmiss,pn} > 0.9");
  myText->Print(fileName,"pdf");
  myText->Clear();



  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pmiss_pn_cut->Draw("colz");
  h_pmiss_pn_cut->SetStats(0);
  line->Draw("same");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_p_theta->Draw("colz");
  h_p_theta->SetStats(0);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_p_phi->Draw("colz");
  h_p_phi->SetStats(0);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pmiss_thetamiss->Draw("colz");
  h_pmiss_thetamiss->SetStats(0);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  



  ///////////////////////////
  // Numer: momentum dependence
  ///////////////////////////
  myText->cd();
  text.DrawLatex(0.2,0.9,"Get n_{eff} vs p (numerator)");
  myText->Print(fileName,"pdf");
  myText->Clear();

  // all angles
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_pmiss_allt_numer->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  // all angles
  myCanvas->Divide(1,1);
  double * justhereforthehist = hist_projections(myCanvas,h_mmiss_pmiss_allt_numer,neff_pbins, 'p', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  // theta interval 1
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_pmiss_int1_numer->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  double * sign_int1 = hist_projections(myCanvas,h_mmiss_pmiss_int1_numer,neff_pbins, 'p', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  TH1D * h_pmiss_int1_numer = new TH1D("pmiss_int1_numer","Efficiency numerator (int1);p_{pred} (GeV/c)",neff_pbins,p_lo,p_hi);
  for (int i=0; i<neff_pbins; i++)
  {
    h_pmiss_int1_numer->SetBinContent(i,*(sign_int1+i));
    h_pmiss_int1_numer->SetBinError(i,sqrt(*(sign_int1+i)));
  }

  // theta interval 2
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_pmiss_int2_numer->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  double * sign_int2 = hist_projections(myCanvas,h_mmiss_pmiss_int2_numer,neff_pbins, 'p', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  TH1D * h_pmiss_int2_numer = new TH1D("pmiss_int2_numer","Efficiency numerator (int2);p_{pred} (GeV/c)",neff_pbins,p_lo,p_hi);
  for (int i=0; i<neff_pbins; i++)
  {
    h_pmiss_int2_numer->SetBinContent(i,*(sign_int2+i));
    h_pmiss_int2_numer->SetBinError(i,sqrt(*(sign_int2+i)));
  }

  // theta interval 3
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_pmiss_int3_numer->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  double * sign_int3 = hist_projections(myCanvas,h_mmiss_pmiss_int3_numer,neff_pbins, 'p', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  TH1D * h_pmiss_int3_numer = new TH1D("pmiss_int3_numer","Efficiency numerator (int3);p_{pred} (GeV/c)",neff_pbins,p_lo,p_hi);
  for (int i=0; i<neff_pbins; i++)
  {
    h_pmiss_int3_numer->SetBinContent(i,*(sign_int3+i));
    h_pmiss_int3_numer->SetBinError(i,sqrt(*(sign_int3+i)));
  }

  // numerator - all theta intervals
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pmiss_int1_numer->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pmiss_int2_numer->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_pmiss_int3_numer->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myText->cd();
  text.DrawLatex(0.2,0.9,"Theta numerator");
  myText->Print(fileName,"pdf");
  myText->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_theta_numer->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  myCanvas->Divide(1,1);
  hist_projections(myCanvas,h_mmiss_theta_numer,neff_tbins, 't', Mlow, Mhigh);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();






  // efficiency as a function of phi miss
  myCanvas->Divide(2,1);
  myCanvas->cd(1);
  h_neff_phimiss_denom->Draw();
  myCanvas->cd(2);
  h_neff_phimiss_numer->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  TH1D * h_neff_phimiss = (TH1D*)h_neff_phimiss_numer->Clone();
  h_neff_phimiss->Divide(h_neff_phimiss_denom);
  h_neff_phimiss->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  // missing mass for candidates and detected
  myText->cd();
  text.DrawLatex(0.2,0.9,"Denominator and numerator missing mass");
  myText->Print(fileName,"pdf");
  myText->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_withn->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_mmiss_cand->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  // efficiency results
  myText->cd();
  text.DrawLatex(0.2,0.9,"Efficiency results");
  myText->Print(fileName,"pdf");
  myText->Clear();


  myCanvas->Divide(2,2);
  myCanvas->cd(1);
  h_neff_pmiss_numer->Draw();
  h_neff_pmiss_numer->SetStats(0);
  myCanvas->cd(2);
  h_neff_pmiss_denom->Draw();
  h_neff_pmiss_denom->SetStats(0);
  myCanvas->cd(3);
  TH1D * h_neff_pmiss = (TH1D*)h_neff_pmiss_numer->Clone();
  h_neff_pmiss->Divide(h_neff_pmiss_denom);
  h_neff_pmiss->Draw();
  h_neff_pmiss->SetStats(0);
  h_neff_pmiss->GetYaxis()->SetTitle("efficiency");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  TH1D * h_neff_pmiss_int1 = (TH1D*)h_pmiss_int1_numer->Clone();
  h_neff_pmiss_int1->Divide(h_pmiss_int1_denom);
  h_neff_pmiss_int1->SetLineColor(kMagenta);
  h_neff_pmiss_int1->GetYaxis()->SetRangeUser(0,0.16);
  h_neff_pmiss_int1->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  TH1D * h_neff_pmiss_int2 = (TH1D*)h_pmiss_int2_numer->Clone();
  h_neff_pmiss_int2->Divide(h_pmiss_int2_denom);
  h_neff_pmiss_int2->SetLineColor(kGreen);
  h_neff_pmiss_int2->GetYaxis()->SetRangeUser(0,0.16);
  h_neff_pmiss_int2->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  TH1D * h_neff_pmiss_int3 = (TH1D*)h_pmiss_int3_numer->Clone();
  h_neff_pmiss_int3->Divide(h_pmiss_int3_denom);
  h_neff_pmiss_int3->SetLineColor(kBlue);
  h_neff_pmiss_int3->GetYaxis()->SetRangeUser(0,0.16);
  h_neff_pmiss_int3->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_neff_pmiss_int1->Draw();
  h_neff_pmiss_int2->Draw("same");
  h_neff_pmiss_int3->Draw("same");
  h_neff_pmiss_int1->SetStats(0);
  h_neff_pmiss_int2->SetStats(0);
  h_neff_pmiss_int3->SetStats(0);
  h_neff_pmiss_int3->GetXaxis()->SetRangeUser(0,1);
  h_neff_pmiss_int3->GetYaxis()->SetRangeUser(0,0.20);
  h_neff_pmiss_int3->SetTitle("Neutron Efficiency in #theta Ranges");
  h_neff_pmiss_int3->SetStats(0);
  TLegend * leg2 = new TLegend(0.65,0.75,0.89,0.89);
  leg2->SetTextFont(72);
  leg2->SetTextSize(0.04);
  leg2->AddEntry(h_neff_pmiss_int1,"45<#theta_{pred}<50","l");
  leg2->AddEntry(h_neff_pmiss_int2,"50<#theta_{pred}<55","l");
  leg2->AddEntry(h_neff_pmiss_int3,"55<#theta_{pred}<60","l");
  //leg2->AddEntry(h_neff_pmiss_int4,"70<#theta_{miss}<80","l");
  leg2->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();




  // efficiency as a function of theta miss
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_neff_thetamiss_denom->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_neff_thetamiss_numer->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  TH1D * h_neff_thetamiss = (TH1D*)h_neff_thetamiss_numer->Clone();
  h_neff_thetamiss->Divide(h_neff_thetamiss_denom);
  h_neff_thetamiss->Draw();
  h_neff_thetamiss->SetStats(0);
  // print output
  ofstream outtheta(theta_name);
  for (int i=0; i<h_neff_thetamiss->GetNbinsX(); i++) {
    outtheta << h_neff_thetamiss->GetXaxis()->GetBinCenter(i) << ' ' << h_neff_thetamiss->GetBinContent(i) << ' ' << h_neff_thetamiss->GetBinError(i) << '\n';
  }
  outtheta.close();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();




  // original method
  myCanvas->Divide(2,2);
  myCanvas->cd(1);
  h_pmiss_allt_numerD->Draw();
  myCanvas->cd(2);
  h_pmiss_int1_numerD->Draw();
  myCanvas->cd(3);
  h_pmiss_int2_numerD->Draw();
  myCanvas->cd(4);
  h_pmiss_int3_numerD->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  // original method
  myCanvas->Divide(2,2);
  myCanvas->cd(1);
  h_pmiss_allt_denomD->Draw();
  myCanvas->cd(2);
  h_pmiss_int1_denomD->Draw();
  myCanvas->cd(3);
  h_pmiss_int2_denomD->Draw();
  myCanvas->cd(4);
  h_pmiss_int3_denomD->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  // original method
  myCanvas->Divide(2,2);
  myCanvas->cd(1);
  TH1D * h_neff_allt_D = (TH1D*)h_pmiss_allt_numerD->Clone();
  h_neff_allt_D->Divide(h_pmiss_allt_denomD);
  h_neff_allt_D->Draw();
  // print output
  ofstream outallp(p_name);
  for (int i=0; i<h_neff_allt_D->GetNbinsX(); i++) {
    outallp << h_neff_allt_D->GetXaxis()->GetBinCenter(i) << ' ' << h_neff_allt_D->GetBinContent(i) << ' ' << h_neff_allt_D->GetBinError(i) << '\n';
  }
  outallp.close();

  myCanvas->cd(2);
  TH1D * h_neff_int1_D = (TH1D*)h_pmiss_int1_numerD->Clone();
  h_neff_int1_D->Divide(h_pmiss_int1_denomD);
  h_neff_int1_D->Draw();
  myCanvas->cd(3);
  TH1D * h_neff_int2_D = (TH1D*)h_pmiss_int2_numerD->Clone();
  h_neff_int2_D->Divide(h_pmiss_int2_denomD);
  h_neff_int2_D->Draw();
   myCanvas->cd(4);
  TH1D * h_neff_int3_D = (TH1D*)h_pmiss_int3_numerD->Clone();
  h_neff_int3_D->Divide(h_pmiss_int3_denomD);
  h_neff_int3_D->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  myCanvas->Divide(1,1);
  h_neff_allt_D->Draw();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();


  // original method
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_neff_int1_D->SetLineColor(kGreen);
  h_neff_int1_D->SetStats(0);
  h_neff_int1_D->GetYaxis()->SetRangeUser(0,0.16);
  h_neff_int1_D->SetTitle("Neutron Efficiency in #theta Ranges");
  h_neff_int1_D->Draw();
  h_neff_int2_D->SetLineColor(kBlue);
  h_neff_int2_D->SetStats(0);
  h_neff_int2_D->GetYaxis()->SetRangeUser(0,0.16);
  h_neff_int2_D->Draw("same");
  h_neff_int3_D->SetLineColor(kRed);
  h_neff_int3_D->SetStats(0);
  h_neff_int3_D->GetYaxis()->SetRangeUser(0,0.16);
  h_neff_int3_D->Draw("same");
  TLegend * leg3 = new TLegend(0.65,0.75,0.89,0.89);
  leg3->SetTextFont(72);
  leg3->SetTextSize(0.04);
  leg3->AddEntry(h_neff_int1_D,"45<#theta_{pred}<50","l");
  leg3->AddEntry(h_neff_int2_D,"50<#theta_{pred}<55","l");
  leg3->AddEntry(h_neff_int3_D,"55<#theta_{pred}<60","l");
  leg3->Draw();
    // print output
  ofstream outp("neff_out/p_hepin_int.txt");
  for (int i=0; i<h_neff_int1_D->GetNbinsX(); i++) {
    outp << h_neff_int1_D->GetXaxis()->GetBinCenter(i) << ' ';
    outp << h_neff_int1_D->GetBinContent(i) << ' ' << h_neff_int1_D->GetBinError(i) << ' ';
    outp << h_neff_int2_D->GetBinContent(i) << ' ' << h_neff_int2_D->GetBinError(i) << ' ';
    outp << h_neff_int3_D->GetBinContent(i) << ' ' << h_neff_int3_D->GetBinError(i) << '\n';
  }
  outp.close();
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();



  // 2D EFFICIENCY
  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_cand2d->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_det2d->Draw("colz");
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  TH2D * h_eff2d = (TH2D*)h_det2d->Clone();
  h_eff2d->Divide(h_cand2d);
  h_eff2d->Draw("colz");
  h_eff2d->SetMaximum(0.20);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

  myCanvas->Divide(1,1);
  myCanvas->cd(1);
  h_eff2d->Draw("surf1");
  h_eff2d->SetMaximum(0.20);
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();

/*  // plot cross sections
  TGraph *grEff[n_tmiss_cut];
  TMultigraph *mgEff = new TMultiGraph();
  TLegend* legendEff = new TLegend[40,45,50,60];
*/

  myCanvas->Divide(3,2);
  //myCanvas->cd(1);
  auto legend = new TLegend(0.8,0.8,0.8,0.8);
  for (int i=0; i<h_eff2d->GetNbinsY(); i++)
  {
    myCanvas->cd(i+1);
    TH1D * proj1 = h_eff2d->ProjectionX("",i,i+1,"d"); // overflow/underflow?
    proj1->SetTitle("Efficiency");
    proj1->Draw();
    //legend->AddEntry(proj1,Form("%.2f",h_eff2d->GetYaxis()->GetBinCenter(i)));
    //legend->Draw();
  }
  
  myCanvas->Print(fileName,"pdf");
  myCanvas->Clear();
  






std::cout << h_mmiss_withn->Integral(0,200) << '\n';
std::cout << h_mmiss_cand->Integral(0,200) << '\n';
std::cout << "The neutron efficiency is " << h_mmiss_withn->Integral(0,200)/h_mmiss_cand->Integral(0,200) << '\n';



  // wrap it up
  sprintf(fileName,"%s]",pdfFile);
  myCanvas->Print(fileName,"pdf");

  //outFile->Close();
}


Double_t background_poly(Double_t *x, Double_t *par) {
  return ( par[0] + par[1]*x[0] + par[2]*x[0]*x[0] + par[3]*x[0]*x[0]*x[0] );
}

Double_t signal(Double_t *x, Double_t *par) {
  return par[0]*exp(-pow((x[0]-par[1]),2.)/(2*pow(par[2],2.))); 
}

Double_t mmiss_signal_gauss(Double_t *x, Double_t *par) {
  return signal(x,par) + signal(x,&par[3]);
}

Double_t mmiss_signal_poly(Double_t *x, Double_t *par) {
  return signal(x,par) + background_poly(x,&par[3]);
}




void printProgress(double percentage) {
  int val = (int) (percentage * 100);
  int lpad = (int) (percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush(stdout);
}


double get_pin_mmiss(TVector3 p_b, TVector3 p_e, TVector3 ppi){

  double Ebeam = p_b.Mag();
  double Ee = p_e.Mag();
  double Epi = sqrt(ppi.Mag2() + m_piplus*m_piplus);
  double emiss = Ebeam - Ee + mp - Epi;
  TVector3 pmiss = p_b - p_e - ppi;

  double mmiss = sqrt( (emiss*emiss) - pmiss.Mag2() );

  return mmiss;
}


/*double func_neff_fit(double *x, double *par)
{
  // par[0] = linear transition in pmiss
  // par[1] = linear transition in thetamiss
  // par[2] = low pmiss slope
  // par[3] = high pmiss slope
  // par[4] = low thetamiss slope
  // par[5] = high thetamiss slope
  double p = x[0];
  double t = x[1];
  double neff_value = 0;
  if      (p<par[0] && t<par[1])
  else if (p<par[0] && t>par[1])
  else if (p>par[0] && t<par[1])
  else if (p>par[0] && t>par[1])
  return value;
}*/


double * hist_projections(TCanvas * can, TH2D * hist2d, int num_hist, char v, double Mlow, double Mhigh)
{
  double p_start_val[num_hist];
  double x_min = hist2d->GetXaxis()->GetXmin();
  double x_max = hist2d->GetXaxis()->GetXmax();
  double dp = (x_max-x_min)/num_hist;
  double * S = new double[num_hist];
  TLegend *legend = new TLegend(0.9,0.3,0.6,0.9);
  for (int i=0; i<num_hist; i++)
  {
    p_start_val[i] = x_min + i*dp;
    int bin1 = hist2d->GetXaxis()->FindBin(p_start_val[i]);
    int bin2 = hist2d->GetXaxis()->FindBin(p_start_val[i]+dp) - 1;

    // make projection for x interval
    can->cd(1);//can->cd(i+1);
    TH1D * proj = hist2d->ProjectionY("",bin1,bin2);
    proj->GetXaxis()->SetTitleSize(0.05);
    proj->GetXaxis()->SetLabelSize(0.05);
    proj->GetYaxis()->SetLabelSize(0.05);
    proj->SetStats(0);

    // create name of missing mass histogram for current momentum/theta interval
    std::ostringstream sObj1, sObj2;
    std::string leftTitle = "("; std::string midTitle = ",";
    std::string rightTitle;
    if (v=='p')
    {
      rightTitle = ") GeV/c";
      sObj1 << std::fixed << std::setprecision(3) << p_start_val[i];
      sObj2 << std::fixed << std::setprecision(3) << p_start_val[i] + dp;
    }
    else if (v=='t')
    {
      rightTitle = ") deg";
      sObj1 << std::fixed << std::setprecision(0) << p_start_val[i];
      sObj2 << std::fixed << std::setprecision(0) << p_start_val[i] + dp;
    }
    else
    {
      std::cout << "Invalid projection variable for missing mass\n";
    }
    std::string result = leftTitle + sObj1.str() + midTitle + sObj2.str() + rightTitle;
    //proj->SetTitle(result.c_str());
    proj->SetTitle("");
    //proj->SetTitleSize(0.05);

    // draw
    proj->Scale(1.0/proj->GetMaximum());
    proj->SetLineColor(i+30);
    proj->SetLineWidth(2);
    proj->SetMarkerStyle(i+19);
    proj->SetMarkerSize(1.5);
    proj->Draw("SAME");
    legend->AddEntry(proj,result.c_str(),"lpf");
    S[i] = proj->Integral(proj->GetXaxis()->FindBin(Mlow),proj->GetXaxis()->FindBin(Mhigh));
  }
  //legend->SetTextSize(0.01);
  legend->Draw();
  return S;
}


