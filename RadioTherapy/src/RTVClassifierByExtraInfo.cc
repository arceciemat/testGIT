#include "RTVClassifierByExtraInfo.hh"
#include "G4UIcommand.hh"
#include "RTExtraInfoMgr.hh"

//---------------------------------------------------------------
RTVClassifierByExtraInfo::RTVClassifierByExtraInfo(G4String name) : GmVClassifier( name )
{
  theExtraInfoMgr = RTExtraInfoMgr::GetInstance();
}

//---------------------------------------------------------------
G4String RTVClassifierByExtraInfo::GetIndexName(G4int index)
{
   return G4UIcommand::ConvertToString( G4int(index) );
}
/*	if( theNExtraInfoLong == 2 ) {
	  G4Exception(" RTPhaseSpaceUA::BeginOfRunAction"
		      ,"Two many extra info providers for variables of type long",
		      JustWarning,
		      G4String("This provider info will not be used: "
			       + eiProviders[ii]->GetName()
			       + " , position "+ GmGenUtils::itoa(jj)).c_str());
	} else {
*/
