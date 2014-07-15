#include "RTPSPDoseHistos.hh"
#include "RTVerbosity.hh"

#include "G4PhantomParameterisation.hh"
#include "GamosCore/GamosReadDICOM/include/GmRegularParamUtils.hh"
//#include "ReadPhantom/include/GmRegularParamUtils.hh"
#include "GamosCore/GamosBase/Base/include/GmParameterMgr.hh"
#include "GamosCore/GamosScoring/Management/include/GmVPrimitiveScorer.hh"
#include "GamosCore/GamosUtils/include/GmNumberOfEvent.hh"
#include "G4tgrFileIn.hh"
#include "GamosCore/GamosUtils/include/GmGenUtils.hh"
#include "GamosCore/GamosReadDICOM/include/GmSqdoseHeader.hh"
#include "G4UIcommand.hh"

//------------------------------------------------------------------
RTPSPDoseHistos::RTPSPDoseHistos(G4String name, GmSqdoseHeader* doseh) :
  GmVPSPrinter( name )
{
  G4String fileName = GmParameterMgr::GetInstance()->GetStringValue(theName+":FileName","");
  if( fileName == "" ) fileName = "dose_"+name;
  theAnaMgr = GmAnalysisMgr::GetInstance( fileName );

  theDoseHeader = doseh;
  BookHistos();

  theNHistos = 0;

}


//------------------------------------------------------------------
void RTPSPDoseHistos::BookHistos()
{
  
  G4RotationMatrix phantomRot;
  if( !theDoseHeader ){
    theRegParam = GmRegularParamUtils::GetInstance()->GetPhantomParam();
    theNVoxelX = G4int(theRegParam->GetNoVoxelX());
    theNVoxelY = G4int(theRegParam->GetNoVoxelY());
    theNVoxelZ = G4int(theRegParam->GetNoVoxelZ());
    theDimX = theRegParam->GetVoxelHalfX();
    theDimY = theRegParam->GetVoxelHalfY();
    theDimZ = theRegParam->GetVoxelHalfZ();
    thePhantomTranslation = GmRegularParamUtils::GetInstance()->GetPhantomMotherTranslation();
    phantomRot = GmRegularParamUtils::GetInstance()->GetPhantomMotherRotation( true );
  } else {
    theNVoxelX = G4int(theDoseHeader->GetNoVoxelX());
    theNVoxelY = G4int(theDoseHeader->GetNoVoxelY());
    theNVoxelZ = G4int(theDoseHeader->GetNoVoxelZ());
    theDimX = theDoseHeader->GetVoxelHalfX();
    theDimY = theDoseHeader->GetVoxelHalfY();
    theDimZ = theDoseHeader->GetVoxelHalfZ();
    thePhantomTranslation = theDoseHeader->GetTranslation();
    phantomRot = theDoseHeader->GetRotation();
  }
  thePhantomTranslation = phantomRot * thePhantomTranslation;

#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(warningVerb) ) G4cout << RTVerb(warningVerb) << " RTPSPDoseHistos nvoxel " << theNVoxelX << " " << theNVoxelY << " " << theNVoxelZ << G4endl;
#endif
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(warningVerb) ) G4cout << " RTPSPDoseHistos half dim " << theDimX << " " << theDimY << " " << theDimZ << G4endl;
#endif
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(warningVerb) ) G4cout << " RTPSPDoseHistos translation " <<  thePhantomTranslation << G4endl;
#endif
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(warningVerb) ) G4cout << " RTPSPDoseHistos rotation " << phantomRot << G4endl;
#endif

  double doseMin = -17;
  double doseMax = -12;
  //--- define dose - volume histo
  //  theAnaMgr->CreateHisto2D("RTPSPDoseHistos: Dose-volume",200, int(log10(minDose)),int(log10(maxDose))+1.,36021);
  theAnaMgr->CreateHisto1D("RTPSPDoseHistos: Dose",100,doseMin,doseMax,36201);
  theAnaMgr->CreateHisto1D("RTPSPDoseHistos: Dose-volume",100,doseMin,doseMax,36202);
    
}

