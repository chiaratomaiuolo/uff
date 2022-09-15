#include "construction.hh"
#include "detector.hh"
MyDetectorConstruction::MyDetectorConstruction()
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
    //Defining material of the detector (most of materials are already implemented)
    //inside Geant4, in G4Nist library. 
    
    //An instance of Nist is defined for selecting the material. 
    G4NistManager *nist = G4NistManager::Instance();
    
    //We want to construct aerogel, so we consider the needed elements. 
    G4Material *SiO2 = new G4Material("SiO2", 2.201*g/cm3,2);
    SiO2->AddElement(nist->FindOrBuildElement("Si"), 1);
    SiO2->AddElement(nist->FindOrBuildElement("O"), 2);   


    G4Material *H2O = new G4Material("H2O", 1.000*g/cm3,2);
    H2O->AddElement(nist->FindOrBuildElement("H"), 2);
    H2O->AddElement(nist->FindOrBuildElement("O"), 1);

    G4Element *C = nist->FindOrBuildElement("C");

    G4Material *Aerogel = new G4Material("Aerogel", 0.200*g/cm3, 3);
    Aerogel->AddMaterial(SiO2, 62.5*perCent);
    Aerogel->AddMaterial(H2O, 37.4*perCent);
    Aerogel->AddElement(C, 0.1*perCent);
    //Necesary to insert the optical characteristics of aerogel in order to see 
    //Cherenkov light.
    //(in principle, we construct an array of photon energies and associate point by point
    //the value of the refractive index, here we consider the index of refraction 
    //constant in the interval)
    G4double energy[2] = {1.239841939*eV/0.2,1.239841939*eV/0.9};
    G4double rindexAerogel[2] = {1.1,1.1};
    G4double rindexWorld[2] = {1.0,1.0};

    G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable();
    mptAerogel->AddProperty("RINDEX", energy, rindexAerogel, 2);

    Aerogel->SetMaterialPropertiesTable(mptAerogel); 

    //Now a 'world' air box is created, the detector will stay inside.
    //3 parts are required for defining an object: 
    //1) solid = dimensions
    //2) logical = material 
    //3) physical = for defining the position wrt other pieces of the detector
    //
    //solid features: shape and dimensions
    G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR"); 
    G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
    mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);

    worldMat->SetMaterialPropertiesTable(mptWorld);

    
    G4Box *solidWorld = new G4Box("solid world", 0.5*m, 0.5*m, 0.5*m); 
    //logical features: material
    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    //pysical features: rotations
    G4VPhysicalVolume *physWorld = new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicWorld, "physWorld",0,false, 0, true);
    G4Box *solidRadiator = new G4Box("solidRadiator", 0.4*m,0.4*m,0.01*m);
    G4LogicalVolume *logicRadiator = new G4LogicalVolume(solidRadiator, Aerogel, "logicalRadiator");
    G4VPhysicalVolume *physRadiator = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.25*m), logicRadiator, "physRadiator",logicWorld,false,0,true);
    
    G4Box *solidDetector = new G4Box("solidDetector", 0.005*m,0.005*m,0.01*m);

    logicDetector = new G4LogicalVolume(solidDetector, worldMat,"logicDetector");
    //G4VPhysicalVolume *physDetector = new G4PVPlacement(0,G4ThreeVector(0.,0.,0.49*m),logicDetector,"physDetector",logicWorld,false,1,true);
    //The wall has to contain an array of sensors, so we loop on positions
    for(G4int i=0;i<100;i++)
    {
        for(G4int j=0;j<100;j++)
        {
            G4VPhysicalVolume *physDetector = new G4PVPlacement(0,G4ThreeVector(-0.5*m+(i+0.5)*m/100,-0.5*m+(j+0.5)*m/100,0.49*m),logicDetector,"physDetector",logicWorld,false,i+j*100,true); 
        }
    }
    //Always return the 'mother' volume, because contains all the other volumes
    return physWorld; 
    
}

void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");

    logicDetector->SetSensitiveDetector(sensDet);
}







