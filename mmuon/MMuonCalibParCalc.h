#ifndef MARS_MMuonCalibParCalc
#define MARS_MMuonCalibParCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MMuonSearchPar;
class MMuonCalibPar;
class MGeomCam;
class MMuonSetup;
class MHSingleMuon;

class MMuonCalibParCalc : public MTask
{
private:
    MGeomCam       *fGeomCam;         //!
    MMuonCalibPar  *fMuonCalibPar;    //!
    MMuonSearchPar *fMuonSearchPar;   //!
    MMuonSetup     *fMuonSetup;       //!
    MHSingleMuon   *fHist;            //!

    //Bool_t fEnableImpactCalc; // If true, the impact calculation will be done, which consumes a lot of time.

    Int_t   PreProcess(MParList *plist);
    Int_t   Process();

    void    FillHist();
    void    CalcPhi();
    void    CalcImpact(Int_t effbinnum, Float_t startfitval, Float_t endfitval);
    Float_t CalcWidth();

public:
    MMuonCalibParCalc(const char *name=NULL, const char *title=NULL);

    //void EnableImpactCalc(Bool_t b=kTRUE) { fEnableImpactCalc = b; }

    ClassDef(MMuonCalibParCalc, 0) // task to calculate muon parameters
};

#endif