//------------------------------------------------------------------
void RTPSPDoseHistos::CheckHistoLimits( G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax )
{
  if( nXmin < 0 ) {
    G4Exception("RTPSPDoseHistos::FillHisto",
		"X min too small",
		JustWarning,
		G4String(G4UIcommand::ConvertToString(nXmin)+" < "
			 +G4UIcommand::ConvertToString(0)).c_str());
    nXmin = 0;
  }
  if( nXmax >= theNVoxelX ) {
    G4Exception("RTPSPDoseHistos::FillHisto",
		"X max too big",
		JustWarning,
		G4String(G4UIcommand::ConvertToString(nXmax)+" >= "
			 +G4UIcommand::ConvertToString(theNVoxelX)).c_str());
    nXmax = theNVoxelX-1;
  }
  if( nYmin < 0 ) {
    G4Exception("RTPSPDoseHistos::FillHisto",
		"Y min too small",
		JustWarning,
		G4String(G4UIcommand::ConvertToString(nYmin)+" < "
			 +G4UIcommand::ConvertToString(0)).c_str());
    nYmin = 0;
  }
  if( nYmax >= theNVoxelY ) {
    G4Exception("RTPSPDoseHistos::FillHisto",
		"Y max too big",
		JustWarning,
		G4String(G4UIcommand::ConvertToString(nYmax)+" >= "
			 +G4UIcommand::ConvertToString(theNVoxelY)).c_str());
    nYmax = theNVoxelY-1;
  }
  if( nZmin < 0 ) {
    G4Exception("RTPSPDoseHistos::FillHisto",
		"Z min too small",
		JustWarning,
		G4String(G4UIcommand::ConvertToString(nZmin)+" < "
			 +G4UIcommand::ConvertToString(0)).c_str());
    nZmin = 0;
  }
  if( nZmax >= theNVoxelZ ) {
    G4Exception("RTPSPDoseHistos::FillHisto",
		"Z max too big",
		JustWarning,
		G4String(G4UIcommand::ConvertToString(nZmax)+" >= "
			 +G4UIcommand::ConvertToString(theNVoxelZ)).c_str());
    nZmax = theNVoxelZ-1;
  }
}


//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto( std::vector<G4String>& wl )
{
  if( wl[1] == "Profile1N_X") {
    FillHisto1N_X(wl);
  }else if( wl[1] == "Profile1N_Y") {
    FillHisto1N_Y(wl);
  }else if( wl[1] == "Profile1N_Z") {
    FillHisto1N_Z(wl);
  }else if( wl[1] == "Profile2N_XY") {
    FillHisto2N_XY(wl);
  }else if( wl[1] == "Profile2N_XZ") {
    FillHisto2N_XZ(wl);
  }else if( wl[1] == "Profile2N_YZ") {
    FillHisto2N_YZ(wl);
  }    

}


