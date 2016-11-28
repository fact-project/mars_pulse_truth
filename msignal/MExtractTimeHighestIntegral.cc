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
!   Author(s): Thomas Bretz, 02/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Hendrik Bartko, 02/2004 <mailto:hbartko@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractTimeHighestIntegral
//
//  Calculates the arrival time as the mean time of the fWindowSize time slices
//  which have the highest integral content.
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractTimeHighestIntegral.h"

#include "MLog.h"
#include "MLogManip.h"
#include "MPedestalPix.h"

ClassImp(MExtractTimeHighestIntegral);

using namespace std;

const Byte_t MExtractTimeHighestIntegral::fgHiGainFirst  =  0;
const Byte_t MExtractTimeHighestIntegral::fgHiGainLast   = 14;
const Byte_t MExtractTimeHighestIntegral::fgLoGainFirst  =  3;
const Byte_t MExtractTimeHighestIntegral::fgLoGainLast   = 14;
const Byte_t MExtractTimeHighestIntegral::fgHiGainWindowSize = 6;
const Byte_t MExtractTimeHighestIntegral::fgLoGainWindowSize = 6;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Calls: 
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
//
MExtractTimeHighestIntegral::MExtractTimeHighestIntegral(const char *name, const char *title)
    : fHiGainWindowSize(fgHiGainWindowSize), 
      fLoGainWindowSize(fgLoGainWindowSize)
{

  fName  = name  ? name  : "MExtractTimeHighestIntegral";
  fTitle = title ? title : "Task to extract the signal from the FADC slices";
  
  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
}

// --------------------------------------------------------------------------
//
// SetRange: 
//
// Calls:
// - MExtractor::SetRange(hifirst,hilast,lofirst,lolast);
//
void MExtractTimeHighestIntegral::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{
  
  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  Int_t range = fHiGainLast - fHiGainFirst + 1;
  
  if (range < 2)
    {
      *fLog << warn << GetDescriptor()
            << Form("%s%2i%s%2i%s",": Hi-Gain Extraction range [",(int)fHiGainFirst,","
                    ,fHiGainLast,"] too small, ") << endl;
      *fLog << warn << GetDescriptor()
            << " will move higher limit to obtain 4 slices " << endl;
      SetRange(fHiGainFirst, fHiGainLast+4-range,fLoGainFirst,fLoGainLast);
    }
  
  if (fLoGainLast != 0)
    {
      range = fLoGainLast - fLoGainFirst + 1;
      
      if (range < 2)
        {
          *fLog << warn << GetDescriptor()
                << Form("%s%2i%s%2i%s",": Lo-Gain Extraction range [",(int)fLoGainFirst,","
                        ,fLoGainLast,"] too small, ") << endl;
          *fLog << warn << GetDescriptor()
                << " will move lower limit to obtain 4 slices " << endl;
          SetRange(fHiGainFirst, fHiGainLast,fLoGainFirst,fLoGainLast+4-range);
        }
    }

  SetWindowSize(fHiGainWindowSize,fLoGainWindowSize);

  fNumHiGainSamples = fHiGainLast-fHiGainFirst+1;
  fNumLoGainSamples = fLoGainLast ? fLoGainLast-fLoGainFirst+1 : 0;
  
}

// --------------------------------------------------------------------------
//
// Checks:
// - if a window is odd, subtract one
// - if a window is bigger than the one defined by the ranges, set it to the available range
// - if a window is smaller than 2, set it to 2
// 
void MExtractTimeHighestIntegral::SetWindowSize(Byte_t windowh, Byte_t windowl)
{

  fHiGainWindowSize = windowh & ~1;
  fLoGainWindowSize = windowl & ~1;

  if (fHiGainWindowSize != windowh)
    *fLog << warn << GetDescriptor() << ": Hi Gain window size has to be even, set to: " 
          << int(fHiGainWindowSize) << " samples " << endl;
  
  if (fLoGainWindowSize != windowl)
    *fLog << warn << GetDescriptor() << ": Lo Gain window size has to be even, set to: " 
          << int(fLoGainWindowSize) << " samples " << endl;
    
  const Byte_t availhirange = (fHiGainLast-fHiGainFirst+1) & ~1;
  const Byte_t availlorange = (fLoGainLast-fLoGainFirst+1) & ~1;

  if (fHiGainWindowSize > availhirange)
    {
      *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i%s%2i%s",": Hi Gain window size: ",(int)fHiGainWindowSize,
                    " is bigger than available range: [",(int)fHiGainFirst,",",(int)fHiGainLast,"]") << endl;
      *fLog << warn << GetDescriptor() 
            << ": Will set window size to: " << (int)availhirange << endl;
      fHiGainWindowSize = availhirange;
    }
  
  if (fLoGainWindowSize > availlorange)
    {
      *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i%s%2i%s",": Lo Gain window size: ",(int)fLoGainWindowSize,
                    " is bigger than available range: [",(int)fLoGainFirst,",",(int)fLoGainLast,"]") << endl;
      *fLog << warn << GetDescriptor() 
            << ": Will set window size to: " << (int)availlorange << endl;
      fLoGainWindowSize= availlorange;
    }

  fHiGainWindowSizeSqrt = TMath::Sqrt((Float_t)fHiGainWindowSize);
  fLoGainWindowSizeSqrt = TMath::Sqrt((Float_t)fLoGainWindowSize);
}


