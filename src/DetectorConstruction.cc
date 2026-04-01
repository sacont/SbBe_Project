#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"

#include <cmath>

DetectorConstruction::DetectorConstruction()
{
}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
  G4bool checkOverlaps = true;

  G4NistManager *nist = G4NistManager::Instance();

  //////////////////World Construction/////////////////////

  G4double xWorld = 2.5 * m;
  G4double yWorld = 2.5 * m;
  G4double zWorld = 2.5 * m;

  G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box *solidWorld = new G4Box("solidWorld", 0.5 * xWorld, 0.5 * yWorld, 0.5 * zWorld);
  G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
  G4VPhysicalVolume *physWorld =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicWorld, "physWorld", 0, false, 0, checkOverlaps);

  //////////////////Envelope/////////////////////

  double inch = 2.54 * cm;
  G4double env_sizeXY = 200 * cm, env_sizeZ = 200 * cm;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");

  auto solidEnv = new G4Box("Envelope", 0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);
  auto logicEnv = new G4LogicalVolume(solidEnv, env_mat, "Envelope");

  G4VisAttributes* EnvVisAtt = new G4VisAttributes(G4Colour(.25, .25, .25, .25));
  EnvVisAtt->SetForceSolid(true);
  logicEnv->SetVisAttributes(EnvVisAtt);

  new G4PVPlacement(nullptr, G4ThreeVector(), logicEnv, "Envelope", logicWorld, false, 0, checkOverlaps);

  //////////////////Xenon Cylinder/////////////////////

  G4double rIn_LXe  = 0.0 * cm;
  G4double rOut_LXe = 1.5 * cm;
  G4double h_LXe    = 1.5875 * cm;
  G4double hz_LXe   = 0.5 * h_LXe;

  G4Material* LXe = nist->FindOrBuildMaterial("G4_lXe");
  G4Material* H2Approx = nist->FindOrBuildMaterial("G4_H");
  G4Material* HeApprox = nist->FindOrBuildMaterial("G4_He");

  const G4double density = LXe->GetDensity();

  // Toggle the active dopant by commenting one block and uncommenting the other.
