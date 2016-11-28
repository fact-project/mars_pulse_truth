#ifndef MARS_MHDrsCalibrationCheck
#define MARS_MHDrsCalibrationCheck

#ifndef MARS_MHCamEvent
#include "MHCamEvent.h"
#endif

class MPedestalSubtractedEvt;

class MHDrsCalibrationCheck : public MHCamEvent
{
private:
    MPedestalSubtractedEvt *fEvt;     //!

    std::vector<double> fSum1;
    std::vector<double> fSum2;

    UInt_t fNumPixels;
    UInt_t fNumSamples;

    void InitHistogram();

public:
    MHDrsCalibrationCheck(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *pList);
    Bool_t ReInit(MParList *);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    void Paint(Option_t *o="");

    ClassDef(MHDrsCalibrationCheck, 1) // A list of histograms storing the Fadc spektrum of one pixel
};

#endif

