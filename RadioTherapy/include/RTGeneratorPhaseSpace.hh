#ifndef RTGeneratorPhaseSpace_HH
#define RTGeneratorPhaseSpace_HH

class G4Event;
class GmParticleSource;
class GmIsotopeMgr;
class iaea_record_type;
#include "globals.hh"
#include <iostream>
#include <fstream>
#include "GamosCore/GamosGenerator/include/GmGeneratorFromFile.hh"
class G4ParticleDefinition;
class RTPhaseSpaceHistos;
class RTExtraInfoMgr;
enum MWR {NOMir, MirX, MirY, MirXY};

class RTGeneratorPhaseSpace: public GmGeneratorFromFile
{
public:
  RTGeneratorPhaseSpace();
  ~RTGeneratorPhaseSpace();
  void GeneratePrimaries(G4Event* evt);

private:
  FILE* p_rfile;
  iaea_record_type *p_iaea_read;

  G4int theMaxNReuse, theNReused;
  G4int theMaxNRecycle, theNRecycled;
  G4String theFileName;
  G4int theNEventsInFile;
  G4bool bMaxNReuseAutomatic;
  G4ThreeVector theInitialDisp;
  MWR MirrorWhenReuse;

  G4double theInitialRotAngleX;
  G4double theInitialRotAngleY;
  G4double theInitialRotAngleZ;

  G4int theNEventsSkip;
  G4ParticleDefinition* theParticleDef;

  G4bool bHistos;
  RTPhaseSpaceHistos* theHistos;

  RTExtraInfoMgr* theExtraInfoMgr;

  enum transType { TTDisplacement, TTRotateXYZ, TTRotateThetaPhiSelf } ;/// " D RXYZ, RTPS 
  
  struct transformation_struct{
    transType operation;
    G4double  opx;
    G4double  opy;
    G4double  opz;
  };
  
  std::vector<transformation_struct> theTransformations;

  G4int theNNewEvents;
  G4double theNParticlesRead;
  G4double theNOrigEvents;  
};

#endif
