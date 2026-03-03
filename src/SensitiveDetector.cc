#include "SensitiveDetector.hh"


SensitiveDetector::SensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{
    fTotalEnergyDeposited = 0.;
    ti = 0;
    tf = 0;
    Tof = 0;
    gotTi = gotTf = filledTof = false;
}

SensitiveDetector::~SensitiveDetector()
{

}

void SensitiveDetector::Initialize(G4HCofThisEvent *)
{
    ti = 0;
    tf = 0;
    Tof = 0;
    gotTi = gotTf = filledTof = false;
    fTotalEnergyDeposited = 0. ;
    fEnteredTarget = false;

}


G4bool SensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
     
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint* postStep = aStep->GetPostStepPoint();

    G4ThreeVector prePos  = preStepPoint->GetPosition();
    G4ThreeVector postPos = postStep->GetPosition();
    G4double x = postPos.x()/cm;
    G4double y = postPos.y()/cm;
    G4double z = postPos.z()/cm;


    G4double fGlobalTime = preStepPoint->GetGlobalTime();
    G4Track* track = aStep->GetTrack();
    G4StepStatus stepStatus = preStepPoint->GetStepStatus();
    const G4VProcess* process = postStep->GetProcessDefinedStep();
    G4String procName = process->GetProcessName();
    G4String volName = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName(); 
    
    G4int trackID = track->GetTrackID();
    G4int parentID = track->GetParentID();
    G4int pdgEncoding = track->GetDefinition()->GetPDGEncoding();
    G4double deltaT = aStep->GetDeltaTime();
    G4ThreeVector posNeutron = preStepPoint->GetPosition();
    G4ThreeVector momNeutron = preStepPoint->GetMomentum();
    G4double fmomNeutron = momNeutron.mag();
    G4double fenergy = (1 * eV);
    G4int pdg = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
    
    G4double KE = preStepPoint->GetKineticEnergy();
    G4double KEev = KE / eV;
    G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();
    auto secondaries = aStep->GetSecondaryInCurrentStep();
    
    
    G4double energyDeposited = aStep->GetTotalEnergyDeposit();

    

    if (process->GetProcessName() == "hadElastic" &&
        particleName == "neutron" && parentID == 0 ){
   

        int Z = -1;

     const auto* hadProc = dynamic_cast<const G4HadronicProcess*>(process);
    if (hadProc) {
      const G4Nucleus* nuc = hadProc->GetTargetNucleus();
      if (nuc) Z = nuc->GetZ_asInt();
    }

    // If Geant4 doesn't provide the target nucleus, fall back to Xe
    if (Z < 0 && volName == "LXe") Z = 54;

    if (Z == 54){ 
        
        analysisManager->FillH2(0, fGlobalTime, energyDeposited/keV);
    }
    else if (Z == 1 && volName == "LXe")  {
        if (!gotTi){
        ti = fGlobalTime;
        gotTi = true;
        }
        analysisManager->FillH2(1, fGlobalTime, energyDeposited/keV);
    }
    

    if ((Z == 1 || Z == 6) && volName == "Reflect"){
        if (!gotTf){
            tf = fGlobalTime;
            gotTf = true;
        }
        analysisManager->FillH2(2, fGlobalTime, energyDeposited/keV);
        
    }
    if(gotTi && gotTf && !filledTof){
        Tof = tf - ti;
        analysisManager->FillH1(1, Tof);
        filledTof = true;
    }    
    


  }
    

    

    
   
   
   
    if(volName == "LXe"){
        fEnteredTarget = true;
    }
  
    
    
     
  
    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent *)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    if (gotTi && gotTf) {
        analysisManager->FillNtupleIColumn(0, 1, eventID);
        analysisManager->AddNtupleRow(0);
    }
}