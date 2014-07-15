#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

#include <math.h>

#include "RTPhaseSpaceUA.hh"
#include "RTPhaseSpaceHistos.hh"
#include "RTVExtraInfoProviderFloat.hh"
#include "RTExtraInfoMgr.hh"

#define RECORD_TO_PRINT 4
#define MAIN
#define Z_OK 0
//extern "C" int def(FILE *source, FILE *dest, int level);
//extern "C" int inf(FILE *source, FILE *dest);
//extern "C" void zerr(int ret);

#include "iaea_header.hh"
#include "iaea_record.hh"
#include "iaea_utilities.hh"
#include "make_zlib.c"

#include "GamosCore/GamosBase/Base/include/GmAnalysisMgr.hh"
#include "GamosCore/GamosUtils/include/GmGenUtils.hh"
#include "RadioTherapy/include/RTVerbosity.hh"
#include "GamosCore/GamosBase/Base/include/GmParameterMgr.hh"
#include "GamosCore/GamosUtils/include/GmNumberOfEvent.hh"

#include "G4GeometryTolerance.hh"

//---------------------------------------------------------------------
RTPhaseSpaceUA::RTPhaseSpaceUA()
{

  GmParameterMgr* paramMgr = GmParameterMgr::GetInstance();
  
  G4String foutName = paramMgr->GetStringValue("RTPhaseSpaceUA:TextFileName","");
  fileOut = 0;
  if( foutName != "" ) fileOut = new std::ofstream(foutName);

  theFileName = paramMgr->GetStringValue("RTPhaseSpaceUA:FileName","test");
  bZStopInFileName = G4bool(paramMgr->GetNumericValue("RTPhaseSpaceUA:ZStopInFileName", 0 ));

  theZStops = paramMgr->GetVNumericValue("RTPhaseSpaceUA:ZStops", std::vector<G4double>()  );
  std::sort(theZStops.begin(),theZStops.end());
  for( unsigned int ii = 0; ii < theZStops.size(); ii++ ){
#ifndef GAMOS_NO_VERBOSE
    if( RTVerb(infoVerb) )  G4cout << " ZSTOP : " << theZStops[ii] << G4endl;
#endif
  }

  //order Z stops for faster searching
  
  bKillAfterLastZStop = G4bool(paramMgr->GetNumericValue("RTPhaseSpaceUA:KillAfterLastZStop", 0 ));

  bStoreZ = G4bool(paramMgr->GetNumericValue("RTPhaseSpaceUA:StoreZ", FALSE ));

  bHistos = G4bool(paramMgr->GetNumericValue("RTPhaseSpaceUA:Histos", TRUE ));

  if( bHistos ) {
    theHistos = new RTPhaseSpaceHistos(this); 
    theHistos->SetClassifier( theClassifier );
  }

  theNEventsToSave = G4int(paramMgr->GetNumericValue("RTPhaseSpaceUA:NEventsToSave", -1 ));
  nrecorded = 0;

  theMaxNTracksInZ = G4int(paramMgr->GetNumericValue("RTPhaseSpaceUA:MaxNTracksInFile", -1 ));

  bNotStoreBackwards = G4bool(paramMgr->GetNumericValue("RTPhaseSpaceUA:NotStoreBackwards", TRUE ));

#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(infoVerb) ) G4cout << " bStoreZ " << bStoreZ << G4endl;
#endif

  theMaxNRecorded = G4int(paramMgr->GetNumericValue("RTPhaseSpaceUA:MaxNTracksRecorded", INT_MAX ));

}


//---------------------------------------------------------------------
G4bool RTPhaseSpaceUA::CreateIAEAheaders()
{
  G4bool isOK = TRUE;
  for( unsigned int ii = 0; ii < theZStops.size(); ii++ ){
    //    G4cout << " call CreateIAEAheader " << ii << G4endl;
    G4bool isOKnew = CreateIAEAheader( theZStops[ii] );
    isOK = isOK && isOKnew;
  }
  return isOK;
}

