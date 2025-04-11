#include <TAxis.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TMath.h>
#include <TObject.h>
#include <TROOT.h>
#include <TRandom3.h>
#include <TString.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TTree.h>
#include <TVector3.h>

#include <fstream>
#include <iostream>

// Include CLAS12 libraries:
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/includes/clas12_include.h"

// Include libraries:
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/namespaces/gemc/targets.h"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/namespaces/general_utilities/utilities.h"

// Include classes:
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/classes/AMaps/AMaps.cpp"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/classes/DSCuts/DSCuts.h"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/classes/hPlots/hsPlots.cpp"

using namespace std;
using namespace utilities;

void GENIE_to_LUND_converter(TString InputFiles = "", /* TString OutputFileDir = "", */ /* TString outputFile = "", */ int nFiles = 1000, string target = "liquid", int A = 1, int Z = 1) {
    // Converter settings -----------------------------------------------------------------------------------------------------------------------------------------------

    bool PrintOut = false;

    bool apply_fiducial_cuts = false;

    // Proceeding input arguments ---------------------------------------------------------------------------------------------------------------------------------------

    TString target_element = (utilities::FindInString(InputFiles, "H1"))     ? "H1"
                             : (utilities::FindInString(InputFiles, "D2"))   ? "D2"
                             : (utilities::FindInString(InputFiles, "C12"))  ? "C12"
                             : (utilities::FindInString(InputFiles, "Ar40")) ? "Ar40"
                                                                             : "UNKNOWN";
    if (target_element == "UNKNOWN") { std::cerr << "\033[31m\nTarget element not recognized. Please use H1, D2, C12, or Ar40. Aborting...\n\033[0m", exit(0); }

    TString genie_tune = (utilities::FindInString(InputFiles, "G18_10a_00_000"))     ? "G18_10a_00_000"
                         : (utilities::FindInString(InputFiles, "GEM21_11a_00_000")) ? "GEM21_11a_00_000"
                                                                                     : "UNKNOWN";
    if (genie_tune == "UNKNOWN") { std::cerr << "\033[31m\nGENIE tune not recognized. Please use G18_10a_00_000 or GEM21_11a_00_000. Aborting...\n\033[0m", exit(0); }

    TString beam_e = (utilities::GetBeam(InputFiles, "2070MeV"))        ? "2070MeV"
                     : (utilities::FindInString(InputFiles, "4029MeV")) ? "4029MeV"
                     : (utilities::FindInString(InputFiles, "5986MeV")) ? "5986MeV"
                                                                        : "UNKNOWN";
    if (beam_e == "UNKNOWN") { std::cerr << "\033[31m\nBeam energy not recognized. Please use 2070MeV, 4029MeV, or 5986MeV. Aborting...\n\033[0m", exit(0); }

    double beamE = (beam_e == "2070MeV") ? 2.07052 : (beam_e == "4029MeV") ? 4.02962 : (beam_e == "5986MeV") ? 5.98636 : -9999;
    if (beamE < 0) { std::cerr << "\033[31m\nBeam energy not recognized. Please use 2070MeV, 4029MeV, or 5986MeV. Aborting...\n\033[0m", exit(0); }

    TString Q2_cut = (beam_e == "2070MeV") ? "Q2_0_02" : (beam_e == "4029MeV") ? "Q2_0_25" : (beam_e == "5986MeV") ? "Q2_0_40" : "UNKNOWN";
    if (Q2_cut == "UNKNOWN") { std::cerr << "\033[31m\nQ2 cut not recognized. Aborting...\n\033[0m", exit(0); }

    TString lundfile_prefix = target_element + "_" + genie_tune + "_" + Q2_cut + "_" + beam_e;

    // Read in target parameter files -----------------------------------------------------------------------------------------------------------------------------------

    cout << "\033[33m\nConverting files:\t\033[0m" << InputFiles << endl;
    TChain* InChain = new TChain("gst");
    InChain->Add(InputFiles);

    cout << "\033[33m\nLUND file prefix: \t\033[0m" << lundfile_prefix << endl;

    TString OutputFileBase = "/lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples";
    TString OutputFileDir = OutputFileBase + "/" + target_element + "/" + genie_tune + "/" + beam_e + "_" + Q2_cut;
    system(("mkdir -p " + std::string(OutputFileDir.Data())).c_str());

    std::string pdfFileName = OutputFileDir + "/" + "_" + lundfile_prefix + "_plots.pdf";
    const char* pdfFile = pdfFileName.c_str();

    // Make lundfiles directory:
    TString lundfiles_Path = OutputFileDir + "/lundfiles";
    cout << "\033[33m\nGenerating lundfiles directory: \t\033[0m" << lundfiles_Path << endl;
    system(("mkdir -p " + std::string(lundfiles_Path.Data())).c_str());

    // Make mchipo directory:
    TString mchipo_Path = OutputFileDir + "/mchipo";
    cout << "\033[33m\nGenerating mchipo directory: \t\033[0m" << mchipo_Path << endl;
    system(("mkdir -p " + std::string(mchipo_Path.Data())).c_str());

    // Make reconhipo directory:
    TString reconhipo_Path = OutputFileDir + "/reconhipo";
    cout << "\033[33m\nGenerating reconhipo directory: \t\033[0m" << reconhipo_Path << endl;
    system(("mkdir -p " + std::string(reconhipo_Path.Data())).c_str());

    cout << "\033[33m\nSaving lundfiles into \033[0m" << lundfiles_Path << endl;
    cout << "\n";

    exit(0);

    // Acceptance maps --------------------------------------------------------------------------------------------------------------------------------------------------

    std::string AcceptanceMapsDirectory = "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/data/AcceptanceMaps";

    AMaps aMaps_master = AMaps(AcceptanceMapsDirectory, "Uniform_1e_sample_" + GetBeamEnergyFromDouble(beamE), beamE, "AMaps", false, false, {1, 1, 1});

    int HistElectronSliceNumOfXBins = aMaps_master.GetHistElectronSliceNumOfXBins();

    int HistElectronSliceNumOfYBins = aMaps_master.GetHistElectronSliceNumOfYBins();

    vector<vector<double>> ElectronMomSliceLimits = aMaps_master.GetElectronMomSliceLimits();

    // FD theta acceptance limits ---------------------------------------------------------------------------------------------------------------------------------------

    DSCuts ThetaFD = DSCuts("Theta FD", "FD", "", "", 1, 5., 40.);

    // Monitoring histograms variables ----------------------------------------------------------------------------------------------------------------------------------

    std::vector<TObject*> histList;

    TH2D* theta_e_VS_phi_e = new TH2D("theta_e_VS_phi_e;#theta_{e} vs. #phi_{e};#phi_{e} [#circ];#theta_{e}", HistElectronSliceNumOfXBins, -180., 180., HistElectronSliceNumOfYBins,
                                      ThetaFD.GetLowerCut(), ThetaFD.GetUpperCut());
    histList->push_back(theta_e_VS_phi_e);

    hsPlots theta_e_VS_phi_e_BySliceOfPe(ElectronMomSliceLimits, hsPlots::TH2D_TYPE, HistoList, "theta_e_VS_phi_e", "#theta_{e} vs. #phi_{e}", HistElectronSliceNumOfXBins, -180., 180.,
                                         HistElectronSliceNumOfYBins, ThetaFD.GetLowerCut(), ThetaFD.GetUpperCut());

    // TTree variables --------------------------------------------------------------------------------------------------------------------------------------------------

    TTree* T = (TTree*)InChain->Get("gst");

    double RES_ID = 0.;  // WAS targP = 0.; // polarization
    double beamP = 0.;   // polarization
    Int_t interactN = 1;
    int beamType = 11;

    // double beamE = beamE_in_lundfiles;  // GeV

    // will be coded into 1,2,3,4:
    Bool_t qel;
    Bool_t mec;
    Bool_t res;
    Bool_t dis;

    Int_t resid;

    // final state particles:
    Int_t nf;
    Int_t nfn;
    Int_t nfp;
    Int_t pdgf[250];    //[nf]
    Double_t Ef[250];   //[nf]
    Double_t pxf[250];  //[nf]
    Double_t pyf[250];  //[nf]
    Double_t pzf[250];  //[nf]

    // electron info:
    Double_t El;
    Double_t pxl;
    Double_t pyl;
    Double_t pzl;

    T->SetBranchAddress("qel", &qel);
    T->SetBranchAddress("mec", &mec);
    T->SetBranchAddress("res", &res);
    T->SetBranchAddress("dis", &dis);

    T->SetBranchAddress("resid", &resid);  // Added by J. L. Barrow

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
    int passed_events = 0, current_file_index = 1, events_in_current_file = 0;

    TString outfilename = Form("%s/%s_%d.txt", lundfiles_Path.Data(), lundfile_prefix.Data(), current_file_index);
    ofstream outfile(outfilename);

    for (Long64_t ev = 0; ev < total_entries; ev++) {
        T->GetEntry(ev);

        double P_e = sqrt(pxl * pxl + pyl * pyl + pzl * pzl);
        double theta_e = acos(pzl / sqrt(pxl * pxl + pyl * pyl + pzl * pzl)) * 180. / TMath::Pi();
        double phi_e = atan2(pyl, pxl) * 180. / TMath::Pi();

        bool e_inFD = aMaps_master.IsInFDQuery((!apply_fiducial_cuts), ThetaFD, "Electron", P_e, theta_e, phi_e, false, true);

        // Apply electron acceptance cuts:
        if (!e_inFD) continue;

        theta_e_VS_phi_e->Fill(phi_e, theta_e);
        theta_e_VS_phi_e_BySliceOfPe->Fill(P_e, phi_e, theta_e);

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
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), m_p, vtx);
            else if (pdgf[iPart] == 2112)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), m_n, vtx);
            else if (pdgf[iPart] == 211)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), m_piplus, vtx);
            else if (pdgf[iPart] == -211)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), m_piminus, vtx);
            else if (pdgf[iPart] == 111)
                outfile << addParticle(++part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), m_pizero, vtx);
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

            outfilename = Form("%s/%s_%d.txt", lundfiles_Path.Data(), lundfile_prefix.Data(), current_file_index);
            outfile.open(outfilename);
        }
    }

    // Create a canvas
    TCanvas* MainCanvas = new TCanvas("MainCanvas", "Canvas for saving histograms", 800, 600);
    MainCanvas->cd()->SetGrid();
    MainCanvas->cd()->SetBottomMargin(0.14), MainCanvas->cd()->SetLeftMargin(0.18), MainCanvas->cd()->SetRightMargin(0.12);

    // offset the multi-page PDF
    MainCanvas->Print(Form("%s[", pdfFile));  // Open the PDF file

    // Loop through the list of histograms
    for (int i = 0; i < histList.size(); i++) {
        MainCanvas->cd();  // Select the canvas
        MainCanvas->Clear();

        // MainCanvas->cd();
        histList.at(i)->GetXaxis()->SetTitleSize(0.06);
        histList.at(i)->GetXaxis()->SetLabelSize(0.0425);
        histList.at(i)->GetXaxis()->CenterTitle(true);
        // histList.at(i)->GetYaxis()->SetTitle("Number of events");
        histList.at(i)->GetYaxis()->SetTitleSize(0.06);
        histList.at(i)->GetYaxis()->SetLabelSize(0.0425);
        histList.at(i)->GetYaxis()->CenterTitle(true);
        // histList.at(i)->SetLineWidth(2);
        // histList.at(i)->SetLineStyle(0);
        // histList.at(i)->SetLineColor(kBlue);
        histList.at(i)->Draw("colz");  // Draw the histogram on the canvas
        MainCanvas->Print(pdfFile);    // Save the current canvas (histogram) to the PDF
    }

    // End the multi-page PDF
    MainCanvas->Print(Form("%s]", pdfFile));  // Close the PDF file

    outfile.close();  // Close the last file if it was opened
    cout << "\033[33m\nFINISHED!\n\033[0m" << endl;

    cout << "\033[33m\nSummary:\n\033[0m";
    cout << "\t\033[33mTotal entries scanned:\033[0m " << total_entries << endl;
    cout << "\t\033[33mEvents passing cuts:\033[0m " << passed_events << endl;
    cout << "\t\033[33mOutput files written:\033[0m " << current_file_index << endl;
}
