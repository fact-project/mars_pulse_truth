#ifndef MARS_MFCosmics
#define MARS_MFCosmics

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MRawEvtData;

class MPedestalCam;
class MBadPixelsCam;
class MExtractedSignalCam;

class MFCosmics : public MFilter
{
private:
    static const TString fgNamePedestalCam;

    MPedestalCam        *fPedestals; // Pedestals of all pixels in the camera
    MExtractedSignalCam *fSignals;   // Calibration events of all pixels in the camera
    MBadPixelsCam       *fBadPixels; // Bad pixel used for exclusions

    MRawEvtData         *fRawEvt;    // raw event data (time slices)

    TString fNamePedestalCam;

    Int_t   fCut[2];
    Bool_t  fResult;

    Float_t fMaxEmptyPixels;         // Maximum number of empty pixels before declaring event as cosmic
    Float_t fSqrtHiGainSamples;      // Square root of the number of used Hi-Gain Samples

    Float_t fMinAcceptedFraction;      // return error if exceeded
    Float_t fMaxAcceptedFraction;      // return error if exceeded

    // MFCosmics
    Bool_t CosmicsRejection() const;

    // MTask
    Bool_t ReInit(MParList *pList);
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();
    Int_t  PostProcess();
    Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MFilter
    Bool_t IsExpressionTrue() const { return fResult; }
  
public:
    MFCosmics(const char *name=NULL, const char *title=NULL);

    void    SetMaxEmptyPixels(const Float_t n) { fMaxEmptyPixels = n;    }
    Float_t GetMaxEmptyPixels() const          { return fMaxEmptyPixels; }

    void    SetMaxAcceptedFraction(const Float_t n) { fMaxAcceptedFraction = n;    }
    void    SetMinAcceptedFraction(const Float_t n) { fMinAcceptedFraction = n;    }

    void    SetNamePedestalCam(const char *name) { fNamePedestalCam = name; }

    ClassDef(MFCosmics, 0)   // Filter to perform a cosmics rejection
};

#endif



