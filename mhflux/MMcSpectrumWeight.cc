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
!   Author(s): Thomas Bretz 5/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Marcos Lopez 10/2003 <mailto:marcos@gae.ucm.es>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MMcSpectrumWeight
//               
//  Change the spectrum of the MC showers simulated with Corsika (a power law)
//  to a new one, which can be either, again a power law but with a different
//  spectral index, or a generalizeed spectrum. The new spectrum can be 
//  pass to this class in different ways:
//
//    1. If the new spectrum will be a power law, just introduce the slope
//       of this power law.
//    2. If the new spectrum will have a general shape:
//       The new spectrum is passed as a char* (SetFormula())
//
//  Method:
//  -------
//
//   - Corsika spectrum: dN/dE = A * E^(a)
//     with a = fOldSlope, and A = N/integral{E*de} from ELowLim to EUppLim
//
//   - New spectrum:     dN/dE = B * g(E)
//     where B = N/integral{g*dE} from ELowLim to EUppLim, and N=NumEvents
//
//  For converting the spectrum simulated with Corsika to the new one, we
//  apply a weight to each event, given by:
//
//      W(E) = B/A * g(E)/E^(a)
//
//  In the case the new spectrum is simply a power law: dN/dE = B * E^(b), we
//  have: 
//
//      W(E) = B/A * E^(b-a)
//
//  (The factor B/A is used in order both the original and new spectrum have 
//   the same area (i.e. in order they represent the same number of showers))
//
//
//  If using SetFormula you can specify formulas accepted by TF1, eg:
//      pow(X, -2.6)
//  (Rem: all capital (!) 'X' are replaced by the corresponding %s.fEnergy
//        automatically)
//
//  For more details of the setup see MMcSpectrumWeight::ReadEnv
//
//
//  Input Containers:
//    MMcEvt
//    MMcCorsikaRunHeader
//    [MPointingPos]
//    [MHillas]
//
//  Output Container:
//    MWeight [MParameterD]
//
//////////////////////////////////////////////////////////////////////////////
#include "MMcSpectrumWeight.h"

#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TSpline.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"
#include "MParList.h"
#include "MParameters.h"

#include "MHillas.h"
#include "MPointingPos.h"

#include "MMcEvt.hxx"
#include "MMcCorsikaRunHeader.h"

ClassImp(MMcSpectrumWeight);

using namespace std;

void MMcSpectrumWeight::Init(const char *name, const char *title)
{
    fName  = name  ? name  : "MMcSpectrumWeight";
    fTitle = title ? title : "Task to calculate weights to change the energy spectrum"; 

    AddToBranchList("MMcEvt.fEnergy");

    fNameWeight  = "MWeight";
    fNameMcEvt   = "MMcEvt";

    fNewSlope    = -99;
    fOldSlope    = -99;

    fEnergyMin   = -1;
    fEnergyMax   = -2;

    fNorm        =  1;
    fNormEnergy  = -1;

    fAllowChange = kFALSE;

    fFunc        = NULL;
    fMcEvt       = NULL;
    fHillas      = NULL;
    fWeight      = NULL;
    fWeightsZd   = NULL;
    fWeightsSize = NULL;
    fPointing    = NULL;
}

// ---------------------------------------------------------------------------
//
// Default Constructor.
//
MMcSpectrumWeight::MMcSpectrumWeight(const char *name, const char *title)
{
    Init(name,title);
} 

// ---------------------------------------------------------------------------
//
// Destructor. If necessary delete fFunc
//
MMcSpectrumWeight::~MMcSpectrumWeight()
{
    if (fFunc)
        delete fFunc;
//    if (fWeightsSize)
//        delete fWeightsSize;
}

