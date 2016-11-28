#ifndef MARS_MSimGeomCam
#define MARS_MSimGeomCam

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MParList;
class MPhotonEvent;
class MPhotonStatistics;
class MSignalCam;
class MParameterD;
class MRawRunHeader;
class MParSpline;

class MSimGeomCam : public MTask
{
private:
    MGeomCam          *fGeom;     //!
    MPhotonEvent      *fEvt;      //! Event stroing the photons
    MPhotonStatistics *fStat;     //!
    MParameterD       *fPulsePos; //! Intended pulse position in digitization window [ns]
    MRawRunHeader     *fHeader;   //! Length of digitization window
    MParSpline        *fPulse;    //!

    TString fNameGeomCam;

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MTask
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

public:
    MSimGeomCam(const char *name=NULL, const char *title=NULL);

    void SetNameGeomCam(const char *name="MGeomCam") { fNameGeomCam=name; }

    ClassDef(MSimGeomCam, 0) // Task to tag each photon in a MPhotonEvent with a pixel index from a MGeomCam
};

#endif
