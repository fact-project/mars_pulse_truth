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
!   Author(s): Markus Gaug       05/2004 <mailto:markus@ifae.es> 
!
!   Copyright: MAGIC Software Development, 2002-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractTimeAndChargeSpline
//
//   Fast Spline extractor using a cubic spline algorithm of Numerical Recipes. 
//   It returns the integral below the interpolating spline. 
// 
//   Call: SetRange(fHiGainFirst, fHiGainLast, fLoGainFirst, fLoGainLast) 
//         to modify the ranges. Ranges have to be an even number. In case of odd 
//         ranges, the last slice will be reduced by one.
//
//  Defaults are: 
// 
//   fHiGainFirst =  fgHiGainFirst =  3 
//   fHiGainLast  =  fgHiGainLast  =  14
//   fLoGainFirst =  fgLoGainFirst =  3 
//   fLoGainLast  =  fgLoGainLast  =  14
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractAmplitudeSpline.h"

#include "MExtractedSignalCam.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MExtractAmplitudeSpline);

using namespace std;

const Byte_t  MExtractAmplitudeSpline::fgHiGainFirst  = 2;
const Byte_t  MExtractAmplitudeSpline::fgHiGainLast   = 14;
const Byte_t  MExtractAmplitudeSpline::fgLoGainFirst  = 3;
const Byte_t  MExtractAmplitudeSpline::fgLoGainLast   = 14;
const Float_t MExtractAmplitudeSpline::fgResolution   = 0.003;
// --------------------------------------------------------------------------
//
// Default constructor.
//
// Calls: 
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
// 
MExtractAmplitudeSpline::MExtractAmplitudeSpline(const char *name, const char *title) 
    : fHiGainSignal(NULL), fLoGainSignal(NULL),
      fHiGainFirstDeriv(NULL), fLoGainFirstDeriv(NULL),
      fHiGainSecondDeriv(NULL), fLoGainSecondDeriv(NULL)
{

  fName  = name  ? name  : "MExtractAmplitudeSpline";
  fTitle = title ? title : "Signal Extractor for a fixed FADC window using a fast spline";

  SetResolution();
  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
}

MExtractAmplitudeSpline::~MExtractAmplitudeSpline()
{
  
  if (fHiGainSignal)
    delete [] fHiGainSignal;
  if (fLoGainSignal)
    delete [] fLoGainSignal;
  if (fHiGainFirstDeriv)
    delete [] fHiGainFirstDeriv;
  if (fLoGainFirstDeriv)
    delete [] fLoGainFirstDeriv;
  if (fHiGainSecondDeriv)
    delete [] fHiGainSecondDeriv;
  if (fLoGainSecondDeriv)
    delete [] fLoGainSecondDeriv;
  
}

// --------------------------------------------------------------------------
//
// SetRange: 
//
// Checks: 
// - if the window defined by (fHiGainLast-fHiGainFirst-1) are odd, subtract one
// - if the window defined by (fLoGainLast-fLoGainFirst-1) are odd, subtract one
// - if the Hi Gain window is smaller than 2, set fHiGainLast to fHiGainFirst+1
// - if the Lo Gain window is smaller than 2, set fLoGainLast to fLoGainFirst+1
// 
// Calls:
// - MExtractor::SetRange(hifirst,hilast,lofirst,lolast);
// 
// Sets:
// - fNumHiGainSamples to: (Float_t)(fHiGainLast-fHiGainFirst+1)
// - fNumLoGainSamples to: (Float_t)(fLoGainLast-fLoGainFirst+1)
// - fSqrtHiGainSamples to: TMath::Sqrt(fNumHiGainSamples)
// - fSqrtLoGainSamples to: TMath::Sqrt(fNumLoGainSamples)  
//  
void MExtractAmplitudeSpline::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{


  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  fNumHiGainSamples = 2.;
  fNumLoGainSamples = lolast == 0 ? 0. : 2.;

  fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
  fSqrtLoGainSamples = TMath::Sqrt(fNumLoGainSamples);

  fHiLoLast  = 0;
  
}

