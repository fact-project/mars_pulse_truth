#ifndef MARS_MAlphaFitter
#define MARS_MAlphaFitter

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class TF1;
class TH1D;
class TH3D;

class MAlphaFitter : public MParContainer
{
public:
    enum ScaleMode_t {
        kNone,         // No scaling
        kEntries,      // scale by the number of entries in on and off
        kIntegral,     // scale by the integral in on and off
        kOffRegion,    // scale by the integral between fScaleMin, fScaleMax in on and off
        kBackground,   // scale by the integral between fBgMin, fBgMax in on and off
        kLeastSquare,  // not yet implemented
        kUserScale     // scale by fixed factor set by SetScaleUser
    };
    enum Strategy_t {
        kSignificance,
        kSignificanceChi2,
        kSignificanceLogExcess,
        kSignificanceSqrtExcess,
        kSignificanceExcess,
        kExcess,
        kGaussSigma,
        kWeakSource,
        kWeakSourceLogExcess
    };
    enum SignalFunc_t {
        kGauss, kThetaSq
    };

private:
    // Fitting Setup
    Float_t fSigInt;            // minimum of range to fit the signal
    Float_t fSigMax;            // maximum of range to fit the signal
    Float_t fBgMin;             // minimum of range to fit the background
    Float_t fBgMax;             // minimum of range to fit the background
    Float_t fScaleMin;          // minimum of range to determin the scale factor of the background
    Float_t fScaleMax;          // maximum of range to determin the scale factor of the background
    Int_t   fPolynomOrder;      // order of polyom to be fitted to the background
    Bool_t  fFitBackground;     // Backround fit: yes/no
    SignalFunc_t fSignalFunc;   // Type of signal function
    // Result
    Double_t fSignificance;     // significance of an unknown signal (Li/Ma 17)
    Double_t fErrorExcess;      // Simple error propagation
    Double_t fEventsExcess;     // calculated number of excess events (signal-bg)
    Double_t fEventsSignal;     // calculated number of signal events
    Double_t fEventsBackground; // calculated number of bg events (fScaleFactor already applied)

    Double_t fChiSqSignal;      // Reduced (chi^2/NDF) chisq of signal fit
    Double_t fChiSqBg;          // Reduced (chi^2/NDF) chisq of bg fit
    Double_t fIntegralMax;      // Calculated bin border to which it was integrated
    Double_t fScaleFactor;      // Scale factor determined for off-data

    TArrayD fCoefficients;      // Fit result
    TArrayD fErrors;            // Fit errors

    // Function
    TF1 *fFunc;                 // fit function (gauss + polynom)

    // Scaling setup
    ScaleMode_t fScaleMode;     // scaling mode
    Double_t    fScaleUser;     // user scale factor

    // Minimization strategy
    Strategy_t fStrategy;       // How to calc minimization value

    Double_t DoOffFit(const TH1D &hon, const TH1D &hof, Bool_t paint);
    Bool_t   FitOff(TH1D &h, Int_t paint);
    void     FitResult(const TH1D &h);

public:
    // Implementing the function yourself is only about 5% faster
    MAlphaFitter(const char *name=0, const char *title=0);
    MAlphaFitter(const MAlphaFitter &f) : MParContainer(f), fFunc(0)
    {
        f.Copy(*this);
    }
    ~MAlphaFitter();

    // TObject
    void Clear(Option_t *o="");
    void Print(Option_t *o="") const; //*MENU*
    void Copy(TObject &o) const;

    // Setter
    void SetScaleUser(Float_t scale)       { fScaleUser = scale; fScaleMode=kUserScale; }
    void SetScaleMode(ScaleMode_t mode)    { fScaleMode    = mode; }
    void SetMinimizationStrategy(Strategy_t mode) { fStrategy = mode; }
    void SetSignalIntegralMax(Float_t s)   { fSigInt       = s; }
    void SetSignalFitMax(Float_t s)        { fSigMax       = s; }
    void SetBackgroundFitMin(Float_t s)    { fBgMin        = s; }
    void SetBackgroundFitMax(Float_t s)    { fBgMax        = s; }
    void SetScaleMin(Float_t s)            { fScaleMin     = s; }
    void SetScaleMax(Float_t s)            { fScaleMax     = s; }
    void SetPolynomOrder(Int_t s)
    {
        if (s==fPolynomOrder)
            return;

        fPolynomOrder = s;

        SetSignalFunction(fSignalFunc);
    }
    void SetSignalFunction(SignalFunc_t func);

    void EnableBackgroundFit(Bool_t b=kTRUE) { fFitBackground=b; }

    void FixScale()
    {
        fScaleMode = kUserScale;
        fScaleUser = fScaleFactor;
    }

