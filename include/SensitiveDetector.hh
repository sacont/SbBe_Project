#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4String.hh"
#include "globals.hh"

#include <vector>

class G4Step;
class G4TouchableHistory;
class G4HCofThisEvent;

class SensitiveDetector : public G4VSensitiveDetector
{
public:
    SensitiveDetector(G4String name);
    virtual ~SensitiveDetector();

    virtual void Initialize(G4HCofThisEvent*) override;
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* history) override;
    virtual void EndOfEvent(G4HCofThisEvent*) override;

private:
    G4double fTotalLXeEnergyDeposited;

    // tagged first-scatter quantities
    G4double fRecoilEnergy;
    G4double fFirstScatterLXeEdep;
    G4double fScatterTime;
    G4double fFirstTaggedToF;
    G4double fEJ309EnterTime;
    G4int    fRecoilZ;

    // inclusive recoil sums over all LXe elastic scatters in event
    G4double fEdepXeInclusive;
    G4double fEdepDopantInclusive;

    G4bool fGotLXeScatter;
    G4bool fGotEJ309Entry;
    G4bool fFilledPair;

    std::vector<G4double> fTaggedEntryToFs;
};

#endif
