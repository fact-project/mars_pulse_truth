#ifndef MARS_MSimCalibrationSignal
#define MARS_MSimCalibrationSignal

#ifndef MARS_MRead
#include "MRead.h"
#endif

class MGeomCam;
class MParList;
class MParSpline;
class MPhotonEvent;
class MPhotonStatistics;
class MParameterD;
class MRawRunHeader;
class MRawEvtHeader;

class MSimCalibrationSignal : public MRead
{
private:
    MParList          *fParList;    //! Store pointer to MParList for initializing ReInit
    MGeomCam          *fGeom;       //! Camera geometry to know the number of expected pixels
    MParSpline        *fPulse;      //! Pulse Shape to get pulse width from
    MParameterD       *fPulsePos;   //! Expected position at which the pulse should be
    MParameterD       *fTrigger;    //! Position in analog channels at which the triggersignal  is raised
    MRawRunHeader     *fRunHeader;  //! Digitization window and frequency

    MRawEvtHeader     *fEvtHeader;  //! Event header which is filled by the trigger pattern
    MPhotonEvent      *fEvt;        //! Photon event into which the new photons are stored
    MPhotonStatistics *fStat;       //! Photon statistic which is filled

    TString fNameGeomCam;           // Name of the camera geometry

    UInt_t  fNumEvents;             // Number of events to produce
    UInt_t  fNumPhotons;            // Average number of photons to produce
    Float_t fTimeJitter;            // Time jitter (sigma)

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MTask
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

    // MSimCalibrationSignal
    Bool_t CallReInit();

    //Int_t ReadRouting(const char *fname, TObjArray &arr);
public:
    MSimCalibrationSignal(const char *name=NULL, const char *title=NULL);

    void SetNameGeomCam(const char *name="MGeomCam") { fNameGeomCam = name; }

    UInt_t  GetEntries() { return fNumEvents; }
    TString GetFullFileName() const { return "cer000001"; }
    //virtual Bool_t  Rewind();

    ClassDef(MSimCalibrationSignal, 0) // Task to create a fake signal (derives from MRead)
};

#endif
