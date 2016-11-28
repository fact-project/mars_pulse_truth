/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz, 3/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MAlphaFitter
//
// Create a single Alpha-Plot. The alpha-plot is fitted online. You can
// check the result when it is filles in the MStatusDisplay
// For more information see MHFalseSource::FitSignificance
//
// For convinience (fit) the output significance is stored in a
// container in the parlisrt
//
// Version 2:
// ----------
//  + Double_t fSignificanceExc;  // significance of a known excess
//
// Version 3:
// ----------
//  + TArrayD fErrors;  // errors of coefficients
//
// Version 4:
// ----------
//  + Double_t fErrorExcess;
//  - Double_t fSignificanceExc;
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MAlphaFitter.h"

#include <TF1.h>
#include <TH1.h>
#include <TH3.h>

#include <TRandom.h>
#include <TFeldmanCousins.h>

#include <TLine.h>
#include <TLatex.h>
#include <TVirtualPad.h>

#include "MMath.h"
#include "MString.h"

#include "MLogManip.h"

ClassImp(MAlphaFitter);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MAlphaFitter::MAlphaFitter(const char *name, const char *title) : fSigInt(15),
    fSigMax(75), fBgMin(45), fBgMax(85), fScaleMin(40), fScaleMax(80),
    fPolynomOrder(2), fFitBackground(kTRUE), fFunc(0),
    fScaleMode(kOffRegion), fScaleUser(1), fStrategy(kSignificance)
{
    fName  = name  ? name  : "MAlphaFitter";
    fTitle = title ? title : "Fit alpha";

    SetSignalFunction(kGauss);

    Clear();
}

// --------------------------------------------------------------------------
//
// Destructor
//
MAlphaFitter::~MAlphaFitter()
{
    delete fFunc;
}

// --------------------------------------------------------------------------
//
// Re-initializes fFunc either according to SignalFunc_t
//
void MAlphaFitter::SetSignalFunction(SignalFunc_t func)
{
    if (gROOT->GetListOfFunctions()->FindObject(""))
    {
        gLog << err << "MAlphaFitter::SetSignalFunction -- '' found!" << endl;
        return;
    }

    delete fFunc;
    fFunc = 0;

    switch (func)
    {
    case kGauss:
        fFunc=new TF1("", MString::Format("gaus(0) + pol%d(3)", fPolynomOrder).Data());
        break;
    case kThetaSq:
        if (fPolynomOrder>0)
            fPolynomOrder = 1;
        fFunc=new TF1("", "[0]*exp(-0.5*((sqrt(x)-[1])/[2])^2) + expo(3)");
        break;
    }

    fSignalFunc=func;

    fFunc->SetName("Dummy");
    gROOT->GetListOfFunctions()->Remove(fFunc);

    fCoefficients.Set(3+fPolynomOrder+1);
    fCoefficients.Reset();

    fErrors.Set(3+fPolynomOrder+1);
    fErrors.Reset();
}

// --------------------------------------------------------------------------
//
// Reset variables which belong to results. Reset the arrays.
//
void MAlphaFitter::Clear(Option_t *o)
{
    fSignificance=0;
    fErrorExcess=0;
    fEventsExcess=0;
    fEventsSignal=0;
    fEventsBackground=0;

    fChiSqSignal=0;
    fChiSqBg=0;
    fIntegralMax=0;
    fScaleFactor=1;

    fCoefficients.Reset();
    fErrors.Reset();
}

// --------------------------------------------------------------------------
//
// Returns fFunc->Eval(d) or 0 if fFunc==NULL
//
Double_t MAlphaFitter::Eval(Double_t d) const
{
    return fFunc ? fFunc->Eval(d) : 0;
}

