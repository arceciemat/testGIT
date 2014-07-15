#include "RTReuseAtZPlaneUA.hh"
#include "RTReuseAtZPlaneMessenger.hh"
#include "RTReuseAtZPlaneTrackInfo.hh"

//---------------------------------------------------------------------
RTReuseAtZPlaneUA::RTReuseAtZPlaneUA()
{
  new RTReuseAtZPlaneMessenger();

}

//---------------------------------------------------------------------
RTReuseAtZPlaneUA::~RTReuseAtZPlaneUA()
{

}

//---------------------------------------------------------------------
void RTReuseAtZPlaneUA::UserSteppingAction(const G4Step* aStep)
{
  G4VUserTrackInformation* trkInfo = aStep->GetTrack()->GetUserInformation();
  if( trkInfo ) {
    RTReuseAtZPlaneTrackInfo* trkInfoS = (RTReuseAtZPlaneTrackInfo*)trkInfo;
    if( trkInfoS->IsReuse() ){
      G4TrackVector secondariesTrack = GetStepSecondaries();
      G4TrackVector::const_iterator ite;
      for( ite = secondariesTrack.begin(); ite != secondariesTrack.end(); ite++ ){
	RTReuseAtZPlaneTrackInfo * ti = new RTReuseAtZPlaneTrackInfo; 
	//	G4cout << " adding RTReuseAtZPlaneTrackInfo " << *ite << " -> " << ti << G4endl;	
	(*ite)->SetUserInformation( ti );
      }
    }
  }
}
