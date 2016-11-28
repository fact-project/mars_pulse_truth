#ifndef MARS_MHexagonalFTCalc
#define MARS_MHexagonalFTCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif
#ifndef MARS_MHexagonalFT
#include "MHexagonalFT.h"
#endif
#ifndef MARS_MArrayD
#include "MArrayD.h"
#endif

class MArrayD;
class MSignalCam;
class MHexagonFreqSpace;

class MHexagonalFTCalc : public MTask
{
private:
    static const char *fgNameGeomCam;       // Default name of used geometry
    static const char *fgNameSignalCamIn;  // Default name of used input MSignalCam
    static const char *fgNameSignalCamOut; // Default name of used output MSignalCam

    TString fNameGeomCam;       // name of used geometry
    TString fNameSignalCamIn;  // name of used input MSignalCam
    TString fNameSignalCamOut; // name of used output MSignalCam

    MSignalCam       *fEvtIn;  // input MSignalCam
    MSignalCam       *fEvtOut; // output MSignalCam

    MHexagonFreqSpace *fFreq1;  //!
    MHexagonFreqSpace *fFreq2;  //!

    Float_t fMaxAmplitude;      // Maximum amplitude to cut
    Float_t fMaxRowFraction;    // Maximum fraction of rows (frequency space) to remove

    Bool_t fSkipBwdTrafo;       // flag whether backward trafo should be done

    MHexagonalFT       fHFT;    // Fourier transformation

    MArrayI fMap;    //! Pixel mapping between space space and frequency space
    MArrayD fOffset; //! Fourier transformation of a constant 1 for all 'valid' pixels

    Int_t PreProcess(MParList *p);
    Int_t Process();

    void CleanFreqSpace(MArrayD &re, MArrayD &im);

public:
    MHexagonalFTCalc(const char *name=0, const char *title=0);

    void SetSkipBwdTrafo(Bool_t b=kTRUE) { fSkipBwdTrafo=b; } // Don't do a backward transoformation

    const MHexagonalFT &GetHFT() const { return fHFT; }       // return the MHexagonalFT object used

    void SetNameGeomCam(const char *n) { fNameGeomCam = n; }       // name of camera geometry used 
    void SetNameSignalCamIn(const char *n)     { fNameSignalCamIn = n; }  // name of input MSignalCam used
    void SetNameSignalCamOut(const char *n="") { fNameSignalCamOut = n; } // name of ouput MSignalCam used

    void SetMaxAmplitude(Float_t max) { fMaxAmplitude=max; } // amplitude to cut at
    void SetMaxRowFraction(Float_t max) { fMaxRowFraction=max; } // row (frequency) to cut at

    ClassDef(MHexagonalFTCalc,0)//Task to calculate Hexagonal Fourier Transformation
};
    
#endif