    // Getter
    Double_t GetSignalIntegralMax() const  { return fSigInt; }

    Double_t GetEventsExcess() const       { return fEventsExcess; }
    Double_t GetEventsSignal() const       { return fEventsSignal; }
    Double_t GetEventsBackground() const   { return fEventsBackground; }

    Double_t GetSignificance() const       { return fSignificance; }
    Double_t GetErrorExcess() const        { return fErrorExcess; }
    Double_t GetChiSqSignal() const        { return fChiSqSignal; }
    Double_t GetChiSqBg() const            { return fChiSqBg; }
    Double_t GetScaleFactor() const        { return fScaleFactor; }
    Double_t GetMinimizationValue() const;

    ScaleMode_t GetScaleMode() const       { return fScaleMode; }

    Double_t GetGausSigma() const          { return fCoefficients[2]; }
    Double_t GetGausMu() const             { return fCoefficients[1]; }
    Double_t GetGausA() const              { return fCoefficients[0]; }
    Double_t GetCoefficient(Int_t i) const { return fCoefficients[i]; }
    const TArrayD &GetCoefficients() const { return fCoefficients; }
    const TArrayD &GetErrors() const       { return fErrors; }
    Double_t Eval(Double_t d) const;

    Double_t CalcUpperLimit() const;

    // Interface to fit
    Bool_t Fit(TH1D &h, Bool_t paint=kFALSE);
    Bool_t Fit(const TH1D &on, const TH1D &off, Double_t alpha, Bool_t paint=kFALSE);
    Bool_t Fit(TH1D &on, TH1D *off, Double_t alpha, Bool_t paint=kFALSE)
    {
        return off ? Fit(on, *off, alpha, paint) : Fit(on, paint);
    }
    Bool_t Fit(TH1D &on, TH1D *off, Bool_t paint=kFALSE)
    {
        return off ? Fit(on, *off, 1, paint) : Fit(on, paint);
    }
    Bool_t ScaleAndFit(TH1D &on, TH1D *off, Bool_t paint=kFALSE)
    {
        const Double_t alpha = off ? Scale(*off, on) : 1;
        return off ? Fit(on, *off, alpha, paint) : Fit(on, paint);
    }

    Bool_t FitAlpha(const TH3D &h, Bool_t paint=kFALSE);
    Bool_t FitEnergy(const TH3D &h, UInt_t bin, Bool_t paint=kFALSE);
    Bool_t FitTheta(const TH3D &h,  UInt_t bin, Bool_t paint=kFALSE);
    //Bool_t FitTime(const TH3D &h,  UInt_t bin, Bool_t paint=kFALSE);

    Bool_t FitAlpha(const TH3D &on, const TH3D &off, Bool_t paint=kFALSE);
    Bool_t FitEnergy(const TH3D &on, const TH3D &off, UInt_t bin, Bool_t paint=kFALSE);
    Bool_t FitTheta(const TH3D &on, const TH3D &off, UInt_t bin, Bool_t paint=kFALSE);
    //Bool_t FitTime(const TH3D &on, const TH3D &off, UInt_t bin, Bool_t paint=kFALSE);

    Bool_t FitAlpha(const TH3D &on, const TH3D *off, Bool_t paint=kFALSE)
    {
        return off ? FitAlpha(on, *off, paint) : FitAlpha(on, paint);
    }
    Bool_t FitEnergy(const TH3D &on, const TH3D *off, UInt_t bin, Bool_t paint=kFALSE)
    {
        return off ? FitEnergy(on, *off, bin, paint) : FitEnergy(on, bin, paint);
    }
    Bool_t FitTheta(const TH3D &on, const TH3D *off, UInt_t bin, Bool_t paint=kFALSE)
    {
        return off ? FitTheta(on, *off, bin, paint) : FitTheta(on, bin, paint);
    }/*
    Bool_t FitTime(const TH3D &on, const TH3D *off, UInt_t bin, Bool_t paint=kFALSE)
    {
        return off ? FitTime(on, *off, bin, paint) : FitTime(on, bin, paint);
    }*/

    Double_t Scale(TH1D &off, const TH1D &on) const;

    Bool_t ApplyScaling(const TH3D &hon, TH3D &hof, UInt_t bin) const;
    Bool_t ApplyScaling(const TH3D &hon, TH3D &hof) const;

    // Interface to result
    void PaintResult(Float_t x=0.04, Float_t y=0.94, Float_t size=0.035, Bool_t draw=kFALSE) const;
    void DrawResult(Float_t x=0.04, Float_t y=0.94, Float_t size=0.035) const { PaintResult(x, y, size, kTRUE); }

    // MTask
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MAlphaFitter, 4)
};

#endif
