#ifndef RTClassifierByEILong_hh
#define RTClassifierByEILong_hh 1

#include <map>
#include "globals.hh"
#include "RTVClassifierByExtraInfo.hh"

class RTClassifierByEILong  : public RTVClassifierByExtraInfo
{
public: // with description
  RTClassifierByEILong(G4String);
  virtual ~RTClassifierByEILong();
  
  virtual G4int GetIndexFromStep(const G4Step*);

  virtual void SetParameters( std::vector<G4String>& param );

  virtual G4String GetIndexName(G4int);

  virtual std::set<G4int> GetIndexSet() const;

  virtual void SetIndices( std::vector<G4String> wl );

private:
  std::map<G4int,G4String> theIndexNames;
  std::map<G4int,G4int> theIndexMap;
  G4int theFirstBit;
  G4int theNBits;
  G4int theLastBit;
  G4int theMaxNFound;
};

#endif
