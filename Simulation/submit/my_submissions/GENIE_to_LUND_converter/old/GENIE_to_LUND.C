#include <fstream>
#include <iostream>

#include "../../../targets.h"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/classes/AMaps/AMap.cpp"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/classes/DSCuts/DSCuts.h"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/namespaces/general_utilities/utilities.h"
#include "TFile.h"
#include "TRandom3.h"
#include "TString.h"
#include "TTree.h"

using namespace std;
using namespace utilities;

void GENIE_to_LUND(TString inputFile = "", TString outputFileDir = "", TString outputFile = "", int nFiles = 800, string target = "liquid", int A = 1, int Z = 1,
                   double beamE_in_lundfiles = -99) {
    // Read in target parameter files
    cout << "\nConverting file:\t" << inputFile << endl;
    TFile* inFile = new TFile(inputFile);

    cout << "\nMaking LUND file: \t" << outputFile << endl;

    // Make lundfiles directory:
    TString lundfiles_Path = outputFileDir + "/lundfiles";
    cout << "\nGenerating lundfiles directory: \t" << lundfiles_Path << endl;
    system(("mkdir -p " + std::string(lundfiles_Path.Data())).c_str());

    // Make mchipo directory:
    TString mchipo_Path = outputFileDir + "/mchipo";
    cout << "\nGenerating mchipo directory: \t" << mchipo_Path << endl;
    system(("mkdir -p " + std::string(mchipo_Path.Data())).c_str());

    // Make reconhipo directory:
    TString reconhipo_Path = outputFileDir + "/reconhipo";
    cout << "\nGenerating reconhipo directory: \t" << reconhipo_Path << endl;
    system(("mkdir -p " + std::string(reconhipo_Path.Data())).c_str());

    cout << "\nSaving lundfiles into " << lundfiles_Path << endl;
    cout << "\n";

    // Acceptance maps ----------------------------------------------------

    std::string AcceptanceMapsDirectory = "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/data/AcceptanceMaps";

    AMaps aMaps_master = AMaps(AcceptanceMapsDirectory, "Uniform_1e_sample_" + GetBeamEnergyFromDouble(beamE_in_lundfiles), beamE_in_lundfiles, "AMaps", false, false, {1, 1, 1});

    bool apply_fiducial_cuts = false;

    // TTree variables ------------------------------------------------

    DSCuts ThetaFD = DSCuts("Theta FD", "FD", "", "", 1, 5., 40.);

    // TTree variables ------------------------------------------------

    TTree* T = (TTree*)inFile->Get("gst");

    double RES_ID = 0.;  // WAS targP = 0.; // polarization
    double beamP = 0.;   // polarization
    Int_t interactN = 1;
    int beamType = 11;

    double beamE = beamE_in_lundfiles;  // GeV

    Bool_t qel;
    Bool_t mec;
    Bool_t res;
    Bool_t dis;
    // will be coded into 1,2,3,4
    Int_t resid;
    // final state particles
    Int_t nf;
    Int_t pdgf[125];    //[nf]
    Double_t Ef[125];   //[nf]
    Double_t pxf[125];  //[nf]
    Double_t pyf[125];  //[nf]
    Double_t pzf[125];  //[nf]
    // electron info
    Double_t El;
    Double_t pxl;
    Double_t pyl;
    Double_t pzl;

    T->SetBranchAddress("qel", &qel);
    T->SetBranchAddress("mec", &mec);
    T->SetBranchAddress("res", &res);
    T->SetBranchAddress("dis", &dis);
    // Added by J. L. Barrow
    T->SetBranchAddress("resid", &resid);

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
    cout << "Number of events " << nEvents << endl;

    TString formatstring, outstring;

    // Check the number of files is not more than what is in the file
    if (nFiles > nEvents / 10000) { nFiles = nEvents / 10000; }

    // Split large GENIE output into 10000 lund files
    for (int iFiles = 1; iFiles < nFiles + 1; iFiles++) {
        TString outfilename = Form("%s/%s_%d.txt", lundfiles_Path.Data(), outputFile.Data(), iFiles);
        ofstream outfile;
        outfile.open(outfilename);
        int start = (iFiles - 1) * 10000;
        int end = iFiles * 10000;

        int i = 0;

        for (int i = 0; i < 10000; i++) {
            T->GetEntry(i + start);

            double P_e = sqrt(pxl * pxl + pyl * pyl + pzl * pzl);
            double theta_e = acos(pzl / sqrt(pxl * pxl + pyl * pyl + pzl * pzl)) * 180. / TMath::Pi();
            double phi_e = atan2(pyl, pxl) * 180. / TMath::Pi();

            bool e_inFD = aMaps_master.IsInFDQuery((!apply_fiducial_cuts), ThetaFD, "Electron", P_e, theta_e, phi_e, false, true);

            if (e_inFD) {
                // Stores reaction mechanism qel = 1, mec = 2, rec = 3, dis=4
                double code = 0.;

                if (qel) {
                    code = 1.;
                } else if (mec) {
                    code = 2.;
                } else if (res) {
                    code = 3.;
                } else if (dis) {
                    code = 4.;
                }

                if (code < .01) { continue; }

                RES_ID = double(resid);

                int nf_mod = 1;
                for (int iPart = 0; iPart < nf; iPart++) {
                    if (pdgf[iPart] == 2212)
                        nf_mod++;
                    else if (pdgf[iPart] == 2112)
                        nf_mod++;
                    else if (pdgf[iPart] == 211)
                        nf_mod++;
                    else if (pdgf[iPart] == -211)
                        nf_mod++;
                    else if (pdgf[iPart] == 111)
                        nf_mod++;
                    else if (pdgf[iPart] == 22)
                        nf_mod++;
                }

                // LUND header for the event:
                formatstring = "%i \t %i \t %i \t %f \t %f \t %i \t %f \t %i \t %d \t %.2f \n";
                outstring = Form(formatstring, nf_mod, A, Z, RES_ID /*targP*/, beamP, beamType, beamE, interactN, i, code);
                outfile << outstring;

                auto vtx = randomVertex(target);  // get vertex of event

                int part_num = 0;
                // electron
                outfile << addParticle(1, 1, 11, TVector3(pxl, pyl, pzl), mass_e, vtx);
                part_num++;

                for (int iPart = 0; iPart < nf; iPart++) {
                    if (pdgf[iPart] == 2212) {  // p
                        part_num++;
                        outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_p, vtx);
                    } else if (pdgf[iPart] == 2112) {  // n
                        part_num++;
                        outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_n, vtx);
                    } else if (pdgf[iPart] == 211) {  // pi+
                        part_num++;
                        outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
                    } else if (pdgf[iPart] == -211) {  // pi-
                        part_num++;
                        outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
                    } else if (pdgf[iPart] == 111) {  // pi0
                        part_num++;
                        outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
                    } else if (pdgf[iPart] == 22) {  // gamma
                        part_num++;
                        outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
                    }
                }
            }
        }

        cout << "\nSaving file: " << outfilename << endl;
        outfile.close();
    }

    cout << "\nFINISHED!\n" << endl;
}