void MExtractTimeHighestIntegral::FindTimeHiGain(Byte_t *ptr, Float_t &time, Float_t &deltatime, Byte_t &sat, const MPedestalPix &ped) const
{

  const Byte_t *end = ptr + fHiGainLast - fHiGainFirst +1 ;

  Int_t sum=0;    // integral content of the actual window
  Int_t max = 0;  // highest integral content of all windows
  
  //
  // Calculate the sum of the first fWindowSize slices
  //
  sat = 0;
  Byte_t *p = ptr;
  
  while (p<ptr+fHiGainWindowSize)
    {
      sum += *p;
      if (*p++ >= fSaturationLimit)
        sat++;
    }
  
  //
  // Check for saturation in all other slices
  //
  while (p<end)
    if (*p++ >= fSaturationLimit)
      sat++;
  
  //
  // Calculate the i-th sum as
  //    sum_i+1 = sum_i + slice[i+8] - slice[i]
  // This is fast and accurate (because we are using int's)
  //
  max=sum;
  Byte_t *ptrmax=ptr;  // pointer to the first slice of the maximum window
  
  for (p=ptr; p+fHiGainWindowSize<end; p++)
    {
      sum += *(p+fHiGainWindowSize) - *p;
      
      if (sum>max)
	{
	  max = sum;
	  ptrmax = p+1;
	}
    }
  
  // now calculate the time for the maximum window
  Int_t timesignalsum = 0;
  Int_t timesquaredsum =0;
  Int_t timesum =0;
  
  for (p=ptrmax; p < ptrmax + fHiGainWindowSize; p++)
    {
      timesignalsum += *p*(p-ptr);
      timesum += p-ptr;
      timesquaredsum  += (p-ptr)*(p-ptr);
    }
  
  const Float_t pedes  = ped.GetPedestal();
  const Float_t pedrms = ped.GetPedestalRms();
  const Float_t pedsubsum = max - fHiGainWindowSize*pedes;
  const Float_t pedsubtimesignalsum = timesignalsum - timesum*pedes;

  time      = pedsubsum != 0 ? pedsubtimesignalsum / pedsubsum  + Float_t(fHiGainFirst): 1;
  deltatime = pedsubsum != 0 ? pedrms / pedsubsum * sqrt(timesquaredsum - fHiGainWindowSize*time) : 1;
}

void MExtractTimeHighestIntegral::FindTimeLoGain(Byte_t *ptr, Float_t &time, Float_t &deltatime, Byte_t &sat, const MPedestalPix &ped) const
{

  const Byte_t *end = ptr + fLoGainLast - fLoGainFirst +1 ;

  Int_t sum=0;    // integral content of the actual window
  Int_t max = 0;  // highest integral content of all windows
  
  //
  // Calculate the sum of the first fWindowSize slices
  //
  sat = 0;
  Byte_t *p = ptr;
  
  while (p<ptr+fLoGainWindowSize)
    {
      sum += *p;
      if (*p++ >= fSaturationLimit)
        sat++;
    }
  
  //
  // Check for saturation in all other slices
  //
  while (p<end)
    if (*p++ >= fSaturationLimit)
      sat++;
  
  //
  // Calculate the i-th sum as
  //    sum_i+1 = sum_i + slice[i+8] - slice[i]
  // This is fast and accurate (because we are using int's)
  //
  max=sum;
  Byte_t *ptrmax=ptr;  // pointer to the first slice of the maximum window
  
  for (p=ptr; p+fLoGainWindowSize<end; p++)
    {
      sum += *(p+fLoGainWindowSize) - *p;
      
      if (sum>max)
	{
	  max = sum;
	  ptrmax = p+1;
	}
    }
  
  // now calculate the time for the maximum window
  Int_t timesignalsum = 0;
  Int_t timesquaredsum =0;
  Int_t timesum =0;
  
  for (p=ptrmax; p < ptrmax + fLoGainWindowSize; p++)
    {
      timesignalsum += *p*(p-ptr);
      timesum += p-ptr;
      timesquaredsum  += (p-ptr)*(p-ptr);
    }
  
  const Float_t pedes  = ped.GetPedestal();
  const Float_t pedrms = ped.GetPedestalRms();
  const Float_t pedsubsum = max - fLoGainWindowSize*pedes;
  const Float_t pedsubtimesignalsum = timesignalsum - timesum*pedes;

  time      = pedsubsum != 0 ? pedsubtimesignalsum / pedsubsum  + Float_t(fLoGainFirst) : 1;
  deltatime = pedsubsum != 0 ? pedrms / pedsubsum * sqrt(timesquaredsum - fLoGainWindowSize*time) : 1;
}

// --------------------------------------------------------------------------
//
// In addition to the resources of the base-class MExtractor:
//   MJPedestal.MExtractor.WindowSizeHiGain: 6
//   MJPedestal.MExtractor.WindowSizeLoGain: 6
//
Int_t MExtractTimeHighestIntegral::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Byte_t hw = fHiGainWindowSize;
    Byte_t lw = fLoGainWindowSize;

    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "HiGainWindowSize", print))
    {
        hw = GetEnvValue(env, prefix, "HiGainWindowSize", hw);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "LoGainWindowSize", print))
    {
        lw = GetEnvValue(env, prefix, "LoGainWindowSize", lw);
        rc = kTRUE;
    }

    if (rc)
        SetWindowSize(hw, lw);

    return MExtractTime::ReadEnv(env, prefix, print) ? kTRUE : rc;
}

void MExtractTimeHighestIntegral::Print(Option_t *o) const
{
    *fLog << all;
    *fLog << GetDescriptor() << ":" << endl;
    *fLog << " Windows: Hi-Gain=" << (int)fHiGainWindowSize << "  Lo-Gain=" << (int)fLoGainWindowSize << endl;
    MExtractTime::Print(o);
}
