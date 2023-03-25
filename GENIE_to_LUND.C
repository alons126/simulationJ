#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "targets.h"
#include <fstream>
#include <iostream>

using namespace std;


void GENIE_to_LUND(TString inputFile = "", TString outputFile = "", int nFiles = 800, string target = "liquid", int A = 1, int Z = 1) 
{
  //Read in target parameter files                                                           
  cout << "\nConverting file " << inputFile << endl;
  TFile* inFile = new TFile(inputFile);
  cout << "\nMaking LUND file " << outputFile <<endl;

  TString lundPath = "/lustre19/expphy/volatile/clas12/asportes/simulationFiles/Ca48_G18_10a_02_11b_Q205_598636MeV/lundfiles/"; // Ca48
//  TString lundPath = "/lustre19/expphy/volatile/clas12/asportes/simulationFiles/598636MeV_Q2_0_5_test_5/lundfiles/";
//  TString lundPath = "./lundfiles/";

  //  int nFiles =  800;
  
  TTree* T = (TTree*)inFile->Get("gst");

  double RES_ID = 0.; //WAS targP = 0.; // polarization
  double beamP = 0.; // polarization
  Int_t interactN = 1; 
  int beamType = 11;
  
  double beamE = -99;	// GeV
  
  Bool_t          qel;
  Bool_t          mec;
  Bool_t          res;
  Bool_t          dis;
  //will be coded into 1,2,3,4
  Int_t           resid;
  //final state particles 
  Int_t           nf;
  Int_t           pdgf[125];   //[nf]                                   
  Double_t        Ef[125];   //[nf]                                  
  Double_t        pxf[125];   //[nf]                                    
  Double_t        pyf[125];   //[nf]                                    
  Double_t        pzf[125];   //[nf]    
  //electron info
  Double_t        El;
  Double_t        pxl;
  Double_t        pyl;
  Double_t        pzl;
  
  T->SetBranchAddress("qel", &qel);
  T->SetBranchAddress("mec", &mec);
  T->SetBranchAddress("res", &res);
  T->SetBranchAddress("dis", &dis);
  //Added by J. L. Barrow
  T->SetBranchAddress("resid",&resid);
  
  T->SetBranchAddress("pdgf", pdgf);
  T->SetBranchAddress("nf", &nf);
  T->SetBranchAddress("Ef", Ef);
  T->SetBranchAddress("pxf", pxf);
  T->SetBranchAddress("pyf", pyf);
  T->SetBranchAddress("pzf", pzf);
  
  T->SetBranchAddress("El", &El);
  T->SetBranchAddress("pxl", &pxl);
  T->SetBranchAddress("pyl", &pyl);
  T->SetBranchAddress("pzl", &pzl);
  
  int nEvents = T->GetEntries();
  cout<<"\nNumber of events (nEvents): "<<nEvents<<endl;
  
  TString formatstring, outstring;
  
  //Check the number of files is not more than what is in the file 
  if( nFiles  > nEvents/10000)
    nFiles = nEvents/10000;

    cout<<"\nnumber of lund files (nFiles): "<<nFiles<<endl;
    cout<<"\nEvents/file (nEvents/nFiles): "<<nEvents/nFiles<<endl;

    //Split large GENIE output into 10000 lund files
  for (int iFiles = 1; iFiles < (nFiles + 1); iFiles++)
//  for (int iFiles = 1; iFiles < nFiles; iFiles++) // Original
    {
      TString outfilename = Form("%s/%s_%d.dat",lundPath.Data(),outputFile.Data(),iFiles);	
      ofstream outfile;
      outfile.open(outfilename); 
      int start = (iFiles-1)*10000;
      int end   = iFiles*10000;
      
      for (int i = 0; i < 10000; i++)
	{
	  T->GetEntry(i + start);
	  
	  //Stores reaction mechanism qel = 1, mec = 2, rec = 3, dis=4
	  double code = 0.;
	  
	  if(qel)
	    code = 1.;
	  else if(mec)
	    code = 2.;
	  else if(res)
	    code = 3.;
	  else if(dis)
	    code = 4.;
	  
	  if(code < .01)
	    continue;

	  
	  RES_ID = double(resid);

//        //<editor-fold desc="Failed gen">
//        int nf_mod = 1;
//	  for(int iPart = 0; iPart < nf; iPart++)
//	    {
//          if(abs(pdgf[iPart]) < 10000 && pdgf[iPart] != 11)
//            nf_mod++;
//	    }
//        //</editor-fold>

      //<editor-fold desc="Ca48">
      int nf_mod = 1;
	  for(int iPart = 0; iPart < nf; iPart++)
	    {
          if(pdgf[iPart] == 2212) // Proton
            nf_mod++;
	      else if(pdgf[iPart] == 2112) // Neutron
		    nf_mod++;
	      else if(pdgf[iPart] == 211) // Pi+
		    nf_mod++;
	      else if(pdgf[iPart] == -211) // Pi-
		    nf_mod++;
	      else if(pdgf[iPart] == 111) // Pi0
		    nf_mod++;
	      else if(pdgf[iPart] == 321) // K+
		    nf_mod++;
	      else if(pdgf[iPart] == -321) // K-
		    nf_mod++;
	      else if(pdgf[iPart] == 311) // K0
		    nf_mod++;
	      else if(pdgf[iPart] == -311) // anti-K0 (?)
		    nf_mod++;
	      else if(pdgf[iPart] == 22) // Photons
		    nf_mod++;
	      else if(pdgf[iPart] == 3122) // Lambda
		    nf_mod++;
	      else if(pdgf[iPart] == 3222) // Sigma+
		    nf_mod++;
	      else if(pdgf[iPart] == 3112) // Sigma-
		    nf_mod++;
	      else if(pdgf[iPart] == 3212) // Sigma0
		    nf_mod++;
	      else if(pdgf[iPart] == -11) // Positron
		    nf_mod++;
	      else if(pdgf[iPart] == -2112) // Anti-neutron
		    nf_mod++;
	      else if(pdgf[iPart] == -2212) // Anti-proton
		    nf_mod++;
	      else if(pdgf[iPart] == 130) // K0_L
		    nf_mod++;
          else if(pdgf[iPart] == 12) // nu_e
              nf_mod++;
	      else if(pdgf[iPart] == -12) // Anti-nu_e
		    nf_mod++;
	      else if(pdgf[iPart] == 13) // Mu
		    nf_mod++;
	      else if(pdgf[iPart] == -13) // Anti-Mu
		    nf_mod++;
	      else if(pdgf[iPart] == -3122) // Anti-Lambda
		    nf_mod++;
	    }
        //</editor-fold>

        //<editor-fold desc="T5">
        //	  int nf_mod = 1;
//	  for(int iPart = 0; iPart < nf; iPart++)
//	    {
//          if(pdgf[iPart] == 2212) // Proton
//            nf_mod++;
//	      else if(pdgf[iPart] == 2112) // Neutron
//		    nf_mod++;
//	      else if(pdgf[iPart] == 211) // Pi+
//		    nf_mod++;
//	      else if(pdgf[iPart] == -211) // Pi-
//		    nf_mod++;
//	      else if(pdgf[iPart] == 111) // Pi0
//		    nf_mod++;
//	      else if(pdgf[iPart] == 321) // K+
//		    nf_mod++;
//	      else if(pdgf[iPart] == -321) // K-
//		    nf_mod++;
//	      else if(pdgf[iPart] == 311) // K0
//		    nf_mod++;
//	      else if(pdgf[iPart] == -311) // anti-K0 (?)
//		    nf_mod++;
//	      else if(pdgf[iPart] == 22) // Photons
//		    nf_mod++;
//	      else if(pdgf[iPart] == 3122) // Lambda
//		    nf_mod++;
//	      else if(pdgf[iPart] == 3222) // Sigma+
//		    nf_mod++;
//	      else if(pdgf[iPart] == 3112) // Sigma-
//		    nf_mod++;
//	      else if(pdgf[iPart] == 3212) // Sigma0
//		    nf_mod++;
//	      else if(pdgf[iPart] == -11) // Positron
//		    nf_mod++;
//	      else if(pdgf[iPart] == -2112) // Anti-neutron
//		    nf_mod++;
//	      else if(pdgf[iPart] == -2212) // Anti-proton
//		    nf_mod++;
//	      else if(pdgf[iPart] == 130) // K0_L
//		    nf_mod++;
//	      else if(pdgf[iPart] == -12) // nu_e
//		    nf_mod++;
//	      else if(pdgf[iPart] == 13) // Mu
//		    nf_mod++;
//	      else if(pdgf[iPart] == -13) // Anti-Mu
//		    nf_mod++;
//	      else if(pdgf[iPart] == -3122) // Anti-Lambda
//		    nf_mod++;
//	    }
        //</editor-fold>

//        //<editor-fold desc="original">
//        	  int nf_mod = 1;
//	  for(int iPart = 0; iPart < nf; iPart++)
//	    {
//	      if(pdgf[iPart] == 2212)
//		nf_mod++;
//	      else if(pdgf[iPart] == 2112)
//		nf_mod++;
////	      else if(pdgf[iPart] == 211)
////		nf_mod++;
////	      else if(pdgf[iPart] == -211)
////		nf_mod++;
//	    }
//        //</editor-fold>

	  // LUND header for the event:
	  formatstring = "%i \t %i \t %i \t %f \t %f \t %i \t %f \t %i \t %d \t %.2f \n";
	  outstring = Form(formatstring, nf_mod, A, Z, RES_ID/*targP*/, beamP, beamType, beamE, interactN, i, code);
	  outfile << outstring; 


	  auto vtx = randomVertex(target); //get vertex of event                                  

      //<editor-fold desc="original">
        //	  int part_num = 0;
//	  //electron
//	  outfile << addParticle(1,11,TVector3(pxl,pyl,pzl),mass_e,vtx);
//	  part_num++;

//	  for(int iPart = 0; iPart < nf; iPart++)
//	    {
//            outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_p,vtx);
//            part_num++;
//
////            if(pdgf[iPart] == 2212)
////		{//p
////		  part_num++;
////		  outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_p,vtx);
////		}
////	      else if(pdgf[iPart] == 2112)
////		{//n
////		  part_num++;
////		  outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_n,vtx);
////		}
////	      else if(pdgf[iPart] == 211)
////		{//pi+
////		  part_num++;
////		  outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
////		}
////	      else if(pdgf[iPart] == -211)
////		{//pi-
////		  part_num++;
////		  outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
////		}
//
//	    }
//
//	}
        //</editor-fold>

	  int part_num = 0;
	  //electron
	  outfile << addParticle(1,11,TVector3(pxl,pyl,pzl),mass_e,vtx);
	  part_num++;

	  for(int iPart = 0; iPart < nf; iPart++)
	    {
            if(pdgf[iPart] == 2212)
            {//p
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_p,vtx);
            }
            else if(pdgf[iPart] == 2112)
            {//n
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_n,vtx);
            }
            else if(pdgf[iPart] == 211)
            {//pi+
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == -211)
            {//pi-
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }


            else if(pdgf[iPart] == 111)
            { // Pi0
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 321)
            { // K+
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == -321)
            { // K-
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 311)
            { // K0
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == -311)
            { // anti-K0 (?)
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 22)
            { // Photons
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 3122)
            { // Lambda
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 3222)
            { // Sigma+
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 3112)
            { // Sigma-
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 3212)
            { // Sigma0
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == -11)
            { // Positron
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == -2112)
            { // Anti-neutron
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == -2212)
            { // Anti-proton
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 130)
            { // K0_L
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 12)
            { // Nu_e
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == -12)
            { // Anti-nu_e
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == 13)
            { // Mu
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == -13)
            { // Anti-Mu
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
            else if(pdgf[iPart] == -3122)
            { // Anti-Lambda
                part_num++;
                outfile << addParticle(part_num,pdgf[iPart],TVector3(pxf[iPart], pyf[iPart], pzf[iPart]),mass_pi,vtx);
            }
	    }

	}

      outfile.close();
      
    }

  cout << "\nFINISHED.\n";

}
