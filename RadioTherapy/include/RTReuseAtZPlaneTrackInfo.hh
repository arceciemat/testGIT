// --------------------------------------------------------------
#ifndef RTReuseAtZPlaneTrackInfo_hh
#define RTReuseAtZPlaneTrackInfo_hh

#include "G4VUserTrackInformation.hh"
#include "globals.hh"

class RTReuseAtZPlaneTrackInfo : public G4VUserTrackInformation
{
public:

  RTReuseAtZPlaneTrackInfo();

  virtual ~RTReuseAtZPlaneTrackInfo();

  void Print() const {};

  G4bool IsReuse() const {
    return bReuse; }

private:
  G4bool bReuse;

};

#endif