// --------------------------------------------------------------------------
//
// ReInit
//
// Calls:
// - MExtractor::ReInit(pList);
// - fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast+fHiLoLast, fNumHiGainSamples,
//                                fLoGainFirst, fLoGainLast, fNumLoGainSamples);
// 
// Deletes all arrays, if not NULL
// Creates new arrays according to the extraction range
//
Bool_t MExtractAmplitudeSpline::ReInit(MParList *pList)
{

  if (!MExtractor::ReInit(pList))
    return kFALSE;

  fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast+fHiLoLast, fNumHiGainSamples,
                              fLoGainFirst, fLoGainLast, fNumLoGainSamples);

  if (fHiGainSignal)
    delete [] fHiGainSignal;
  if (fHiGainFirstDeriv)
    delete [] fHiGainFirstDeriv;
  if (fHiGainSecondDeriv)
    delete [] fHiGainSecondDeriv;
  if (fLoGainSignal)
    delete [] fLoGainSignal;
  if (fLoGainFirstDeriv)
    delete [] fLoGainFirstDeriv;
  if (fLoGainSecondDeriv)
    delete [] fLoGainSecondDeriv;
  
  Int_t range = fHiGainLast - fHiGainFirst + 1 + fHiLoLast;

  fHiGainSignal      = new Float_t[range];
  memset(fHiGainSignal,0,range*sizeof(Float_t));
  fHiGainFirstDeriv  = new Float_t[range];
  memset(fHiGainFirstDeriv,0,range*sizeof(Float_t));
  fHiGainSecondDeriv = new Float_t[range];
  memset(fHiGainSecondDeriv,0,range*sizeof(Float_t));

  *fLog << endl;
  *fLog << inf << GetDescriptor() << ": Using for High-Gain Extraction " << range
        << " FADC samples from " 
        << Form("%s%2i%s"," High Gain slice ",(Int_t)fHiGainFirst," to (including) ")
        << Form("%s%2i",fHiLoLast ? "Low Gain slice " : " High Gain slice ",
                fHiLoLast ?  (Int_t)fHiLoLast : (Int_t)fHiGainLast ) 
        << endl;

  range = fLoGainLast ? fLoGainLast - fLoGainFirst + 1 : 0;

  fLoGainSignal = new Float_t[range];
  memset(fLoGainSignal,0,range*sizeof(Float_t));
  fLoGainFirstDeriv  = new Float_t[range];
  memset(fLoGainFirstDeriv,0,range*sizeof(Float_t));
  fLoGainSecondDeriv = new Float_t[range];
  memset(fLoGainSecondDeriv,0,range*sizeof(Float_t));
  
  *fLog << endl;
  *fLog << inf << GetDescriptor() << ": Using for Low-Gain Extraction " << range
        << " FADC samples from " 
        << Form("%s%2i%s%2i"," Low Gain slice ",(Int_t)fLoGainFirst,
                " to (including) ",(Int_t)fLoGainLast) << endl;

  return kTRUE;
}


