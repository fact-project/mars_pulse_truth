/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analyzing Data of imaging Cerenkov telescopes.
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
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzbrug.de>
!   Author(s): Markus Gaug 09/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2002-2007
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractTimeAndChargeSpline
//
//   Fast Spline extractor using a cubic spline algorithm, adapted from 
//   Numerical Recipes in C++, 2nd edition, pp. 116-119.
//   
//   The coefficients "ya" are here denoted as "fHiGainSignal" and "fLoGainSignal"
//   which means the FADC value subtracted by the clock-noise corrected pedestal.
//
//   The coefficients "y2a" get immediately divided 6. and are called here 
//   "fHiGainSecondDeriv" and "fLoGainSecondDeriv" although they are now not exactly 
//   the second derivative coefficients any more. 
// 
//   The calculation of the cubic-spline interpolated value "y" on a point 
//   "x" along the FADC-slices axis becomes:
// 
//   y =    a*fHiGainSignal[klo] + b*fHiGainSignal[khi] 
//       + (a*a*a-a)*fHiGainSecondDeriv[klo] + (b*b*b-b)*fHiGainSecondDeriv[khi]
//
//   with:
//   a = (khi - x)
//   b = (x - klo)
//
//   and "klo" being the lower bin edge FADC index and "khi" the upper bin edge FADC index.
//   fHiGainSignal[klo] and fHiGainSignal[khi] are the FADC values at "klo" and "khi".
//
//   An analogues formula is used for the low-gain values.
//
//   The coefficients fHiGainSecondDeriv and fLoGainSecondDeriv are calculated with the 
//   following simplified algorithm:
//
//   for (Int_t i=1;i<range-1;i++) {
//       pp                   = fHiGainSecondDeriv[i-1] + 4.;
//       fHiGainFirstDeriv[i] = fHiGainSignal[i+1] - 2.*fHiGainSignal[i] + fHiGainSignal[i-1]
//       fHiGainFirstDeriv[i] = (6.0*fHiGainFirstDeriv[i]-fHiGainFirstDeriv[i-1])/pp;
//   }
// 
//   for (Int_t k=range-2;k>=0;k--)
//       fHiGainSecondDeriv[k] = (fHiGainSecondDeriv[k]*fHiGainSecondDeriv[k+1] + fHiGainFirstDeriv[k])/6.;
// 
//
//   This algorithm takes advantage of the fact that the x-values are all separated by exactly 1
//   which simplifies the Numerical Recipes algorithm.
//   (Note that the variables "fHiGainFirstDeriv" are not real first derivative coefficients.)
//
//   The algorithm to search the time proceeds as follows:
//
//   1) Calculate all fHiGainSignal from fHiGainFirst to fHiGainLast 
//      (note that an "overlap" to the low-gain arrays is possible: i.e. fHiGainLast>14 in the case of 
//      the MAGIC FADCs).
//   2) Remember the position of the slice with the highest content "fAbMax" at "fAbMaxPos".
//   3) If one or more slices are saturated or fAbMaxPos is less than 2 slices from fHiGainFirst, 
//      return fAbMaxPos as time and fAbMax as charge (note that the pedestal is subtracted here).
//   4) Calculate all fHiGainSecondDeriv from the fHiGainSignal array
//   5) Search for the maximum, starting in interval fAbMaxPos-1 in steps of 0.2 till fAbMaxPos-0.2.
//      If no maximum is found, go to interval fAbMaxPos+1. 
//      --> 4 function evaluations
//   6) Search for the absolute maximum from fAbMaxPos to fAbMaxPos+1 in steps of 0.2 
//      --> 4 function  evaluations
//   7) Try a better precision searching from new max. position fAbMaxPos-0.2 to fAbMaxPos+0.2
//      in steps of 0.025 (83 psec. in the case of the MAGIC FADCs).
//      --> 14 function evaluations
//   8) If Time Extraction Type kMaximum has been chosen, the position of the found maximum is 
//      returned, else:
//   9) The Half Maximum is calculated. 
//  10) fHiGainSignal is called beginning from fAbMaxPos-1 backwards until a value smaller than fHalfMax
//      is found at "klo". 
//  11) Then, the spline value between "klo" and "klo"+1 is halfed by means of bisection as long as 
//      the difference between fHalfMax and spline evaluation is less than fResolution (default: 0.01).
//      --> maximum 12 interations.
//   
//  The algorithm to search the charge proceeds as follows:
//
//  1) If Charge Type: kAmplitude was chosen, return the Maximum of the spline, found during the 
//     time search.
//  2) If Charge Type: kIntegral was chosen, sum the fHiGainSignal between:
//     (Int_t)(fAbMaxPos - fRiseTimeHiGain) and 
//     (Int_t)(fAbMaxPos + fFallTimeHiGain)
//     (default: fRiseTime: 1.5, fFallTime: 4.5)
//                                           sum the fLoGainSignal between:
//     (Int_t)(fAbMaxPos - fRiseTimeHiGain*fLoGainStretch) and 
//     (Int_t)(fAbMaxPos + fFallTimeHiGain*fLoGainStretch)
//     (default: fLoGainStretch: 1.5)
//       
//  The values: fNumHiGainSamples and fNumLoGainSamples are set to:
//  1) If Charge Type: kAmplitude was chosen: 1.
//  2) If Charge Type: kIntegral was chosen: fRiseTimeHiGain + fFallTimeHiGain
//                 or: fNumHiGainSamples*fLoGainStretch in the case of the low-gain
//
//  Call: SetRange(fHiGainFirst, fHiGainLast, fLoGainFirst, fLoGainLast) 
//        to modify the ranges.
// 
//        Defaults: 
//        fHiGainFirst =  2 
//        fHiGainLast  =  14
//        fLoGainFirst =  2 
//        fLoGainLast  =  14
//
//  Call: SetResolution() to define the resolution of the half-maximum search.
//        Default: 0.01
//
//  Call: SetRiseTime() and SetFallTime() to define the integration ranges 
//        for the case, the extraction type kIntegral has been chosen.
//
//  Call: - SetChargeType(MExtractTimeAndChargeSpline::kAmplitude) for the 
//          computation of the amplitude at the maximum (default) and extraction 
//          the position of the maximum (default)
//          --> no further function evaluation needed
//        - SetChargeType(MExtractTimeAndChargeSpline::kIntegral) for the 
//          computation of the integral beneith the spline between fRiseTimeHiGain
//          from the position of the maximum to fFallTimeHiGain after the position of 
//          the maximum. The Low Gain is computed with half a slice more at the rising
//          edge and half a slice more at the falling edge.
//          The time of the half maximum is returned.
//          --> needs one function evaluations but is more precise
//        
//////////////////////////////////////////////////////////////////////////////
#include "MExtractTimeAndChargeSpline.h"

