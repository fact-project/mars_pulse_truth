#ifndef MARS_MExtractTimeAndChargeSpline
#define MARS_MExtractTimeAndChargeSpline

#ifndef MARS_MExtractTimeAndCharge
#include "MExtractTimeAndCharge.h"
#endif

#ifndef MARS_MExtralgoSpline
#include "MExtralgoSpline.h"
#endif

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

class MPedestalPix;

class MExtractTimeAndChargeSpline : public MExtractTimeAndCharge
{
private:
    static const Byte_t  fgHiGainFirst;      //! Default for fHiGainFirst  (now set to: 2)
    static const Byte_t  fgHiGainLast;       //! Default for fHiGainLast   (now set to: 14)
    static const Int_t   fgLoGainFirst;      //! Default for fLoGainFirst  (now set to: 2)
    static const Byte_t  fgLoGainLast;       //! Default for fLoGainLast   (now set to: 14)
    static const Float_t fgResolution;       //! Default for fResolution   (now set to: 0.003)
    static const Float_t fgRiseTimeHiGain;   //! Default for fRiseTime     (now set to: 1.5)
    static const Float_t fgFallTimeHiGain;   //! Default for fFallTime     (now set to: 4.5)
    static const Float_t fgLoGainStretch;    //! Default for fLoGainStretch    (now set to: 1.5)
    static const Float_t fgOffsetLoGain;     //! Default for fOffsetLoGain     (now set to 1.7)
//    static const Float_t fgLoGainStartShift; //! Default for fLoGainStartShift (now set to -1.6)

//    MArrayF fHiGainSignal;                   //! Need fast access to the signals in a float way
//    MArrayF fLoGainSignal;                   //! Store them in separate arrays
    MArrayF fHiGainFirstDeriv;               //! High-gain discretized first derivatives
    MArrayF fLoGainFirstDeriv;               //! Low-gain discretized first derivatives
    MArrayF fHiGainSecondDeriv;              //! High-gain discretized second derivatives
    MArrayF fLoGainSecondDeriv;              //! Low-gain discretized second derivatives

    Float_t fResolution;                     // The time resolution in FADC units

    Float_t fRiseTimeHiGain;                 // The usual rise time of the pulse in the high-gain
    Float_t fFallTimeHiGain;                 // The usual fall time of the pulse in the high-gain
    Float_t fRiseTimeLoGain;                 // The usual rise time of the pulse in the low-gain
    Float_t fFallTimeLoGain;                 // The usual fall time of the pulse in the low-gain

    Float_t fLoGainStretch;                  // The stretch of the low-gain w.r.t. the high-gain pulse
    Float_t fHeightTm;

//    Int_t   fRandomIter;                     //! Counter used to randomize weights for noise calculation

    Int_t   ReadEnv(const TEnv &env, TString prefix, Bool_t print);
    Bool_t  InitArrays(Int_t n);

private:
    MExtralgoSpline::ExtractionType_t fExtractionType;

public:
    MExtractTimeAndChargeSpline(const char *name=NULL, const char *title=NULL);

    Float_t GetRiseTimeHiGain() const { return fRiseTimeHiGain; }
    Float_t GetFallTimeHiGain() const { return fFallTimeHiGain; }

    void SetRange(UShort_t hifirst=0, UShort_t hilast=0, Int_t lofirst=0, Byte_t lolast=0 );

    void SetResolution(const Float_t f=fgResolution)  { fResolution  = f;  }

    void SetRiseTimeHiGain(const Float_t f=fgRiseTimeHiGain)
    {
        fRiseTimeHiGain    = f;
        fRiseTimeLoGain    = f*fLoGainStretch;
        fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
        fWindowSizeHiGain  = TMath::Nint(fRiseTimeHiGain + fFallTimeHiGain);
    }
    void SetFallTimeHiGain(const Float_t f=fgFallTimeHiGain)
    {
        fFallTimeHiGain    = f;
        fFallTimeLoGain    = f*fLoGainStretch;
        fNumHiGainSamples  = fRiseTimeHiGain + fFallTimeHiGain;
        fNumLoGainSamples  = fLoGainLast ? fRiseTimeLoGain + fFallTimeLoGain : 0.;
        fSqrtLoGainSamples = TMath::Sqrt(fNumLoGainSamples);
        fWindowSizeLoGain  = TMath::Nint(fRiseTimeLoGain + fFallTimeLoGain);
    }

    void SetHeightTm(const Double_t h) { fHeightTm = h; }

    void SetLoGainStretch(const Float_t f=fgLoGainStretch) { fLoGainStretch = f;   }

    void SetChargeType(const MExtralgoSpline::ExtractionType_t typ=MExtralgoSpline::kIntegralRel);
/*
    void FindTimeAndChargeHiGain(Byte_t *first, Byte_t *logain, Float_t &sum, Float_t &dsum,
                                 Float_t &time, Float_t &dtime,
                                 Byte_t &sat, const MPedestalPix &ped, const Bool_t abflag);
    void FindTimeAndChargeLoGain(Byte_t *first, Float_t &sum,  Float_t &dsum,
                                 Float_t &time, Float_t &dtime,
                                 Byte_t &sat, const MPedestalPix &ped, const Bool_t abflag);
*/
    void FindTimeAndChargeHiGain2(const Float_t *firstused, Int_t num, Float_t &sum, Float_t &dsum,
                                  Float_t &time, Float_t &dtime,
                                  Byte_t sat, Int_t maxpos) const;

    void FindTimeAndChargeLoGain2(const Float_t *firstused, Int_t num, Float_t &sum,  Float_t &dsum,
                                  Float_t &time, Float_t &dtime,
                                  Byte_t sat, Int_t maxpos) const;

    ClassDef(MExtractTimeAndChargeSpline, 5)   // Task to Extract Arrival Times and Charges using a Cubic Spline
};

#endif
