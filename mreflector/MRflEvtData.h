#ifndef MARS_MRflEvtData
#define MARS_MRflEvtData

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

class TH1;
class TH2;
class TH3;

class MRflSinglePhoton;

class MRflEvtData : public MParContainer, public MCamEvent
{
    TClonesArray fList;
    Int_t fPos;

public:
    MRflEvtData(const char *name=NULL, const char *title=NULL);
    MRflEvtData(const MRflEvtData &dat);

    TObject *Clone(Option_t *) const;

    void Reset() { fPos = 0; }

    Int_t GetNumPhotons() const { return fList.GetEntriesFast(); }

    MRflSinglePhoton &GetNewPhoton();
    void FixSize();

    const MRflSinglePhoton &GetPhoton(Int_t i) const;

    void FillRad(TH1 &hist, Float_t scale=1) const;
    void FillRad(TH2 &hist, Double_t x, Float_t scale=1) const;
    void Fill(TH2 &hist, Float_t scale=1) const;
    void Fill(TH3 &hist, Double_t z, Float_t scale=1) const;

    void Print(Option_t *o="") const;
    void Paint(Option_t *o="");

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void   DrawPixelContent(Int_t num) const;

    ClassDef(MRflEvtData, 0) // All Photons of a event from the reflector program
};

#endif
