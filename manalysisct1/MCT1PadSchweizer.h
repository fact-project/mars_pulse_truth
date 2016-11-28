#ifndef MARS_MCT1PadSchweizer
#define MARS_MCT1PadSchweizer

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MH
#include "MH.h"
#endif

class TH1D;
class TH2D;
class TH3D;

class MGeomCam;
class MCerPhotEvt;
class MPedPhotCam;
class MMcEvt;
class MSigmabar;
class MParList;
class MBlindPixels;

class MCT1PadSchweizer : public MTask
{
private:
    MGeomCam       *fCam;
    MCerPhotEvt    *fEvt; 
    MSigmabar      *fSigmabar;
    MMcEvt         *fMcEvt;
    MPedPhotCam   *fPed;
    MBlindPixels   *fBlinds;

    Int_t          fPadFlag;
    Int_t          fRunType;
    Int_t          fGroup;

    Int_t          fErrors[8];

    // plots used for the padding
    TH2D           *fHBlindPixIdTheta; // 2D-histogram (blind pixel Id vs. Theta)
    TH2D           *fHBlindPixNTheta; // 2D-histogram (no.of blind pixels vs. Theta)
    TH2D           *fHSigmaTheta;    // 2D-histogram (sigmabar vs. Theta)
    TH3D           *fHSigmaPixTheta; // 3D-histogram (Theta, pixel, sigma)
    TH3D           *fHDiffPixTheta;  // 3D-histogram (Theta, pixel, sigma^2-sigmabar^2)

    // plots for checking the padding
    TH2D           *fHSigmaPedestal; // 2D-histogram : pedestal sigma after
                                     //                versus before padding
    TH2D           *fHPhotons;       // 2D-histogram : no.of photons after
                                     //                versus before padding
    TH1D           *fHNSB;           // 1D-histogram : additional NSB


public:
    MCT1PadSchweizer(const char *name=NULL, const char *title=NULL);
    ~MCT1PadSchweizer();

    void SetHistograms(TH2D *hist2, TH3D *hist3, TH3D *hist3Diff,
                       TH2D *hist2Pix, TH2D *hist2PixN);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();
    
    void SetPadFlag(Int_t padflag);

    ClassDef(MCT1PadSchweizer, 0)    // task for the padding (Schweizer)
}; 

#endif