//---------------------------------------------------------------------
G4bool RTPhaseSpaceUA::CreateIAEAheader(G4double zstop)
{

  //  G4cout << G4endl << " RTPhaseSpaceUA::CreateIAEAheader" << zstop << G4endl;
 
  // Creating IAEA phsp header and allocating memory for it
  iaea_header_type * p_iaea_header = (iaea_header_type *) calloc(1, sizeof(iaea_header_type));
  //---- check that z stops are not repeated
  if( p_iaea_headers.find( zstop ) !=  p_iaea_headers.end() ) {
    G4Exception("RTPhaseSpaceUA::CreateIAEAheader ",
		"Repeated Zstop ",
		FatalErrorInArgument,
		(" Zstp VALUE = " + GmGenUtils::ftoa(zstop)).c_str());
  }
  p_iaea_headers[zstop] = p_iaea_header;

  p_iaea_header->initialize_counters();   
  
  // Creating IAEA record and allocating memory for it
  iaea_record_type * p_iaea_record = (iaea_record_type *) calloc(1, sizeof(iaea_record_type));
  p_iaea_records[zstop] = p_iaea_record;

  // Opening phase space file for output
  char fiaeaphsp[81]={'\0'};;
  
  //strcpy(fiaeaphsp,"test");
  
  G4String fname = theFileName;
  if( theZStops.size() != 1 || bZStopInFileName ) fname += "_" + GmGenUtils::ftoa(zstop);
  strcpy(fiaeaphsp,fname.c_str());
  
  p_iaea_record->p_file = open_file(fiaeaphsp,(char *)(".IAEAphsp"),(char *)("wb"));
  if(p_iaea_record->p_file == NULL) return(FAIL);
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(infoVerb) ) G4cout << " Phase Space record file: " << fiaeaphsp << ".IAEAphsp" << G4endl;
#endif
  
  // Setting default i/o flags 
  if(p_iaea_record->initialize() != OK) return(FAIL);
 
  // Changing default i/o flags 
  p_iaea_record->iextralong = theNExtraInfoLong;
  p_iaea_record->iextrafloat = theNExtraInfoFloat;
  if(bStoreZ) {
    p_iaea_record->iz = 1;          // Z is stored  
  }else {
    p_iaea_record->iz = 0;          // Z is not stored  
  }
  
  p_iaea_record->z = zstop/cm;        // Z constant 
  
  // Store read/write logical block in the PHSP header
  if( p_iaea_header->set_record_contents(p_iaea_record) == FAIL) return FAIL;
  
  //  printf("\n DEFINED PHASE SPACE RECORD LENGTH = %i\n",p_iaea_header->record_length);
  
  return(OK);
}


//---------------------------------------------------------------------
void RTPhaseSpaceUA::UserSteppingAction(const G4Step* aStep)
{ 
  preR= aStep->GetPreStepPoint()->GetPosition();
  R= aStep->GetPostStepPoint()->GetPosition();
   // G4cout << " preR " << preR.z() << " R " << R.z() << " zstop " << theZStop << G4endl;
  //--- Check if it crosses Z plane
  for( unsigned int ii = 0; ii < theZStops.size(); ii++ ){
    double zstop = theZStops[ii];
    //    G4cout << " zstop " << zstop << " "  << R.z() << " " << (preR.z()-zstop) * (R.z()-zstop) << G4endl;
    if ( R.z() < zstop ) break;
    if ( bNotStoreBackwards && aStep->GetTrack()->GetMomentumDirection().z() < 0. ) break;
    if ( (preR.z()-zstop) * (R.z()-zstop) < 0. 
       || fabs(R.z()-zstop) < G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() ){ // track reaches plane 
      
      if( bKillAfterLastZStop && ii == theZStops.size()-1 ) aStep->GetTrack()->SetTrackStatus(fStopAndKill);
#ifndef GAMOS_NO_VERBOSE
      if( RTVerb(debugVerb) ) G4cout << "RTPhaseSpaceUA TRACK to PHASE SPACE  preR " << preR.z() << " R " << R.z() << " zstop " << zstop << G4endl;
#endif
      // const G4Track* track = aStep->GetTrack();
      
      //G4Track* tracknc = const_cast<G4Track*>(track);
      //tracknc->SetTrackStatus(fStopAndKill);
      //      if(  aStep->GetTrack()->GetParentID() == 1 ) {
      StoreTrackRecord( aStep, zstop );
      //      }
    };
  }
}


