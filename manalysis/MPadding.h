#ifndef MARS_MPadding
#define MARS_MPadding

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TH1D;
class TH2D;
class MGeomCam;
class MCerPhotEvt;
class MPedPhotCam;
class MMcEvt;
class MSigmabar;
class MParList;

class MPadding : public MTask
{
private:
    MGeomCam     *fCam;
    MCerPhotEvt  *fEvt;
    MSigmabar    *fSigmabar;
    MMcEvt       *fMcEvt;
    MPedPhotCam  *fPed;

    Int_t     fRunType;
    Int_t     fGroup;

    TString   fDatabaseFilename; // data file used for generating fHSigmabarMax histogram
    Double_t  fFixedSigmabar;    // fixed sigmabar value

    Bool_t    fHSigMaxAllocated; // flag whether MPadding allocated it
    TH1D     *fHSigmabarMax;     // histogram (sigmabarmax vs. Theta)
    TH2D     *fHSigmaTheta;      // 2D-histogram (sigmabar vs. Theta)
    TH2D     *fHSigmaPedestal;   //-> for testing: plot of padded vs orig. pedestal sigmas
    TH2D     *fHPhotons;         //-> for testing: no.of photons after versus before padding
    TH2D     *fHSigmaOld;        //-> histogram (sigma vs. Theta) before padding
    TH2D     *fHSigmaNew;        //-> histogram (sigma vs. Theta) after padding
    TH1D     *fHNSB;             //-> histogram of added NSB

    Double_t CalcOtherSig(const Double_t mySig, const Double_t theta) const;
    Bool_t   Padding(const Double_t quadDiff, const Double_t theta);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

public:
    MPadding(const char *name=NULL, const char *title=NULL);
    ~MPadding();

    void SetRunType(Int_t runtype) { fRunType =  runtype; }
    void SetGroup(Int_t group)     { fGroup   =  group; }

    Bool_t SetDefiningHistogram(TH1D *hist);
    void SetDatabaseFile(char *filename) { fDatabaseFilename = filename; }

    Bool_t SetSigmaThetaHist(TH2D *histo);

    void SetTargetLevel(Double_t sigmabar);

    ClassDef(MPadding, 0)   // task for the padding
}; 

#endif



