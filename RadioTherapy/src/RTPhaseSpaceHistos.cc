#include "RTPhaseSpaceHistos.hh"
#include "RTPhaseSpaceUA.hh"
#include "RTVerbosity.hh"
#include "EGSPhspParticle.hh"

#include "GamosCore/GamosBase/Base/include/GmAnalysisMgr.hh"
#include "GamosCore/GamosBase/Base/include/GmParameterMgr.hh"
#include "GamosCore/GamosUserActionMgr/include/GmUserSteppingActionList.hh"
#include "GamosCore/GamosUtils/include/GmGenUtils.hh"
#include "GamosCore/GamosBase/Base/include/GmVClassifier.hh"

#include "G4Run.hh"
#include "G4ios.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Trajectory.hh"
#include "G4UserSteppingAction.hh"

//----------------------------------------------------------------
RTPhaseSpaceHistos::RTPhaseSpaceHistos(RTPhaseSpaceUA* ua)
{
  theZstops = ua->GetZStops();

  Initialise();
}


//----------------------------------------------------------------
RTPhaseSpaceHistos::RTPhaseSpaceHistos(G4double zstop)
{
  theZstops.push_back( zstop );

  Initialise();
}
 

//----------------------------------------------------------------
void RTPhaseSpaceHistos::Initialise()
{
  G4String fileName = GmParameterMgr::GetInstance()->GetStringValue("RTPhaseSpaceHistos:FileName","phaseSpace");

  //  G4String suffix = GmParameterMgr::GetInstance()->GetStringValue("GmAnalysisMgr:FileNameSuffix","");
  //  if(suffix != "" ) fileName += suffix;

  //  G4String prefix = GmParameterMgr::GetInstance()->GetStringValue("GmAnalysisMgr:FileNamePrefix","");
  // if(prefix != "" ) fileName = prefix + fileName;

  theAnaMgr = GmAnalysisMgr::GetInstance(fileName);

  DefineHistoLimits();
  /*  BookHistos("ALL"); 
  BookHistos("gamma");
  BookHistos("e-");
  BookHistos("e+");
  bHadrons = G4bool(GmParameterMgr::GetInstance()->GetNumericValue("RTPhaseSpaceHistos:Hadrons",0));

  if( bHadrons ) {
    BookHistos("neutron");
    BookHistos("proton");
   }*/

  theClassifier = 0;

}

//---------------------------------------------------------------------
void RTPhaseSpaceHistos::DefineHistoLimits()
{
  theNbins = G4int( GmParameterMgr::GetInstance()->GetNumericValue("RTPhaseSpaceHistos:Nbins", 100 ) );
  theHisR = 100.;
  theHisAng = 180.;
  theHisE = 10.*MeV;
}

