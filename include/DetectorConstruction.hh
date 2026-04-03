#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4GenericMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct() override;
    virtual void ConstructSDandField() override;

private:
    void DefineCommands();
    void SetEJ309Distance(G4double value);
    void SetEJ309Angle(G4double value);

    G4LogicalVolume* logicLXe = nullptr;
    G4LogicalVolume* logicReflect = nullptr;
    G4LogicalVolume* logicLead = nullptr;
    G4GenericMessenger* fMessenger = nullptr;
    double fScintDistance = 50.0;
    double fScintAngleDeg = 30.0;
};

#endif
