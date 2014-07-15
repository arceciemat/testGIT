#------------------------------------------------------------------------------
# 
# Module : RadioTherapy
# Package: 
#
#------------------------------------------------------------------------------
# List external includes needed.

# List external includes needed.
include(GamosSetup)
include(UseRoot)

#
# Define the GAMOS Module.
#
include(Geant4MacroDefineModule)
GEANT4_DEFINE_MODULE(NAME RadioTherapy

   HEADERS
       RTPhaseSpaceGenerator.hh
       RTVerbosity.hh
       RTPhaseSpaceHistos.hh
       RTExtraInfoProviderZOrigin.hh
       RTExtraInfoProviderOrigin.hh
       iaea_record.hh
       RTReuseAtZPlaneUA.hh
       RTPSPDoseHistos.hh
       iaea_header.hh
       RTReuseAtZPlaneMessenger.hh
       RTExtraInfoProviderInteractions.hh
       RTProdCutsStudyUA.hh
       RTZRLimitsUA.hh
       RTVExtraInfoProviderLong.hh
       RTGeneratorPhaseSpace.hh
       RTExtraInfoProviderZLastNoMaterial.hh
       RTVExtraInfoProviderLongByRegion.hh
       RTVZRLimitsUA.hh
       RTCutsStudyFilter.hh
       RTExtraInfoProviderCrossings.hh
       RTPSPDoseHistosVector.hh
       EGSPhspParticle.hh
       iaea_utilities.hh
       RTExtraInfoTrackInfo.hh
       RTReuseAtZPlaneTrackInfo.hh
       RTClassifierByEILong.hh
       iaea_config.hh
       RTMinRangeLimitsStudyUA.hh
       RTPhaseSpaceUA.hh
       RTFilterByEILong.hh
       RTVClassifierByExtraInfo.hh
       RTExtraInfoMgr.hh
       RTVExtraInfoProviderFloat.hh
       RTExtraInfoProviderZLast.hh
       RTReuseAtZPlaneProcess.hh
       RTZRLimitsAutoUA.hh
       RTVExtraInfoProvider.hh
       RTPhaseSpaceMessenger.hh

   SOURCES
       plugin.cc
       RTProdCutsStudyUA.cc
       RTCutsStudyFilter.cc
       RTVExtraInfoProviderFloat.cc
       RTPSPDoseHistosVector.cc
       RTPhaseSpaceHistos.cc
       RTPhaseSpaceUA.cc
       RTVClassifierByExtraInfo.cc
       RTMinRangeLimitsStudyUA.cc
       RTExtraInfoMgr.cc
       RTExtraInfoProviderInteractions.cc
       RTVExtraInfoProviderLongByRegion.cc
       RTPSPDoseHistos.cc
       EGSPhspParticle.cc
       iaea_utilities.cc
       RTExtraInfoProviderZLast.cc
       RTClassifierByEILong.cc
       RTExtraInfoProviderZLastNoMaterial.cc
       RTExtraInfoProviderOrigin.cc
       RTExtraInfoProviderCrossings.cc
       RTGeneratorPhaseSpace.cc
       iaea_header.cc
       RTReuseAtZPlaneTrackInfo.cc
       RTReuseAtZPlaneMessenger.cc
       RTZRLimitsUA.cc
       iaea_record.cc
       RTExtraInfoProviderZOrigin.cc
       RTFilterByEILong.cc
       RTExtraInfoTrackInfo.cc
       RTVZRLimitsUA.cc
       RTVExtraInfoProviderLong.cc
       RTVExtraInfoProvider.cc
       RTReuseAtZPlaneProcess.cc
       RTReuseAtZPlaneUA.cc
       RTVerbosity.cc
       RTZRLimitsAutoUA.cc
 
   GRANULAR_DEPENDENCIES

   GLOBAL_DEPENDENCIES
    ${Geant4_LIBRARIES}
    ${Root_LIBRARIES}
  GamosCore_GamosPhysics_Cuts
  GamosCore_GamosGeometry
  GamosCore_GamosUtils
  GamosCore_GamosReadDICOM
  GamosCore_GamosScoring_Management
  GamosCore_GamosUserActionMgr
  GamosCore_GamosBase_Base
  GamosCore_GamosGenerator

   LINK_LIBRARIES
)

# List any source specific properties here
