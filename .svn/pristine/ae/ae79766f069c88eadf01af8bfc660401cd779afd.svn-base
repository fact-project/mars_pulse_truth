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
!   Author(s): Markus Gaug,    09/2004 <mailto:markus@ifae.es> 
!   Author(s): Hendrik Bartko, 01/2004 <mailto:hbartko@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2002-2005
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
//   MExtractSlidingWindow
//
//   Extracts the signal from a sliding window of size fHiGainWindowSize and 
//   fLoGainWindowSize, respectively. The signal is the one which maximizes 
//   the clock-noise and pedestal-corrected integral contents.
//
//   The amplitude-weighted arrival time is calculated from the window with 
//   the highest integral using the following formula:
//
//   t = sum(s(i) * i) / sum(i) 
// 
//   where i denotes the FADC slice index and s(i) the clock-noise and 
///  pedestal-corrected FADC value at slice index i. The sum runs over the 
//   extraction window size.
//
//   Call: SetRange(higainfirst, higainlast, logainfirst, logainlast) 
//         to modify the ranges in which the window is allowed to move. 
//
//   Defaults are: 
// 
//   fHiGainFirst =  fgHiGainFirst =  0 
//   fHiGainLast  =  fgHiGainLast  =  14
//   fLoGainFirst =  fgLoGainFirst =  2 
//   fLoGainLast  =  fgLoGainLast  =  14
//
//  Call: SetWindowSize(windowhigain, windowlogain) 
//        to modify the sliding window widths. Windows have to be an even number. 
//        Odd numbers are possible since the clock-noise is corrected for.
//
//  Defaults are:
//
//  fHiGainWindowSize = 6
//  fLoGainWindowSize = 6
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractTimeAndChargeSlidingWindow.h"

#include "MPedestalPix.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MExtractTimeAndChargeSlidingWindow);

using namespace std;

const Byte_t  MExtractTimeAndChargeSlidingWindow::fgHiGainFirst  = 0;
const Byte_t  MExtractTimeAndChargeSlidingWindow::fgHiGainLast   = 16;
const Byte_t  MExtractTimeAndChargeSlidingWindow::fgLoGainFirst  = 2;
const Byte_t  MExtractTimeAndChargeSlidingWindow::fgLoGainLast   = 14;
const Byte_t  MExtractTimeAndChargeSlidingWindow::fgHiGainWindowSize = 6;
const Byte_t  MExtractTimeAndChargeSlidingWindow::fgLoGainWindowSize = 8;

// --------------------------------------------------------------------------
//
// Default constructor.
//
// Calls: 
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
// 
// Initializes:
// - fWindowSizeHiGain to fgHiGainWindowSize
// - fWindowSizeLoGain to fgLoGainWindowSize
//
MExtractTimeAndChargeSlidingWindow::MExtractTimeAndChargeSlidingWindow(const char *name, const char *title) 
{
  
  fName  = name  ? name  : "MExtractTimeAndChargeSlidingWindow";
  fTitle = title ? title : "Calculate arrival times and charges using a sliding window";

  fWindowSizeHiGain = fgHiGainWindowSize;
  fWindowSizeLoGain = fgLoGainWindowSize;

  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
}

//-------------------------------------------------------------------
//
// Set the ranges
//
// Calls:
// - MExtractor::SetRange(hifirst,hilast,lofirst,lolast);
// - SetWindowSize(fWindowSizeHiGain,fWindowSizeLoGain);
//
void MExtractTimeAndChargeSlidingWindow::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{

  MExtractor::SetRange(hifirst, hilast, lofirst, lolast);

  //
  // Redo the checks if the window is still inside the ranges
  //
  SetWindowSize(fWindowSizeHiGain,fWindowSizeLoGain);
  
}

