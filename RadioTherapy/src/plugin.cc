#include "Reflex/PluginService.h"

#include "RTPhaseSpaceUA.hh"
#include "RTGeneratorPhaseSpace.hh"
#include "RTZRLimitsAutoUA.hh"
#include "RTZRLimitsUA.hh"
#include "RTVerbosity.hh"
#include "RTPSPDoseHistos.hh"
#include "RTReuseAtZPlaneUA.hh"

#include "RTProdCutsStudyUA.hh"
#include "RTMinRangeLimitsStudyUA.hh"
#include "RTCutsStudyFilter.hh"
#include "RTClassifierByEILong.hh"
#include "RTExtraInfoProviderOrigin.hh"
#include "RTExtraInfoProviderInteractions.hh"
#include "RTExtraInfoProviderCrossings.hh"
#include "RTExtraInfoProviderZLast.hh"
#include "RTExtraInfoProviderZOrigin.hh"
#include "RTFilterByEILong.hh"

PLUGINSVC_FACTORY(RTPhaseSpaceUA,GmUserAction*())
PLUGINSVC_FACTORY(RTGeneratorPhaseSpace,G4VUserPrimaryGeneratorAction*())
PLUGINSVC_FACTORY(RTVerbosity,GmVVerbosity*())
PLUGINSVC_FACTORY(RTZRLimitsAutoUA,GmUserAction*())
PLUGINSVC_FACTORY(RTZRLimitsUA,GmUserAction*())
PLUGINSVC_FACTORY(RTPSPDoseHistos,GmVPSPrinter*(G4String))
PLUGINSVC_FACTORY(RTReuseAtZPlaneUA,GmUserAction*())

PLUGINSVC_FACTORY(RTProdCutsStudyUA,GmUserAction*())
PLUGINSVC_FACTORY(RTMinRangeLimitsStudyUA,GmUserAction*())
PLUGINSVC_FACTORY(RTCutsStudyFilter,GmVFilter*(G4String))

PLUGINSVC_FACTORY(RTExtraInfoProviderOrigin,GmUserAction*())
PLUGINSVC_FACTORY(RTExtraInfoProviderInteractions,GmUserAction*())
PLUGINSVC_FACTORY(RTExtraInfoProviderCrossings,GmUserAction*())
PLUGINSVC_FACTORY(RTExtraInfoProviderZLast,GmUserAction*())
PLUGINSVC_FACTORY(RTExtraInfoProviderZOrigin,GmUserAction*())
PLUGINSVC_FACTORY(RTClassifierByEILong,GmVClassifier*(G4String))
PLUGINSVC_FACTORY(RTFilterByEILong,GmVFilter*(G4String))
