#ifndef MARS_MAnalogChannels
#define MARS_MAnalogChannels

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TObjArray;
class MAnalogSignal;

class MAnalogChannels : public MParContainer
{
private:
    TObjArray *fArray;

    Int_t fValidRangeMin;
    Int_t fValidRangeMax;

public:
    MAnalogChannels(const char *name=0, const char *title=0);
    MAnalogChannels(Int_t n, Int_t len, const char *name=0, const char *title=0);
    ~MAnalogChannels()
    {
        Clear();
    }

    void Clear(Option_t *o="");

    void Init(UInt_t n, UInt_t len);

    void InitNumChannels(UInt_t n) { Init(n, GetNumSamples());  }
    void InitLenChannels(UInt_t l) { Init(GetNumChannels(), l); }

    MAnalogSignal &operator[](UInt_t i);
    MAnalogSignal *operator()(UInt_t i);

    const MAnalogSignal &operator[](UInt_t i) const;
    const MAnalogSignal *operator()(UInt_t i) const;

    UInt_t GetNumChannels() const;
    UInt_t GetNumSamples() const;

    void SetValidRange(Int_t min, Int_t max) { fValidRangeMin=min; fValidRangeMax=max; }

    Int_t GetValidRangeMin() const { return fValidRangeMin; }
    Int_t GetValidRangeMax() const { return fValidRangeMax; }

    ClassDef(MAnalogChannels, 1) // Parameter container for a collection of analog signals
};

#endif