//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto1N_X( std::vector<G4String>& wl  )
{
  G4int nXmin = G4UIcommand::ConvertToInt(wl[2]);
  G4int nXmax = G4UIcommand::ConvertToInt(wl[3]);
  G4int nYmin = G4UIcommand::ConvertToInt(wl[4]);
  G4int nYmax = G4UIcommand::ConvertToInt(wl[5]);
  G4int nZmin = G4UIcommand::ConvertToInt(wl[6]);
  G4int nZmax = G4UIcommand::ConvertToInt(wl[7]);
  G4String hisName = wl[0];

  CheckHistoLimits( nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );
  FillHisto1N_X( hisName, nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );

}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto1N_Y( std::vector<G4String>& wl  )
{
  G4int nXmin = G4UIcommand::ConvertToInt(wl[4]);
  G4int nXmax = G4UIcommand::ConvertToInt(wl[5]);
  G4int nYmin = G4UIcommand::ConvertToInt(wl[2]);
  G4int nYmax = G4UIcommand::ConvertToInt(wl[3]);
  G4int nZmin = G4UIcommand::ConvertToInt(wl[6]);
  G4int nZmax = G4UIcommand::ConvertToInt(wl[7]);
  G4String hisName = wl[0];

  CheckHistoLimits( nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );
  FillHisto1N_Y( hisName, nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );

}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto1N_Z( std::vector<G4String>& wl  )
{
  G4int nXmin = G4UIcommand::ConvertToInt(wl[4]);
  G4int nXmax = G4UIcommand::ConvertToInt(wl[5]);
  G4int nYmin = G4UIcommand::ConvertToInt(wl[6]);
  G4int nYmax = G4UIcommand::ConvertToInt(wl[7]);
  G4int nZmin = G4UIcommand::ConvertToInt(wl[2]);
  G4int nZmax = G4UIcommand::ConvertToInt(wl[3]);
  G4String hisName = wl[0];

  CheckHistoLimits( nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );
  FillHisto1N_Z( hisName, nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );

}

 
//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto1N_X( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax )
{ 
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << " FillHisto1N_X " << hisName 
	 << " nXmin " << nXmin << " nXmax " << nXmax 
	 << " nYmin " << nYmin << " nYmax " << nYmax 
	 << " nZmin " << nZmin << " nZmax " << nZmax 
	 << G4endl;
#endif

  theNHistos++;
  G4int ih = 66300+theNHistos;
  theAnaMgr->CreateHisto1D(hisName.c_str(),nXmax-nXmin+1,ConvertN2DimX(nXmin)-theDimX,ConvertN2DimX(nXmax)+theDimX,ih);

  GmHisto1* his = theAnaMgr->GetHisto1( ih );

  std::map<G4int,G4double*>::const_iterator ite;
  for( ite = theMap.begin(); ite != theMap.end(); ite++ ) {
    G4int copyNo = (*ite).first;
    G4int ix = copyNo%theNVoxelX;
    if( ix < nXmin || ix > nXmax ) continue;
    G4int iy = (copyNo/theNVoxelX)%theNVoxelY;
    if( iy < nYmin || iy > nYmax ) continue;
    G4int iz = copyNo/theNVoxelX/theNVoxelY;
    if( iz < nZmin || iz > nZmax ) continue;
    G4double hx = ConvertN2DimX(ix);
    G4double dose = (*(*ite).second)*theNevInv*theUnitInv;
    G4double doseError = theScorer->GetError( copyNo )*theUnitInv;
    //    G4cout << " FILLX " << copyNo << " " << ix << " " << iy << " " << iz << " dose " << dose << " dose " << dose << " +- " << sqrt(doseError) << G4endl;
    G4double hError = his->GetBinError(ix+1);
    his->Fill( hx, dose );
    //    G4cout << dose << " " << his->GetName() << " =?= " << his->GetBinContent(ix+1) << " HXERROR " << ix << " " << hError << " =?= " << his->GetBinError(ix+1) << " " << doseError << G4endl;
    his->SetBinError(ix+1, sqrt(sqr(hError)+sqr(doseError)));
  }
  
}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto1N_Y( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax )
{ 
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << " FillHisto1N_Y " << hisName 
	 << " nXmin " << nXmin << " nXmax " << nXmax 
	 << " nYmin " << nYmin << " nYmax " << nYmax 
	 << " nZmin " << nZmin << " nZmax " << nZmax 
	 << G4endl;
#endif

  theNHistos++;
  G4int ih = 66300+theNHistos;
  theAnaMgr->CreateHisto1D(hisName.c_str(),nYmax-nYmin+1,ConvertN2DimY(nYmin)-theDimY,ConvertN2DimY(nYmax)+theDimY,ih);

  GmHisto1* his = theAnaMgr->GetHisto1( ih );

  std::map<G4int,G4double*>::const_iterator ite;
  for( ite = theMap.begin(); ite != theMap.end(); ite++ ) {
    G4int copyNo = (*ite).first;
    G4int ix = copyNo%theNVoxelX;
    if( ix < nXmin || ix > nXmax ) continue;
    G4int iy = (copyNo/theNVoxelX)%theNVoxelY;
    if( iy < nYmin || iy > nYmax ) continue;
    G4int iz = copyNo/theNVoxelX/theNVoxelY;
    if( iz < nZmin || iz > nZmax ) continue;
    G4double hy = ConvertN2DimY(iy);
    G4double dose = (*(*ite).second)*theNevInv*theUnitInv;
    G4double doseError = theScorer->GetError( copyNo )*theUnitInv;
    G4double hError = his->GetBinError(iy);
    his->Fill( hy, dose );
    //    G4cout << " HY " << hy << " " << dose << G4endl;
    his->SetBinError(iy+1, sqrt(sqr(hError)+sqr(doseError)));
  }
  
}


//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto1N_Z( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax )
{ 
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(debugVerb) ) G4cout << " FillHisto1N_Z " << hisName 
	 << " nXmin " << nXmin << " nXmax " << nXmax 
	 << " nYmin " << nYmin << " nYmax " << nYmax 
	 << " nZmin " << nZmin << " nZmax " << nZmax 
	 << G4endl;
#endif

  theNHistos++;
  G4int ih = 66300+theNHistos;
  theAnaMgr->CreateHisto1D(hisName.c_str(),nZmax-nZmin+1,ConvertN2DimZ(nZmin)-theDimZ,ConvertN2DimZ(nZmax)+theDimZ,ih);

  GmHisto1* his = theAnaMgr->GetHisto1( ih );

  std::map<G4int,G4double*>::const_iterator ite;
  for( ite = theMap.begin(); ite != theMap.end(); ite++ ) {
    G4int copyNo = (*ite).first;
    G4int ix = copyNo%theNVoxelX;
    if( ix < nXmin || ix > nXmax ) continue;
    G4int iy = (copyNo/theNVoxelX)%theNVoxelY;
    if( iy < nYmin || iy > nYmax ) continue;
    G4int iz = copyNo/theNVoxelX/theNVoxelY;
    if( iz < nZmin || iz > nZmax ) continue;
    G4double dose = (*(*ite).second)*theNevInv*theUnitInv;
    G4double hz = ConvertN2DimZ(iz);
    G4double doseError = theScorer->GetError( copyNo )*theUnitInv;
    G4double hError = his->GetBinError(iz);
    his->Fill( hz, dose );
    //    his->SetBinError(iz, sqrt(doseError) );
    his->SetBinError(iz+1, sqrt(sqr(hError)+sqr(doseError)) );
    //    G4cout << " HZ " << hz << " " << dose << " +- " << doseError << " oldError " << hError << G4endl;
  }
  
}


