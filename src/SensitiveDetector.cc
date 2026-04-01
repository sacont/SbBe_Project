#include "SensitiveDetector.hh"

#include "G4AnalysisManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"
#include "G4HCofThisEvent.hh"
#include "G4SystemOfUnits.hh"

#include "G4HadronicProcess.hh"
#include "G4Nucleus.hh"

SensitiveDetector::SensitiveDetector(G4String name)
  : G4VSensitiveDetector(name),
    fTotalEnergyDeposited(0.),
    fRecoilEnergy(0.),
    fScatterTime(-1.),
    fEJ309EnterTime(-1.),
    fRecoilZ(-1),
    fEdepXeInclusive(0.),
    fEdepDopantInclusive(0.),
    fGotLXeScatter(false),
    fGotEJ309Entry(false),
    fFilledPair(false)
{}

SensitiveDetector::~SensitiveDetector() {}

namespace {
G4int GetTargetAtomicNumber(const G4VProcess* process)
{
    if (!process) {
        return -1;
    }

    const auto* hadProc = dynamic_cast<const G4HadronicProcess*>(process);
    if (!hadProc) {
        return -1;
    }

    const G4Nucleus* nucleus = hadProc->GetTargetNucleus();
    return nucleus ? nucleus->GetZ_asInt() : -1;
}
}

void SensitiveDetector::Initialize(G4HCofThisEvent*)
{
    fTotalEnergyDeposited = 0.;

    fRecoilEnergy = 0.;
    fScatterTime = -1.;
    fEJ309EnterTime = -1.;
    fRecoilZ = -1;

    fEdepXeInclusive = 0.;
    fEdepDopantInclusive  = 0.;

    fGotLXeScatter = false;
    fGotEJ309Entry = false;
    fFilledPair = false;
}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    auto* analysisManager = G4AnalysisManager::Instance();

    G4StepPoint* preStepPoint  = aStep->GetPreStepPoint();
    G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
    G4Track* track             = aStep->GetTrack();

    if (!preStepPoint || !postStepPoint || !track) return true;

    const G4VPhysicalVolume* prePV  = preStepPoint->GetTouchableHandle()->GetVolume();
    const G4VPhysicalVolume* postPV = postStepPoint->GetTouchableHandle()->GetVolume();

    const G4String preVolName  = (prePV)  ? prePV->GetName()  : "";
    const G4String postVolName = (postPV) ? postPV->GetName() : "";

    const G4String particleName = track->GetDefinition()->GetParticleName();
    const G4int parentID = track->GetParentID();

    const G4double edep = aStep->GetTotalEnergyDeposit();
    if (edep > 0.) {
        fTotalEnergyDeposited += edep;
    }

    const G4VProcess* process = postStepPoint->GetProcessDefinedStep();

    // ------------------------------------------------------------
    // A) Inclusive LXe recoil spectrum:
    //    accumulate ALL primary-neutron hadElastic recoils in LXe
    // ------------------------------------------------------------
    if (particleName == "neutron" &&
        parentID == 0 &&
        preVolName == "LXe" &&
        process &&
        process->GetProcessName() == "hadElastic")
    {
        const G4double preKE  = preStepPoint->GetKineticEnergy();
        const G4double postKE = postStepPoint->GetKineticEnergy();

        G4double recoilKE = preKE - postKE;
        if (recoilKE < 0.) recoilKE = 0.;

        const G4int Z = GetTargetAtomicNumber(process);

        if (Z == 54)             fEdepXeInclusive += recoilKE;
        else if (Z == 1 || Z == 2) fEdepDopantInclusive += recoilKE;
    }

    // ------------------------------------------------------------
    // B) Tagged sample:
    //    first LXe elastic scatter only, later entering EJ309
    // ------------------------------------------------------------
    if (!fGotLXeScatter &&
        particleName == "neutron" &&
        parentID == 0 &&
        preVolName == "LXe" &&
        process &&
        process->GetProcessName() == "hadElastic")
    {
        const G4double preKE  = preStepPoint->GetKineticEnergy();
        const G4double postKE = postStepPoint->GetKineticEnergy();

        G4double recoilKE = preKE - postKE;
        if (recoilKE < 0.) recoilKE = 0.;

        const G4int Z = GetTargetAtomicNumber(process);

        if (recoilKE > 0. && (Z == 54 || Z == 1 || Z == 2)) {
            fScatterTime   = postStepPoint->GetGlobalTime();
            fRecoilEnergy  = recoilKE;
            fRecoilZ       = Z;
            fGotLXeScatter = true;
        }
    }

    // First entry of same neutron into EJ309
    if (fGotLXeScatter && !fGotEJ309Entry &&
        particleName == "neutron" &&
        parentID == 0 &&
        preVolName == "EJ309" &&
        postVolName == "EJ309" &&
        preStepPoint->GetStepStatus() == fGeomBoundary)
    {
        fEJ309EnterTime = preStepPoint->GetGlobalTime();
        fGotEJ309Entry = true;
    }

    // Fill tagged once
    if (fGotLXeScatter && fGotEJ309Entry && !fFilledPair)
    {
        const G4double tof = fEJ309EnterTime - fScatterTime;

        if (tof >= 0.) {
            analysisManager->FillH1(1, tof / ns);  // tagged ToF all
            analysisManager->FillH2(2, tof / ns, fRecoilEnergy / keV);
            analysisManager->FillH2(3, tof / ns, fRecoilEnergy / keV);

            if (fRecoilZ == 54) {
                analysisManager->FillH2(0, tof / ns, fRecoilEnergy / keV);
                analysisManager->FillH2(4, tof / ns, fRecoilEnergy / keV);
                analysisManager->FillH1(2, tof / ns);
                analysisManager->FillH1(4, fRecoilEnergy / keV);
                fFilledPair = true;
            }
            else if (fRecoilZ == 1 || fRecoilZ == 2) {
                analysisManager->FillH2(1, tof / ns, fRecoilEnergy / keV);
                analysisManager->FillH2(5, tof / ns, fRecoilEnergy / keV);
                analysisManager->FillH1(3, tof / ns);
                analysisManager->FillH1(5, fRecoilEnergy / keV);
                fFilledPair = true;
            }
        }
    }

    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    auto* analysisManager = G4AnalysisManager::Instance();

    if (fTotalEnergyDeposited > 0.) {
        analysisManager->FillH1(0, fTotalEnergyDeposited / keV);
    }

    // inclusive event-level recoil sums in H-doped LXe, meaning any recoils in HLXe cylinder
    if (fEdepXeInclusive > 0.) {
        analysisManager->FillH1(6, fEdepXeInclusive / keV);
    }
    if (fEdepDopantInclusive > 0.) {
        analysisManager->FillH1(7, fEdepDopantInclusive / keV);
    }
}