// ---------------------------------------------------------------------------
//
// Search for
//   - fNameMcEvt [MMcEvtBasic]
//
// Find/Create:
//   - fNameWeight [MWeight]
//
Int_t MMcSpectrumWeight::PreProcess(MParList *pList)
{
    fMcEvt = (MMcEvt*)pList->FindObject(fNameMcEvt, "MMcEvtBasic");
    if (!fMcEvt)
    {
        *fLog << err << fNameMcEvt << " [MMcEvtBasic] not found... abort." << endl;
        return kFALSE;
    }

    fWeight = (MParameterD*)pList->FindCreateObj("MParameterD", fNameWeight);
    if (!fWeight)
        return kFALSE;

    if (fWeightsZd)
    {
        fPointing = (MPointingPos*)pList->FindObject("MPointingPos");
        if (!fPointing)
        {
            *fLog << err << "MPointingPos not found... abort." << endl;
            return kFALSE;
        }
    }

    if (fWeightsSize)
    {
        fHillas = (MHillas*)pList->FindObject("MHillas");
        if (!fHillas)
        {
            *fLog << err << "MHillas not found... abort." << endl;
            return kFALSE;
        }
    }

    return kTRUE;
}

// ---------------------------------------------------------------------------
//
// Replace {fNameMcEvt}.fEnergy by "(x)" and return the result.
//
TString MMcSpectrumWeight::ReplaceX(TString str) const
{
    return str.ReplaceAll(MString::Format("%s.fEnergy", fNameMcEvt.Data()), "(x)");
}

// ---------------------------------------------------------------------------
//
// Return the function corresponding to the mc spectrum with
// slope fOldSlope: pow({fNameMcEvt}.fEnergy, fOldSlope)
//
// The slope is returned as %.3f
//
TString MMcSpectrumWeight::GetFormulaSpecOld(const char *name) const
{
    return MString::Format("pow(%s.fEnergy, %.3f)", name, fOldSlope);
}

// ---------------------------------------------------------------------------
//
// Return the function corresponding to the new spectrum with
// slope fNewSlope: pow({fNameMcEvt}.fEnergy, fNewSlope)
//
// The slope is returned as %.3f
//
// If a different formula is set (SetFormula()) this formula is returned
// unchanged.
//
TString MMcSpectrumWeight::GetFormulaSpecNew(const char *name) const
{
    TString str = fFormula.IsNull() ? MString::Format("pow(%s.fEnergy, %.3f)", name, fNewSlope) : fFormula;
    if (!fFormula.IsNull())
        str.ReplaceAll("X", MString::Format("(%s.fEnergy)", name));

    return str;
}

// ---------------------------------------------------------------------------
//
// Return the formula to calculate weights.
// Is is compiled by
//   o1 = integral(fEnergyMin, fEnergyMax, GetFormulaSpecOldX());
//   n1 = integral(fEnergyMin, fEnergyMax, GetFormulaSpecNewX());
//   o2 = CalcSpecOld(fNormEnergy);
//   n2 = CalcSpecNew(fNormEnergy);
//
//   result (fNormEnergy<0):
//      fNorm*o1/n1*GetFormulaNewSpec()/GetFormulaOldSpec()
//
//   result (fNormEnergy>=0):
//      fNorm*o2/n2*GetFormulaNewSpec()/GetFormulaOldSpec()
//
// fNorm is 1 by default but can be overwritten using SetNorm()
//
// If the formulas GetFormulaSpecOldX() and GetFormulaSpecNewX()
// are equal only fNorm is returned.
//
// The normalization constant is returned as %.16e
//
// Example: 0.3712780019*(pow(MMcEvt.fEnergy,-2.270))/(pow(MMcEvt.fEnergy,-2.600))
//
TString MMcSpectrumWeight::GetFormulaWeights(const char *name) const
{
    if (GetFormulaSpecOld()==GetFormulaSpecNew())
        return MString::Format("%.16e", fNorm);

    const Double_t iold = fNormEnergy<0 ? GetSpecOldIntegral() : CalcSpecOld(fNormEnergy);
    const Double_t inew = fNormEnergy<0 ? GetSpecNewIntegral() : CalcSpecNew(fNormEnergy);

    const Double_t norm = fNorm*iold/inew;

    return MString::Format("%.16e*(%s)/(%s)", norm, GetFormulaSpecNew(name).Data(), GetFormulaSpecOld(name).Data());
}