// --------------------------------------------------------------------------
//
// FindSignalHiGain:
//
// - Loop from ptr to (ptr+fHiGainLast-fHiGainFirst)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// 
// - If fHiLoLast is not 0, loop also from logain to (logain+fHiLoLast)
// - Sum up contents of logain
// - If *logain is greater than fSaturationLimit, raise sat by 1
//
void MExtractAmplitudeSpline::FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat) const
{
  
  Int_t count   = 0;
  Float_t abmaxpos = 0.;
  Byte_t max    = 0;
  Byte_t maxpos = 0;
  
  Int_t range = fHiGainLast - fHiGainFirst + 1;
  Byte_t *end = ptr + range;
  Byte_t *p     = ptr;
  //
  // Check for saturation in all other slices
  //
  while (++p<end)
    {
      
      fHiGainSignal[count] = (Float_t)*p;
      
      if (*p > max)
        {
          max    = *p;
          maxpos =  count;
        }
      
      count++;
      
      if (*p >= fSaturationLimit)
        {
          sat++;
              break;
        }
    }
  
  if (fHiLoLast != 0)
    {
      
      p    = logain;
      end  = logain + fHiLoLast + 1;
      
      while (p<end)
        {
          
          fHiGainSignal[count] = (Float_t)*p;

          if (*p > max)
            {
              max    = *p;
              maxpos =  count;
            }
          
          range++;
          count++;

          if (*p++ >= fSaturationLimit)
            {
              sat++;
              break;
            }
        }
    }

  //
  // allow one saturated slice 
  //
  if (sat > 1)
    return;

  //
  // Don't start if the maxpos is too close to the left limit.
  //
  if (maxpos < 2)
    return;

  Float_t pp;
  fHiGainSecondDeriv[0] = 0.;
  fHiGainFirstDeriv[0]  = 0.;

  for (Int_t i=1;i<range-1;i++)
    {
      pp = fHiGainSecondDeriv[i-1] + 4.;
      fHiGainSecondDeriv[i] = -1.0/pp;
      fHiGainFirstDeriv [i] = fHiGainSignal[i+1] - fHiGainSignal[i] - fHiGainSignal[i] + fHiGainSignal[i-1];
      fHiGainFirstDeriv [i] = (6.0*fHiGainFirstDeriv[i]-fHiGainFirstDeriv[i-1])/pp;
      p++;
    }

  fHiGainSecondDeriv[range-1] = 0.;
  for (Int_t k=range-2;k>=0;k--)
    fHiGainSecondDeriv[k] = (fHiGainSecondDeriv[k]*fHiGainSecondDeriv[k+1] + fHiGainFirstDeriv[k])/6.;
  
  //
  // Now find the maximum  
  //
  Float_t step  = 0.2; // start with step size of 1ns and loop again with the smaller one
  Float_t lower = (Float_t)maxpos-1.;
  Float_t upper = (Float_t)maxpos;
  Float_t x     = lower;
  Float_t y     = 0.;
  Float_t a     = 1.;
  Float_t b     = 0.;
  Int_t   klo = maxpos-1;
  Int_t   khi = maxpos;
  Float_t klocont = fHiGainSignal[klo];
  Float_t khicont = fHiGainSignal[khi];
  sum       = khicont;
  abmaxpos  = lower;

  //
  // Search for the maximum, starting in interval maxpos-1. If no maximum is found, go to 
  // interval maxpos+1.
  //
  while (x<upper-0.3)
    {

      x += step;
      a -= step;
      b += step;

      y = a*klocont
        + b*khicont
        + (a*a*a-a)*fHiGainSecondDeriv[klo] 
        + (b*b*b-b)*fHiGainSecondDeriv[khi];

      if (y > sum)
        {
          sum      = y;
          abmaxpos = x;
        }
    }

  if (abmaxpos > upper-0.1)
    {
      
      upper = (Float_t)maxpos+1;
      lower = (Float_t)maxpos;
      x     = lower;
      a     = 1.;
      b     = 0.;
      khi   = maxpos+1;
      klo   = maxpos;
      klocont = fHiGainSignal[klo];
      khicont = fHiGainSignal[khi];

      while (x<upper-0.3)
        {

          x += step;
          a -= step;
          b += step;
          
          y = a* klocont
            + b* khicont
            + (a*a*a-a)*fHiGainSecondDeriv[klo] 
            + (b*b*b-b)*fHiGainSecondDeriv[khi];
          
          if (y > sum)
            {
              sum    = y;
              abmaxpos = x;
            }
        }
    }

 const Float_t up = abmaxpos+step-0.055;
 const Float_t lo = abmaxpos-step+0.055;
 const Float_t maxpossave = abmaxpos;
 
 x     = abmaxpos;
 a     = upper - x;
 b     = x - lower;

  step  = 0.04; // step size of 83 ps 

  while (x<up)
    {

      x += step;
      a -= step;
      b += step;
      
      y = a* klocont
        + b* khicont
        + (a*a*a-a)*fHiGainSecondDeriv[klo] 
        + (b*b*b-b)*fHiGainSecondDeriv[khi];
      
      if (y > sum)
        {
          sum    = y;
          abmaxpos = x;
        }
    }

 if (abmaxpos < klo + 0.02)
    {
      klo--;
      khi--;
      klocont = fHiGainSignal[klo];
      khicont = fHiGainSignal[khi];
      upper--;
      lower--;
    }
 
  x     = maxpossave;
  a     = upper - x;
  b     = x - lower;

  while (x>lo)
    {

      x -= step;
      a += step;
      b -= step;
      
      y = a* klocont
        + b* khicont
        + (a*a*a-a)*fHiGainSecondDeriv[klo] 
        + (b*b*b-b)*fHiGainSecondDeriv[khi];
      
      if (y > sum)
        sum      = y;
    }
}