//---------------------------------------------------------------------
void RTPhaseSpaceUA::StoreTrackRecord( const G4Step* aStep, G4double zstop)
{
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << " StoreTrackRecord at Z " << zstop << G4endl;
#endif
  if(  theMaxNTracksInZ != -1 ) {
    if( theNEventsStored[zstop] > theMaxNTracksInZ ) return;
    theNEventsStored[zstop]++;
  }

  //int pdgCode = aTrack->GetDefinition()->GetPDGEncoding();

  const G4Track* aTrack = aStep->GetTrack();
  //  int pdgCode = track->GetDefinition()->GetPDGEncoding();
  
  int pdgCode = aTrack->GetDefinition()->GetPDGEncoding();
  G4double          E = aStep->GetPostStepPoint() ->GetKineticEnergy()/MeV;
  G4double       preE = aStep->GetPreStepPoint()  ->GetKineticEnergy()/MeV;
  G4double  KineticEnergyMeV = 0.;

  iaea_record_type * p_iaea_record = p_iaea_records[zstop];
  switch( pdgCode ){
  case 22:
    p_iaea_record->particle = 1; // gamma
    KineticEnergyMeV=preE;
    break;
  case 11:
    p_iaea_record->particle = 2; // electron
    KineticEnergyMeV= preE+(E-preE)*(zstop-preR.z())/(R.z()-preR.z());
    break;
  case -11:
    p_iaea_record->particle = 3; // positron
    KineticEnergyMeV=preE+(E-preE)*(zstop-preR.z())/(R.z()-preR.z());
    break;
  case 2112:
    p_iaea_record->particle = 4; // neutron
    KineticEnergyMeV=preE;
    break;
  case 2212:
    p_iaea_record->particle = 5; // proton
    KineticEnergyMeV=preE+(E-preE)*(zstop-preR.z())/(R.z()-preR.z());
    break;
  default:
    G4Exception("RTPhaseSpaceUA::StoreTrackRecord:",
		" particle code not supported ",
		JustWarning,
		G4String("particle: "+GmGenUtils::itoa(pdgCode)).c_str() );
    return;
  }

  //p_iaea_record->energy = aTrack->GetKineticEnergy()/MeV;
  
  p_iaea_record->energy =  KineticEnergyMeV; 

#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << "RTPhaseSpaceUA::StoreTrackRecord particle " << pdgCode << " E= " << KineticEnergyMeV << G4endl;
  if(fileOut) *fileOut << pdgCode << " " << KineticEnergyMeV << " ";
#endif
  
  double x, y;
  x=preR.x()+(R.x()-preR.x())*(zstop-preR.z())/(R.z()-preR.z());
  y=preR.y()+(R.y()-preR.y())*(zstop-preR.z())/(R.z()-preR.z());
    
  //G4ThreeVector pos = track->GetPosition()/cm;
  if(p_iaea_record->ix > 0) p_iaea_record->x = x/cm;
  if(p_iaea_record->iy > 0) p_iaea_record->y = y/cm; 
  if(p_iaea_record->iz > 0) p_iaea_record->z = zstop/cm; 

#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << "RTPhaseSpaceUA::StoreTrackRecord position " << x/cm << " " << y/cm << " " << zstop/cm << G4endl;
  if(fileOut) *fileOut << x/cm << " " << y/cm << " " << zstop/cm << " ";
#endif
 
  G4ThreeVector momdir = aStep->GetPreStepPoint()->GetMomentumDirection(); 
  //   if(p_iaea_record->iu > 0)
  p_iaea_record->u = momdir.x(); 
  //   if(p_iaea_record->iv > 0)
  p_iaea_record->v = momdir.y(); 
  //   if(p_iaea_record->iw > 0)
  //p_iaea_record->w = fabs(momdir.z())/momdir.z();
  //  G4double charge = aTrack->GetDefinition()->GetPDGCharge();
  p_iaea_record->w = momdir.z();
  //  if( charge != 0 ) p_iaea_record->w *= charge/fabs(charge);
  if( p_iaea_record->w != 0 ) p_iaea_record->iw = G4int(p_iaea_record->w/fabs(p_iaea_record->w));

#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << "RTPhaseSpaceUA::StoreTrackRecord direction " <<  momdir.x() << " " << momdir.y() << " " << momdir.z() << " iw " <<  p_iaea_record->iw << G4endl;
  if(fileOut) *fileOut <<  momdir.x() << " " << momdir.y() << " " << momdir.z() << " ";
#endif

  //   if(p_iaea_record->iweight > 0)
  p_iaea_record->weight = aStep->GetPreStepPoint()->GetWeight();

#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << "RTPhaseSpaceUA::StoreTrackRecord weight " <<   p_iaea_record->weight << G4endl;
  if(fileOut) *fileOut << p_iaea_record->weight << " ";
#endif

  if( bFirstTrackInEvent.find(zstop) == bFirstTrackInEvent.end() ) {
    p_iaea_record->IsNewHistory = 1;
    bFirstTrackInEvent.insert(zstop);
  } else {
    p_iaea_record->IsNewHistory = 0;
  }

  p_iaea_record->iextralong = theNExtraInfoLong;
  p_iaea_record->iextrafloat = theNExtraInfoFloat;

  for( G4int ii = 0; ii < theNExtraInfoLong; ii++ ){
    p_iaea_record->extralong[ii] = theExtraInfoLongs[ii]->to_ulong();
#ifndef GAMOS_NO_VERBOSE
    if( RTVerb(debugVerb) ) G4cout << "RTPhaseSpaceUA::StoreTrackRecord ExtraInfoLong " << ii << " = " << p_iaea_record->extralong[ii]  << G4endl;
    if(fileOut) *fileOut << " " << p_iaea_record->extralong[ii] << " ";
#endif
  } 

  for( G4int ii = 0; ii < theNExtraInfoFloat; ii++ ){
    // recheck ZLast 
    std::vector<RTVExtraInfoProviderFloat*> eiFloat = RTExtraInfoMgr::GetInstance()->GetProvidersFloat();
    std::vector<RTVExtraInfoProviderFloat*>::const_iterator iteei;
    for( iteei = eiFloat.begin(); iteei != eiFloat.end(); iteei++ ){
      (*iteei)->CheckZStop(zstop);
    }
    p_iaea_record->extrafloat[ii] = *(theExtraInfoFloats[ii]);
#ifndef GAMOS_NO_VERBOSE
    if( RTVerb(debugVerb) ) G4cout << "RTPhaseSpaceUA::StoreTrackRecord ExtraInfoFloat " << ii << " = " << p_iaea_record->extrafloat[ii]  << G4endl;
    if(fileOut) *fileOut << p_iaea_record->extrafloat[ii] << " ";
#endif
  } 

  if(fileOut) *fileOut << G4endl;
  p_iaea_record->write_particle();

  // update counters
  iaea_header_type * p_iaea_header = p_iaea_headers[zstop];
  p_iaea_header->update_counters(p_iaea_record);

  nrecorded++;

  if( nrecorded >= theMaxNRecorded ) {
    G4cout << " RTPhaseSpaceUA: maximum number of tracks recorded = " << nrecorded << " , stop run. " << G4endl;
  }

  if( bHistos ) theHistos->FillHistos( p_iaea_record, zstop, aStep );
}

