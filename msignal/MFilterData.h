#ifndef MARS_MFilterData
#define MARS_MFilterData

#include<vector>

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MPedestalSubtractedEvt;

class MFilterData : public MTask
{
private:
    MPedestalSubtractedEvt *fSignalIn;  //! Input container
    MPedestalSubtractedEvt *fSignalOut; //! Output container

    TString fNameSignalIn;
    TString fNameSignalOut;

    std::vector<float> fWeights;

    Int_t  PreProcess(MParList *pList);
    //Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MFilterData(const char *name=NULL, const char *title=NULL);

    void SetNameSignalIn(const char *name="MPedestalSubtractedEvt")  { fNameSignalIn=name; }
    void SetNameSignalOut(const char *name="MPedestalSubtractedEvt") { fNameSignalOut=name; }

    void SetWeights(const std::vector<float> &w) { fWeights = w; }
    void SetAverage(const UInt_t n) { fWeights.assign(n, 1./n); }

    void Filter(UShort_t npix, UShort_t nroi, const Float_t *beg, Float_t *out) const;

    ClassDef(MFilterData, 0) // Class to filter the calibrated data
};

#endif