//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto2N_XY( std::vector<G4String>& wl  )
{
  G4int nXmin = G4UIcommand::ConvertToInt(wl[2]);
  G4int nXmax = G4UIcommand::ConvertToInt(wl[3]);
  G4int nYmin = G4UIcommand::ConvertToInt(wl[4]);
  G4int nYmax = G4UIcommand::ConvertToInt(wl[5]);
  G4int nZmin = G4UIcommand::ConvertToInt(wl[6]);
  G4int nZmax = G4UIcommand::ConvertToInt(wl[7]);
  G4String hisName = wl[0];

  CheckHistoLimits( nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );
  FillHisto2N_XY( hisName, nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );

}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto2N_XZ( std::vector<G4String>& wl  )
{
  G4int nXmin = G4UIcommand::ConvertToInt(wl[2]);
  G4int nXmax = G4UIcommand::ConvertToInt(wl[3]);
  G4int nYmin = G4UIcommand::ConvertToInt(wl[6]);
  G4int nYmax = G4UIcommand::ConvertToInt(wl[7]);
  G4int nZmin = G4UIcommand::ConvertToInt(wl[4]);
  G4int nZmax = G4UIcommand::ConvertToInt(wl[5]);
  G4String hisName = wl[0];

  CheckHistoLimits( nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );
  FillHisto2N_XZ( hisName, nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );

}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto2N_YZ( std::vector<G4String>& wl  )
{
  G4int nXmin = G4UIcommand::ConvertToInt(wl[6]);
  G4int nXmax = G4UIcommand::ConvertToInt(wl[7]);
  G4int nYmin = G4UIcommand::ConvertToInt(wl[2]);
  G4int nYmax = G4UIcommand::ConvertToInt(wl[3]);
  G4int nZmin = G4UIcommand::ConvertToInt(wl[4]);
  G4int nZmax = G4UIcommand::ConvertToInt(wl[5]);
  G4String hisName = wl[0];

  CheckHistoLimits( nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );
  FillHisto2N_YZ( hisName, nXmin, nXmax, nYmin, nYmax, nZmin, nZmax );

}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto2N_XY( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax )
{ 
  theNHistos++;
  G4int ih = 66300+theNHistos;
  theAnaMgr->CreateHisto2D(hisName.c_str(),
			   nXmax-nXmin+1,ConvertN2DimX(nXmin)-theDimX,ConvertN2DimX(nXmax)+theDimX,
			   nYmax-nYmin+1,ConvertN2DimY(nYmin)-theDimY,ConvertN2DimY(nYmax)+theDimY,ih);
  GmHisto2* his = theAnaMgr->GetHisto2( ih );

  std::map<G4int,G4double*>::const_iterator ite;
  for( ite = theMap.begin(); ite != theMap.end(); ite++ ) {
    G4int copyNo = (*ite).first;
    G4int ix = copyNo%theNVoxelX;
    if( ix < nXmin || ix > nXmax ) continue;
    G4int iy = (copyNo/theNVoxelX)%theNVoxelY;
    if( iy < nYmin || iy > nYmax ) continue;
    G4int iz = copyNo/theNVoxelX/theNVoxelY;
    if( iz < nZmin || iz > nZmax ) continue;
    G4double hx = ConvertN2DimX(ix);
    G4double hy = ConvertN2DimY(iy);
    G4double dose = (*(*ite).second)*theNevInv*theUnitInv;
    G4double doseError = theScorer->GetError( copyNo )*theUnitInv;
    G4double hError = his->GetBinError(ix+1,iy+1);
    his->Fill( hx, hy, dose);
    his->SetBinError(ix+1, iy+1, sqrt(sqr(hError)+sqr(doseError)));
    //    G4cout << " HX " << hx << " " << dose << " i " << ix << " hx G4endl;
    //    his->SetBinError(iz, sqrt(doseError) );
  }
  
}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto2N_XZ( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax )
{ 
  theNHistos++;
  G4int ih = 66300+theNHistos;
  theAnaMgr->CreateHisto2D(hisName.c_str(),
			   nXmax-nXmin+1,ConvertN2DimX(nXmin)-theDimX,ConvertN2DimX(nXmax)+theDimX,
			   nZmax-nZmin+1,ConvertN2DimZ(nZmin)-theDimZ,ConvertN2DimZ(nZmax)+theDimZ,ih);
  GmHisto2* his = theAnaMgr->GetHisto2( ih );

  std::map<G4int,G4double*>::const_iterator ite;
  for( ite = theMap.begin(); ite != theMap.end(); ite++ ) {
    G4int copyNo = (*ite).first;
    G4int ix = copyNo%theNVoxelX;
    if( ix < nXmin || ix > nXmax ) continue;
    G4int iy = (copyNo/theNVoxelX)%theNVoxelY;
    if( iy < nYmin || iy > nYmax ) continue;
    G4int iz = copyNo/theNVoxelX/theNVoxelY;
    if( iz < nZmin || iz > nZmax ) continue;
    G4double hx = ConvertN2DimX(ix);
    G4double hz = ConvertN2DimZ(iz);
    G4double dose = (*(*ite).second)*theNevInv*theUnitInv;
    G4double doseError = theScorer->GetError( copyNo )*theUnitInv;
    G4double hError = his->GetBinError(ix+1,iz+1);
    his->Fill( hx, hz, dose);
    his->SetBinError(ix+1, iz+1, sqrt(sqr(hError)+sqr(doseError)));
    //    G4cout << " HX " << hx << " " << dose << G4endl;
    //    his->SetBinError(iz, sqrt(doseError) );
  }
  
}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto2N_YZ( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax )
{ 
  theNHistos++;
  G4int ih = 66300+theNHistos;
  theAnaMgr->CreateHisto2D(hisName.c_str(),
			   nYmax-nYmin+1,ConvertN2DimY(nYmin)-theDimY,ConvertN2DimY(nYmax)+theDimY,
			   nZmax-nZmin+1,ConvertN2DimZ(nZmin)-theDimZ,ConvertN2DimZ(nZmax)+theDimZ,ih);
  GmHisto2* his = theAnaMgr->GetHisto2( ih );

  std::map<G4int,G4double*>::const_iterator ite;
  for( ite = theMap.begin(); ite != theMap.end(); ite++ ) {
    G4int copyNo = (*ite).first;
    G4int ix = copyNo%theNVoxelX;
    if( ix < nXmin || ix > nXmax ) continue;
    G4int iy = (copyNo/theNVoxelX)%theNVoxelY;
    if( iy < nYmin || iy > nYmax ) continue;
    G4int iz = copyNo/theNVoxelX/theNVoxelY;
    if( iz < nZmin || iz > nZmax ) continue;
    G4double hy = ConvertN2DimY(iy);
    G4double hz = ConvertN2DimZ(iz);
    G4double dose = (*(*ite).second)*theNevInv*theUnitInv;
    G4double doseError = theScorer->GetError( copyNo )*theUnitInv;
    G4double hError = his->GetBinError(iy+1,iz+1);
    his->Fill( hy, hz, dose);
    his->SetBinError(iy+1, iz+1, sqrt(sqr(hError)+sqr(doseError)));
    //    G4cout << " HX " << hx << " " << dose << G4endl;
    //    his->SetBinError(iz, sqrt(doseError) );
  }
  
}