#include "RTExtraInfoMgr.hh"
#include "RTVExtraInfoProvider.hh"

//---------------------------------------------------------------------
void RTPhaseSpaceUA::BeginOfRunAction(const G4Run*)
{
  RTExtraInfoMgr * theExtraInfoMgr = RTExtraInfoMgr::GetInstance();
  std::vector<bs32*> eiwordsL = theExtraInfoMgr->GetWordsLong();
  theNExtraInfoLong = 0;
  for( unsigned int jj = 0; jj < eiwordsL.size();jj++ ){
    theExtraInfoLongs.push_back(eiwordsL[jj]);
    theNExtraInfoLong++;
  }

  std::vector<float*> eiwordsF = theExtraInfoMgr->GetWordsFloat();
  theNExtraInfoFloat = 0;
  for( unsigned int jj = 0; jj < eiwordsF.size();jj++ ){
    theExtraInfoFloats.push_back(eiwordsF[jj]);
    theNExtraInfoFloat++;
  }

  CreateIAEAheaders();

}

//---------------------------------------------------------------------
void RTPhaseSpaceUA::BeginOfEventAction(const G4Event*)
{
  bFirstTrackInEvent.clear();
}

//---------------------------------------------------------------------
void RTPhaseSpaceUA::EndOfEventAction(const G4Event* evt)
{
  // ---- loop to stored particles to get rid of particles passing twice by plane ??
  if( theNEventsToSave > 0 && (evt->GetEventID()+1)%theNEventsToSave == 0 
      && evt->GetEventID()+1 != G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed() ) {
    G4int nevents = G4int(GmNumberOfEvent::GetNumberOfEvent() + 1); // at the EndOfEvent the number of events in run is not updated
    CloseIAEAHeaderFiles( nevents );
  } 
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout <<" RTPhaseSpaceUA::EndOfEventAction " << theNEventsToSave << " " << (evt->GetEventID()+1)%theNEventsToSave << " " << evt->GetEventID() << "  =? " <<  GmNumberOfEvent::GetNumberOfEvent() <<G4endl; 
#endif
}


