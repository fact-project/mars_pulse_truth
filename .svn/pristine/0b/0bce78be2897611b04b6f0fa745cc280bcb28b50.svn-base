#ifndef MARS_MAnalogSignal
#define MARS_MAnalogSignal

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

class TF1;
class MSpline3;

class MAnalogSignal : public MArrayF/*TObject*/
{
private:
    MArrayF fDer1; //! Buffer for the derivatives of the corresponding spline
    MArrayF fDer2; //! Buffer for the derivatives of the corresponding spline

public:
    MAnalogSignal(UInt_t n) { Set(n); }

    void   Set(UInt_t n);
    Bool_t AddPulse(const MSpline3 &spline, Float_t t, Float_t f=1);
    Bool_t AddPulse(const TF1 &f1, Float_t t, Float_t f=1);
    void   AddSignal(const MAnalogSignal &s, Int_t delay=0,Float_t dampingFact=1.0);

    // Deprecated. Use MSimRandomPhotons instead
    void AddRandomPulses(const MSpline3 &spline, Float_t num);

    void AddGaussianNoise(Float_t amplitude=1, Float_t offset=0);

    TObjArray *Discriminate(Float_t threshold, Double_t start, Double_t end, Float_t len=-1) const;
    TObjArray *Discriminate(Float_t threshold, Float_t len=-1) const { return Discriminate(threshold, 0, fN-1, len); }

    ClassDef(MAnalogSignal, 1) // Storage class for an analog signal
};

#endif
