#ifndef MARS_MSimCamera
#define MARS_MSimCamera

#ifndef MARS_MTask
#include "MTask.h"
#endif

#include "MArrayF.h"
#include "MMatrix.h"

class MMcEvt;
class MParList;
class MPhotonEvent;
class MPhotonStatistics;
class MRawRunHeader;
class MAnalogChannels;
class MPedestalCam;
class MArrayF;
class MTruePhotonsPerPixelCont;

class MSpline3;
class MParameterD;

class MSimCamera : public MTask
{
private:
    MPhotonEvent      *fEvt;             //! Event stroing the photons
    MPhotonStatistics *fStat;            //! Valid time range of the phootn event
    MRawRunHeader     *fRunHeader;       //! Sampling frequency
    MPedestalCam      *fElectronicNoise; //! Electronic noise (baseline and rms)
    MPedestalCam      *fGain;            //! Electronic noise (baseline and rms)
    MPedestalCam      *fAccidentalPhotons;//! Accidental photon rates

    MAnalogChannels   *fCamera;          //! Output of the analog signals
    MMcEvt            *fMcEvt;           //! For information stored in MMcEvt

    MParameterD       *fCrosstalkCoeffParam;
    MParameterD       *fResidualTimeSpread;    //! Container to store a standard deviation for a residual time spread
    MParameterD       *fGapdTimeJitter;        //! Container to store a standard deviation for a gapd time jitter
    
    MMatrix *fFixTimeOffsetsBetweenPixelsInNs; //! Container to store the fix temporal offsets for each pixel in ns

    MTruePhotonsPerPixelCont    *fTruePhotons;  //! Container to store the number of photons per pixel

    const MSpline3    *fSpline;          // Pulse Shape    

    Bool_t fBaselineGain;  // Should the gain be applied to baseline and electronic noise?

    Double_t fDefaultOffset;  // Default offset added to all channels
    Double_t fDefaultNoise;   // Default noise (RMS) added to all samples
    Double_t fDefaultGain;    // Default gain (multiplication factor to the given/used pulse shape)

    Double_t fACFudgeFactor;

    Double_t fACTimeConstant;

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MTask
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MSimCamera(const char *name=NULL, const char *title=NULL);

    ClassDef(MSimCamera, 0) // Task to simulate the electronic noise and to convert photons into pulses
};

#endif