//---------------------------------------------------------------------
void  RTPhaseSpaceUA::EndOfRunAction(const G4Run*)
{
#ifndef GAMOS_NO_VERBOSE
    if( RTVerb(debugVerb) ) G4cout << " RTPhaseSpaceUA::EndOfRunAction " << GmNumberOfEvent::GetNumberOfEvent() <<G4endl; 
#endif

  CloseIAEAFiles();

  RTExtraInfoMgr::GetInstance()->DumpProviders();

}

//---------------------------------------------------------------------
int RTPhaseSpaceUA::CloseIAEAFiles()
{
  G4int nevents = G4int(GmNumberOfEvent::GetNumberOfEvent());
  CloseIAEAHeaderFiles(nevents);
  CloseIAEARecordFiles();
  return 0;
}


//---------------------------------------------------------------------
int RTPhaseSpaceUA::CloseIAEAHeaderFiles(G4int nevents)
{

  for( unsigned int ii = 0; ii < theZStops.size(); ii++ ){
    G4double zstop = theZStops[ii];
    // Counters for phsp file
    
    iaea_header_type * p_iaea_header = p_iaea_headers[zstop];
    
    p_iaea_header->orig_histories = nevents;

#ifndef GAMOS_NO_VERBOSE
    if( RTVerb(infoVerb) ) G4cout << "RTPhaseSpaceUA::CloseIAEAFiles() Number of Events: " << nevents << "  Number of records: " << p_iaea_header->nParticles << G4endl;
#endif

    //    printf("\n Number of particles written in all Zstops %d \n",nrecorded);
    //    printf(  " Multiple passers discarded %d \n",ndiscarded);
    
    strcpy(p_iaea_header->title,
	   " TEST PHASESPACE for IAEA format (random numbers) ");   
    
    p_iaea_header->iaea_index = 0;    // Assigning IAEA index to the phasespace
    
    // Saving IAEA header file 
    //p_iaea_header->fheader = open_file("test",".IAEAheader","wb");
    
    char L_fname[120];
    G4String fname = theFileName;
    if( theZStops.size() != 1 ) fname += "_" + GmGenUtils::ftoa(zstop);
    strcpy(L_fname,fname.c_str());

    p_iaea_header->fheader = open_file(L_fname,(char *)(".IAEAheader"),(char *)("wb"));
    
    /* Write an IAEA header */
    if( p_iaea_header->write_header() != OK)
      { printf("\n ERROR: writing IAEA header"); return(FAIL); }  
#ifndef GAMOS_NO_VERBOSE
    if( RTVerb(infoVerb) ) G4cout << " Phase Space header file: " << L_fname << ".IAEAheader" << G4endl;
#endif

    fclose(p_iaea_header->fheader);
  }
  
   return(OK);
}

