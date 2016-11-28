#ifndef MARS_MSimSignalCam
#define MARS_MSimSignalCam

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MArrayI
#include "MArrayI.h"
#endif

#ifndef MARS_MArrayD
#include "MArrayD.h"
#endif

class MParList;
class MPhotonEvent;
class MPhotonStatistics;
class MSignalCam;
class MParameterD;

class MSimSignalCam : public MTask
{
private:
    MPhotonEvent      *fEvt;      //! Event containing the photons
    MPhotonStatistics *fStat;     //! Statistics about the event
    MSignalCam        *fSignal;   //! Output container
    MParameterD       *fTrigger;  //! Trigger position w.r.t. analog channels

    MArrayD fCont;   //! Local buffer for contents
    MArrayD fTime;   //! local buffer for time

    // MParContainer
    // Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MTask
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *plist);
    Int_t  Process();

public:
    MSimSignalCam(const char *name=NULL, const char *title=NULL);

    ClassDef(MSimSignalCam, 0) // Task to convert a tagged MPhotonEvent list into MSignalCam
};
    
#endif
