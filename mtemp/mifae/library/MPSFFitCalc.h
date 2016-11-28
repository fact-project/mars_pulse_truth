/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MPSFFitCalc                                                             //
//                                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#ifndef MARS_MPSFFitCalc
#define MARS_MPSFFitCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif
#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif
#ifndef ROOT_TArrayS
#include <TArrayS.h>
#endif

class TSring;
class TH1D;

class MGeomCam;
#ifndef MARS_MCameraDC
#include "MCameraDC.h"
#endif
#ifndef MARS_MPSFFit
#include "MPSFFit.h"
#endif

class MPSFFitCalc : public MTask
{

private:
 
    MGeomCam  *fGeomCam;
    MCameraDC *fCamera; 
    MPSFFit   *fFit;

    MPSFFit   fTotalMeanFit;
    UInt_t    fNumTotalMeanFits;
    
    TH1D *fPedestalDCHist;
    Float_t fPedestalDC;

    TString *fVname;
    TArrayD fVinit; 
    TArrayD fStep; 
    TArrayD fLimlo; 
    TArrayD fLimup; 
    TArrayI fFix;
    TObject *fObjectFit;
    TString fMethod;
    Bool_t fNulloutput;

    Float_t fMaxDC;
    UShort_t fImgCleanMode;
    UShort_t fNumRings;
    Float_t fPedLevel;
    TArrayS fBlindPixels;

    void RingImgClean();
    void RmsImgClean(Float_t pedestal);
    void MaskImgClean(TArrayS blindpixels);

    void InitFitVariables();

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

public:

    enum ImgCleanMode_t{kNone=0,kRing,kRms,kMask,kCombined};
    static const ImgCleanMode_t kDefault = kRing;

    MPSFFitCalc(ImgCleanMode_t imgmode=kDefault, const char *name=NULL, const char *title=NULL);
    ~MPSFFitCalc();
    
    void SetImgCleanMode(ImgCleanMode_t imgmode=kDefault){ fImgCleanMode=imgmode;};
    void SetNumRings(UShort_t numRings) {fNumRings=numRings;}
    void SetPedLevel(Float_t pedestal) {fPedLevel=pedestal;}
    void SetBlindPixels(TArrayS blindpixels) {fBlindPixels=blindpixels;}
    

    ClassDef(MPSFFitCalc, 0)   // Task that fits the PSF using the dc readout of the camera.
};

#endif
