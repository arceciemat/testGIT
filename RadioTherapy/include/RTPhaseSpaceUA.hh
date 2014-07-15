#ifndef __RTPhaseSpaceUA__
#define __RTPhaseSpaceUA__

#include "GamosCore/GamosUserActionMgr/include/GmUserRunAction.hh"
#include "GamosCore/GamosUserActionMgr/include/GmUserEventAction.hh"
#include "GamosCore/GamosUserActionMgr/include/GmUserSteppingAction.hh"
#include "RTVExtraInfoProviderLong.hh"
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
class iaea_header_type;
class iaea_record_type;
class RTPhaseSpaceHistos;

class RTPhaseSpaceUA : public GmUserRunAction, public GmUserEventAction, public GmUserSteppingAction
{
public:
  RTPhaseSpaceUA();
  ~RTPhaseSpaceUA();
  virtual void BeginOfRunAction(const G4Run* aRun);
  virtual void UserSteppingAction(const G4Step* aStep);
  virtual void BeginOfEventAction(const G4Event* anEvent);
  virtual void EndOfEventAction(const G4Event* anEvent);
  virtual void EndOfRunAction(const G4Run* aRun);
  std::vector<G4double> GetZStops() const {
    return theZStops; }

private:
  G4bool CreateIAEAheaders();
  G4bool CreateIAEAheader(G4double zstop);
  int CloseIAEAFiles();
  int CloseIAEAHeaderFiles(G4int nevents);
  int CloseIAEARecordFiles();
  int CompressIAEAFile( char* fiaeaphsp );
  void StoreTrackRecord( const G4Step* aStep, G4double zstop);
  
private:
  G4ThreeVector R;
  G4ThreeVector preR;
  
  G4String theFileName;
  std::map<G4double,iaea_header_type *> p_iaea_headers;
  std::map<G4double,iaea_record_type *> p_iaea_records;
  G4int nrecorded, ndiscarded;
  G4int theMaxNRecorded;
  std::vector<G4double> theZStops;

  G4bool bHistos;
  G4bool bKillAfterLastZStop;
  G4int theNEventsToSave;
  G4bool bStoreZ;
  G4bool bZStopInFileName;

  RTPhaseSpaceHistos* theHistos;

  std::set<G4double> bFirstTrackInEvent;

  G4int theNExtraInfoFloat;
  G4int theNExtraInfoLong;
  std::vector<bs32*> theExtraInfoLongs;
  std::vector<float*> theExtraInfoFloats;

  G4int theMaxNTracksInZ;
  std::map<G4double, G4int> theNEventsStored;

  G4bool bNotStoreBackwards;

  std::ofstream* fileOut;
};

#endif
