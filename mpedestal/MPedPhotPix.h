#ifndef MARS_MPedPhotPix
#define MARS_MPedPhotPix

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef MARS_MMath
#include "MMath.h"
#endif

class MPedPhotPix : public TObject
{
private:
    Float_t fMean;      // [phot] mean value of pedestal (should be 0)
    Float_t fRms;       // [phot] rms of mean

    UInt_t  fNumEvents; // [n] number of events used to calculate mean (0=n/a)

public:
    MPedPhotPix();

    void Clear(Option_t *o="");

    Float_t GetMean() const      { return fMean; }
    Float_t GetRms() const       { return fRms; }
    UInt_t  GetNumEvents() const { return fNumEvents; }

    //void SetMean(Float_t f) { fMean = f; }
    void SetRms(Float_t f)  { MMath::ReducePrecision(f);  fRms  = f; }
    void Set(Float_t m, Float_t r, UInt_t n=1) { MMath::ReducePrecision(r); MMath::ReducePrecision(m); fMean = m; fRms = r; fNumEvents=n; }

    Bool_t IsValid() const { return fRms>=0; }

    ClassDef(MPedPhotPix, 2) // Storage Container for Pedestal information of one pixel in units of photons
};

#endif