// ---------------------------------------------------------------------------
//
// Returns the integral between fEnergyMin and fEnergyMax of
// GetFormulaSpecNewX() describing the destination spectrum
//
Double_t MMcSpectrumWeight::GetSpecNewIntegral(Double_t emin, Double_t emax) const
{
    TF1 funcnew("Dummy", GetFormulaSpecNewX().Data());
    return funcnew.Integral(emin, emax);
}

// ---------------------------------------------------------------------------
//
// Returns the integral between fEnergyMin and fEnergyMax of
// GetFormulaSpecOldX() describing the simulated spectrum
//
Double_t MMcSpectrumWeight::GetSpecOldIntegral(Double_t emin, Double_t emax) const
{
    TF1 funcold("Dummy", GetFormulaSpecOldX().Data());
    return funcold.Integral(emin, emax);
}

// ---------------------------------------------------------------------------
//
// Returns the value of GetFormulaSpecNewX() at the energy e describing
// the destination spectrum
//
Double_t MMcSpectrumWeight::CalcSpecNew(Double_t e) const
{
    TF1 funcnew("Dummy", GetFormulaSpecNewX().Data());
    return funcnew.Eval(e);
}

// ---------------------------------------------------------------------------
//
// Returns the value of GetFormulaSpecOldX() at the energy e describing
// the simulated spectrum
//
Double_t MMcSpectrumWeight::CalcSpecOld(Double_t e) const
{
    TF1 funcnew("Dummy", GetFormulaSpecOldX().Data());
    return funcnew.Eval(e);
}

void MMcSpectrumWeight::SetWeightsSize(TH1D *h)
{
    fWeightsSize=h;
    /*
    if (h==0)
    {
        fWeightsSize=0;
        return;
    }

    if (fWeightsSize)
        delete fWeightsSize;

    const Double_t xmin = TMath::Log10(h->GetXaxis()->GetXmin());
    const Double_t xmax = TMath::Log10(h->GetXaxis()->GetXmax());
    const Double_t xnum = h->GetNbinsX()+1;

    fWeightsSize = new TSpline3("WeightsSize", xmin, xmax,
                                h->GetArray()+1, xnum);*/
}

