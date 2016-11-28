#ifndef MARS_MPedestalPix
#define MARS_MPedestalPix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MPedestalPix : public MParContainer
{
private:
    Float_t fPedestal;         // mean value of pedestal (PMT offset)
    Float_t fPedestalRms;      // root mean square / sigma  / standard deviation of pedestal
    Float_t fPedestalABoffset; // the difference between odd slice pedestal mean and total mean
    UInt_t  fNumEvents;        // number of times, the Process was executed (to estimate the error of pedestal)

public:
    MPedestalPix();

    void Clear(Option_t *o="");
    void Copy(TObject &object) const;
    void Print(Option_t *o="") const;
    
    // Using histograms
    void InitUseHists();

    // Setters
    void SetPedestal(const Float_t f)         { fPedestal = f; }
    void SetPedestalRms(const Float_t f)      { fPedestalRms = f; }
    void SetPedestalABoffset(const Float_t f) { fPedestalABoffset = f; }
    void SetNumEvents(const UInt_t n)         { fNumEvents = n; }

    void Set(const Float_t m, const Float_t r, const Float_t offs=0, const UInt_t n=0);

    // Getters
    Float_t GetPedestal()    const { return fPedestal; }
    Float_t GetPedestalRms() const { return fPedestalRms; }
    Float_t GetPedestalABoffset() const { return fPedestalABoffset; }

    Float_t GetPedestalError() const;
    Float_t GetPedestalRmsError() const;

    UInt_t  GetNumEvents() const { return fNumEvents; }

    Bool_t IsValid() const;

    ClassDef(MPedestalPix, 3) // Storage Container for Pedestal information of one pixel
};

#endif
