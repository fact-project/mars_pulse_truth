#ifndef MARS_MExtralgoSpline
#define MARS_MExtralgoSpline

#ifndef ROOT_TMath
#include <TMath.h>
#endif

class MArrayF;
class TComplex;

class MExtralgoSpline
{
private:
    enum
    {
        kIntegral   = BIT(0),
        kTimeRel    = BIT(1),
        kMaximum    = BIT(2),
        kDynWidth   = BIT(3),
        kFixedWidth = BIT(4),
    };
public:  
    enum ExtractionType_t
    {
        // For backward compatibility
        kAmplitudeAbs  = 0,                    // Height of maximum, absolute height leading edge
        kAmplitudeRel  = kTimeRel,             // Height of maximum, relative height leading edge
        kAmplitude     = kMaximum,             // Position and height of maximum
        kIntegralAbs   = kIntegral,            // Integral, absolute height leading edge
        kIntegralRel   = kIntegral|kTimeRel,   // Integral, relative height leading edge
        kIntegralDyn   = kTimeRel|kDynWidth,   // Integrate between leading edge and falling edge
        kIntegralFixed = kTimeRel|kFixedWidth, // Integrate between leading edge minus fRiseTime and plus fFallTime
    };

private:
    ExtractionType_t fExtractionType;

private:
    //Bool_t fIsOwner; // Owner of derivatives....

    // Input
    Float_t const *fVal;
    const Int_t    fNum;

    Float_t *fDer1;
    Float_t *fDer2;

    Float_t fRiseTime;
    Float_t fFallTime;

    Float_t fHeightTm;

    // Result
    Float_t fTime;
    Float_t fTimeDev;
    Float_t fWidth;
    Float_t fWidthDev;
    Float_t fSignal;
    Float_t fSignalDev;
    Float_t fHeight;

    Double_t ReMul(const TComplex &c1, const TComplex &th) const;

    inline Float_t Eval(Float_t val, Float_t a, Float_t deriv) const
    {
        return a*val + (a*a*a-a)*deriv;
    }

    // Evaluate value of spline in the interval i with x=[0;1[
    inline Float_t Eval(const Int_t i, const Float_t x) const
    {
        // Eval(i,x) =  (fDer2[i+1]-fDer2[i])*x*x*x + 3*fDer2[i]*x*x +
        //              (fVal[i+1]-fVal[i] -2*fDer2[i]-fDer2[i+1])*x + fVal[i];

        // x := [0; 1[
        return Eval(fVal[i], 1-x, fDer2[i]) + Eval(fVal[i+1], x, fDer2[i+1]);
    }

    // Evaluate first derivative of spline in the interval i with x=[0;1[
    inline Double_t EvalDeriv1(const Int_t i, const Float_t x) const
    {
        // x := [0; 1[
        const Double_t difval = fVal[i+1]-fVal[i];
        const Double_t difder = fDer2[i+1]-fDer2[i];

        //return 3*difder*x*x + 6*fDer2[i]*x - 2*fDer2[i] - fDer2[i+1] + difval;
        return 3*difder*x*x + (6*x - 2)*fDer2[i] - fDer2[i+1] + difval;
    }

    // Evaluate second derivative of spline in the interval i with x=[0;1[
    inline Double_t EvalDeriv2(const Int_t i, const Float_t x) const
    {
        // x := [0; 1[
        return 6*(fDer2[i+1]*x + fDer2[i]*(1-x));
    }

    Int_t SolvePol3(Int_t i, Double_t y, Double_t &x1, Double_t &x2, Double_t &x3) const;
    Double_t FindYdn(Int_t i, Double_t y=0, Double_t min=0, Double_t max=1) const;
    Double_t FindYup(Int_t i, Double_t y=0, Double_t min=0, Double_t max=1) const;
    //Double_t FindY(Int_t i, Bool_t downwards, Double_t y=0, Double_t min=0, Double_t max=1) const;