// ---------------------------------------------------------------------------
//
// Initialize fEnergyMin, fEnergymax and fOldSlope from MMcCorsikaRunHeader
// by GetELowLim(), GetEUppLim() and GetSlopeSpec().
//
// If fEnergyMax>fEnergyMin (means: the values have already been
// initialized) and !fAllowChange the consistency of the new values
// with the present values is checked with a numerical precision of 1e-10.
// If one doesn't match kFALSE is returned.
//
// If the mc slope is -1 kFALSE is returned.
//
// If the new slope for the spectrum is -1 it is set to the original MC
// slope.
//
// fFunc is set to the formula returned by GetFormulaWeightsX()
//
Bool_t MMcSpectrumWeight::Set(const MMcCorsikaRunHeader &rh)
{
    if (fEnergyMax>fEnergyMin && !fAllowChange)
    {
        if (TMath::Abs(fEnergyMax-rh.GetEUppLim())>1e-10)
        {
            *fLog << err;
            *fLog << "ERROR - The maximum simulated Monte Carlo energy is not allowed to change ";
            *fLog << "(" << fEnergyMax << " --> " << rh.GetEUppLim() << ")... abort." << endl;
            return kFALSE;
        }

        if (TMath::Abs(fOldSlope-rh.GetSlopeSpec())>1e-10)
        {
            *fLog << err;
            *fLog << "ERROR - The slope of the Monte Carlo is not allowed to change ";
            *fLog << "(" << fOldSlope << " --> " << rh.GetSlopeSpec() << ")... abort." << endl;
            return kFALSE;
        }

        // No change happened
        if (TMath::Abs(fEnergyMin-rh.GetELowLim())<=1e-10)
            return kTRUE;

        // The lower energy limit has changed
        *fLog << warn;
        *fLog << "The minimum simulated Monte Carlo energy has changed from ";
        *fLog << fEnergyMin << "GeV to " << rh.GetELowLim() << "GeV." << endl;
        fEnergyMin = rh.GetELowLim();
    }

    if (fNormEnergy<0 && fEnergyMin>0 && TMath::Abs(fEnergyMin-rh.GetELowLim())>1e-10)
    {
        *fLog << err;
        *fLog << "You try to use changing minimum simulated Monte Carlo energies" << endl;
        *fLog << "together with a normalization calculated from the integral." << endl;
        *fLog << "This is not yet supported. Please switch to a normalization" << endl;
        *fLog << "at a dedicated energy by specifying the energy" << endl;
        *fLog << "    MMcSpectrumWeight.NormEnergy: 500" << endl;
        *fLog << "in your sponde.rc." << endl;
        return kFALSE;
    }

    fOldSlope  = rh.GetSlopeSpec();
    fEnergyMin = rh.GetELowLim();
    fEnergyMax = rh.GetEUppLim();

    if (fNewSlope==-99 && fFormula.IsNull())
    {
        *fLog << inf << "A new slope for the power law has not yet been defined... using " << fOldSlope << "." << endl;
        fNewSlope = fOldSlope;
    }

    if (fFunc)
        delete fFunc;

    if (GetFormulaSpecOld()==GetFormulaSpecNew())
        *fLog << inf << "No spectral change requested..." << endl;
    else
    {
        *fLog << inf << "Weighting from slope " << fOldSlope << " to ";
        if (fFormula.IsNull())
            *fLog << "slope " << fNewSlope << "." << endl;
        else
            *fLog << GetFormulaSpecNewX() << endl;
    }

    fFunc = new TF1("", GetFormulaWeightsX().Data());
    fFunc->SetName("SpectralWeighs");
    gROOT->GetListOfFunctions()->Remove(fFunc);

    return kTRUE;
}