//---------------------------------------------------------------------
void RTPhaseSpaceHistos::BookHistos(G4String partName, G4int index)
{
  G4int hid = theParticleHistoIDs.size();
  theParticleHistoIDs[partName] = hid;

  G4String hnam;
  G4String hgnam = "PhaseSpace: ";

  for( unsigned int zstopID = 0; zstopID < theZstops.size(); zstopID++ ){
    G4int histoNumber = 760000+10000*zstopID+1000*hid+100*index;
    G4String hgnam = "PhaseSpace: " + GmGenUtils::ftoa(theZstops[zstopID]) + ": " + GetHistoName(index) + ":";
    
    hnam = hgnam + partName + G4String(": X at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,-theHisR,theHisR,histoNumber+1);
    hnam = hgnam + partName + G4String(": Y at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,-theHisR,theHisR,histoNumber+2);

    hnam = hgnam + partName + G4String(": XY at Zstop");
    theAnaMgr->CreateHisto2D(hnam,theNbins,-theHisR,theHisR,theNbins,-theHisR,theHisR,histoNumber+9);

    hnam = hgnam + partName + G4String(": R at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,0.,theHisR,histoNumber+3);
    hnam = hgnam + partName + G4String(": Direction Theta at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,0,theHisAng/2.,histoNumber+4);
    hnam = hgnam + partName + G4String(": Direction Phi at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,-theHisAng,theHisAng,histoNumber+5);
    hnam = hgnam + partName + G4String(": Energy at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,0.,theHisE,histoNumber+6); 
    hnam = hgnam + partName + G4String(": Vx at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,0.,1.,histoNumber+7);
    hnam = hgnam + partName + G4String(": Vy at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,0.,1.,histoNumber+8);
    hnam = hgnam + partName + G4String(": Vz at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,0.,1.,histoNumber+9);
   
    hnam = hgnam + partName + G4String(": R vs Direction Theta at Zstop");
    theAnaMgr->CreateHisto2D(hnam,theNbins,0,theHisR,theNbins,0.,theHisAng,histoNumber+11);
    hnam = hgnam + partName + G4String(": R vs Energy at Zstop");
    theAnaMgr->CreateHisto2D(hnam,theNbins,0,theHisR,theNbins,0.,theHisE,histoNumber+12);
    hnam = hgnam + partName + G4String(": Direction Theta vs Energy at Zstop");
    theAnaMgr->CreateHisto2D(hnam,theNbins,0,theHisAng,theNbins,0.,theHisE,histoNumber+13);

    hnam = hgnam + partName + G4String(": Weight at Zstop");
    theAnaMgr->CreateHisto1D(hnam,theNbins,0.,1.1,histoNumber+14);

    hnam = hgnam + partName + G4String(": X vs Energy at Zstop");
    theAnaMgr->CreateHisto2D(hnam,theNbins,0,theHisR,theNbins,0.,theHisE,histoNumber+21);
    hnam = hgnam + partName + G4String(": X vs Vx at Zstop");
    theAnaMgr->CreateHisto2D(hnam,theNbins,0,theHisR,theNbins,0.,1.,histoNumber+22);

    hnam = hgnam + partName + G4String(": X prof weight");
    theAnaMgr->CreateHistoProfile1D(hnam,theNbins,-theHisR,theHisR,histoNumber+30);

  }
}


//---------------------------------------------------------------------
void RTPhaseSpaceHistos::FillHistos( iaea_record_type *p_iaea_record, G4double zstop, const G4Step* aStep )
{
  G4int index;
  if ( aStep && theClassifier ) {
    index = theClassifier->GetIndexFromStep(aStep);
  } else {
    index = 0;
  }

  unsigned int zstopID;
  for( zstopID = 0; zstopID < theZstops.size(); zstopID++ ){
    if( fabs(zstop-theZstops[zstopID]) < 1.E-6 ) break;
  }

  if( theHistos.find(index) == theHistos.end() ) {
    BookHistos("ALL",index);
    BookHistos("gamma",index);
    BookHistos("e-",index);
    BookHistos("e+",index);
    bHadrons = G4bool(GmParameterMgr::GetInstance()->GetNumericValue("RTPhaseSpaceHistos:Hadrons",0));
    
    if( bHadrons ) {
      BookHistos("neutron",index);
      BookHistos("proton",index);
    }
    theHistos.insert(index);
  }

  //  G4int hid = GetHistoID("ALL");
  if( zstopID >= theZstops.size() ) zstopID = 0;
  FillHistos( p_iaea_record, 0, zstopID, index );
  FillHistos( p_iaea_record, p_iaea_record->particle, zstopID, index );
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << " RTPhaseSpaceHistos::FillHistos zstop " << zstop << " zstopID " << zstopID << " particle " << p_iaea_record->particle << " Y " << p_iaea_record->y*cm << G4endl;
#endif
}

//---------------------------------------------------------------------
void RTPhaseSpaceHistos::FillHistos( iaea_record_type *p_iaea_record, G4int hid, G4int zstopID, G4int index )
{
  G4double posX = p_iaea_record->x*cm;
  G4double posY = p_iaea_record->y*cm;
  G4double posR = sqrt(p_iaea_record->x*p_iaea_record->x+p_iaea_record->y*p_iaea_record->y)*cm;
  G4double theta = acos(fabs(p_iaea_record->w))/deg;
  G4double phi = atan2(p_iaea_record->u,p_iaea_record->v)/deg;
  //  G4cout << " phi " << phi << " phix " << atan2(p_iaea_record->x,p_iaea_record->y)/deg  << G4endl;
  G4double energy = fabs(p_iaea_record->energy);
  G4double weight = p_iaea_record->weight;
  //  G4cout << "  RTPhaseSpaceHistos::FillHistos " << hid << " " << posX << " " << posY << " " << posR << " " << theta << " " << phi << " " << energy << G4endl;

  G4int histoNumber = 760000+10000*zstopID+1000*hid+100*index;
  theAnaMgr->GetHisto1(histoNumber+1)->Fill( posX, weight );
  theAnaMgr->GetHisto1(histoNumber+2)->Fill( posY, weight );

  theAnaMgr->GetHisto1(histoNumber+3)->Fill( posR, weight );
  theAnaMgr->GetHisto1(histoNumber+4)->Fill( theta, weight );
  theAnaMgr->GetHisto1(histoNumber+5)->Fill( phi, weight );
  theAnaMgr->GetHisto1(histoNumber+6)->Fill( energy, weight );
  theAnaMgr->GetHisto1(histoNumber+7)->Fill( p_iaea_record->u, weight );
  theAnaMgr->GetHisto1(histoNumber+8)->Fill( p_iaea_record->v, weight );
  theAnaMgr->GetHisto1(histoNumber+9)->Fill( p_iaea_record->w, weight );
  theAnaMgr->GetHisto2(histoNumber+11)->Fill( posR, theta, weight );
  theAnaMgr->GetHisto2(histoNumber+12)->Fill( posR, energy, weight );
  theAnaMgr->GetHisto2(histoNumber+13)->Fill( theta, energy, weight );
  theAnaMgr->GetHisto2(histoNumber+9)->Fill( posX, posY, weight );

  theAnaMgr->GetHisto1(histoNumber+14)->Fill( weight );

  theAnaMgr->GetHisto2(histoNumber+21)->Fill( posX, energy, weight );
  theAnaMgr->GetHisto2(histoNumber+22)->Fill( posX, p_iaea_record->u, weight );

  theAnaMgr->GetHistoProf1(histoNumber+30)->Fill( posX, weight );

}

//---------------------------------------------------------------------
void RTPhaseSpaceHistos::FillHistos( EGSPhspParticle *particle, G4double zstop )
{
  G4int index = 0;
  if( theHistos.find(index) == theHistos.end() ) {
    BookHistos("ALL",index);
    BookHistos("gamma",index);
    BookHistos("e-",index);
    BookHistos("e+",index);
    if( bHadrons ) {
      BookHistos("neutron",index);
      BookHistos("proton",index);
    }
  theHistos.insert(index);
  }

  unsigned int zstopID;
  for( zstopID = 0; zstopID < theZstops.size(); zstopID++ ){
    if( fabs(zstop-theZstops[zstopID]) < 1.E-6 ) break;
  }

  G4int hid = GetHistoID("ALL");
  FillHistos( particle, hid, zstopID, 0 );
  hid = GetHistoID(particle->theCharge);
  FillHistos( particle, hid, zstopID, 0 );
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << " RTPhaseSpaceHistos::FillHistos zstop " << zstop << " zstopID " << zstopID << " particle " << hid << " Y " << particle->thePosY << G4endl;
#endif
}

//---------------------------------------------------------------------
void RTPhaseSpaceHistos::FillHistos( EGSPhspParticle *particle, G4int hid, G4int zstopID, G4int index )
{
  G4double posX = particle->thePosX;
  G4double posY = particle->thePosY;
  G4double posR = sqrt(particle->thePosX*particle->thePosX+particle->thePosY*particle->thePosY)*cm;
  G4double theta = acos(fabs(particle->theDirW))/deg;
  G4double phi = atan2(particle->theDirU,particle->theDirV)/deg;
  //  G4cout << " phi " << phi << " phix " << atan2(particle->x,particle->y)/deg  << G4endl;
  G4double energy = fabs(particle->theEnergy);
  G4double weight = particle->theWeight;
  //  if( weight != 1. ) G4cout << " weight " << G4endl;
  //  G4cout << "  RTPhaseSpaceHistos::FillHistos " << hid << " " << posX << " " << posY << " " << posR << " " << theta << " " << phi << " " << energy << G4endl;

  G4int histoNumber = 760000+10000*zstopID+1000*hid+100*index;
  theAnaMgr->GetHisto1(histoNumber+1)->Fill( posX, weight );
  theAnaMgr->GetHisto1(histoNumber+2)->Fill( posY, weight );

  theAnaMgr->GetHisto1(histoNumber+3)->Fill( posR, weight );
  theAnaMgr->GetHisto1(histoNumber+4)->Fill( theta, weight );
  theAnaMgr->GetHisto1(histoNumber+5)->Fill( phi, weight );
  theAnaMgr->GetHisto1(histoNumber+6)->Fill( energy, weight );
  theAnaMgr->GetHisto1(histoNumber+7)->Fill( particle->theDirU, weight );
  theAnaMgr->GetHisto1(histoNumber+8)->Fill( particle->theDirV, weight );
  theAnaMgr->GetHisto1(histoNumber+9)->Fill( particle->theDirW, weight );
  theAnaMgr->GetHisto2(histoNumber+11)->Fill( posR, theta, weight );
  theAnaMgr->GetHisto2(histoNumber+12)->Fill( posR, energy, weight );
  theAnaMgr->GetHisto2(histoNumber+13)->Fill( theta, energy, weight );
  theAnaMgr->GetHisto2(histoNumber+9)->Fill( posX, posY, weight );

  theAnaMgr->GetHisto1(histoNumber+14)->Fill( weight );
}

//---------------------------------------------------------------------
G4int RTPhaseSpaceHistos::GetHistoID(G4String partName)
{
  std::map<G4String,G4int>::const_iterator ite = theParticleHistoIDs.find(partName);
  if( ite != theParticleHistoIDs.end() ){
    return (*ite).second;
  } else { 
    return -1;
  }
}

//---------------------------------------------------------------------
G4String RTPhaseSpaceHistos::GetHistoName(G4int index)
{
  std::map<G4String,G4int>::const_iterator ite;
  for( ite = theParticleHistoIDs.begin(); ite != theParticleHistoIDs.end(); ite++ ){
    if( (*ite).second == index ) {
      return (*ite).second;
    }
  }

  return "";
}


//---------------------------------------------------------------------
G4int RTPhaseSpaceHistos::GetHistoID(float charge)
{
  G4String partName;
  if( charge == 0. ) {
    partName = "gamma";
  } else if( charge == -1. ) {
    partName = "e-";
  } else if( charge == 1. ) {
    partName = "e+";
  } else {
    G4Exception(" RTPhaseSpaceHistos::GetHistoID",
		"Wrong particle charge",
		FatalErrorInArgument,
		G4String("charge should 0, -1 or 1, while it is " + GmGenUtils::ftoa(charge)).c_str());
  }

  std::map<G4String,G4int>::const_iterator ite = theParticleHistoIDs.find(partName);
  if( ite != theParticleHistoIDs.end() ){
    return (*ite).second;
  } else { 
    return -1;
  }
}