// --------------------------------------------------------------------------
//
// This function implementes the fit to the off-data as used in Fit()
//
Bool_t MAlphaFitter::FitOff(TH1D &h, Int_t paint)
{
    if (h.GetEntries()==0)
        return kFALSE;

    // First fit a polynom in the off region
    fFunc->FixParameter(0, 0);
    fFunc->FixParameter(1, 0);
    fFunc->FixParameter(2, 1);
    fFunc->ReleaseParameter(3);
    if (fPolynomOrder!=1)
        fFunc->FixParameter(4, 0);

    for (int i=5; i<fFunc->GetNpar(); i++)
        if (fFitBackground)
            fFunc->ReleaseParameter(i);
        else
            fFunc->SetParameter(i, 0);

    if (!fFitBackground)
        return kTRUE;

    if (fSignalFunc==kThetaSq)
    {
        const Double_t sum = h.Integral(1, 3)/3;
        const Double_t a   = sum<=1 ? 0 : TMath::Log(sum);
        const Double_t b   = -1.7;

        // Do a best-guess
        fFunc->SetParameter(3, a);
        fFunc->SetParameter(4, b);
    }

    // options : N  do not store the function, do not draw
    //           I  use integral of function in bin rather than value at bin center
    //           R  use the range specified in the function range
    //           Q  quiet mode
    //           E  Perform better Errors estimation using Minos technique
    if (h.Fit(fFunc, "NQI", "", fBgMin, fBgMax))
        return kFALSE;

    fChiSqBg = fFunc->GetChisquare()/fFunc->GetNDF();

    fCoefficients.Set(fFunc->GetNpar(), fFunc->GetParameters());
    fErrors.Set(fFunc->GetNpar());
    for (int i=3; i<fFunc->GetNpar(); i++)
        fErrors[i] = fFunc->GetParError(i);

    // ------------------------------------

    if (paint)
    {
        if (paint==2)
        {
            fFunc->SetLineColor(kBlack);
            fFunc->SetLineWidth(1);
        }
        else
        {
            fFunc->SetRange(0, 90);
            fFunc->SetLineColor(kRed);
            fFunc->SetLineWidth(2);
        }
        fFunc->Paint("same");
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calculate the result of the fit and set the corresponding data members
//
void MAlphaFitter::FitResult(const TH1D &h)
{
    const Double_t alphaw = h.GetXaxis()->GetBinWidth(1);

    const Int_t bin = h.GetXaxis()->FindFixBin(fSigInt*0.999);

    fIntegralMax      = h.GetBinLowEdge(bin+1);
    fEventsBackground = fFunc->Integral(0, fIntegralMax)/alphaw;
    fEventsSignal     = h.Integral(1, bin);
    fEventsExcess     = fEventsSignal-fEventsBackground;
    fSignificance     = MMath::SignificanceLiMaSigned(fEventsSignal, fEventsBackground);
    fErrorExcess      = MMath::ErrorExc(fEventsSignal, fEventsBackground);

    // !Finitite includes IsNaN
    if (!TMath::Finite(fSignificance))
        fSignificance=0;
}

// --------------------------------------------------------------------------
//
// This is a preliminary implementation of a alpha-fit procedure for
// all possible source positions. It will be moved into its own
// more powerfull class soon.
//
// The fit function is "gaus(0)+pol2(3)" which is equivalent to:
//   [0]*exp(-0.5*((x-[1])/[2])^2) + [3] + [4]*x + [5]*x^2
// or
//   A*exp(-0.5*((x-mu)/sigma)^2) + a + b*x + c*x^2
//
// Parameter [1] is fixed to 0 while the alpha peak should be
// symmetric around alpha=0.
//
// Parameter [4] is fixed to 0 because the first derivative at
// alpha=0 should be 0, too.
//
// In a first step the background is fitted between bgmin and bgmax,
// while the parameters [0]=0 and [2]=1 are fixed.
//
// In a second step the signal region (alpha<sigmax) is fittet using
// the whole function with parameters [1], [3], [4] and [5] fixed.
//
// The number of excess and background events are calculated as
//   s = int(hist,    0, 1.25*sigint)
//   b = int(pol2(3), 0, 1.25*sigint)
//
// The Significance is calculated using the Significance() member
// function.
//
Bool_t MAlphaFitter::Fit(TH1D &h, Bool_t paint)
{
    Clear();

    // Check for the region which is not filled...
    // if (alpha0==0)
    //     return kFALSE;

    // Perform fit to the off-data
    if (!FitOff(h, paint))
        return kFALSE;

    fFunc->ReleaseParameter(0);  // It is also released by SetParLimits later on
    //func.ReleaseParameter(1);  // It is also released by SetParLimits later on
    fFunc->ReleaseParameter(2);
    for (int i=3; i<fFunc->GetNpar(); i++)
        fFunc->FixParameter(i, fFunc->GetParameter(i));


    // Do not allow signals smaller than the background
    const Double_t alpha0 = h.GetBinContent(1);
    const Double_t s      = fSignalFunc==kGauss ? fFunc->GetParameter(3) : TMath::Exp(fFunc->GetParameter(3));
    const Double_t A      = alpha0-s;
    //const Double_t dA     = TMath::Abs(A);
    //fFunc->SetParLimits(0, -dA*4, dA*4);  // SetParLimits also releases the parameter
    fFunc->SetParLimits(2, 0, 90);        // SetParLimits also releases the parameter

    // Now fit a gaus in the on region on top of the polynom
    fFunc->SetParameter(0, A);
    fFunc->SetParameter(2, fSigMax*0.75);

    // options : N  do not store the function, do not draw
    //           I  use integral of function in bin rather than value at bin center
    //           R  use the range specified in the function range
    //           Q  quiet mode
    //           E  Perform better Errors estimation using Minos technique
    h.Fit(fFunc, "NQI", "", 0, fSigMax);

    fChiSqSignal = fFunc->GetChisquare()/fFunc->GetNDF();
    fCoefficients.Set(fFunc->GetNpar(), fFunc->GetParameters());
    for (int i=0; i<3; i++)
        fErrors[i] = fFunc->GetParError(i);
    //const Bool_t ok = NDF>0 && chi2<2.5*NDF;

    // ------------------------------------
    if (paint)
    {
        fFunc->SetLineColor(kGreen);
        fFunc->SetLineWidth(2);
        fFunc->Paint("same");
    }
    // ------------------------------------

    //const Double_t s = fFunc->Integral(0, fSigInt)/alphaw;
    fFunc->SetParameter(0, 0);
    fFunc->SetParameter(2, 1);
    //const Double_t b = fFunc->Integral(0, fSigInt)/alphaw;
    //fSignificance = MMath::SignificanceLiMaSigned(s, b);

    // Calculate the fit result and set the corresponding data members
    FitResult(h);

    return kTRUE;
}

Double_t MAlphaFitter::DoOffFit(const TH1D &hon, const TH1D &hof, Bool_t paint)
{
    if (fSignalFunc!=kThetaSq)
        return 0;

    // ----------------------------------------------------------------------------

    const Int_t bin = hon.GetXaxis()->FindFixBin(fSigInt*0.999);

    MAlphaFitter fit(*this);
    fit.EnableBackgroundFit();
    fit.SetBackgroundFitMin(0);

    // produce a histogram containing the off-samples from on-source and
    // off-source in the off-source region and the on-data in the source-region
    TH1D h(hof);
    h.SetDirectory(0);
    h.Add(&hon);

    h.Scale(0.5);
    for (int i=1; i<=bin+3; i++)
    {
        h.SetBinContent(i, hof.GetBinContent(i));
        h.SetBinError(  i, hof.GetBinError(i));
    }

    // Now fit the off-data
    if (!fit.FitOff(h, paint?2:0)) // FIXME: Show fit!
        return -1;

    // Calculate fit-result
    fit.FitResult(h);

    // Do a gaussian error propagation to calculated the error of
    // the background estimated from the fit
    const Double_t ea = fit.fErrors[3];
    const Double_t eb = fit.fErrors[4];
    const Double_t a  = fit.fCoefficients[3];
    const Double_t b  = fit.fCoefficients[4];

    const Double_t t  = fIntegralMax;

    const Double_t ex  = TMath::Exp(t*b);
    const Double_t eab = TMath::Exp(a)/b;

    const Double_t eA = ex-1;
    const Double_t eB = t*ex - eA/b;

    const Double_t w  = h.GetXaxis()->GetBinWidth(1);

    // Error of estimated background
    const Double_t er = TMath::Abs(eab)*TMath::Hypot(eA*ea, eB*eb)/w;

    // Calculate arbitrary scale factor from propagated error from the
    // condition: sqrt(alpha*background) = est.background/est.error
    // const Double_t bg = hof.Integral(1, bin);
    // const Double_t sc = bg * er*er / (fit2.GetEventsBackground()*fit2.GetEventsBackground());
    // Assuming that bg and fit2.GetEventsBackground() are rather identical:
    const Double_t sc = er*er / fit.fEventsBackground;


    /*
     cout << MMath::SignificanceLiMaSigned(hon.Integral(1, bin), fit.GetEventsBackground()/sc, sc) << " ";
     cout << sc << " " << fit.GetEventsBackground() << " ";
     cout << fit.fChiSqBg << endl;
     */
    return sc;
}

Bool_t MAlphaFitter::Fit(const TH1D &hon, const TH1D &hof, Double_t alpha, Bool_t paint)
{
    TH1D h(hon);
    h.SetDirectory(0);
    h.Add(&hof, -1); // substracts also number of entries!
    h.SetEntries(hon.GetEntries());

    MAlphaFitter fit(*this);
    fit.SetPolynomOrder(0);
    if (alpha<=0 || !fit.Fit(h, paint))
        return kFALSE;

    fChiSqSignal  = fit.fChiSqSignal;
    fChiSqBg      = fit.fChiSqBg;
    fCoefficients = fit.fCoefficients;
    fErrors       = fit.fErrors;

    // ----------------------------------------------------------------------------

    const Double_t scale = DoOffFit(hon, hof, paint);
    if (scale<0)
        return kFALSE;

    // ----------------------------------------------------------------------------

    const Int_t bin = hon.GetXaxis()->FindFixBin(fSigInt*0.999);

    fIntegralMax      = hon.GetBinLowEdge(bin+1);
    fEventsBackground = hof.Integral(1, bin);
    fEventsSignal     = hon.Integral(1, bin);
    fEventsExcess     = fEventsSignal-fEventsBackground;
    fScaleFactor      = alpha;
    fSignificance     = MMath::SignificanceLiMaSigned(fEventsSignal, fEventsBackground/alpha, alpha);
    fErrorExcess      = MMath::ErrorExc(fEventsSignal, fEventsBackground/alpha, alpha);

    // !Finitite includes IsNaN
    if (!TMath::Finite(fSignificance))
        fSignificance=0;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calculate the upper limit for fEventsSignal number of observed events
// and fEventsBackground number of background events.
//
// Therefor TFeldmanCousin is used.
//
// The Feldman-Cousins method as described in PRD V57 #7, p3873-3889
//
Double_t MAlphaFitter::CalcUpperLimit() const
{
    // get a FeldmanCousins calculation object with the default limits
    // of calculating a 90% CL with the minimum signal value scanned
    // = 0.0 and the maximum signal value scanned of 50.0
    TFeldmanCousins f;
    f.SetMuStep(0.05);
    f.SetMuMax(100);
    f.SetMuMin(0);
    f.SetCL(90);

    return f.CalculateUpperLimit(fEventsSignal, fEventsBackground);
}

void MAlphaFitter::PaintResult(Float_t x, Float_t y, Float_t size, Bool_t draw) const
{
    const Double_t w  = GetGausSigma();
    const Double_t m  = fIntegralMax;

    const Int_t    l1 = w<=0 ? 0 : (Int_t)TMath::Ceil(-TMath::Log10(w));
    const Int_t    l2 = m<=0 ? 0 : (Int_t)TMath::Ceil(-TMath::Log10(m));
    const TString fmt = MString::Format("\\sigma_{L/M}=%%.1f  \\omega=%%.%df\\circ  E=%%d B=%%d  x<%%.%df  \\tilde\\chi_{b}=%%.1f  \\tilde\\chi_{s}=%%.1f  c=%%.1f  f=%%.2f",
                                        l1<1?1:l1+1, l2<1?1:l2+1);
    const TString txt = MString::Format(fmt.Data(), fSignificance, w, (int)fEventsExcess,
                                        (int)fEventsBackground, m, fChiSqBg, fChiSqSignal,
                                        fCoefficients[3], fScaleFactor);

    // This is totaly weired but the only way to get both options
    // working with this nonsense implementation of TLatex
    TLatex text(x, y, txt);
    text.SetBit(TLatex::kTextNDC);
    text.SetTextSize(size);
    if (draw)
        text.DrawLatex(x, y, txt);
    else
        text.Paint();

    TLine line;
    line.SetLineColor(14);
    if (draw)
        line.DrawLine(m, gPad->GetUymin(), m, gPad->GetUymax());
    else
        line.PaintLine(m, gPad->GetUymin(), m, gPad->GetUymax());
}

void MAlphaFitter::Copy(TObject &o) const
{
    MAlphaFitter &f = static_cast<MAlphaFitter&>(o);

    // Setup
    f.fSigInt       = fSigInt;
    f.fSigMax       = fSigMax;
    f.fBgMin        = fBgMin;
    f.fBgMax        = fBgMax;
    f.fScaleMin     = fScaleMin;
    f.fScaleMax     = fScaleMax;
    f.fPolynomOrder = fPolynomOrder;
    f.fFitBackground= fFitBackground;
    f.fSignalFunc   = fSignalFunc;
    f.fScaleMode    = fScaleMode;
    f.fScaleUser    = fScaleUser;
    f.fStrategy     = fStrategy;
    f.fCoefficients.Set(fCoefficients.GetSize());
    f.fCoefficients.Reset();
    f.fErrors.Set(fCoefficients.GetSize());
    f.fErrors.Reset();

    // Result
    f.fSignificance     = fSignificance;
    f.fErrorExcess      = fErrorExcess;
    f.fEventsExcess     = fEventsExcess;
    f.fEventsSignal     = fEventsSignal;
    f.fEventsBackground = fEventsBackground;
    f.fChiSqSignal      = fChiSqSignal;
    f.fChiSqBg          = fChiSqBg;
    f.fIntegralMax      = fIntegralMax;
    f.fScaleFactor      = fScaleFactor;

    // Function
    delete f.fFunc;

    f.fFunc = new TF1(*fFunc);
    f.fFunc->SetName("Dummy");
    gROOT->GetListOfFunctions()->Remove(f.fFunc);
}

void MAlphaFitter::Print(Option_t *o) const
{
    *fLog << GetDescriptor() << ": Fitting..." << endl;
    *fLog << " ...signal to " << fSigMax << " (integrate into bin at " << fSigInt << ")" << endl;
    *fLog << " ...signal function: ";
    switch (fSignalFunc)
    {
    case kGauss:       *fLog << "gauss(x)/pol" << fPolynomOrder; break;
    case kThetaSq:     *fLog << "gauss(sqrt(x))/expo";  break;
    }
    *fLog << endl;
    if (!fFitBackground)
        *fLog << " ...no background." << endl;
    else
    {
        *fLog << " ...background from " << fBgMin << " to " << fBgMax << endl;
        *fLog << " ...polynom order " << fPolynomOrder << endl;
        *fLog << " ...scale mode: ";
        switch (fScaleMode)
        {
        case kNone:        *fLog << "none.";         break;
        case kEntries:     *fLog << "entries.";      break;
        case kIntegral:    *fLog << "integral.";     break;
        case kOffRegion:   *fLog << "off region (integral between " << fScaleMin << " and " << fScaleMax << ")"; break;
        case kBackground:  *fLog << "background (integral between " << fBgMin    << " and " << fBgMax    << ")"; break;
        case kLeastSquare: *fLog << "least square (N/A)"; break;
        case kUserScale:   *fLog << "user def (" << fScaleUser << ")"; break;
        }
        *fLog << endl;
    }

    if (TString(o).Contains("result"))
    {
        *fLog << "Result:" << endl;
        *fLog << " - Significance (Li/Ma)    " << fSignificance << endl;
        *fLog << " - Excess Events           " << fEventsExcess << endl;
        *fLog << " - Signal Events           " << fEventsSignal << endl;
        *fLog << " - Background Events       " << fEventsBackground << endl;
        *fLog << " - E/sqrt(B>=Alpha)        " << fEventsExcess/TMath::Sqrt(TMath::Max(fEventsBackground,fScaleFactor)) << endl;
        *fLog << " - Chi^2/ndf (Signal)      " << fChiSqSignal << endl;
        *fLog << " - Chi^2/ndf (Background)  " << fChiSqBg << endl;
        *fLog << " - Signal integrated up to " << fIntegralMax << "°" << endl;
        *fLog << " - Off Scale Alpha (Off)   " << fScaleFactor << endl;
    }
}

Bool_t MAlphaFitter::FitEnergy(const TH3D &hon, UInt_t bin, Bool_t paint)
{
    const TString name(MString::Format("TempAlphaEnergy%06d", gRandom->Integer(1000000)));

    TH1D *h = hon.ProjectionZ(name, 0, hon.GetNbinsX()+1, bin, bin, "E");
    h->SetDirectory(0);

    const Bool_t rc = Fit(*h, paint);

    delete h;

    return rc;
}

Bool_t MAlphaFitter::FitTheta(const TH3D &hon, UInt_t bin, Bool_t paint)
{
    const TString name(MString::Format("TempAlphaTheta%06d", gRandom->Integer(1000000)));

    TH1D *h = hon.ProjectionZ(name, bin, bin, 0, hon.GetNbinsY()+1, "E");
    h->SetDirectory(0);

    const Bool_t rc = Fit(*h, paint);

    delete h;

    return rc;
}
/*
Bool_t MAlphaFitter::FitTime(const TH3D &hon, UInt_t bin, Bool_t paint)
{
    const TString name(Form("TempAlphaTime%06d", gRandom->Integer(1000000)));

    hon.GetZaxis()->SetRange(bin,bin);
    TH1D *h = (TH1D*)hon.Project3D("ye");
    hon.GetZaxis()->SetRange(-1,-1);

    h->SetDirectory(0);

    const Bool_t rc = Fit(*h, paint);
    delete h;
    return rc;
}
*/
Bool_t MAlphaFitter::FitAlpha(const TH3D &hon, Bool_t paint)
{
    const TString name(MString::Format("TempAlpha%06d", gRandom->Integer(1000000)));

    TH1D *h = hon.ProjectionZ(name, 0, hon.GetNbinsX()+1, 0, hon.GetNbinsY()+1, "E");
    h->SetDirectory(0);

    const Bool_t rc = Fit(*h, paint);

    delete h;

    return rc;
}

Bool_t MAlphaFitter::FitEnergy(const TH3D &hon, const TH3D &hof, UInt_t bin, Bool_t paint)
{
    const TString name1(MString::Format("TempAlpha%06d_on",  gRandom->Integer(1000000)));
    const TString name0(MString::Format("TempAlpha%06d_off", gRandom->Integer(1000000)));

    TH1D *h1 = hon.ProjectionZ(name1, 0, hon.GetNbinsX()+1, bin, bin, "E");
    h1->SetDirectory(0);

    TH1D *h0 = hof.ProjectionZ(name0, 0, hof.GetNbinsX()+1, bin, bin, "E");
    h0->SetDirectory(0);

    const Bool_t rc = ScaleAndFit(*h1, h0, paint);

    delete h0;
    delete h1;

    return rc;
}

Bool_t MAlphaFitter::FitTheta(const TH3D &hon, const TH3D &hof, UInt_t bin, Bool_t paint)
{
    const TString name1(MString::Format("TempAlpha%06d_on",  gRandom->Integer(1000000)));
    const TString name0(MString::Format("TempAlpha%06d_off", gRandom->Integer(1000000)));

    TH1D *h1 = hon.ProjectionZ(name1, bin, bin, 0, hon.GetNbinsY()+1, "E");
    h1->SetDirectory(0);

    TH1D *h0 = hof.ProjectionZ(name0, bin, bin, 0, hof.GetNbinsY()+1, "E");
    h0->SetDirectory(0);

    const Bool_t rc = ScaleAndFit(*h1, h0, paint);

    delete h0;
    delete h1;

    return rc;
}
/*
Bool_t MAlphaFitter::FitTime(const TH3D &hon, const TH3D &hof, UInt_t bin, Bool_t paint)
{
    const TString name1(Form("TempAlphaTime%06d_on",  gRandom->Integer(1000000)));
    const TString name0(Form("TempAlphaTime%06d_off", gRandom->Integer(1000000)));

    hon.GetZaxis()->SetRange(bin,bin);
    TH1D *h1 = (TH1D*)hon.Project3D("ye");
    hon.GetZaxis()->SetRange(-1,-1);
    h1->SetDirectory(0);

    hof.GetZaxis()->SetRange(bin,bin);
    TH1D *h0 = (TH1D*)hof.Project3D("ye");
    hof.GetZaxis()->SetRange(-1,-1);
    h0->SetDirectory(0);

    const Bool_t rc = ScaleAndFit(*h1, h0, paint);

    delete h0;
    delete h1;

    return rc;
}
*/

Bool_t MAlphaFitter::FitAlpha(const TH3D &hon, const TH3D &hof, Bool_t paint)
{
    const TString name1(MString::Format("TempAlpha%06d_on",  gRandom->Integer(1000000)));
    const TString name0(MString::Format("TempAlpha%06d_off", gRandom->Integer(1000000)));

    TH1D *h1 = hon.ProjectionZ(name1, 0, hon.GetNbinsX()+1, 0, hon.GetNbinsY()+1, "E");
    h1->SetDirectory(0);

    TH1D *h0 = hof.ProjectionZ(name0, 0, hof.GetNbinsX()+1, 0, hof.GetNbinsY()+1, "E");
    h0->SetDirectory(0);

    const Bool_t rc = ScaleAndFit(*h1, h0, paint);

    delete h0;
    delete h1;

    return rc;
}

Bool_t MAlphaFitter::ApplyScaling(const TH3D &hon, TH3D &hof, UInt_t bin) const
{
    const TString name1(MString::Format("TempAlpha%06d_on",  gRandom->Integer(1000000)));
    const TString name0(MString::Format("TempAlpha%06d_off", gRandom->Integer(1000000)));

    TH1D *h1 = hon.ProjectionZ(name1, 0, hon.GetNbinsX()+1, bin, bin, "E");
    h1->SetDirectory(0);

    TH1D *h0 = hof.ProjectionZ(name0, 0, hof.GetNbinsX()+1, bin, bin, "E");
    h0->SetDirectory(0);

    const Double_t scale = Scale(*h0, *h1);

    delete h0;
    delete h1;

    for (int x=0; x<=hof.GetNbinsX()+1; x++)
        for (int z=0; z<=hof.GetNbinsZ()+1; z++)
        {
            hof.SetBinContent(x, bin, z, hof.GetBinContent(x, bin, z)*scale);
            hof.SetBinError(  x, bin, z, hof.GetBinError(  x, bin, z)*scale);
        }

    return scale>0;
}

Bool_t MAlphaFitter::ApplyScaling(const TH3D &hon, TH3D &hof) const
{
    for (int y=0; y<=hof.GetNbinsY()+1; y++)
        ApplyScaling(hon, hof, y);

    return kTRUE;
}

Double_t MAlphaFitter::Scale(TH1D &of, const TH1D &on) const
{
    Float_t scaleon = 1;
    Float_t scaleof = 1;
    switch (fScaleMode)
    {
    case kNone:
        return 1;

    case kEntries:
        scaleon = on.GetEntries();
        scaleof = of.GetEntries();
        break;

    case kIntegral:
        scaleon = on.Integral();
        scaleof = of.Integral();
        break;

    case kOffRegion:
        {
            const Int_t min = on.GetXaxis()->FindFixBin(fScaleMin);
            const Int_t max = on.GetXaxis()->FindFixBin(fScaleMax);
            scaleon = on.Integral(min, max);
            scaleof = of.Integral(min, max);
        }
        break;

    case kBackground:
        {
            const Int_t min = on.GetXaxis()->FindFixBin(fBgMin);
            const Int_t max = on.GetXaxis()->FindFixBin(fBgMax);
            scaleon = on.Integral(min, max);
            scaleof = of.Integral(min, max);
        }
        break;

    case kUserScale:
        scaleon = fScaleUser;
        break;

        // This is just to make some compiler happy
    default:
        return 1;
    }

    if (scaleof!=0)
    {
        of.Scale(scaleon/scaleof);
        return scaleon/scaleof;
    }
    else
    {
        of.Reset();
        return 0;
    }
}

Double_t MAlphaFitter::GetMinimizationValue() const
{
    switch (fStrategy)
    {
    case kSignificance:
        return -GetSignificance();
    case kSignificanceChi2:
        return -GetSignificance()/GetChiSqSignal();
    case kSignificanceLogExcess:
        if (GetEventsExcess()<1)
            return 0;
        return -GetSignificance()*TMath::Log10(GetEventsExcess());
    case kSignificanceSqrtExcess:
        if (GetEventsExcess()<1)
            return 0;
        return -GetSignificance()*TMath::Sqrt(GetEventsExcess());
    case kSignificanceExcess:
        return -GetSignificance()*GetEventsExcess();
    case kExcess:
        return -GetEventsExcess();
    case kGaussSigma:
        return GetGausSigma();
    case kWeakSource:
        if (GetEventsExcess()<1)
            return 0;
        return -GetEventsExcess()/TMath::Sqrt(TMath::Max(GetEventsBackground(), GetEventsBackground()));
    case kWeakSourceLogExcess:
        if (GetEventsExcess()<1)
            return 0;
        return -GetEventsExcess()/TMath::Sqrt(TMath::Max(GetEventsBackground(), GetEventsBackground()))*TMath::Log10(GetEventsExcess());
    }
    return 0;
}

Int_t MAlphaFitter::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    //void SetScaleUser(Float_t scale)       { fScaleUser = scale; fScaleMode=kUserScale; }
    //void SetScaleMode(ScaleMode_t mode)    { fScaleMode    = mode; }

    if (IsEnvDefined(env, prefix, "SignalIntegralMax", print))
    {
        SetSignalIntegralMax(GetEnvValue(env, prefix, "SignalIntegralMax", fSigInt));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "SignalFitMax", print))
    {
        SetSignalIntegralMax(GetEnvValue(env, prefix, "SignalFitMax", fSigMax));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "BackgroundFitMax", print))
    {
        SetBackgroundFitMax(GetEnvValue(env, prefix, "BackgroundFitMax", fBgMax));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "BackgroundFitMin", print))
    {
        SetBackgroundFitMin(GetEnvValue(env, prefix, "BackgroundFitMin", fBgMin));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "ScaleMin", print))
    {
        SetScaleMin(GetEnvValue(env, prefix, "ScaleMin", fScaleMin));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "ScaleMax", print))
    {
        SetScaleMax(GetEnvValue(env, prefix, "ScaleMax", fScaleMax));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "PolynomOrder", print))
    {
        SetPolynomOrder(GetEnvValue(env, prefix, "PolynomOrder", fPolynomOrder));
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "MinimizationStrategy", print))
    {
        TString txt = GetEnvValue(env, prefix, "MinimizationStrategy", "");
        txt = txt.Strip(TString::kBoth);
        txt.ToLower();
        if (txt==(TString)"significance")
            fStrategy = kSignificance;
        if (txt==(TString)"significancechi2")
            fStrategy = kSignificanceChi2;
        if (txt==(TString)"significanceexcess")
            fStrategy = kSignificanceExcess;
        if (txt==(TString)"excess")
            fStrategy = kExcess;
        if (txt==(TString)"gausssigma" || txt==(TString)"gaussigma")
            fStrategy = kGaussSigma;
        if (txt==(TString)"weaksource")
            fStrategy = kWeakSource;
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "Scale", print))
    {
        fScaleUser = GetEnvValue(env, prefix, "Scale", fScaleUser);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "ScaleMode", print))
    {
        TString txt = GetEnvValue(env, prefix, "ScaleMode", "");
        txt = txt.Strip(TString::kBoth);
        txt.ToLower();
        if (txt==(TString)"none")
            fScaleMode = kNone;
        if (txt==(TString)"entries")
            fScaleMode = kEntries;
        if (txt==(TString)"integral")
            fScaleMode = kIntegral;
        if (txt==(TString)"offregion")
            fScaleMode = kOffRegion;
        if (txt==(TString)"background")
            fScaleMode = kBackground;
        if (txt==(TString)"leastsquare")
            fScaleMode = kLeastSquare;
        if (txt==(TString)"userscale")
            fScaleMode = kUserScale;
        if (txt==(TString)"fixed")
            FixScale();
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "SignalFunction", print))
    {
        TString txt = GetEnvValue(env, prefix, "SignalFunction", "");
        txt = txt.Strip(TString::kBoth);
        txt.ToLower();
        if (txt==(TString)"gauss" || txt==(TString)"gaus")
            SetSignalFunction(kGauss);
        if (txt==(TString)"thetasq")
            SetSignalFunction(kThetaSq);
        rc = kTRUE;
    }

    return rc;
}