//------------------------------------------------------------------
G4double RTPSPDoseHistos::ConvertN2DimX(G4int nX)
{
  //  G4cout << "  ConvertN2DimX " << nX << " -> " << (-theNVoxelX+2*nX+1)*theDimX + thePhantomTranslation.x() << " nVoxelX " << theNVoxelX << " dimX " << theDimX << G4endl;
  return (-theNVoxelX+2*nX+1)*theDimX + thePhantomTranslation.x();
}

//------------------------------------------------------------------
G4double RTPSPDoseHistos::ConvertN2DimY(G4int nY)
{
  //G4cout << "  ConvertN2DimY " << nY << " -> " << (-theNVoxelY+2*nY+1)*theDimY + thePhantomTranslation.y() << G4endl;

  return (-theNVoxelY+2*nY+1)*theDimY + thePhantomTranslation.y();
}

//------------------------------------------------------------------
G4double RTPSPDoseHistos::ConvertN2DimZ(G4int nZ)
{
  //G4cout << "  ConvertN2DimZ " << nZ << " -> " << (-theNVoxelZ+2*nZ+1)*theDimZ + thePhantomTranslation.z() << G4endl;
  return (-theNVoxelZ+2*nZ+1)*theDimZ + thePhantomTranslation.z();
}


//------------------------------------------------------------------
void RTPSPDoseHistos::DumpAll( G4THitsMap<G4double>* RunMap, GmVPrimitiveScorer* scorer )
{
  theScorer = scorer;
  theMap = *(RunMap->GetMap());

  //----- Get minimum and maximum dose
  G4double minDose = 1.E99;
  G4double maxDose = 0.;
  theUnitInv = 1./scorer->GetUnit();
  theNevInv = 1./GmNumberOfEvent::GetNumberOfEvent();
  std::map<G4int,G4double*>::iterator ite;
  for(ite = RunMap->GetMap()->begin(); ite != RunMap->GetMap()->end(); ite++){
    G4double val = *(ite->second);
    if( val == 0) continue;
    if( val < minDose ) minDose = val;
    if( val > maxDose ) maxDose = val;
  }  
  minDose *= theUnitInv*theNevInv;
  maxDose *= theUnitInv*theNevInv;
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(warningVerb) ) G4cout << theName << " MINIMUM DOSE " << minDose << " Gy " << G4endl;
#endif
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(warningVerb) ) G4cout << theName << " MAXIMUM DOSE " << maxDose << " Gy " << G4endl;
#endif
//--- Average errors
  G4double theF20 = 0.;
  G4double theF50 = 0.;
  G4double theF90 = 0.;
  G4double theDose20 = maxDose*0.2;
  G4double theDose50 = maxDose*0.5;
  G4double theDose90 = maxDose*0.9;
  G4int theN20 = 0;
  G4int theN50 = 0;
  G4int theN90 = 0;
  //  G4cout << " RTPSPDoseHistos::DumpAll  entries " << RunMap->entries() << G4endl;

  if( !theDoseHeader ){
    theRegParam = GmRegularParamUtils::GetInstance()->GetPhantomParam();
    theNVoxelX = G4int(theRegParam->GetNoVoxelX());
    theNVoxelY = G4int(theRegParam->GetNoVoxelY());
    theNVoxelZ = G4int(theRegParam->GetNoVoxelZ());
    theDimX = theRegParam->GetVoxelHalfX();
    theDimY = theRegParam->GetVoxelHalfY();
    theDimZ = theRegParam->GetVoxelHalfZ();

  } else {
    theNVoxelX = G4int(theDoseHeader->GetNoVoxelX());
    theNVoxelY = G4int(theDoseHeader->GetNoVoxelY());
    theNVoxelZ = G4int(theDoseHeader->GetNoVoxelZ());
    theDimX = theDoseHeader->GetVoxelHalfX();
    theDimY = theDoseHeader->GetVoxelHalfY();
    theDimZ = theDoseHeader->GetVoxelHalfZ();
  }
  //---- Get nz for Z of dose maximum
  // ite += theNVoxelX/2 + theNVoxelX*theNVoxelY/2; // centre voxel in X & Y
  //-  G4int nzMaxDose = 0;
  G4double maxDoseZ = 0.;
  //  for(; ite != RunMap->GetMap()->end(); ite+=theNVoxelX*theNVoxelY){
  for( ite = RunMap->GetMap()->begin(); ite != RunMap->GetMap()->end(); ite++){
    G4int copyNo = ite->first;
    G4int nx = (unsigned int)(copyNo%theNVoxelX);
    G4int ny = (unsigned int)( (copyNo/theNVoxelX)%theNVoxelY );
    if( nx == theNVoxelX/2 && ny == theNVoxelY/2 ) {
      if( *(ite->second) > maxDoseZ ){
	maxDoseZ = *(ite->second);
	//	nzMaxDose = ite->first/(theNVoxelX*theNVoxelY);
      }
    }
  }
  //---- Get nz for Z of dose maximum and nz at Z=50, 100 & 150 mm
  GmHisto1* hisDV = theAnaMgr->GetHisto1D(36202);
  
  for( ite = RunMap->GetMap()->begin(); ite != RunMap->GetMap()->end(); ite++){
    //  G4int copyNo = ite->first;
    //  G4int nx = (unsigned int)(copyNo%theNVoxelX);
    //  G4int ny = (unsigned int)( (copyNo/theNVoxelX)%theNVoxelY );
    //  G4int nz = (unsigned int)(copyNo/(theNVoxelX*theNVoxelY));
    //    G4cout << " RTPSPDoseHistos::DumpAll copyNo " << copyNo << " " << nx << " " << ny << " " << nz << G4endl;
    
    G4double val = *(ite->second)*theNevInv*theUnitInv;
    //    G4double error = scorer->GetError( (*ite).first )*theUnitInv;
    
    G4double log10val = log10(val);
    theAnaMgr->GetHisto1D(36201)->Fill(log10val);

    for( G4int ih = 1; ih < hisDV->GetNbinsX(); ih++ ){
      if( hisDV->GetBinLowEdge(ih) < log10val ) {
	hisDV->SetBinContent(ih, hisDV->GetBinContent(ih)+1 );
      }
    }
    
    if( val > theDose20 ) {
      theF20 += (scorer->GetError( (*ite).first )*theUnitInv)/val;
      theN20++;
      //      G4cout << theN20 << "F20 " << theF20 << " val " << val << " /" << theScorer->GetError( (*ite).first )*theUnitInv << " error " << theScorer->GetError( (*ite).first ) << " index " << (*ite).first << G4endl;
    }
    if( val > theDose50 ) {
      theF50 += (scorer->GetError( (*ite).first )*theUnitInv)/val;
      theN50++;
    }
    if( val > theDose90 ) {
      theF90 += (scorer->GetError( (*ite).first )*theUnitInv)/val;
      theN90++;
      // G4cout << copyNo << " F90 " << theF90 << " N90 " << theN90 << " " << scorer->GetError( (*ite).first )*theUnitInv << " " <<  val << G4endl;
    }
  }
  if( theN20 != 0. ) { 
    theF20 /= theN20;
  } else {
    theF20 = 0.;
  }
  if( theN50 != 0. ) { 
    theF50 /= theN50;
  } else {
    theF50 = 0.;
  }
  if( theN90 != 0. ) { 
    theF90 /= theN90;
  } else {
    theF90 = 0.;
  }
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(warningVerb) ) G4cout << theName << " AVERAGE ERROR 20% = " << theF20 << G4endl;
#endif
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(warningVerb) ) G4cout << theName << " AVERAGE ERROR 50% = " << theF50 << G4endl;
#endif
#ifndef GAMOS_NO_VERBOSE
  if( RTVerb(warningVerb) ) G4cout << theName << " AVERAGE ERROR 90% = " << theF90 << G4endl;
