#ifndef RTPSPDoseHistos_hh
#define RTPSPDoseHistos_hh 1

#include <vector>
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "GamosCore/GamosScoring/Management/include/GmVPSPrinter.hh"
#include "GamosCore/GamosBase/Base/include/GmAnalysisMgr.hh"
class GmSqdoseHeader;
class G4PhantomParameterisation;
// class description:
//
// This class fill histograms to get the PDD in a phantom

class RTPSPDoseHistos : public GmVPSPrinter
{
public: // with description
  RTPSPDoseHistos(G4String name, GmSqdoseHeader* doseh= 0);
  virtual ~RTPSPDoseHistos(){ };
  
  virtual void DumpAll( G4THitsMap<G4double>* RunMap, GmVPrimitiveScorer* theScorer );

private:
  void BookHistos();
  void FillHisto1( GmHisto1* his, G4int ibin, G4double val, G4double error );
 
  void CheckHistoLimits( G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax );
  void FillHisto( std::vector<G4String>& wl );
  void FillHisto1N_X( std::vector<G4String>& wl  );
  void FillHisto1N_Y( std::vector<G4String>& wl  );
  void FillHisto1N_Z( std::vector<G4String>& wl  );
  void FillHisto1N_X( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax );
  void FillHisto1N_Y( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax );
  void FillHisto1N_Z( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax );
  void FillHisto2N_XY( std::vector<G4String>& wl  );
  void FillHisto2N_XZ( std::vector<G4String>& wl  );
  void FillHisto2N_YZ( std::vector<G4String>& wl  );
  void FillHisto2N_XY( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax );
  void FillHisto2N_XZ( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax );
  void FillHisto2N_YZ( const G4String& hisName, G4int nXmin, G4int nXmax, G4int nYmin, G4int nYmax, G4int nZmin, G4int nZmax );
  void FillHistosFromFile( G4String fileName );
  void FillAllHistos2D( G4String dimensions );

  G4double ConvertN2DimX(G4int nX);
  G4double ConvertN2DimY(G4int nY);
  G4double ConvertN2DimZ(G4int nZ);

private:
  void SubstituteNVoxel( G4String& word, const G4String nvox );

  GmAnalysisMgr* theAnaMgr;

  G4PhantomParameterisation* theRegParam;
  GmSqdoseHeader* theDoseHeader;
  G4ThreeVector thePhantomTranslation;

  G4int theNHistos;
  G4double theDimX;
  G4double theDimY;
  G4double theDimZ;
  G4int theNVoxelX;
  G4int theNVoxelY;
  G4int theNVoxelZ;
  G4double theNevInv;
  G4double theUnitInv;

  std::map<G4int,G4double*> theMap;

  GmVPrimitiveScorer* theScorer;
};

#endif
