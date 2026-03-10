#include "SensitiveDetector.hh"


SensitiveDetector::SensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{
    fTotalEnergyDeposited = 0.;
   
    //ToF vs Edep on LXe 
    tElasticLXe = 0.;
    edepElasticLXe = 0.;
    gotElasticLXe = false;
    tEnterReflect = 0.;
    gotReflectEntry = false;
    filledPair = false;
}

SensitiveDetector::~SensitiveDetector()
{

}

void SensitiveDetector::Initialize(G4HCofThisEvent *)
{
  
    fTotalEnergyDeposited = 0. ;
    fEnteredTarget = false;

    tElasticLXe = 0.;
    edepElasticLXe = 0.;
    gotElasticLXe = false;

    tEnterReflect = 0.;
    gotReflectEntry = false;

    filledPair = false;

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

    
    /////Edep vs Global time at various volumes
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
        
        analysisManager->FillH2(1, fGlobalTime, energyDeposited/keV);
    }
    

    if ((Z == 1 || Z == 6) && volName == "Reflect"){
        
        analysisManager->FillH2(2, fGlobalTime, energyDeposited/keV);
        
    }  

  }


// ----------------------------------------
    // 1. Record FIRST elastic collision in LXe
    // ----------------------------------------
    if (!gotElasticLXe &&
        particleName == "neutron" &&
        parentID == 0 &&
        volName == "LXe" &&
        energyDeposited > 0. &&
        process &&
        process->GetProcessName() == "hadElastic" )
    {
        edepElasticLXe = energyDeposited;
        tElasticLXe = fGlobalTime;
        gotElasticLXe = true;
        
    }

    // ----------------------------------------
    // 2. Record FIRST entry into Reflect
    //    but only after LXe elastic happened
    // ----------------------------------------
    if (gotElasticLXe && !gotReflectEntry &&
        particleName == "neutron" &&
        parentID == 0 &&
        volName == "Reflect" &&
        stepStatus == fGeomBoundary
        && process->GetProcessName() == "hadElastic"
        )
    {
        
        
        tEnterReflect = fGlobalTime;
        gotReflectEntry = true;
    }

    // ----------------------------------------
    // 3. Fill once when both are available
    // ----------------------------------------
    if (gotElasticLXe && gotReflectEntry && !filledPair)
    {
        G4double tof = tEnterReflect - tElasticLXe;

        if (tof >= 0.)
        {
            analysisManager->FillH2(3, tof / ns, edepElasticLXe / keV );
            filledPair = true;
            
            //G4cout << edepElasticLXe << G4endl;
        }
    }
  
    
    
     
  
    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent *)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    if (gotTiH && gotTf) {
        analysisManager->FillNtupleIColumn(0, 1, eventID);
        analysisManager->AddNtupleRow(0);
    }
}