#endif
  
  G4String fileName = GmParameterMgr::GetInstance()->GetStringValue(theName+":HistosFileName","");
  if( fileName != "" ) {
    FillHistosFromFile( fileName );
  } else {
    FillHisto1N_X("RTPSPDoseHistos: Dose Profile X_merged",0,theNVoxelX-1,0,theNVoxelY-1,0,theNVoxelZ-1);
    FillHisto1N_Y("RTPSPDoseHistos: Dose Profile Y_merged",0,theNVoxelX-1,0,theNVoxelY-1,0,theNVoxelZ-1);
    FillHisto1N_Z("RTPSPDoseHistos: Dose Profile Z_merged",0,theNVoxelX-1,0,theNVoxelY-1,0,theNVoxelZ-1);
    
    FillHisto2N_XY("RTPSPDoseHistos: Dose XY_merged",0,theNVoxelX-1,0,theNVoxelY-1,0,theNVoxelZ-1);
    FillHisto2N_XZ("RTPSPDoseHistos: Dose XZ_merged",0,theNVoxelX-1,0,theNVoxelY-1,0,theNVoxelZ-1);
    FillHisto2N_YZ("RTPSPDoseHistos: Dose YZ_merged",0,theNVoxelX-1,0,theNVoxelY-1,0,theNVoxelZ-1);
  }   
  
  std::vector<G4String> h2d;
  h2d = GmParameterMgr::GetInstance()->GetVStringValue(theName+":AllHistos2D",h2d);
  for( unsigned int ii = 0; ii < h2d.size(); ii++)  {
    FillAllHistos2D(h2d[ii]);
  }
}


