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
//   MExtractTimeFastSpline
//
//   Fast arrival Time extractor using a cubic spline algorithm of Numerical Recipes. 
//   It returns the position of the half maximum between absolute maximum 
//   and pedestal of the spline that interpolates the FADC slices.
//
//   The precision of the half-maximum searches can be chosen by:
//   SetPrecision().
//
//   The precision of the maximum-finder is fixed to 0.025 FADC units.
// 
//////////////////////////////////////////////////////////////////////////////
#include "MExtractTimeFastSpline.h"

#include "MPedestalPix.h"

#include "MLog.h"
#include "MLogManip.h"


ClassImp(MExtractTimeFastSpline);

using namespace std;

const Byte_t  MExtractTimeFastSpline::fgHiGainFirst  = 2;
const Byte_t  MExtractTimeFastSpline::fgHiGainLast   = 14;
const Byte_t  MExtractTimeFastSpline::fgLoGainFirst  = 3;
const Byte_t  MExtractTimeFastSpline::fgLoGainLast   = 14;
const Float_t MExtractTimeFastSpline::fgResolution   = 0.003;
const Float_t MExtractTimeFastSpline::fgRiseTime     = 2.;

// --------------------------------------------------------------------------
//
// Default constructor.
//
// Calls: 
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
// 
// Initializes:
// - fResolution to fgResolution
//
MExtractTimeFastSpline::MExtractTimeFastSpline(const char *name, const char *title) 
    : fHiGainFirstDeriv(NULL), fLoGainFirstDeriv(NULL),
      fHiGainSecondDeriv(NULL), fLoGainSecondDeriv(NULL)
{

  fName  = name  ? name  : "MExtractTimeFastSpline";
  fTitle = title ? title : "Calculate photons arrival time using a fast spline";

  SetResolution();
  SetRiseTime  ();
  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);

}

