#ifndef MARS_MCT1PadONOFF
#define MARS_MCT1PadONOFF

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
class MRead;
class MFilterList;


class MCT1PadONOFF : public MTask
{
private:
    MGeomCam       *fCam;
    MCerPhotEvt    *fEvt; 
    MSigmabar      *fSigmabar;
    MMcEvt         *fMcEvt;
    MPedPhotCam   *fPed;
    MBlindPixels   *fBlinds;

    TString        fType;           // type of data to be padded
    TFile          *fInfile;        // input file containing padding histograms

    Int_t          fPadFlag;
    Int_t          fIter;

    Int_t          fErrors[9];
    Int_t          fWarnings[2];


    // plots used for the padding
    TH2D           *fHBlindPixIdTheta; // 2D-histogram (blind pixel Id vs. Theta)
    TH2D           *fHBlindPixNTheta; // 2D-histogram (no.of blind pixels vs. Theta)

    TH2D           *fHSigmaTheta;       // 2D-histogram (sigmabar vs. Theta)
    TH2D           *fHSigmaThetaON;     // 2D-histogram (sigmabar vs. Theta)
    TH2D           *fHSigmaThetaOFF;    // 2D-histogram (sigmabar vs. Theta)

    TH3D           *fHSigmaPixTheta;    // 3D-histogram (Theta, pixel, sigma)
    TH3D           *fHSigmaPixThetaON;  // 3D-histogram (Theta, pixel, sigma)
    TH3D           *fHSigmaPixThetaOFF; // 3D-histogram (Theta, pixel, sigma)

    TH3D           *fHDiffPixTheta;     // 3D-histogram (Theta, pixel, sigma^2-sigmabar^2)
    TH3D           *fHDiffPixThetaON;   // 3D-histogram (Theta, pixel, sigma^2-sigmabar^2)
    TH3D           *fHDiffPixThetaOFF;  // 3D-histogram (Theta, pixel, sigma^2-sigmabar^2)

    TH3D           *fHgON;           // matrix (Theta, sigbarold, sigbarnew) for ON data
    TH3D           *fHgOFF;          // matrix (Theta, sigbarold, sigbarnew) for OFF data

    // plots for checking the padding
    TH2D           *fHSigmaPedestal; // 2D-histogram : pedestal sigma after
                                     //                versus before padding
    TH2D           *fHPhotons;       // 2D-histogram : no.of photons after
                                     //                versus before padding
    TH1D           *fHNSB;           // 1D-histogram : additional NSB


public:
    MCT1PadONOFF(const char *name=NULL, const char *title=NULL);
    ~MCT1PadONOFF();

    Bool_t MergeHistograms(TH2D *sigthon,     TH2D *sigthoff,
                           TH3D *sigpixthon,  TH3D *sigpixthoff,
                           TH3D *diffpixthon, TH3D *diffpixthoff,
                           TH2D *blindidthon, TH2D *blindidthoff,
                           TH2D *blindnthon,  TH2D *blindnthoff);

    Bool_t ReadTargetDist(const char *filein);
    Bool_t WriteTargetDist(const char *fileout);

    void SetDataType(const char *type);   // type of data to be padded

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();
    
    void SetPadFlag(Int_t padflag);

    ClassDef(MCT1PadONOFF, 0)    // task for the ON-OFF padding 
}; 

#endif






