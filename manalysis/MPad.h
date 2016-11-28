#ifndef MARS_MPad
#define MARS_MPad

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
class MPointingPos;
class MParList;
class MBadPixelsCam;
class MRead;
class MFilterList;


class MPad : public MTask
{
private:
    MGeomCam       *fCam;
    MCerPhotEvt    *fEvt; 
    MPointingPos   *fPointPos;
    MPedPhotCam    *fPed;
    MBadPixelsCam  *fBad;

    TString  fNamePedPhotCam; // name of the 'MPedPhotCam' container
    TString  fType;           // type of data to be padded
    TFile    *fInfile;        // input file containing padding histograms

    Int_t          fIter;

    Int_t          fInf[9];
    Int_t          fErrors[9];
    Int_t          fWarnings[3];

    //----------------------------------
    // plots used for the padding
    // for all plots it is assumed that the pedestal RMS is given in units of "number of photons"

    // original distributions
    TH2D  *fHSigmaThetaMC;     // 2D-histogram (sigmabar_inner vs. Theta)
    TH2D  *fHSigmaThetaON;     // 2D-histogram (sigmabar_inner vs. Theta)
    TH2D  *fHSigmaThetaOFF;    // 2D-histogram (sigmabar_inner vs. Theta)

    TH2D  *fHSigmaThetaOuterMC;   // 2D-histogram (sigmabar_outer vs. Theta)
    TH2D  *fHSigmaThetaOuterON;   // 2D-histogram (sigmabar_outer vs. Theta)
    TH2D  *fHSigmaThetaOuterOFF;  // 2D-histogram (sigmabar_outer vs. Theta)

    TH3D  *fHDiffPixThetaMC;   // 3D-histogram (Theta, pixel, (sigma^2-sigmabar^2)/area )
    TH3D  *fHDiffPixThetaON;   // 3D-histogram (Theta, pixel, (sigma^2-sigmabar^2)/area )
    TH3D  *fHDiffPixThetaOFF;  // 3D-histogram (Theta, pixel, (sigma^2-sigmabar^2)/area )

    //---------------------
    // target distributions
    TH2D  *fHSigmaTheta;       // 2D-histogram (sigmabar_inner vs. Theta)
    TH2D  *fHSigmaThetaOuter;  // 2D-histogram (sigmabar_outer vs. Theta)

    TH3D  *fHDiffPixThetaTargetMC;   // 3D-histogram (Theta, pixel, (sigma^2-sigmabar^2)/area )
    TH3D  *fHDiffPixThetaTargetON;   // 3D-histogram (Theta, pixel, (sigma^2-sigmabar^2)/area )
    TH3D  *fHDiffPixThetaTargetOFF;  // 3D-histogram (Theta, pixel, (sigma^2-sigmabar^2)/area )

    //---------------------
    // matrices according to which the padding is performed
    TH3D  *fHgMC;        // matrix (Theta, sigbarold, sigbarnew) for MC data
    TH3D  *fHgON;        // matrix (Theta, sigbarold, sigbarnew) for ON data
    TH3D  *fHgOFF;       // matrix (Theta, sigbarold, sigbarnew) for OFF data

    TH3D  *fHgOuterMC;   // matrix (Theta, sigbarold, sigbarnew) for MC data
    TH3D  *fHgOuterON;   // matrix (Theta, sigbarold, sigbarnew) for ON data
    TH3D  *fHgOuterOFF;  // matrix (Theta, sigbarold, sigbarnew) for OFF data

    //-------------------------------
    // plots for checking the padding
    TH2D  *fHSigmaPedestal; // 2D-histogram : pedestal sigma after
                            //                versus before padding
    TH2D  *fHPhotons;       // 2D-histogram : no.of photons/area after
                            //                versus before padding
    TH1D  *fHNSB;           // 1D-histogram : additional NSB/area

    //-------------------------------
    Bool_t MergeABC(TString tA, TString tB, TString tC,
                    TH2D *hA,   TH2D *hB,   TH2D *hC,   TH2D *hM,
                    TH3D *hgA,  TH3D *hgB,  TH3D *hgC,  TString canv);

    Bool_t Merge2Distributions(TH1D *hista, TH1D *histb, TH1D *histap,
                               TH2D *fHga,  TH2D *fHgb,  Int_t nbinssig,
                               TString canv);

    Bool_t UpdateHg(TH2D *fHga, TH1D *histap, TH2D *fHge, TH3D *fHgA,
                    Int_t nbinssig, Int_t l); 

public:
    MPad(const char *name=NULL, const char *title=NULL);
    ~MPad();

    void SetDataType(const char *type);       // type of data to be padded
    void SetNamePedPhotCam(const char *name); // name of MPedPhotCam container

    Bool_t ReadPadHistograms(TString type, TString filein);

    Bool_t MergeONOFFMC(TString nameon="", TString nameoff="",
                        TString namemc="", TString fileout="");

    Bool_t WritePaddingDist(TString fileout);
    Bool_t ReadPaddingDist(TString  filein);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    ClassDef(MPad, 0)    // task for the padding 
}; 

#endif