MExtractTimeFastSpline::~MExtractTimeFastSpline()
{
  
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
// Calls:
// - MExtractor::SetRange(hifirst,hilast,lofirst,lolast);
// - Deletes x, if not NULL
// - Creates x according to the range
//
void MExtractTimeFastSpline::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{

  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  if (fHiGainFirstDeriv)
    delete [] fHiGainFirstDeriv;
  if (fLoGainFirstDeriv)
    delete [] fLoGainFirstDeriv;
  if (fHiGainSecondDeriv)
    delete [] fHiGainSecondDeriv;
  if (fLoGainSecondDeriv)
    delete [] fLoGainSecondDeriv;
  
  Int_t range = fHiGainLast - fHiGainFirst + 1;

  if (range < 2)
    {
      *fLog << warn << GetDescriptor()
            << Form("%s%2i%s%2i%s",": Hi-Gain Extraction range [",(int)fHiGainFirst,","
                    ,fHiGainLast,"] too small, ") << endl;
      *fLog << warn << GetDescriptor()
            << " will move higher limit to obtain 4 slices " << endl;
      SetRange(fHiGainFirst, fHiGainLast+4-range,fLoGainFirst,fLoGainLast);
      range = fHiGainLast - fHiGainFirst + 1;
    }
  

  fHiGainFirstDeriv = new Float_t[range];
  memset(fHiGainFirstDeriv,0,range*sizeof(Float_t));
  fHiGainSecondDeriv = new Float_t[range];
  memset(fHiGainSecondDeriv,0,range*sizeof(Float_t));

  range = fLoGainLast - fLoGainFirst + 1;

  if (range >= 2)
    {
      
      fLoGainFirstDeriv = new Float_t[range];
      memset(fLoGainFirstDeriv,0,range*sizeof(Float_t));
      fLoGainSecondDeriv = new Float_t[range];
      memset(fLoGainSecondDeriv,0,range*sizeof(Float_t));

    }

}


// --------------------------------------------------------------------------
//
// Calculates the arrival time for each pixel 
//
void MExtractTimeFastSpline::FindTimeHiGain(Byte_t *first, Float_t &time, Float_t &dtime, 
                                        Byte_t &sat, const MPedestalPix &ped) const
{

  const Int_t range = fHiGainLast - fHiGainFirst + 1;
  const Byte_t *end = first + range;
  Byte_t *p = first;
  Byte_t max    = 0;
  Byte_t maxpos = 0;

  //
  // Check for saturation in all other slices
  //
  while (p<end)
    {
      if (*p > max)
        {
          max    = *p;
          maxpos =  p-first;
        }

      if (*p++ >= fSaturationLimit)
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

  if (maxpos < 1)
    {
      time = -999.;
      return;
    }
  
  Float_t pp;

  p = first;
  fHiGainSecondDeriv[0] = 0.;
  fHiGainFirstDeriv[0]  = 0.;

  for (Int_t i=1;i<range-1;i++)
    {
      p++;
      pp = fHiGainSecondDeriv[i-1] + 4.;
      fHiGainSecondDeriv[i] = -1.0/pp;
      fHiGainFirstDeriv [i] = *(p+1) - 2.* *(p) + *(p-1);
      fHiGainFirstDeriv [i] = (6.0*fHiGainFirstDeriv[i]-fHiGainFirstDeriv[i-1])/pp;
    }

  fHiGainSecondDeriv[range-1] = 0.;

  for (Int_t k=range-2;k>0;k--)
    fHiGainSecondDeriv[k] = fHiGainSecondDeriv[k]*fHiGainSecondDeriv[k+1] + fHiGainFirstDeriv[k];
  for (Int_t k=range-2;k>0;k--)
    fHiGainSecondDeriv[k] /= 6.;
  
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
  Float_t klocont = (Float_t)*(first+klo);
  Float_t khicont = (Float_t)*(first+khi);
  time          = upper;
  Float_t abmax = khicont;

  //
  // Search for the maximum, starting in interval maxpos-1 in steps of 0.2 till maxpos-0.2.
  // If no maximum is found, go to interval maxpos+1.
  //
  Float_t higainklo = fHiGainSecondDeriv[klo];
  Float_t higainkhi = fHiGainSecondDeriv[khi];

  while ( x < upper - 0.3 )
    {
      
      x += step;
      a -= step;
      b += step;
      
      y = a*klocont
        + b*khicont
        + (a*a*a-a)*higainklo
        + (b*b*b-b)*higainkhi;
      
      if (y > abmax)
        {
          abmax  = y;
          time   = x;
        }
    }
  
  //
  // Search for the absolute maximum from maxpos to maxpos+1 in steps of 0.2
  //
  if (time > upper-0.1)
    {

      upper = (Float_t)maxpos+1.;
      lower = (Float_t)maxpos;
      x     = lower;
      a     = 1.;
      b     = 0.;
      khi   = maxpos+1;
      klo   = maxpos;
      klocont = (Float_t)*(first+klo);
      khicont = (Float_t)*(first+khi);

      higainklo = fHiGainSecondDeriv[klo];
      higainkhi = fHiGainSecondDeriv[khi];

      while (x<upper-0.3)
        {

          x += step;
          a -= step;
          b += step;
          
          y = a* klocont
            + b* khicont
            + (a*a*a-a)*higainklo
            + (b*b*b-b)*higainkhi;
          
          if (y > abmax)
            {
              abmax  = y;
              time   = x;
            }
        }
  }

  //
  // Now, the time, abmax and khicont and klocont are set correctly within the previous precision.
  // Try a better precision. 
  //
  const Float_t up = time+step-0.035;
  const Float_t lo = time-step+0.035;
  const Float_t maxpossave = time;
  
  x     = time;
  a     = upper - x;
  b     = x - lower;

  step  = 0.025; // step size of 83 ps 

  higainklo = fHiGainSecondDeriv[klo];
  higainkhi = fHiGainSecondDeriv[khi];

  //
  // First, try from time up to time+0.2 in steps of 83ps.
  //
  while ( x < up )
    {

      x += step;
      a -= step;
      b += step;
      
      y = a* klocont
        + b* khicont
        + (a*a*a-a)*higainklo
        + (b*b*b-b)*higainkhi;
      
      if (y > abmax)
        {
          abmax  = y;
          time   = x;
        }
      
    }


  //
  // Second, try from time down to time-0.2 in steps of 0.04.
  //
  x     = maxpossave;
  //
  // Test the possibility that the absolute maximum has not been found between
  // maxpos and maxpos+0.02, then we have to look between maxpos-0.02 and maxpos
  // which requires new setting of klocont and khicont
  //
  if (x < klo + 0.02)
    {
      klo--;
      khi--;
      klocont = (Float_t)*(first+klo);
      khicont = (Float_t)*(first+khi);
      upper--;
      lower--;
    }
  
  a     = upper - x;
  b     = x - lower;

  higainklo = fHiGainSecondDeriv[klo];
  higainkhi = fHiGainSecondDeriv[khi];

  while ( x > lo )
    {

      x -= step;
      a += step;
      b -= step;
      
      y = a* klocont
        + b* khicont
        + (a*a*a-a)*higainklo
        + (b*b*b-b)*higainkhi;
      
      if (y > abmax)
        {
          abmax  = y;
          time   = x;
        }
    }

#if 0
  const Float_t pedes   = ped.GetPedestal();
  const Float_t halfmax = pedes + (abmax - pedes)/2.;

  //
  // Now, loop from the maximum bin leftward down in order to find the position of the half maximum.
  // First, find the right FADC slice:
  // 
  klo   = maxpos;
  while (klo > maxpos-fStartBeforeMax)
    {
      if (*(first+klo) < (Byte_t)halfmax)
        break;
      klo--;
    }

  //
  // Loop from the beginning of the slice upwards to reach the halfmax:
  // With means of bisection:
  // 
  x     = (Float_t)klo;
  a     = 1.;
  b     = 0.;
  klocont = (Float_t)*(first+klo);
  khicont = (Float_t)*(first+klo+1);

  step = 0.5;
  Bool_t back = kFALSE;

  while (step > fResolution)
    {
      
      if (back)
        {
          x -= step;
          a += step;
          b -= step;
        }
      else
        {
          x += step;
          a -= step;
          b += step;
        }
      
      y = a*klocont
        + b*khicont
        + (a*a*a-a)*fHiGainSecondDeriv[klo] 
        + (b*b*b-b)*fHiGainSecondDeriv[khi];

      if (y >= halfmax)
        back = kTRUE;
      else
        back = kFALSE;

      step /= 2.;
      
    }
  time  = (Float_t)fHiGainFirst + x;

#endif
  dtime = 0.035;

}


// --------------------------------------------------------------------------
//
// Calculates the arrival time for each pixel 
//
void MExtractTimeFastSpline::FindTimeLoGain(Byte_t *first, Float_t &time, Float_t &dtime, 
                                        Byte_t &sat, const MPedestalPix &ped) const
{
  
  const Int_t range = fLoGainLast - fLoGainFirst + 1;
  const Byte_t *end = first + range;
  Byte_t       *p   = first;
  Byte_t max    = 0;
  Byte_t maxpos = 0;

  //
  // Check for saturation in all other slices
  //
  while (p<end)
    {
      if (*p > max)
        {
          max    = *p;
          maxpos =  p-first;
        }

      if (*p++ >= fSaturationLimit)
        {
          sat++;
          break;
        }
    }
  
  if (sat)
    return;

  if (maxpos < 1)
    return;
  
  Float_t pp;

  p = first;
  fLoGainSecondDeriv[0] = 0.;
  fLoGainFirstDeriv[0]  = 0.;

  for (Int_t i=1;i<range-1;i++)
    {
      p++;
      pp = fLoGainSecondDeriv[i-1] + 4.;
      fLoGainSecondDeriv[i] = -1.0/pp;
      fLoGainFirstDeriv [i] = *(p+1) - 2.* *(p) + *(p-1);
      fLoGainFirstDeriv [i] = (6.0*fLoGainFirstDeriv[i]-fLoGainFirstDeriv[i-1])/pp;
    }

  fLoGainSecondDeriv[range-1] = 0.;

  for (Int_t k=range-2;k>0;k--)
    fLoGainSecondDeriv[k] = fLoGainSecondDeriv[k]*fLoGainSecondDeriv[k+1] + fLoGainFirstDeriv[k];
  for (Int_t k=range-2;k>0;k--)
    fLoGainSecondDeriv[k] /= 6.;
  
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
  Float_t klocont = (Float_t)*(first+klo);
  Float_t khicont = (Float_t)*(first+khi);
  time          = upper;
  Float_t abmax = khicont;

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

      if (y > abmax)
        {
          abmax  = y;
          time   = x;
        }

    }

 if (time > upper-0.1)
    {

      upper = (Float_t)maxpos+1.;
      lower = (Float_t)maxpos;
      x     = lower;
      a     = 1.;
      b     = 0.;
      khi   = maxpos+1;
      klo   = maxpos;
      klocont = (Float_t)*(first+klo);
      khicont = (Float_t)*(first+khi);

      while (x<upper-0.3)
        {

          x += step;
          a -= step;
          b += step;
          
          y = a* klocont
            + b* khicont
            + (a*a*a-a)*fLoGainSecondDeriv[klo] 
            + (b*b*b-b)*fLoGainSecondDeriv[khi];
          
          if (y > abmax)
            {
              abmax  = y;
              time   = x;
            }
        }
  }
 
  const Float_t up = time+step-0.055;
  const Float_t lo = time-step+0.055;
  const Float_t maxpossave = time;

  x     = time;
  a     = upper - x;
  b     = x - lower;

  step  = 0.025; // step size of 165 ps 

  while (x<up)
    {

      x += step;
      a -= step;
      b += step;
      
      y = a* klocont
        + b* khicont
        + (a*a*a-a)*fLoGainSecondDeriv[klo] 
        + (b*b*b-b)*fLoGainSecondDeriv[khi];
      
      if (y > abmax)
        {
          abmax  = y;
          time   = x;
        }
      
    }

  if (time < klo + 0.01)
    {
      klo--;
      khi--;
      klocont = (Float_t)*(first+klo);
      khicont = (Float_t)*(first+khi);
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
      
      if (y > abmax)
        {
          abmax  = y;
          time   = x;
        }
      
    }

  const Float_t pedes   = ped.GetPedestal();
  const Float_t halfmax = pedes + (abmax - pedes)/2.;

  //
  // Now, loop from the maximum bin leftward down in order to find the position of the half maximum.
  // First, find the right FADC slice:
  // 
  klo   = maxpos;
  while (klo > maxpos-4)
    {
      if (*(first+klo) < (Byte_t)halfmax)
        break;
      klo--;
    }

  //
  // Loop from the beginning of the slice upwards to reach the halfmax:
  // With means of bisection:
  // 
  x     = (Float_t)klo;
  a     = 1.;
  b     = 0.;
  klocont = (Float_t)*(first+klo);
  khicont = (Float_t)*(first+klo+1);
  time  = x;

  step = 0.5;
  Bool_t back = kFALSE;

  while (step > fResolution)
    {
      
      if (back)
        {
          x -= step;
          a += step;
          b -= step;
        }
      else
        {
          x += step;
          a -= step;
          b += step;
        }
      
      y = a*klocont
        + b*khicont
        + (a*a*a-a)*fLoGainSecondDeriv[klo] 
        + (b*b*b-b)*fLoGainSecondDeriv[khi];

      if (y >= halfmax)
        back = kTRUE;
      else
        back = kFALSE;

      step /= 2.;

    }

  time  = (Float_t)fLoGainFirst + x;
  dtime = fResolution;
}

// --------------------------------------------------------------------------
//
// In addition to the resources of the base-class MExtractor:
//   MJPedestal.MExtractor.Resolution: 0.003
//   MJPedestal.MExtractor.RiseTime:   1.5
//
Int_t MExtractTimeFastSpline::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "HiGainWindowSize", print))
    {
        SetResolution(GetEnvValue(env, prefix, "Resolution", fResolution));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "LoGainWindowSize", print))
    {
        SetRiseTime(GetEnvValue(env, prefix, "RiseTime", fRiseTime));
        rc = kTRUE;
    }

    return MExtractTime::ReadEnv(env, prefix, print) ? kTRUE : rc;
}

void MExtractTimeFastSpline::Print(Option_t *o) const
{
    *fLog << all;
    *fLog << GetDescriptor() << ":" << endl;
    *fLog << " Resolution:     " << fResolution << endl;
    *fLog << " RiseTime:       " << fRiseTime << endl;
    MExtractTime::Print(o);
}