    Int_t EvalDerivEq0(const Int_t i, Double_t &x1, Double_t &x2) const;
/*
    inline void EvalDerivEq0(const Int_t i, Float_t &rc1, Float_t &rc2) const
    {
        // --- ORIGINAL CODE ---
        Double_t sumder = fDer2[i]+fDer2[i+1];
        Double_t difder = fDer2[i]-fDer2[i+1];

        Double_t sqt1  = sumder*sumder - fDer2[i]*fDer2[i+1];
        Double_t sqt2  = difder*(fVal[i+1]-fVal[i]);
        Double_t sqt3  = sqrt(3*sqt1 + 3*sqt2);
        Double_t denom = -3*(fDer2[i+1]-fDer2[i]);

        rc1 = (3*fDer2[i] + sqt3)/denom;
        rc2 = (3*fDer2[i] - sqt3)/denom;

        // --- NEW CODE ---
        Double_t sumder = fDer2[i]+fDer2[i+1];
        Double_t difder = fDer2[i]-fDer2[i+1];

        Double_t sqt1  = sumder*sumder - fDer2[i]*fDer2[i+1];
        Double_t sqt2  = difder*(fVal[i+1]-fVal[i]);
        Double_t sqt3  = sqt1+sqt2<0 ? 0 : sqrt((sqt1 + sqt2)/3);

        rc1 = (fDer2[i] + sqt3)/difder;
        rc2 = (fDer2[i] - sqt3)/difder;
    }*/

    // Calculate the "Stammfunktion" of the Eval-function
    inline Double_t EvalPrimitive(Int_t i, Float_t x) const
    {
        Align(i, x);

        if (x==0)
            return -fDer2[i]/4;

        if (x==1)
            return (fVal[i+1] + fVal[i])/2 - fDer2[i+1]/4 - fDer2[i]/2;

        const Double_t x2  = x*x;
        const Double_t x4  = x2*x2;
        const Double_t x1  = 1-x;
        const Double_t x14 = x1*x1*x1*x1;

        return x2*fVal[i+1]/2 + (x4/2-x2)*fDer2[i+1]/2 + (x-x2/2)*fVal[i] + (x2/2-x-x14/4)*fDer2[i];

    }

    inline void Align(Int_t &i, Float_t &x) const
    {
        if (i<0)
        {
            x += i;
            i=0;
        }
        if (i>=fNum-1)
        {
            x += i-(fNum-2);
            i=fNum-2;
        }
    }

    // Calculate the intgeral of the Eval-function in
    // bin i from 0 <= a < b < 1
    inline Double_t EvalInteg(Int_t i, Float_t a, Float_t b) const
    {
        return EvalPrimitive(i, b)-EvalPrimitive(i, a);
    }

    // Identical to EvalInteg(i, 0, 1) but much faster
    // Be carefull: NO RANGECHECK!
    inline Double_t EvalInteg(Int_t i) const
    {
        return (fVal[i+1] + fVal[i])/2 - (fDer2[i+1] + fDer2[i])/4;
    }

    // Identical to sum of EvalInteg(i, 0, 1) for i=a to i=b-1,
    // but much faster
    // It is identical to EvalInteg(fVal[a], fVal[b])
    // Be carefull: NO RANGECHECK!
    inline Double_t EvalInteg(Int_t a, Int_t b) const
    {
       /*
        Double_t sum = 0;
        for (int i=a; i<b; i++)
            sum += EvalInteg(i);

        return sum;
        */

        if (a==b)
            return 0;

        Double_t sum=0;
        for (const Float_t *ptr=fDer2+a+1; ptr<fDer2+b; ptr++)
            sum -= *ptr;

        sum -= (fDer2[a]+fDer2[b])/2;

        sum /= 2;

        for (const Float_t *ptr=fVal+a+1; ptr<fVal+b; ptr++)
            sum += *ptr;

        sum += (fVal[a]+fVal[b])/2;

        return sum;
    }

    // Calculate the intgeral of the Eval-function betwen x0 and x1
    inline Double_t EvalInteg(Float_t x0, Float_t x1) const
    {
        // RANGE CHECK MISSING!

        const Int_t min = TMath::CeilNint(x0);
        const Int_t max = TMath::FloorNint(x1);

        // This happens if x0 and x1 are in the same interval
        if (min>max)
            return EvalInteg(max, x0-max, x1-max);

        // Sum complete intervals
        Double_t sum = EvalInteg(min, max);

        // Sum the incomplete intervals at the beginning and end
        sum += EvalInteg(min-1, 1-(min-x0), 1);
        sum += EvalInteg(max,   0, x1-max);

        // return result
        return sum;
    }