// ---------------------------------------------------------------------------
//
// completes a simulated spectrum starting at an energy fEnergyMin down to
// an energy emin.
//
// It is assumed that the contents of MMcSpectrumWeight for the new spectrum
// correctly describe the spectrum within the histogram, and fEnergyMin
// and fEnergyMax correctly describe the range.
//
// If scale is given the histogram statistics is further extended by the
// new spectrum according to the scale factor (eg. 1.2: by 20%)
//
// In the 1D case it is assumed that the x-axis is a zenith angle binning.
// In the 2D case the x-axis is assumed to be zenith angle, the y-axis
// to be energy.
//
void MMcSpectrumWeight::CompleteEnergySpectrum(TH1 &h, Double_t emin, Double_t scale) const
{
    if (h.InheritsFrom(TH3::Class()))
    {
        return;
    }

    if (fEnergyMin < emin)
    {
        *fLog << err << "ERROR - MMcSpctrumWeight::CompleteEnergySpectrum: fEnergyMin (";
        *fLog << fEnergyMin << ") smaller than emin (" << emin << ")." << endl;
        return;
    }

    // Total number of events for the new spectrum in the same
    // energy range as the current histogram is filled
    const Double_t norm = GetSpecNewIntegral();

    // Check if it is only a histogram in ZA
    if (!h.InheritsFrom(TH2::Class()))
    {
        // Warning: Simply scaling the zenith angle distribution might
        // increase fluctuations for low statistics.
        const Double_t f = GetSpecNewIntegral(emin, fEnergyMax)/norm;
        h.Scale(f*scale);
        return;
    }

    const TAxis &axey = *h.GetYaxis();

    // Find energy range between the minimum energy to be filled (emin)
    // and the minimum energy corresponding to the data filled into
    // this histogram (fEnergyMin)
    const Int_t first = axey.FindFixBin(emin);
    const Int_t last  = axey.FindFixBin(fEnergyMin); // data range min energy
    const Int_t max   = axey.FindFixBin(fEnergyMax); // data range max energy

    for (int x=1; x<=h.GetNbinsX(); x++)
    {
        // Ratio between the number of events in the zenith angle
        // bin corresponding to x and the new spectrum.
#if ROOT_VERSION_CODE < ROOT_VERSION(5,26,00)
        const Double_t f = h.Integral(x, x, -1, 9999)/norm;
#else
        const Double_t f = h.Integral(x, x)/norm;
#endif

        // Fill histogram with the "new spectrum" between
        // emin and fEnergyMin.
        if (emin<fEnergyMin)
            for (int y=first; y<=last; y++)
            {
                // Check if the bin is only partly filled by the energy range
                const Double_t lo = axey.GetBinLowEdge(y)  <emin       ? emin       : axey.GetBinLowEdge(y);
                const Double_t hi = axey.GetBinLowEdge(y+1)>fEnergyMin ? fEnergyMin : axey.GetBinLowEdge(y+1);

                // Add the new spectrum extending the existing spectrum
                h.AddBinContent(h.GetBin(x, y), f*GetSpecNewIntegral(lo, hi));
            }

        // If scale is >1 we also have to increse the statistics f the
        // histogram according to scale.
        if (scale>1)
            for (int y=first; y<=max; y++)
            {
                // Check if the bin is only partly filled by the energy range
                const Double_t lo = axey.GetBinLowEdge(y)  <emin       ? emin       : axey.GetBinLowEdge(y);
                const Double_t hi = axey.GetBinLowEdge(y+1)>fEnergyMax ? fEnergyMax : axey.GetBinLowEdge(y+1);

                // Use the analytical solution to scale the histogram
                h.AddBinContent(h.GetBin(x, y), f*GetSpecNewIntegral(lo, hi)*(scale-1));
            }
    }
}

// ---------------------------------------------------------------------------
//
// The current contants are printed
//
void MMcSpectrumWeight::Print(Option_t *o) const
{
    const TString opt(o);

    const Bool_t hasnew = opt.Contains("new") || opt.IsNull();
    const Bool_t hasold = opt.Contains("old") || opt.IsNull();

    *fLog << all << GetDescriptor() << endl;

    if (hasold)
    {
        *fLog << " Simulated energy range:   " << fEnergyMin << "GeV - " << fEnergyMax << "GeV" << endl;
        *fLog << " Simulated spectral slope: ";
        if (fOldSlope==-99)
            *fLog << "undefined" << endl;
        else
            *fLog << fOldSlope << endl;
    }
    if (hasnew)
    {
        *fLog << " New spectral slope:       ";
        if (fNewSlope==-99)
            *fLog << "undefined/no change" << endl;
        else
            *fLog << fNewSlope << endl;
    }
    *fLog << " Additional user norm.:    " << fNorm << endl;
    *fLog << " Spectra are normalized:   " << (fNormEnergy<0?"by integral":MString::Format("at %.1fGeV", fNormEnergy)) << endl;
    if (hasold)
    {
        *fLog << " Old Spectrum:             ";
        if (fNewSlope==-99)
            *fLog << "undefined";
        else
            *fLog << GetFormulaSpecOldX();
        if (fEnergyMin>=0 && fEnergyMax>0)
            *fLog << "   (I=" << GetSpecOldIntegral() << ")";
        *fLog << endl;
    }
    if (hasnew)
    {
        *fLog << " New Spectrum:             ";
        if (fNewSlope==-99 && fFormula.IsNull())
            *fLog << "undefined/no change";
        else
            *fLog << GetFormulaSpecNewX();
        if (fEnergyMin>=0 && fEnergyMax>0)
            *fLog << "   (I=" << GetSpecNewIntegral() << ")";
        *fLog << endl;
    }
    if (fFunc)
        *fLog << " Weight func:              " << fFunc->GetTitle()   << endl;
}

