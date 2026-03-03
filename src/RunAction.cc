#include "MyRunAction.hh"

RunAction::RunAction()
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetNtupleMerging(true);



    
    ////////////////1D Histo//////////////

    //0 Histo1D
    analysisManager->CreateH1("Edep", "Energy deposit", 100, 0., 30) ; 
    analysisManager->SetH1XAxisTitle(0, "KeV");
    analysisManager->SetH1YAxisTitle(0, "Counts");

    //1 histo1d
    analysisManager->CreateH1("ToF", "ToF", 500, 0., 1000) ; 
    analysisManager->SetH1XAxisTitle(1, "ns");
    analysisManager->SetH1YAxisTitle(1, "Counts");




    /////////////////2D Histo//////////////////////
    //0 Histo2D
    analysisManager->CreateH2("EdepVsToFXenon", "Energy Deposited vs Time of Flight", 500, 80, 1000, 500, 0 , 1) ; 
    analysisManager->SetH2XAxisTitle(0, "ToF(ns)");
    analysisManager->SetH2YAxisTitle(0, "Edep (keV)");
    
    //1 Histo2D
    analysisManager->CreateH2("EdepVsToFHydrogen", "Energy Deposited vs Time of Flight", 500, 86, 1000, 500, 0 , 30) ; 
    analysisManager->SetH2XAxisTitle(1, "ToF(ns)");
    analysisManager->SetH2YAxisTitle(1, "Edep (keV)");
    //2 Histo2D
    analysisManager->CreateH2("EdepVsToFEJ309", "Energy Deposited vs Time of Flight", 1000, 308., 1000, 1000, 0 , 30) ; 
    analysisManager->SetH2XAxisTitle(2, "ToF(ns)");
    analysisManager->SetH2YAxisTitle(2, "Edep (keV)");





    ////////////nTuples//////////////
    //0 ntuple Gammas
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

    std::stringstream strRunID;
    strRunID << runID;

    analysisManager->OpenFile("0degree" + strRunID.str() + ".root");
    

}


void RunAction::EndOfRunAction(const G4Run *run)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    
    analysisManager->Write();

    analysisManager->CloseFile();

    G4int runId = run->GetRunID();

    G4cout << "Finishing run " << runId << G4endl;

}