//------------------------------------------------------------------
void RTPSPDoseHistos::FillHistosFromFile( G4String fileName )
{
  G4tgrFileIn fin = G4tgrFileIn::GetInstance(fileName);
  std::vector<G4String> wl;
  unsigned int ii = 1;
  for( ;; ii++){
    if( ! fin.GetWordsInLine(wl) ) break;
    if( wl.size() != 8 ) {
      G4Exception("RTPSPDoseHistos::FillHistosFromFile",
		  G4String("Wrong number of arguments in line number: " + GmGenUtils::itoa(ii)).c_str(),
		  FatalErrorInArgument,
		  G4String("There should be 8: HISTO_TYPE HISTO_NAME X_MIN X_MAX Y_MIN Y_MAX Z_MIN Z_MAX, there are "+GmGenUtils::itoa(wl.size())).c_str());
    }
    float hminX;
    float hmaxX;
    float hminY;
    float hmaxY;
    float hminZ;
    float hmaxZ;
    SubstituteNVoxel( wl[2], "NVoxelX" );
    SubstituteNVoxel( wl[3], "NVoxelX" );
    SubstituteNVoxel( wl[4], "NVoxelY" );
    SubstituteNVoxel( wl[5], "NVoxelY" );
    SubstituteNVoxel( wl[6], "NVoxelZ" );
    SubstituteNVoxel( wl[7], "NVoxelZ" );

    hminX = GmGenUtils::GetInteger(wl[2]);
    hmaxX = GmGenUtils::GetInteger(wl[3]);
    hminY = GmGenUtils::GetInteger(wl[4]);
    hmaxY = GmGenUtils::GetInteger(wl[5]);
    hminZ = GmGenUtils::GetInteger(wl[6]);
    hmaxZ = GmGenUtils::GetInteger(wl[7]);

    if( wl[0] == "1X" ) {
      FillHisto1N_X(wl[1],G4int(hminX),G4int(hmaxX),G4int(hminY),G4int(hmaxY),G4int(hminZ),G4int(hmaxZ));
    } else if( wl[0] == "1Y" ) {
      //      G4cout << "CALLING 1N_Y " << wl[1]<< " " <<hminX<< " " <<hmaxX<< " " <<hminY<< " " <<hmaxY<< " " <<hminZ<< " " <<hmaxZ << G4endl;
      FillHisto1N_Y(wl[1],G4int(hminX),G4int(hmaxX),G4int(hminY),G4int(hmaxY),G4int(hminZ),G4int(hmaxZ));
    } else if( wl[0] == "1Z" ) {
      //      G4cout << "CALLING 1N_Z " << wl[1]<< " " <<hminX<< " " <<hmaxX<< " " <<hminY<< " " <<hmaxY<< " " <<hminZ<< " " <<hmaxZ << G4endl;
      FillHisto1N_Z(wl[1],G4int(hminX),G4int(hmaxX),G4int(hminY),G4int(hmaxY),G4int(hminZ),G4int(hmaxZ));
    }
  }
		  
}