// --------------------------------------------------------------------------
//
// FindSignalLoGain:
//
// - Loop from ptr to (ptr+fLoGainLast-fLoGainFirst)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// 
void MExtractAmplitudeSpline::FindSignalLoGain(Byte_t *ptr, Float_t &sum, Byte_t &sat) const
{
  
  Int_t count   = 0;
  Float_t abmaxpos = 0.;
  Byte_t max    = 0;
  Byte_t maxpos = 0;
  
  Int_t range = fLoGainLast - fLoGainFirst + 1;
  Byte_t *end = ptr + range;
  Byte_t *p   = ptr;
  //
  // Check for saturation in all other slices
  //
  while (++p<end)
    {
      
      fLoGainSignal[count] = (Float_t)*p;
      
      if (*p > max)
        {
          max    = *p;
          maxpos =  count;
        }
      
      range++;
      count++;
      
      if (*p >= fSaturationLimit)
        {
          sat++;
              break;
        }
    }
  

  //
  // allow one saturated slice 
  //
  if (sat > 1)
    return;

  //
  // Don't start if the maxpos is too close to the left limit.
  //
  if (maxpos < 2)
    return;

  Float_t pp;
  fLoGainSecondDeriv[0] = 0.;
  fLoGainFirstDeriv[0]  = 0.;

  for (Int_t i=1;i<range-1;i++)
    {
      pp = fLoGainSecondDeriv[i-1] + 4.;
      fLoGainSecondDeriv[i] = -1.0/pp;
      fLoGainFirstDeriv [i] = fLoGainSignal[i+1] - fLoGainSignal[i] - fLoGainSignal[i] + fLoGainSignal[i-1];
      fLoGainFirstDeriv [i] = (6.0*fLoGainFirstDeriv[i]-fLoGainFirstDeriv[i-1])/pp;
      p++;
    }

  fLoGainSecondDeriv[range-1] = 0.;
  for (Int_t k=range-2;k>=0;k--)
    fLoGainSecondDeriv[k] = (fLoGainSecondDeriv[k]*fLoGainSecondDeriv[k+1] + fLoGainFirstDeriv[k])/6.;
  
  //
  // Now find the maximum  
  //
  Float_t step  = 0.2; // start with step size of 1ns and loop again with the smaller one
  Float_t lower = (Float_t)maxpos-1.;
  Float_t upper = (Float_t)maxpos;
  Float_t x     = lower;
  Float_t y     = 0.;
  Float_t a     = 1.;
  Float_t b     = 0.;
  Int_t   klo = maxpos-1;
  Int_t   khi = maxpos;
  Float_t klocont = fLoGainSignal[klo];
  Float_t khicont = fLoGainSignal[khi];
  sum       = khicont;
  abmaxpos  = lower;

  //
  // Search for the maximum, starting in interval maxpos-1. If no maximum is found, go to 
  // interval maxpos+1.
  //
  while (x<upper-0.3)
    {

      x += step;
      a -= step;
      b += step;

      y = a*klocont
        + b*khicont
        + (a*a*a-a)*fLoGainSecondDeriv[klo] 
        + (b*b*b-b)*fLoGainSecondDeriv[khi];

      if (y > sum)
        {
          sum      = y;
          abmaxpos = x;
        }
    }

  if (abmaxpos > upper-0.1)
    {
      
      upper = (Float_t)maxpos+1;
      lower = (Float_t)maxpos;
      x     = lower;
      a     = 1.;
      b     = 0.;
      khi   = maxpos+1;
      klo   = maxpos;
      klocont = fLoGainSignal[klo];
      khicont = fLoGainSignal[khi];

      while (x<upper-0.3)
        {

          x += step;
          a -= step;
          b += step;
          
          y = a* klocont
            + b* khicont
            + (a*a*a-a)*fLoGainSecondDeriv[klo] 
            + (b*b*b-b)*fLoGainSecondDeriv[khi];
          
          if (y > sum)
            {
              sum    = y;
              abmaxpos = x;
            }
        }
    }

 const Float_t up = abmaxpos+step-0.055;
 const Float_t lo = abmaxpos-step+0.055;
 const Float_t maxpossave = abmaxpos;
 
 x     = abmaxpos;
 a     = upper - x;
 b     = x - lower;

  step  = 0.04; // step size of 83 ps 

  while (x<up)
    {

      x += step;
      a -= step;
      b += step;
      
      y = a* klocont
        + b* khicont
        + (a*a*a-a)*fLoGainSecondDeriv[klo] 
        + (b*b*b-b)*fLoGainSecondDeriv[khi];
      
      if (y > sum)
        {
          sum    = y;
          abmaxpos = x;
        }
    }

 if (abmaxpos < klo + 0.02)
    {
      klo--;
      khi--;
      klocont = fLoGainSignal[klo];
      khicont = fLoGainSignal[khi];
      upper--;
      lower--;
    }
 
  x     = maxpossave;
  a     = upper - x;
  b     = x - lower;

  while (x>lo)
    {

      x -= step;
      a += step;
      b -= step;
      
      y = a* klocont
        + b* khicont
        + (a*a*a-a)*fLoGainSecondDeriv[klo] 
        + (b*b*b-b)*fLoGainSecondDeriv[khi];
      
      if (y > sum)
        sum      = y;
    }
}