// ----------------------------------------------------------------------------
//
// Executed each time a new root file is loaded
// We will need fOldSlope and fE{Upp,Low}Lim to calculate the weights
//
Bool_t MMcSpectrumWeight::ReInit(MParList *plist)
{
    MMcCorsikaRunHeader *rh = (MMcCorsikaRunHeader*)plist->FindObject("MMcCorsikaRunHeader");
    if (!rh)
    {
        *fLog << err << "MMcCorsikaRunHeader not found... abort." << endl;
        return kFALSE;
    }

    return Set(*rh);
}

/*
 * This could be used to improve the Zd-weighting within a bin.
 * Another option is to use more bins, or both.
 * Note that it seems unnecessary, because the shape within the
 * theta-bins should be similar in data and Monte Carlo... hopefully.
 *
void MMcSpectrumWeight::InitZdWeights()
{
    TH2D w(*fWeightsZd);

    for (int i=1; i<=w.GetNbinsX(); i++)
    {
         const Double_t tmin = w.GetBinLowEdge(i)  *TMath::DegToRad();
         const Double_t tmax = w.GetBinLowEdge(i+1)*TMath::DegToRad();

         const Double_t wdth  = tmax-tmin;
         const Double_t integ = cos(tmin)-cos(tmax);

         w.SetBinContent(i, w.GetBinContent(i)*wdth/integ);
    }

    //  const Int_t i = fWeightsZd->GetXaxis()->FindFixBin(fPointing->GetZd());
    //  const Double_t theta = fPointing->GetZd()*TMath::DegToRad();
    //  w = sin(theta)*w.GetBinContent(i);
}
*/

// ----------------------------------------------------------------------------
//
// Fill the result of the evaluation of fFunc at fEvEvt->GetEnergy
// into the weights container.
//
Int_t MMcSpectrumWeight::Process()
{
    Double_t w = 1;

    if (fWeightsZd)
    {
        const Int_t i = fWeightsZd->GetXaxis()->FindFixBin(fPointing->GetZd());
        w = fWeightsZd->GetBinContent(i);
    }
    if (fWeightsSize)
    {
        const Int_t i = fWeightsSize->GetXaxis()->FindFixBin(fHillas->GetSize());
        w *= fWeightsSize->GetBinContent(i);
       // w *= fWeightsSize->Eval(TMath::Log10(fHillas->GetSize()));
    }

    const Double_t e = fMcEvt->GetEnergy();

    fWeight->SetVal(fFunc->Eval(e)*w);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//
//   MMcSpectrumWeight.NewSlope:   -2.6
//    The new slope of the spectrum
//
//   MMcSpectrumWeight.Norm:        1.0
//    An additional artificial scale factor
//
//   MMcSpectrumWeight.NormEnergy:  200
//    To normalize at a given energy instead of the integral
//
//   MMcSpectrumWeight.Formula:     pow(X, -2.6)
//    A formula to which the spectrum is weighted (use a capital X for
//    the energy)
//
Int_t MMcSpectrumWeight::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NewSlope", print))
    {
        rc = kTRUE;
        SetNewSlope(GetEnvValue(env, prefix, "NewSlope", fNewSlope));
    }
    if (IsEnvDefined(env, prefix, "Norm", print))
    {
        rc = kTRUE;
        SetNorm(GetEnvValue(env, prefix, "Norm", fNorm));
    }
    if (IsEnvDefined(env, prefix, "NormEnergy", print))
    {
        rc = kTRUE;
        SetNormEnergy(GetEnvValue(env, prefix, "NormEnergy", fNormEnergy));
    }
    if (IsEnvDefined(env, prefix, "Formula", print))
    {
        rc = kTRUE;
        SetFormula(GetEnvValue(env, prefix, "Formula", fFormula));
    }

    return rc;
}