// -----------------------------------------------------------------------------------------
//
// Checks:
// - if a window is bigger than the one defined by the ranges, set it to the available range
// - if a window is smaller than 2, set it to 2
// 
// Sets:
// - fNumHiGainSamples to: (Float_t)fWindowSizeHiGain
// - fNumLoGainSamples to: (Float_t)fWindowSizeLoGain
// - fSqrtHiGainSamples to: TMath::Sqrt(fNumHiGainSamples)
// - fSqrtLoGainSamples to: TMath::Sqrt(fNumLoGainSamples)  
//  
void MExtractTimeAndChargeSlidingWindow::SetWindowSize(Int_t windowh, Int_t windowl)
{
  
  fWindowSizeHiGain = windowh;
  fWindowSizeLoGain = windowl;

  const Int_t availhirange = (Int_t)(fHiGainLast-fHiGainFirst+1);

  if (fWindowSizeHiGain > availhirange)
    {
      *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i%s%2i%s",": Hi Gain window size: ",(int)fWindowSizeHiGain,
                    " is bigger than available range: [",(int)fHiGainFirst,",",(int)fHiGainLast,"]") << endl;
      *fLog << warn << GetDescriptor() 
            << ": Will set window size to: " << (int)availhirange << endl;
      fWindowSizeHiGain = availhirange;
    }
  
  if (fWindowSizeHiGain<1) 
    {
      fWindowSizeHiGain = 1;
      *fLog << warn << GetDescriptor() << ": High Gain window size too small, set to one sample" << endl;
    }
  
  if (fLoGainLast != 0 && fWindowSizeLoGain != 0)
    {
      const Int_t availlorange = (Int_t)(fLoGainLast-fLoGainFirst+1);
      
      if (fWindowSizeLoGain > availlorange)
        {
          *fLog << warn << GetDescriptor() 
                << Form("%s%2i%s%2i%s%2i%s",": Lo Gain window size: ",(int)fWindowSizeLoGain,
                        " is bigger than available range: [",(int)fLoGainFirst,",",(int)fLoGainLast,"]") << endl;
          *fLog << warn << GetDescriptor() 
                << ": Will set window size to: " << (int)availlorange << endl;
          fWindowSizeLoGain = availlorange;
        }
    }
  
  fNumHiGainSamples = (Float_t)fWindowSizeHiGain;
  fNumLoGainSamples = fLoGainLast ? (Float_t)fWindowSizeLoGain : 0.;
  
  fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
  fSqrtLoGainSamples = TMath::Sqrt(fNumLoGainSamples);

  switch (fWindowSizeHiGain)
    {
    case 2:
      SetResolutionPerPheHiGain(0.050);
      break;
    case 4:
      SetResolutionPerPheHiGain(0.039);
      break;
    case 6:
    case 8:
      SetResolutionPerPheHiGain(0.011);
      break;
    case 14:
      SetResolutionPerPheHiGain(0.009);
      break;
    default:
      *fLog << warn << GetDescriptor() << ": Could not set the high-gain extractor resolution per phe for window size " 
            << fWindowSizeHiGain << endl;
    }
  
  switch (fWindowSizeLoGain)
    {
    case 2:
      SetResolutionPerPheLoGain(0.028);
      break;
    case 4:
      SetResolutionPerPheLoGain(0.013);
      break;
    case 6:
      SetResolutionPerPheLoGain(0.008);
      break;
    case 8:
    case 10:
      SetResolutionPerPheLoGain(0.005);
      break;
    default:
      *fLog << warn << GetDescriptor() << ": Could not set the low-gain extractor resolution per phe for window size " 
            << fWindowSizeLoGain << endl;
    }
}

// --------------------------------------------------------------------------
//
// InitArrays
//
// Gets called in the ReInit() and initialized the arrays
//
Bool_t MExtractTimeAndChargeSlidingWindow::InitArrays()
{
  Int_t range = (Int_t)(fHiGainLast - fHiGainFirst + 1 + fHiLoLast);
  fHiGainSignal.Set(range);
  range = (Int_t)(fLoGainLast - fLoGainFirst + 1);
  fLoGainSignal.Set(range);

  return kTRUE;
  
}

// --------------------------------------------------------------------------
//
// Calculates the arrival time for each pixel 
//
void MExtractTimeAndChargeSlidingWindow::FindTimeAndChargeHiGain(Byte_t *first, Byte_t *logain, Float_t &sum, Float_t &dsum, 
                                                          Float_t &time, Float_t &dtime, 
                                                          Byte_t &sat, const MPedestalPix &ped, const Bool_t abflag)
{
  
  Int_t range = fHiGainLast - fHiGainFirst + 1;
  const Byte_t *end = first + range;
  Byte_t       *p  = first;
  
  Float_t max = 0;  // highest integral content of all windows
  sat         = 0;

  const Float_t pedes  = ped.GetPedestal();
  const Float_t ABoffs = ped.GetPedestalABoffset();

  const Float_t PedMean[2] = { pedes + ABoffs, pedes - ABoffs };

  fMaxBinContent = 0;
  //
  // Check for saturation in all other slices
  //
  Int_t ids = fHiGainFirst;
  Float_t *sample = fHiGainSignal.GetArray();

  while (p<first+fWindowSizeHiGain)
    {

      const Float_t signal = (Float_t)*p - PedMean[(ids++ + abflag) & 0x1];      
      sum                 += signal;
      *sample++            = signal;
      
      if (*p > fMaxBinContent)
	fMaxBinContent = *p;

      if (*p++ >= fSaturationLimit)
        if (!sat)
            sat = ids-2;
    }

  if (IsNoiseCalculation())
    return;
   
  //
  // Check for saturation in all other slices
  //
  while (p<end)
    {
      if (*p > fMaxBinContent)
	fMaxBinContent = *p;
      
      if (*p++ >= fSaturationLimit)
	if (!sat)
	  sat = ids-2;
    }

  //
  // Calculate the i-th sum as
  //    sum_i+1 = sum_i + slice[i+8] - slice[i]
  // This is fast and accurate (because we are using int's)
  //
  Int_t count    = 0;
  max            = sum;
  Int_t idx      =  0;  // idx of the first slice of the maximum window
  
  for (p=first; p+fWindowSizeHiGain<end; p++)
    {
      
      const Float_t signal = (Float_t)*(p+fWindowSizeHiGain) - PedMean[(ids++ + abflag) & 0x1];      
      sum                 += signal - *(sample-fWindowSizeHiGain);
      *sample++            = signal;
      
      if (sum>max)
        {
          max   = sum;
          idx   = count+1;
        }
      count++;
    }
  
  // 
  // overlap bins
  // 
  Byte_t *l = logain;
  while (l < logain+fHiLoLast)
    {
      
      const Float_t signal = (Float_t)*l - PedMean[(ids++ + abflag) & 0x1];          
      sum                 += signal - *(sample-fWindowSizeHiGain);
      *sample++            = signal;

      if (*l > fMaxBinContent)
        fMaxBinContent = *logain;
      
      if (*l++ >= fSaturationLimit)
        if (!sat)
          sat = ids-2;
      
      if (sum>max)
        {
          max   = sum;
          idx   = count+1;
        }
      count++;
    } /* while (l<logain+fHiLoLast) */

  //
  // now calculate the time for the maximum window
  //
  Float_t timesignalsum  = 0.;
  Int_t   timesquaredsum = 0;
  
  for (Int_t i=idx; i<idx+fWindowSizeHiGain; i++)
    {
      timesignalsum   += fHiGainSignal[i]*i;
      timesquaredsum  += i*i;
    }
  
  sum   = max;

  time  = max > 0.1 ? timesignalsum / max  + Float_t(fHiGainFirst) : 1.;
  dtime = max > 0.1 ? ped.GetPedestalRms() / max * sqrt(timesquaredsum - fWindowSizeHiGain*time) : 1.;

}