//---------------------------------------------------------------------
int RTPhaseSpaceUA::CloseIAEARecordFiles()
{
  for( unsigned int ii = 0; ii < theZStops.size(); ii++ ){
    G4double zstop = theZStops[ii];
    
    iaea_record_type * p_iaea_record = p_iaea_records[zstop];
    fclose(p_iaea_record->p_file);
  }
  
  return (OK);
}


//---------------------------------------------------------------------
int  RTPhaseSpaceUA::CompressIAEAFile( char* fiaeaphsp )
{
  for( unsigned int ii = 0; ii < theZStops.size(); ii++ ){
    G4double zstop = theZStops[ii];
   // ===============================================================
   // Compresssing IAEA generated phsp file
    printf(" Compressing phsp file (%s.IAEAphsp -> %s.IAEAzlib) ...\n",
	   fiaeaphsp,fiaeaphsp);
    FILE *p_rfile = open_file(fiaeaphsp,(char *)(".IAEAphsp"),(char *)("rb"));
    FILE *p_wfile = open_file(fiaeaphsp,(char *)(".IAEAzlib"),(char *)("wb"));
    // Deflating source PHSP file with MAX COMPRESSION = 9
    int ret = def(p_rfile, p_wfile, 9);
    if( ret != Z_OK) {zerr(ret);return(FAIL);} 
    fclose(p_rfile); fclose(p_wfile);
    // ===============================================================
    // Inflating IAEA compressederated phsp file
    printf(" Decompressing ZLIB phsp file (%s.IAEAzlib -> uncompress.IAEAphsp)...\n",
	   fiaeaphsp);
    
    // Decompressing just created IAEA phsp 
    p_rfile = open_file(fiaeaphsp,(char *)(".IAEAzlib"),(char *)("rb"));
    p_wfile = open_file((char *)("uncompress"),(char *)(".IAEAphsp"),(char *)("wb"));
    
    ret = inf(p_rfile, p_wfile); // Inflating compressed file
    if( ret != Z_OK) {zerr(ret);return(FAIL);} 
    fclose(p_rfile); fclose(p_wfile);
    printf(" Decompressing finished, reading phsp file <uncompress.IAEAphsp>\n");
    printf(" Writing verification phsp file <verif.IAEAphsp>\n");
    printf(" All files with extension <IAEAphsp> must be identical\n\n");
    
    /* Opening existing IAEA header file */
    iaea_header_type *p_iaea_headerR;
    p_iaea_headerR = (iaea_header_type *) calloc(1, sizeof(iaea_header_type));
    //p_iaea_headerR->fheader = open_file("test",".IAEAheader","rb");
    
    char  L_fname[120];
    G4String fname = theFileName;
    if( theZStops.size() != 1 ) fname += "_" + GmGenUtils::ftoa(zstop);

    strcpy(L_fname,fname.c_str());
    
    p_iaea_headerR->fheader = open_file(L_fname,(char *)(".IAEAheader.z"),(char *)("rb"));
    printf("\n==================================================\n");
    printf(  "        READING IAEA phasespace header            \n");
    printf(  "==================================================\n");
    // Getting read/write logical block from header
    if( p_iaea_headerR->read_header() != OK) 
      { printf("\n ERROR: reading IAEA header"); return(FAIL);}
    if( p_iaea_headerR->print_header() != OK) return(FAIL);
    fclose(p_iaea_headerR->fheader); 
    
    //=========================================================================
    // Test block to read and write from the just created PHSP in IAEA format
    //
    
    // Creating IAEA record and allocating memory for it
    iaea_record_type *p_iaea_check;
    p_iaea_check = (iaea_record_type *) calloc(1, sizeof(iaea_record_type));
    // Get read/write logical block from the header
    if( p_iaea_headerR->get_record_contents(p_iaea_check) == FAIL) return FAIL;
    
    p_rfile = open_file((char *)("uncompress"),(char *)(".IAEAphsp"),(char *)("rb"));
    p_wfile = open_file((char *)("verif"),(char *)(".IAEAphsp"),(char *)("wb"));
    // Reading phasespace in IAEA format   
    for(long jrecord=0; jrecord<p_iaea_headerR->nParticles; jrecord++)
      {
	/* Read */
	p_iaea_check-> p_file = p_rfile;
	if( p_iaea_check->read_particle() == FAIL) break;
	
	if(jrecord<RECORD_TO_PRINT)
	  {
	    int iaea_charge[MAX_NUM_PARTICLES]={0,-1,+1,0,+1},j ;
	    int charge = iaea_charge[p_iaea_check->particle - 1];
	    printf("\n # %ld particle with a record length %d read (New History: %d)",
		   jrecord+1, p_iaea_headerR->record_length,p_iaea_check->IsNewHistory);
	    printf("\n Q %d E %f X %f Y %f Z %f \n\t u %f v %f w %f W %f Part %d \n",
		   charge, p_iaea_check->energy, 
		   p_iaea_check->x, 
		   p_iaea_check->y, 
		   p_iaea_check->z, 
		   p_iaea_check->u, 
		   p_iaea_check->v, 
		   p_iaea_check->w, 
		   p_iaea_check->weight, 
		   p_iaea_check->particle);
	    if( p_iaea_check->iextrafloat > 0) printf(" EXTRA FLOATs:"); 
	    for(j=0;j<p_iaea_check->iextrafloat;j++) 
	      printf(" F%i %f",j+1,p_iaea_check->extrafloat[j]);
	    if( p_iaea_check->iextralong > 0)  printf(" EXTRA LONGs:"); 
	    for(j=0;j<p_iaea_check->iextralong;j++)  
	      printf(" L%i %ld",j+1,p_iaea_check->extralong[j]);
	    printf("\n"); 
	  }
	
	/* Write */
	p_iaea_check-> p_file = p_wfile;
	if( p_iaea_check->write_particle() == FAIL) {
	  printf("\n ERROR: writing IAEA particle"); break; }
      }
    
    free(p_iaea_headerR);
    fclose(p_rfile);
    fclose(p_wfile);
    free(p_iaea_check);   
  }

  //=========================================================================
  
  // *************************************************************
  printf("\n\n Normal Program Termination\n");
  
  return(OK);
}


RTPhaseSpaceUA::~RTPhaseSpaceUA()
{
  for( unsigned int ii = 0; ii < theZStops.size(); ii++ ){
    double zstop = theZStops[ii];
    free (p_iaea_headers[zstop]);

    iaea_record_type * p_iaea_record = p_iaea_records[zstop];
    free(p_iaea_record); 
  }
}