#include "MPedestalPix.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MExtractTimeAndChargeSpline);

using namespace std;

const Byte_t  MExtractTimeAndChargeSpline::fgHiGainFirst      = 0;
const Byte_t  MExtractTimeAndChargeSpline::fgHiGainLast       = 14;
const Int_t   MExtractTimeAndChargeSpline::fgLoGainFirst      = 1;
const Byte_t  MExtractTimeAndChargeSpline::fgLoGainLast       = 14;
const Float_t MExtractTimeAndChargeSpline::fgResolution       = 0.05;
const Float_t MExtractTimeAndChargeSpline::fgRiseTimeHiGain   = 0.64;
const Float_t MExtractTimeAndChargeSpline::fgFallTimeHiGain   = 0.76;
const Float_t MExtractTimeAndChargeSpline::fgLoGainStretch    = 1.5;
const Float_t MExtractTimeAndChargeSpline::fgOffsetLoGain     = 1.3;

// --------------------------------------------------------------------------
//
// Default constructor.
//
// Calls: 
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
// 
// Initializes:
// - fResolution     to fgResolution
// - fRiseTimeHiGain to fgRiseTimeHiGain
// - fFallTimeHiGain to fgFallTimeHiGain
// - Charge Extraction Type to kAmplitude
// - fLoGainStretch  to fgLoGainStretch
//
MExtractTimeAndChargeSpline::MExtractTimeAndChargeSpline(const char *name, const char *title) 
    : fRiseTimeHiGain(0), fFallTimeHiGain(0), fHeightTm(0.5), fExtractionType(MExtralgoSpline::kIntegralRel)
{

  fName  = name  ? name  : "MExtractTimeAndChargeSpline";
  fTitle = title ? title : "Calculate photons arrival time using a fast spline";

  SetResolution();
  SetLoGainStretch();
  SetOffsetLoGain(fgOffsetLoGain);

  SetRiseTimeHiGain();
  SetFallTimeHiGain();

  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
}


