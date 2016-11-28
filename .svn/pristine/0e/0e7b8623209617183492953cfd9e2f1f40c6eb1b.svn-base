#ifndef MARS_MMcCollectionAreaCalc
#define MARS_MMcCollectionAreaCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#include <TH2.h>
#include <TF1.h>

class MParList;
class MMcEvt;
class MMcEvtBasic;
class MMcTrig;
class MHMcCollectionArea;
class MBinning;

class MMcCollectionAreaCalc : public MTask
{
private:
    const MMcEvt       *fMcEvt;
    const MMcEvtBasic  *fMcEvtBasic;
    const MMcTrig      *fMcTrig;

    MBinning           *fBinsTheta;
    MBinning           *fBinsEnergy;
    // Coarse zenith angle and energy bins used in the analysis

    TF1                *fSpectrum; 
    // Tentative energy spectrum. This modifies slightly the calculation
    // of the effective area (see MHMcCollectionArea::Calc)


    MHMcCollectionArea *fCollArea;

    TString fObjName;

    Int_t  PreProcess(MParList *pList);
    Int_t  Process();
    Int_t  PostProcess();

public:
    MMcCollectionAreaCalc(const char *name = NULL, const char *title = NULL);

    void SetSpectrum(TF1 *f) { fSpectrum = f; }

    ClassDef(MMcCollectionAreaCalc, 0) // Task to calculate the collection area histogram
};

#endif 