    // We search for the maximum from x=i-1 to x=i+1
    // (Remeber: i corresponds to the value in bin i, i+1 to the
    //  next bin and i-1 to the last bin)
    inline void GetMaxAroundI(Int_t i, Float_t &xmax, Float_t &ymax) const
    {
        Float_t xmax1=0, xmax2=0;
        Float_t ymax1=0, ymax2=0;

        Bool_t rc1 = i>0      && GetMax(i-1, xmax1, ymax1);
        Bool_t rc2 = i<fNum-1 && GetMax(i,   xmax2, ymax2);

        // In case the medium bin is the first or last bin
        // take the lower or upper edge of the region into account.
        if (i==0)
        {
            xmax1 = 0;
            ymax1 = fVal[0];
            rc1 = kTRUE;
        }
        if (i>=fNum-1)
        {
            xmax2 = fNum-1;
            ymax2 = fVal[fNum-1];
            rc2 = kTRUE;
        }

        // Take a default in case no maximum is found
        // FIXME: Check THIS!!!
        xmax=i;
        ymax=fVal[i];

        if (rc1)
        {
            ymax = ymax1;
            xmax = xmax1;
        }
        else
            if (rc2)
            {
                ymax = ymax2;
                xmax = xmax2;
            }

        if (rc2 && ymax2>ymax)
        {
            ymax = ymax2;
            xmax = xmax2;
        }
    }

    inline Bool_t GetMax(Int_t i, Float_t &xmax, Float_t &ymax, Float_t min=0, Float_t max=1) const
    {
        // Find analytical maximum in the bin i in the interval [min,max[

        Double_t x1=-1;  // This initialisation should not really be
        Double_t x2=-1;  // necessary but makes valgriund happy.

        if (!EvalDerivEq0(i, x1, x2))
            return kFALSE;

        const Bool_t ismax1 = x1>=min && x1<max && EvalDeriv2(i, x1)<0;
        const Bool_t ismax2 = x2>=min && x2<max && EvalDeriv2(i, x2)<0;

        if (!ismax1 && !ismax2)
            return kFALSE;

        if (ismax1 && !ismax2)
        {
            xmax = i+x1;
            ymax = Eval(i, x1);
            return kTRUE;
        }

        if (!ismax1 && ismax2)
        {
            xmax = i+x2;
            ymax = Eval(i, x2);
            return kTRUE;
        }

        // Somehting must be wrong...
        return kFALSE;
    }

    void InitDerivatives() const;
    Float_t CalcIntegral(Float_t start) const;
    Float_t CalcIntegral(Float_t beg, Float_t width) const;

public:
    MExtralgoSpline(const Float_t *val, Int_t n, Float_t *der1, Float_t *der2)
        : fExtractionType(kIntegralRel), fVal(val), fNum(n), fDer1(der1), fDer2(der2), fHeightTm(0.5), fTime(0), fTimeDev(-1), fSignal(0), fSignalDev(-1)
    {
        InitDerivatives();
    }

    void SetRiseFallTime(Float_t rise, Float_t fall) { fRiseTime=rise; fFallTime=fall; }
    void SetExtractionType(ExtractionType_t typ)     { fExtractionType = typ; }
    void SetHeightTm(Float_t h)                      { fHeightTm = h; }

    Float_t GetTime() const      { return fTime; }
    Float_t GetWidth() const     { return fWidth; }
    Float_t GetSignal() const    { return fSignal; }
    Float_t GetHeight() const    { return fHeight; }

    Float_t GetTimeDev() const   { return fTimeDev; }
    Float_t GetWidthDev() const  { return fWidthDev; }
    Float_t GetSignalDev() const { return fSignalDev; }

    void GetSignal(Float_t &sig, Float_t &dsig) const { sig=fSignal; dsig=fSignalDev; }
    void GetWidth(Float_t &sig, Float_t &dsig) const  { sig=fWidth; dsig=fWidthDev; }
    void GetTime(Float_t &sig, Float_t &dsig) const   { sig=fTime; dsig=fTimeDev; }

    Float_t ExtractNoise(/*Int_t iter*/);
    void Extract(Int_t maxpos, Bool_t width=kFALSE);

    Float_t EvalAt(const Float_t x) const;
    Float_t Deriv1(const Float_t x) const;

    Double_t SearchYdn(Double_t maxpos, Double_t y) const;
    Double_t SearchYup(Double_t maxpos, Double_t y) const;

    Double_t SearchYdn(Float_t y) const { return SearchYdn(fNum, y); }
    Double_t SearchYup(Float_t y) const { return SearchYup(0,    y); }

    MArrayF GetIntegral(bool norm=false) const;

    Float_t GetIntegral(Float_t beg, Float_t end) const
    {
        return CalcIntegral(beg, end-beg);
    }
};

inline Float_t MExtralgoSpline::EvalAt(const Float_t x) const
{
    Int_t   i = TMath::FloorNint(x);
    Float_t f = x-i;

    Align(i, f);

    return Eval(i, f);
}

inline Float_t MExtralgoSpline::Deriv1(const Float_t x) const
{
    Int_t   i = TMath::FloorNint(x);
    Float_t f = x-i;

    Align(i, f);

    return EvalDeriv1(i, f);
}

#endif
