#ifndef MARS_MEventRate
#define MARS_MEventRate

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MEventRate : public MParContainer
{
private:
    Double_t fRate;      // [Hz] Event rate
    UInt_t   fNumEvents; // Number of events correspoding to this rate

public:
    MEventRate(const char *name=NULL, const char *title=NULL);

    void SetRate(Double_t r, UInt_t n) { fRate = r; fNumEvents = n; }
    Double_t GetRate() const { return fRate; }
    Double_t GetError() const;// { return TMath::Sqrt(1./fNumEvents); }
    UInt_t GetNumEvents() const { return fNumEvents; }

    ClassDef(MEventRate, 1) // Storage Container for the event rate
};

#endif