//   G4Material* activeDopant = HeApprox;
//   G4double activeDopantMassFraction = 9.79e-4;
//   G4String dopedMaterialName = "LXe_He";

  
  G4Material* activeDopant = H2Approx;
  G4double activeDopantMassFraction = 9.79e-4;
  G4String dopedMaterialName = "LXe_H2";
  

  G4Material* dopedLXe = new G4Material(dopedMaterialName, density, 2);
  dopedLXe->AddMaterial(LXe, 1. - activeDopantMassFraction);
  dopedLXe->AddMaterial(activeDopant, activeDopantMassFraction);

  G4Tubs* solidLXe = new G4Tubs("solidLXe", rIn_LXe, rOut_LXe, hz_LXe, 0. * deg, 360. * deg);
  logicLXe = new G4LogicalVolume(solidLXe, dopedLXe, "logicLXe");

  G4VPhysicalVolume* physLXe =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicLXe, "LXe", logicEnv, false, 0, checkOverlaps);

  ///////////////////Lead around source/////////////////////

  G4Material* leadMat = nist->FindOrBuildMaterial("G4_Pb");

  const G4ThreeVector sourcePosition(-50.0 * cm, 0., 0.);
  const G4double leadThickness = 1.5 * cm;
  const G4double sourceCavityHalfSize = 1.0 * cm;
  const G4double sourceCavityHalfLength = 1.0 * cm;

  const G4double sideWallHalfX = sourceCavityHalfLength;
  const G4double sideWallHalfYZ = sourceCavityHalfSize;
  const G4double sideWallHalfThickness = 0.5 * leadThickness;

  G4Box* solidLeadSide = new G4Box("solidLeadSide",
                                   sideWallHalfX,
                                   sideWallHalfThickness,
                                   sideWallHalfYZ);
  logicLead = new G4LogicalVolume(solidLeadSide, leadMat, "logicLead");

  new G4PVPlacement(nullptr,
                    sourcePosition + G4ThreeVector(0., sourceCavityHalfSize + sideWallHalfThickness, 0.),
                    logicLead, "LeadTop", logicEnv, false, 0, checkOverlaps);

  new G4PVPlacement(nullptr,
                    sourcePosition + G4ThreeVector(0., -(sourceCavityHalfSize + sideWallHalfThickness), 0.),
                    logicLead, "LeadBottom", logicEnv, false, 1, checkOverlaps);

  G4Box* solidLeadZWall = new G4Box("solidLeadZWall",
                                    sideWallHalfX,
                                    sideWallHalfYZ,
                                    sideWallHalfThickness);
  G4LogicalVolume* logicLeadZWall = new G4LogicalVolume(solidLeadZWall, leadMat, "logicLeadZWall");

  new G4PVPlacement(nullptr,
                    sourcePosition + G4ThreeVector(0., 0., sourceCavityHalfSize + sideWallHalfThickness),
                    logicLeadZWall, "LeadPlusZ", logicEnv, false, 2, checkOverlaps);
  new G4PVPlacement(nullptr,
                    sourcePosition + G4ThreeVector(0., 0., -(sourceCavityHalfSize + sideWallHalfThickness)),
                    logicLeadZWall, "LeadMinusZ", logicEnv, false, 3, checkOverlaps);

  G4Box* solidLeadXWall = new G4Box("solidLeadXWall",
                                    sideWallHalfThickness,
                                    sourceCavityHalfSize + leadThickness,
                                    sourceCavityHalfSize + leadThickness);
  G4LogicalVolume* logicLeadXWall = new G4LogicalVolume(solidLeadXWall, leadMat, "logicLeadXWall");

  new G4PVPlacement(nullptr,
                    sourcePosition + G4ThreeVector(-(sourceCavityHalfLength + sideWallHalfThickness), 0., 0.),
                    logicLeadXWall, "LeadBack", logicEnv, false, 4, checkOverlaps);
  new G4PVPlacement(nullptr,
                    sourcePosition + G4ThreeVector(sourceCavityHalfLength + sideWallHalfThickness, 0., 0.),
                    logicLeadXWall, "LeadFront", logicEnv, false, 5, checkOverlaps);

  ///////////////////EJ309 cylinder/////////////////////

  G4double rIn_Reflect  = 0.0 * inch;
  G4double rOut_Reflect = 3.0 * inch;
  G4double h_Reflect    = 3.0 * inch;
  G4double hz_Reflect   = 0.5 * h_Reflect;

  G4Material* EJ309 = new G4Material("EJ309", 0.964 * g / cm3, 2);

  G4Element* elH = nist->FindOrBuildElement("H");
  G4Element* elC = nist->FindOrBuildElement("C");

  EJ309->AddElement(elC, 0.906);
  EJ309->AddElement(elH, 0.094);

  // Match the slide geometry: the EJ309 is placed downward from the beam axis (+x).
  const G4double scintDistance = 50.0 * cm;
  const G4double scintAngleFromBeam = 30.0 * deg;
  const G4ThreeVector scintPosition(scintDistance * std::cos(scintAngleFromBeam),
                                    0.,
                                    -scintDistance * std::sin(scintAngleFromBeam));

  auto rot = new G4RotationMatrix();
  rot->rotateY(90. * deg + scintAngleFromBeam);

  G4Tubs* solidReflect = new G4Tubs("solidReflect", rIn_Reflect, rOut_Reflect, hz_Reflect, 0. * deg, 360. * deg);
  logicReflect = new G4LogicalVolume(solidReflect, EJ309, "logicReflect");
  G4VPhysicalVolume* physReflect =
      new G4PVPlacement(rot, scintPosition, logicReflect, "EJ309", logicEnv, false, 0, checkOverlaps);

  ///////////////////Colors/////////////////////

  G4VisAttributes* Yellow    = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 0.5));
  G4VisAttributes* SlateBlue = new G4VisAttributes(G4Colour(0.42, 0.35, 0.8, 0.75));
  G4VisAttributes* LeadGray  = new G4VisAttributes(G4Colour(0.35, 0.35, 0.35, 0.8));

  Yellow->SetForceSolid(true);
  SlateBlue->SetForceSolid(true);
  LeadGray->SetForceSolid(true);

  logicLXe->SetVisAttributes(Yellow);
  logicReflect->SetVisAttributes(SlateBlue);
  logicLead->SetVisAttributes(LeadGray);
  logicLeadZWall->SetVisAttributes(LeadGray);
  logicLeadXWall->SetVisAttributes(LeadGray);

  return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
    SensitiveDetector *sensDet = new SensitiveDetector("SensitiveDetector");
    G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);

    logicLXe->SetSensitiveDetector(sensDet);
    logicReflect->SetSensitiveDetector(sensDet);
}