//-------------------------------------------------------------------
//
// Set the ranges
// In order to set the fNum...Samples variables correctly for the case, 
// the integral is computed, have to overwrite this function and make an 
// explicit call to SetChargeType().
//
void MExtractTimeAndChargeSpline::SetRange(UShort_t hifirst, UShort_t hilast, Int_t lofirst, Byte_t lolast)
{
  MExtractor::SetRange(hifirst, hilast, lofirst, lolast);

  SetChargeType(fExtractionType);
}

//-------------------------------------------------------------------
//
// Set the Charge Extraction type. Possible are:
// - kAmplitude: Search the value of the spline at the maximum
// - kIntegral:  Integral the spline from fHiGainFirst to fHiGainLast,   
//               by counting the edge bins only half and setting the 
//               second derivative to zero, there.
//
void MExtractTimeAndChargeSpline::SetChargeType(MExtralgoSpline::ExtractionType_t typ)
{
  fExtractionType = typ;

  InitArrays(fHiGainFirstDeriv.GetSize());

  switch (fExtractionType)
  {
  case MExtralgoSpline::kAmplitude:
  case MExtralgoSpline::kAmplitudeRel:
  case MExtralgoSpline::kAmplitudeAbs:
      SetResolutionPerPheHiGain(0.053);
      SetResolutionPerPheLoGain(0.016);
      return;

  case MExtralgoSpline::kIntegralRel:
  case MExtralgoSpline::kIntegralAbs:
      switch (fWindowSizeHiGain)
      {
      case 1:
          SetResolutionPerPheHiGain(0.041);
          break;
      case 2:
          SetResolutionPerPheHiGain(0.064);
          break;
      case 3:
      case 4:
          SetResolutionPerPheHiGain(0.050);
          break;
      case 5:
      case 6:
          SetResolutionPerPheHiGain(0.030);
          break;
      default:
          *fLog << warn << GetDescriptor() << ": Could not set the high-gain extractor resolution per phe for window size "
              << fWindowSizeHiGain << "... using default!" << endl;
          SetResolutionPerPheHiGain(0.050);
          break;
      }

      switch (fWindowSizeLoGain)
      {
      case 1:
      case 2:
          SetResolutionPerPheLoGain(0.005);
          break;
      case 3:
      case 4:
          SetResolutionPerPheLoGain(0.017);
          break;
      case 5:
      case 6:
      case 7:
          SetResolutionPerPheLoGain(0.005);
          break;
      case 8:
      case 9:
          SetResolutionPerPheLoGain(0.005);
          break;
      default:
          *fLog << warn << "Could not set the low-gain extractor resolution per phe for window size "
              << fWindowSizeLoGain << "... using default!" << endl;
          SetResolutionPerPheLoGain(0.005);
          break;
      }
  }
}

// --------------------------------------------------------------------------
//
// InitArrays
//
// Gets called in the ReInit() and initialized the arrays
//
Bool_t MExtractTimeAndChargeSpline::InitArrays(Int_t n)
{
    // Initialize arrays to the maximum number of entries necessary
    fHiGainFirstDeriv .Set(n);
    fHiGainSecondDeriv.Set(n);

    fLoGainFirstDeriv .Set(n);
    fLoGainSecondDeriv.Set(n);

    fRiseTimeLoGain = fRiseTimeHiGain * fLoGainStretch;
    fFallTimeLoGain = fFallTimeHiGain * fLoGainStretch;

    switch (fExtractionType)
    {
    case MExtralgoSpline::kAmplitude:
    case MExtralgoSpline::kAmplitudeRel:
    case MExtralgoSpline::kAmplitudeAbs:
        fNumHiGainSamples  = 1.;
        fNumLoGainSamples  = fLoGainLast ? 1. : 0.;
        fSqrtHiGainSamples = 1.;
        fSqrtLoGainSamples = 1.;
        fWindowSizeHiGain  = 1;
        fWindowSizeLoGain  = 1;
        fRiseTimeHiGain    = 0.5;
        break;

    case MExtralgoSpline::kIntegralAbs:
    case MExtralgoSpline::kIntegralRel:
        fNumHiGainSamples  = fRiseTimeHiGain + fFallTimeHiGain;
        fNumLoGainSamples  = fLoGainLast ? fRiseTimeLoGain + fFallTimeLoGain : 0.;
        fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
        fSqrtLoGainSamples = TMath::Sqrt(fNumLoGainSamples);
        fWindowSizeHiGain  = TMath::CeilNint(fRiseTimeHiGain + fFallTimeHiGain);
        fWindowSizeLoGain  = TMath::CeilNint(fRiseTimeLoGain + fFallTimeLoGain);
        break;
    }

    return kTRUE;
}

