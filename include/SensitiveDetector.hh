#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Proton.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Neutron.hh"
#include "G4VProcess.hh"
#include "G4HadronicProcess.hh"
#include "G4Nucleus.hh"

#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include <set>                      
#include <unordered_set>
#include <unordered_map>   
class SensitiveDetector : public G4VSensitiveDetector
{
public:
    SensitiveDetector(G4String);
    ~SensitiveDetector();

private:
    virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);


    virtual void Initialize(G4HCofThisEvent*) override;
    virtual void EndOfEvent(G4HCofThisEvent *) override;

    G4double fTotalEnergyDeposited;
    G4double fEdepPVT;

    G4double tiH;
    G4double tiXe;
    G4double tf;
    G4double Tof;
    G4bool gotTiH;
    G4bool gotTiXe;
    G4bool filledTofH;
    G4bool filledTofXe;
    G4bool gotTf;
    

    //Edep in LXe vs Time of FLight Variables 

    G4double tElasticLXe; //time when neutron had Elastic Collision
    G4double edepElasticLXe; //deposited energy
    G4bool gotElasticLXe; //found Elastic Collision

    G4double tEnterReflect; //time when entered Reflect
    G4bool gotReflectEntry; //Found Reflect Entry
    G4bool filledPair; //Prevents multiple filling per event



    bool fEnteredTarget = false;
    
};

#endif