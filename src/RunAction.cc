#include "MyRunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4ios.hh"

#include <cstdlib>
#include <sstream>
#include <vector>

std::vector<G4String> RunAction::fCompletedRunFiles;
G4String RunAction::fMergedFileStem;

RunAction::RunAction()
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetNtupleMerging(true);
    analysisManager->SetFileName("output");

    ////////////////// 1D Histograms //////////////////

    // H1(0): total deposited energy
    analysisManager->CreateH1("TotalEdep",
                              "Total deposited energy per event",
                              200, 0., 11000.);
    analysisManager->SetH1XAxisTitle(0, "Edep (keV)");
    analysisManager->SetH1YAxisTitle(0, "Counts");

    // H1(1): reserved / optional
    analysisManager->CreateH1("ToF_All",
                              "Neutron ToF (all tagged recoils)",
                              500, 0., 500.);
    analysisManager->SetH1XAxisTitle(1, "ToF (ns)");
    analysisManager->SetH1YAxisTitle(1, "Counts");

    // H1(2): ToF for Xe recoils
    analysisManager->CreateH1("ToF_Xe",
                              "Neutron ToF for Xe recoils",
                              500, 0., 500.);
    analysisManager->SetH1XAxisTitle(2, "ToF (ns)");
    analysisManager->SetH1YAxisTitle(2, "Counts");

    // H1(3): ToF for active dopant recoils (H or He)
    analysisManager->CreateH1("ToF_H",
                              "Neutron ToF for dopant recoils",
                              500, 0., 500.);
    analysisManager->SetH1XAxisTitle(3, "ToF (ns)");
    analysisManager->SetH1YAxisTitle(3, "Counts");

    // H1(4): recoil energy for Xe recoils
    analysisManager->CreateH1("RecoilE_Xe",
                              "Xe recoil energy",
                              300, 0., 400.);
    analysisManager->SetH1XAxisTitle(4, "Recoil Energy (keV)");
    analysisManager->SetH1YAxisTitle(4, "Counts");

    // H1(5): recoil energy for active dopant recoils (H or He)
    analysisManager->CreateH1("RecoilE_H",
                              "Dopant recoil energy",
                              400, 0., 11000.);
    analysisManager->SetH1XAxisTitle(5, "Recoil Energy (keV)");
    analysisManager->SetH1YAxisTitle(5, "Counts");

    // H1(6): inclusive Xe recoil in H-doped LXe
    analysisManager->CreateH1("Recoil_Xe_Inclusive",
                              "Inclusive Xe recoil energy in H-doped LXe",
                              300, 0., 400.0);
    analysisManager->SetH1XAxisTitle(6, "Recoil Energy (keV)");
    analysisManager->SetH1YAxisTitle(6, "Counts");

    // H1(7): inclusive dopant recoil in doped LXe
    analysisManager->CreateH1("Recoil_H_Inclusive",
                              "Inclusive dopant recoil energy in doped LXe",
                              400, 0., 11000.0);
    analysisManager->SetH1XAxisTitle(7, "Recoil Energy (keV)");
    analysisManager->SetH1YAxisTitle(7, "Counts");

    // H1(8): all primary-neutron EJ309 entry times, independent of LXe tagging
    analysisManager->CreateH1("EJ309EntryTime_All",
                              "Primary neutron entry time into EJ309",
                              1000, 0., 1000.);
    analysisManager->SetH1XAxisTitle(8, "Entry Time (ns)");
    analysisManager->SetH1YAxisTitle(8, "Counts");

    // H1(9): tagged ToF for every EJ309 re-entry after the first LXe scatter
    analysisManager->CreateH1("ToF_AllTaggedEntries",
                              "Tagged neutron ToF for every EJ309 entry after first LXe scatter",
                              1000, 0., 1000.);
    analysisManager->SetH1XAxisTitle(9, "ToF (ns)");
    analysisManager->SetH1YAxisTitle(9, "Counts");


    ////////////////// 2D Histograms //////////////////

    // H2(0): ToF vs recoil energy for Xe recoils
    analysisManager->CreateH2("ToFvsRecoil_Xe",
                              "Neutron ToF vs Xe recoil energy",
                              500, 0., 500.,
                              300, 0., 200.);
    analysisManager->SetH2XAxisTitle(0, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(0, "Recoil Energy (keV)");

    // H2(1): ToF vs recoil energy for active dopant recoils (H or He)
    analysisManager->CreateH2("ToFvsRecoil_H",
                              "Neutron ToF vs dopant recoil energy",
                              500, 0., 500.,
                              400, 0., 11000.);
    analysisManager->SetH2XAxisTitle(1, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(1, "Recoil Energy (keV)");

    // H2(2): ToF vs recoil energy for all tagged recoils on one plot
    analysisManager->CreateH2("ToFvsRecoil_All",
                              "Neutron ToF vs recoil energy (Xe + H)",
                              500, 0., 500.,
                              400, 0., 11000.);
    analysisManager->SetH2XAxisTitle(2, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(2, "Recoil Energy (keV)");

    // H2(3): tagged deposited energy in LXe vs neutron ToF
    analysisManager->CreateH2("EdepVsToF_Tagged",
                              "Tagged LXe deposited energy vs neutron ToF",
                              500, 0., 500.,
                              300, 0., 400.);
    analysisManager->SetH2XAxisTitle(3, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(3, "Edep (keV)");

    // H2(4): tagged deposited energy vs neutron ToF for Xe recoils
    analysisManager->CreateH2("EdepVsToF_Xe",
                              "Tagged LXe deposited energy vs neutron ToF for Xe recoils",
                              500, 0., 500.,
                              300, 0., 400.);
    analysisManager->SetH2XAxisTitle(4, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(4, "Edep (keV)");

    // H2(5): tagged deposited energy vs neutron ToF for dopant recoils
    analysisManager->CreateH2("EdepVsToF_H",
                              "Tagged LXe deposited energy vs neutron ToF for dopant recoils",
                              500, 0., 500.,
                              400, 0., 11000.);
    analysisManager->SetH2XAxisTitle(5, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(5, "Edep (keV)");

    // H2(6): tagged deposited energy vs neutron ToF for every EJ309 re-entry
    analysisManager->CreateH2("EdepVsToF_AllTaggedEntries",
                              "Tagged LXe deposited energy vs neutron ToF for every EJ309 entry",
                              1000, 0., 1000.,
                              400, 0., 400.);
    analysisManager->SetH2XAxisTitle(6, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(6, "Edep (keV)");

    // H2(7): first tagged LXe-scatter step deposited energy vs neutron ToF
    analysisManager->CreateH2("StepEdepVsToF_Tagged",
                              "First tagged LXe-scatter step deposited energy vs neutron ToF",
                              500, 0., 500.,
                              300, 0., 400.);
    analysisManager->SetH2XAxisTitle(7, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(7, "Step Edep (keV)");

    // H2(8): first tagged LXe-scatter step deposited energy vs neutron ToF for Xe recoils
    analysisManager->CreateH2("StepEdepVsToF_Xe",
                              "First tagged LXe-scatter step deposited energy vs neutron ToF for Xe recoils",
                              500, 0., 500.,
                              300, 0., 400.);
    analysisManager->SetH2XAxisTitle(8, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(8, "Step Edep (keV)");

    // H2(9): first tagged LXe-scatter step deposited energy vs neutron ToF for dopant recoils
    analysisManager->CreateH2("StepEdepVsToF_H",
                              "First tagged LXe-scatter step deposited energy vs neutron ToF for dopant recoils",
                              500, 0., 500.,
                              400, 0., 11000.);
    analysisManager->SetH2XAxisTitle(9, "ToF (ns)");
    analysisManager->SetH2YAxisTitle(9, "Step Edep (keV)");


    ////////////////// Ntuple //////////////////

    analysisManager->CreateNtuple("Neutron", "Neutron");
    analysisManager->CreateNtupleDColumn("fGlobalTime");
    analysisManager->CreateNtupleIColumn("EventId");
    analysisManager->FinishNtuple(0);
}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run *run)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    G4int runID = run->GetRunID();

    G4String fileStem = analysisManager->GetFileName();
    if (fileStem.empty()) {
        fileStem = "output";
    }
    fMergedFileStem = fileStem;

    std::stringstream strRunID;
    strRunID << fileStem << "_" << runID << ".root";

    analysisManager->OpenFile(strRunID.str());
}

void RunAction::EndOfRunAction(const G4Run *run)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    G4String outputFile = analysisManager->GetFileName();
    analysisManager->Write();
    analysisManager->CloseFile();

    if (!outputFile.empty()) {
        fCompletedRunFiles.push_back(outputFile);
    }

    G4int runId = run->GetRunID();
    G4cout << "Finishing run " << runId << G4endl;
}

void RunAction::MergeCompletedRunFiles()
{
    if (fCompletedRunFiles.size() <= 1 || fMergedFileStem.empty()) {
        return;
    }

    G4String mergedFile = fMergedFileStem + ".root";
    G4String command = "hadd -f \"" + mergedFile + "\"";
    for (const auto &file : fCompletedRunFiles) {
        command += " \"" + file + "\"";
    }

    G4cout << "Merging " << fCompletedRunFiles.size()
           << " run files into " << mergedFile << G4endl;

    const int status = std::system(command.c_str());
    if (status != 0) {
        G4cerr << "Automatic merge failed. You can merge manually with:" << G4endl;
        G4cerr << command << G4endl;
    }
    else {
        for (const auto &file : fCompletedRunFiles) {
            if (std::remove(file.c_str()) != 0) {
                G4cerr << "Merged successfully, but could not delete temporary file: "
                       << file << G4endl;
            }
        }
    }

    fCompletedRunFiles.clear();
    fMergedFileStem.clear();
}