// --------------------------------------------------------------------------
//
// Calculates the arrival time for each pixel 
//
void MExtractTimeAndChargeSlidingWindow::FindTimeAndChargeLoGain(Byte_t *first, Float_t &sum, Float_t &dsum, 
                                                          Float_t &time, Float_t &dtime, 
                                                          Byte_t &sat, const MPedestalPix &ped, const Bool_t abflag) 
{
  
  Int_t range = fLoGainLast - fLoGainFirst + 1;
  const Byte_t *end = first + range;
  Byte_t       *p  = first;
  
  Float_t max = 0;  // highest integral content of all windows
  Int_t count = 0;  // counter to recognize the AB-flag

  Float_t pedes        = ped.GetPedestal();
  const Float_t ABoffs = ped.GetPedestalABoffset();

  Float_t PedMean[2] = { pedes + ABoffs, pedes - ABoffs };
  //
  // Check for saturation in all other slices
  //
  Int_t ids = fLoGainFirst;

  while (p<first+fWindowSizeLoGain)
    {
      const Float_t signal = (Float_t)*p - PedMean[(ids++ + abflag) & 0x1];      
      sum                 += signal;
      fLoGainSignal[count] = signal;
      
      if (*p++ >= fSaturationLimit)
        sat++;

      count++;
    }

  //
  // Check for saturation in all other slices
  //
  while (p<end)
    if (*p++ >= fSaturationLimit)
      sat++;
  
  if (IsNoiseCalculation())
    return;

  //
  // Calculate the i-th sum as
  //    sum_i+1 = sum_i + slice[i+8] - slice[i]
  // This is fast and accurate (because we are using int's)
  //
  count          = 0;
  max            = sum;
  Int_t idx      =  0;  // idx of the first slice of the maximum window
  
  for (p=first; p+fWindowSizeLoGain<end; p++)
    {

      const Float_t signal = (Float_t)*(p+fWindowSizeLoGain) - PedMean[(ids++ + abflag) & 0x1];      
      sum                 += signal - fLoGainSignal[count];
      fLoGainSignal[count + fWindowSizeLoGain] = signal;

      if (sum>max)
	{
	  max   = sum;
          idx   = count+1;
	}
      count++;
    }
  
  //
  // now calculate the time for the maximum window
  //
  Float_t timesignalsum  = 0;
  Int_t   timesquaredsum = 0;
  
  for (Int_t i=idx; i<idx+fWindowSizeLoGain; i++)
    {
      timesignalsum   += fLoGainSignal[i]*i;
      timesquaredsum  += i*i;
    }
  
  sum   = max;

  time  = max > 0.1 ? timesignalsum / max  + Float_t(fLoGainFirst) : 1.;
  dtime = max > 0.1 ? ped.GetPedestalRms() / max * sqrt(timesquaredsum - fWindowSizeLoGain*time) : 1.;
}

// --------------------------------------------------------------------------
//
// In addition to the resources of the base-class MExtractor:
//   MJPedestal.MExtractor.WindowSizeHiGain: 6
//   MJPedestal.MExtractor.WindowSizeLoGain: 6
//
Int_t MExtractTimeAndChargeSlidingWindow::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
  
  Byte_t hw = fWindowSizeHiGain;
  Byte_t lw = fWindowSizeLoGain;
  
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
  
  return MExtractTimeAndCharge::ReadEnv(env, prefix, print) ? kTRUE : rc;

}