void MExtractTimeAndChargeSpline::FindTimeAndChargeHiGain2(const Float_t *ptr, Int_t num,
                                                           Float_t &sum, Float_t &dsum,
                                                           Float_t &time, Float_t &dtime,
                                                           Byte_t sat, Int_t maxpos) const
{
    // Do some handling if maxpos is last slice!
    MExtralgoSpline s(ptr, num, fHiGainFirstDeriv.GetArray(), fHiGainSecondDeriv.GetArray());

    s.SetExtractionType(fExtractionType);
    s.SetHeightTm(fHeightTm);
    s.SetRiseFallTime(fRiseTimeHiGain, fFallTimeHiGain);

    if (IsNoiseCalculation())
    {
        sum  = s.ExtractNoise();
        dsum = 1;
        return;
    }

    s.Extract(maxpos);
    s.GetTime(time, dtime);
    s.GetSignal(sum, dsum);
}

void MExtractTimeAndChargeSpline::FindTimeAndChargeLoGain2(const Float_t *ptr, Int_t num,
                                                           Float_t &sum,  Float_t &dsum,
                                                           Float_t &time, Float_t &dtime,
                                                           Byte_t sat, Int_t maxpos) const
{
    MExtralgoSpline s(ptr, num, fLoGainFirstDeriv.GetArray(), fLoGainSecondDeriv.GetArray());

    s.SetExtractionType(fExtractionType);
    s.SetHeightTm(fHeightTm);
    s.SetRiseFallTime(fRiseTimeLoGain, fFallTimeLoGain);

    if (IsNoiseCalculation())
    {
        sum = s.ExtractNoise();
        return;
    }

    s.Extract(maxpos);
    s.GetTime(time, dtime);
    s.GetSignal(sum, dsum);
}

// --------------------------------------------------------------------------
//
// In addition to the resources of the base-class MExtractor:
//   Resolution
//   RiseTimeHiGain
//   FallTimeHiGain
//   LoGainStretch
//   ExtractionType: amplitude, integral
//
Int_t MExtractTimeAndChargeSpline::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

  Bool_t rc = kFALSE;
  
  if (IsEnvDefined(env, prefix, "Resolution", print))
    {
      SetResolution(GetEnvValue(env, prefix, "Resolution",fResolution));
      rc  = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "RiseTimeHiGain", print))
    {
      SetRiseTimeHiGain(GetEnvValue(env, prefix, "RiseTimeHiGain", fRiseTimeHiGain));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "FallTimeHiGain", print))
    {
      SetFallTimeHiGain(GetEnvValue(env, prefix, "FallTimeHiGain", fFallTimeHiGain));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "LoGainStretch", print))
    {
      SetLoGainStretch(GetEnvValue(env, prefix, "LoGainStretch", fLoGainStretch));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "HeightTm", print))
    {
      fHeightTm = GetEnvValue(env, prefix, "HeightTm", fHeightTm);
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "ExtractionType", print))
  {
      TString type = GetEnvValue(env, prefix, "ExtractionType", "");
      type.ToLower();
      type = type.Strip(TString::kBoth);
      if (type==(TString)"amplitude")
          SetChargeType(MExtralgoSpline::kAmplitude);
      if (type==(TString)"integralabsolute")
          SetChargeType(MExtralgoSpline::kIntegralAbs);
      if (type==(TString)"integralrelative")
          SetChargeType(MExtralgoSpline::kIntegralRel);
      rc=kTRUE;
  }

  return MExtractTimeAndCharge::ReadEnv(env, prefix, print) ? kTRUE : rc;
}
