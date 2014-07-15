#ifndef RTVClassifierByExtraInfo_hh
#define RTVClassifierByExtraInfo_hh 1

#include <vector>
#include "globals.hh"
#include "GamosCore/GamosBase/Base/include/GmVClassifier.hh"
class RTExtraInfoMgr;

class RTVClassifierByExtraInfo : public GmVClassifier
{
public: // with description
  RTVClassifierByExtraInfo(G4String);
  virtual ~RTVClassifierByExtraInfo(){};
  
  virtual G4int GetIndexFromStep(const G4Step*) = 0;

  virtual void SetParameters( std::vector<G4String>& ){};

  virtual G4String GetIndexName(G4int);

protected:

  RTExtraInfoMgr* theExtraInfoMgr;
};

#endif