//------------------------------------------------------------------
void RTPSPDoseHistos::SubstituteNVoxel( G4String& word, const G4String nvox )
{ 
  G4int inv = word.find(nvox);
  G4String nvoxStr;
  if( nvox == "NVoxelX" ) {
    nvoxStr = GmGenUtils::itoa(theNVoxelX); 
  } else if( nvox == "NVoxelY" ) {
    nvoxStr = GmGenUtils::itoa(theNVoxelY); 
  } else if( nvox == "NVoxelZ" ) {
    nvoxStr = GmGenUtils::itoa(theNVoxelZ); 
  }

  if( inv != G4int(G4String::npos) ) {
    if( inv != 0 ) {
      word = word.substr(0,inv-1) + nvoxStr + word.substr(inv+7,word.length());
    } else {
      word = nvoxStr + word.substr(inv+7,word.length());
    }
  }
}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillHisto1( GmHisto1* his, G4int ibin, G4double val, G4double error )
{
  G4double herror = 0.;  
  if( his->GetBinContent(ibin+1) != 0.) {
    herror = his->GetBinError(ibin+1);
  } 
  his->SetBinContent(ibin+1, his->GetBinContent(ibin+1)+val);
  his->SetBinError(ibin+1, sqrt(herror*herror+error*error) );
}

//------------------------------------------------------------------
void RTPSPDoseHistos::FillAllHistos2D( G4String dimensions )
{
  if( dimensions == "XY" ) {
    for( G4int ii = 0; ii < theNVoxelZ; ii++ ){
      FillHisto2N_XY( "RTPSPDoseHistos: Dose XY_" + GmGenUtils::itoa(ii), 0 , theNVoxelX, 0, theNVoxelY, ii, ii );
    }
    
  } else if( dimensions == "XZ" ) {
    for( G4int ii = 0; ii < theNVoxelY; ii++ ){
      FillHisto2N_XY( "RTPSPDoseHistos: Dose XZ_" + GmGenUtils::itoa(ii), 0 , theNVoxelX, ii, ii, 0, theNVoxelZ );
    } 
    
  } else if( dimensions == "YZ" ) {
    for( G4int ii = 0; ii < theNVoxelX; ii++ ){
      FillHisto2N_XY( "RTPSPDoseHistos: Dose YZ_" + GmGenUtils::itoa(ii), ii, ii, 0 , theNVoxelY, 0, theNVoxelZ );
    }
    
  }  else {
    G4Exception("RTPSPDoseHistos::FillAllHistos2D",
		"Wrong dimensions",
		JustWarning,
		G4String("It must be XY, XZ or YZ, it is " + dimensions).c_str());
  }

}
