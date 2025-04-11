#include <TFile.h>
#include <TRandom3.h>
#include <TString.h>
#include <TTree.h>

#include <fstream>
#include <iostream>

#include "../../../targets.h"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/classes/AMaps/AMap.cpp"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/classes/DSCuts/DSCuts.h"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/namespaces/general_utilities/utilities.h"

using namespace std;
using namespace utilities;

void GENIE_to_LUND(TString inputFile = "", TString outputFileDir = "", TString outputFile = "", int nFiles = 800, string target = "liquid", int A = 1, int Z = 1,
                   double beamE_in_lundfiles = -99) {
    // Read in target parameter files
    cout << "\033[33m\nConverting file:\t\033[0m" << inputFile << endl;
    TFile* inFile = new TFile(inputFile);

    cout << "\033[33m\nMaking LUND file: \t\033[0m" << outputFile << endl;

    // Make lundfiles directory:
    TString lundfiles_Path = outputFileDir + "/lundfiles";
    cout << "\033[33m\nGenerating lundfiles directory: \t\033[0m" << lundfiles_Path << endl;
    system(("mkdir -p " + std::string(lundfiles_Path.Data())).c_str());

    // Make mchipo directory:
    TString mchipo_Path = outputFileDir + "/mchipo";
    cout << "\033[33m\nGenerating mchipo directory: \t\033[0m" << mchipo_Path << endl;
    system(("mkdir -p " + std::string(mchipo_Path.Data())).c_str());

    // Make reconhipo directory:
    TString reconhipo_Path = outputFileDir + "/reconhipo";
    cout << "\033[33m\nGenerating reconhipo directory: \t\033[0m" << reconhipo_Path << endl;
    system(("mkdir -p " + std::string(reconhipo_Path.Data())).c_str());

    cout << "\033[33m\nSaving lundfiles into \033[0m" << lundfiles_Path << endl;
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
    cout << "\033[33mNumber of events \033[0m" << nEvents << endl;

    TString formatstring, outstring;

    // Check the number of files is not more than what is in the file
    if (nFiles > nEvents / 10000) { nFiles = nEvents / 10000; }

    cout << "\033[33mMaximum number of output files allowed: \033[0m" << nFiles << endl;

    // Split large GENIE output into 10000 lund files
    Long64_t total_entries = T->GetEntries();
    int passed_events = 0;
    int current_file_index = 1;
    int events_in_current_file = 0;

    TString outfilename = Form("%s/%s_%d.txt", lundfiles_Path.Data(), outputFile.Data(), current_file_index);
    ofstream outfile(outfilename);

    for (Long64_t ev = 0; ev < total_entries; ev++) {
        T->GetEntry(ev);

        double P_e = sqrt(pxl * pxl + pyl * pyl + pzl * pzl);
        double theta_e = acos(pzl / sqrt(pxl * pxl + pyl * pyl + pzl * pzl)) * 180. / TMath::Pi();
        double phi_e = atan2(pyl, pxl) * 180. / TMath::Pi();

        bool e_inFD = aMaps_master.IsInFDQuery((!apply_fiducial_cuts), ThetaFD, "Electron", P_e, theta_e, phi_e, false, true);

        // Apply electron acceptance cuts:
        if (!e_inFD) continue;

        double code = (qel) ? 1. : (mec) ? 2. : (res) ? 3. : (dis) ? 4. : 0.;

        if (code < .01) continue;

        RES_ID = double(resid);

        int nf_mod = 1;
        for (int iPart = 0; iPart < nf; iPart++) {
            if (pdgf[iPart] == 2212 || pdgf[iPart] == 2112 || pdgf[iPart] == 211 || pdgf[iPart] == -211 || pdgf[iPart] == 111 || pdgf[iPart] == 22) { ++nf_mod; }
        }

        formatstring = "%i \t %i \t %i \t %f \t %f \t %i \t %f \t %i \t %d \t %.2f \n";
        outstring = Form(formatstring, nf_mod, A, Z, RES_ID, beamP, beamType, beamE, interactN, ev, code);
        outfile << outstring;

        auto vtx = randomVertex(target);

        int part_num = 1;
        outfile << addParticle(part_num, 1, 11, TVector3(pxl, pyl, pzl), mass_e, vtx);

        for (int iPart = 0; iPart < nf; iPart++) {
            if (pdgf[iPart] == 2212)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_p, vtx);
            else if (pdgf[iPart] == 2112)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_n, vtx);
            else if (pdgf[iPart] == 211)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
            else if (pdgf[iPart] == -211)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
            else if (pdgf[iPart] == 111)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi0, vtx);
            else if (pdgf[iPart] == 22)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), 0., vtx);
        }

        passed_events++;
        events_in_current_file++;

        // If not enough events left to fill a complete file, stop early
        if ((total_entries - ev) < 10000 && events_in_current_file > 0) {
            cout << "\033[33mFewer than 10,000 events left (\033[0m" << (total_entries - ev) << "\033[33m). Ending early.\033[0m" << endl;
            break;
        }

        if (events_in_current_file == 10000) {
            outfile.close();
            cout << "\033[33m\nSaved file #\033[0m" << current_file_index << ": " << outfilename << endl;

            ++current_file_index;
            events_in_current_file = 0;

            if (current_file_index > nFiles) {
                cout << "\033[33mReached file limit (\033[0m" << nFiles << "\033[33m). Stopping event writing.\033[0m" << endl;
                break;
            }

            outfilename = Form("%s/%s_%d.txt", lundfiles_Path.Data(), outputFile.Data(), current_file_index);
            outfile.open(outfilename);
        }
    }

    outfile.close();  // Close the last file if it was opened
    cout << "\033[33m\nFINISHED!\n\033[0m" << endl;

    cout << "\033[33m\nSummary:\n\033[0m";
    cout << "\t\033[33mTotal entries scanned:\033[0m " << total_entries << endl;
    cout << "\t\033[33mEvents passing cuts:\033[0m " << passed_events << endl;
    cout << "\t\033[33mOutput files written:\033[0m " << current_file_index << endl;
}
