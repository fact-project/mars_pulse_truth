#ifndef MARS_MHFadcPix
#define MARS_MHFadcPix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TH1
#include <TH1.h>
#endif

class MRawEvtData;

class MHFadcPix : public MParContainer
{
public:
    enum Type_t { kSlices, kValue };

private:
    TH1F fHistHi;
    TH1F fHistLo;

    UInt_t fPixId;

    Type_t fType;

public:
    MHFadcPix(Int_t pixid=-1, Type_t t=kValue);

    TH1F *GetHistHi() { return &fHistHi; }
    TH1F *GetHistLo() { return &fHistLo; }

    const TH1F *GetHistHi() const { return &fHistHi; }
    const TH1F *GetHistLo() const { return &fHistLo; }

    void Init(Byte_t nhi, Byte_t nlo);
    Bool_t Fill(const MRawEvtData &evt);

    void DrawHi();
    void DrawLo();

    TObject *Clone(const char *) const;

    void Draw(Option_t *opt=NULL);

    ClassDef(MHFadcPix, 1) // Conatiner to hold two histograms container spektrums for the lo-/hi gain of one pixel
};

#endif

