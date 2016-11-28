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
!   Author(s): Hendrik Bartko, 01/2004 <mailto:hbartko@mppmu.mpg.de>
!   Author(s): Markus Gaug, 04/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractSlidingWindow
//
//  Extracts the signal from a sliding window of size fHiGainWindowSize and 
//  fLoGainWindowSize. The signal is the one which maximizes the integral 
//  contents. 
//
//  Call: SetRange(higainfirst, higainlast, logainfirst, logainlast) 
//  to modify the ranges in which the window is allowed to move. 
//  Defaults are: 
// 
//   fHiGainFirst =  fgHiGainFirst =  0 
//   fHiGainLast  =  fgHiGainLast  =  14
//   fLoGainFirst =  fgLoGainFirst =  3 
//   fLoGainLast  =  fgLoGainLast  =  14
//
//  Call: SetWindowSize(windowhigain, windowlogain) 
//  to modify the sliding window widths. Windows have to be an even number. 
//  In case of odd numbers, the window will be modified.
//
//  Defaults are:
//
//   fHiGainWindowSize = fgHiGainWindowSize = 6
//   fLoGainWindowSize = fgLoGainWindowSize = 6
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractSlidingWindow.h"
#include "MExtractor.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MExtractedSignalCam.h"

ClassImp(MExtractSlidingWindow);

using namespace std;

const Byte_t MExtractSlidingWindow::fgHiGainFirst      = 0;
const Byte_t MExtractSlidingWindow::fgHiGainLast       = 14;
const Byte_t MExtractSlidingWindow::fgLoGainFirst      = 3;
const Byte_t MExtractSlidingWindow::fgLoGainLast       = 14;
const Byte_t MExtractSlidingWindow::fgHiGainWindowSize = 6;
const Byte_t MExtractSlidingWindow::fgLoGainWindowSize = 6;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets:
// - fWindowSizeHiGain to fgHiGainWindowSize
// - fWindowSizeLoGain to fgLoGainWindowSize
//
// Calls: 
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
//
MExtractSlidingWindow::MExtractSlidingWindow(const char *name, const char *title)
    : fWindowSizeHiGain(fgHiGainWindowSize), 
      fWindowSizeLoGain(fgLoGainWindowSize)
{

  fName  = name  ? name  : "MExtractSlidingWindow";
  fTitle = title ? title : "Signal Extractor for a sliding FADC window";

  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
}

// --------------------------------------------------------------------------
//
// SetRange: 
//
// Calls:
// - MExtractor::SetRange(hifirst,hilast,lofirst,lolast);
// - SetWindowSize(fWindowSizeHiGain,fWindowSizeLoGain);
//
void MExtractSlidingWindow::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{

  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  //
  // Redo the checks if the window is still inside the ranges
  //

  SetWindowSize(fWindowSizeHiGain,fWindowSizeLoGain);
  
}


// --------------------------------------------------------------------------
//
// The ReInit calls:
// -  MExtractor::ReInit()
// -  fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast+fHiLoLast, fNumHiGainSamples,
//                                fLoGainFirst, fLoGainLast, fNumLoGainSamples);
//
Bool_t MExtractSlidingWindow::ReInit(MParList *pList)
{

  MExtractor::ReInit(pList);
  
  fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast+fHiLoLast, fNumHiGainSamples,
                              fLoGainFirst, fLoGainLast, fNumLoGainSamples);


  *fLog << dec << endl;
  *fLog << inf << "Taking " << fNumHiGainSamples
        << " HiGain samples starting with slice " << (Int_t)fHiGainFirst
        << " to " << (Int_t)(fHiGainLast+fHiLoLast) << " incl" << endl;
  *fLog << inf << "Taking " << fNumLoGainSamples
        << " LoGain samples starting with slice " << (Int_t)fLoGainFirst
        << " to " << (Int_t)fLoGainLast << " incl" << endl;

  return kTRUE;
  
}

// --------------------------------------------------------------------------
//
// Checks:
// - if a window is odd, subtract one
// - if a window is bigger than the one defined by the ranges, set it to the available range
// - if a window is smaller than 2, set it to 2
// 
// Sets:
// - fNumHiGainSamples to: (Float_t)fWindowSizeHiGain
// - fNumLoGainSamples to: (Float_t)fWindowSizeLoGain
// - fSqrtHiGainSamples to: TMath::Sqrt(fNumHiGainSamples)
// - fSqrtLoGainSamples to: TMath::Sqrt(fNumLoGainSamples)  
//  
void MExtractSlidingWindow::SetWindowSize(Byte_t windowh, Byte_t windowl)
{
  
  fWindowSizeHiGain = windowh & ~1;
  fWindowSizeLoGain = windowl & ~1;

  if (fWindowSizeHiGain != windowh)
    *fLog << warn << GetDescriptor() << ": Hi Gain window size has to be even, set to: " 
          << int(fWindowSizeHiGain) << " samples " << endl;
  
  if (fWindowSizeLoGain != windowl)
    *fLog << warn << GetDescriptor() << ": Lo Gain window size has to be even, set to: " 
          << int(fWindowSizeLoGain) << " samples " << endl;
    
  const Byte_t availhirange = (fHiGainLast-fHiGainFirst+1) & ~1;

  if (fWindowSizeHiGain > availhirange)
    {
      *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i%s%2i%s",": Hi Gain window size: ",(int)fWindowSizeHiGain,
                    " is bigger than available range: [",(int)fHiGainFirst,",",(int)fHiGainLast,"]") << endl;
      *fLog << warn << GetDescriptor() 
            << ": Will set window size to: " << (int)availhirange << endl;
      fWindowSizeHiGain = availhirange;
    }
  
  if (fWindowSizeHiGain<2) 
    {
      fWindowSizeHiGain = 2;
      *fLog << warn << GetDescriptor() << ": Hi Gain window size set to two samples" << endl;
    }
  
  if (fLoGainLast != 0 && fWindowSizeLoGain != 0)
    {
      const Byte_t availlorange = (fLoGainLast-fLoGainFirst+1) & ~1;
      
      if (fWindowSizeLoGain > availlorange)
        {
          *fLog << warn << GetDescriptor() 
                << Form("%s%2i%s%2i%s%2i%s",": Lo Gain window size: ",(int)fWindowSizeLoGain,
                        " is bigger than available range: [",(int)fLoGainFirst,",",(int)fLoGainLast,"]") << endl;
          *fLog << warn << GetDescriptor() 
                << ": Will set window size to: " << (int)availlorange << endl;
          fWindowSizeLoGain = availlorange;
        }
      
      if (fWindowSizeLoGain<2) 
        {
          fWindowSizeLoGain = 2;
          *fLog << warn << GetDescriptor() << ": Lo Gain window size set to two samples" << endl;
        }
    }
  
  fNumHiGainSamples = (Float_t)fWindowSizeHiGain;
  fNumLoGainSamples = (Float_t)fWindowSizeLoGain;
  
  fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
  fSqrtLoGainSamples = TMath::Sqrt(fNumLoGainSamples);

}


// --------------------------------------------------------------------------
//
// FindSignalHiGain:
//
// - Loop from ptr to (ptr+fWindowSizeHiGain)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// - Loop from (ptr+fWindowSizeHiGain) to (ptr+fHiGainLast-fHiGainFirst)
// - Sum the content of *(ptr+fWindowSizeHiGain) and subtract *ptr
// - Check if the sum has become bigger and store it in case yes.
// 
void MExtractSlidingWindow::FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &max, Byte_t &sat) const
{
    const Byte_t *end = ptr + fHiGainLast - fHiGainFirst + 1;

    Int_t sum=0;
    Int_t sumtot =0;

    //
    // Calculate the sum of the first fWindowSize slices
    //
    sat = 0;
    Byte_t *p = ptr;
    
    while (p<ptr+fWindowSizeHiGain-fHiLoLast)
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
    // If part of the "low-Gain" slices are used, 
    // repeat steps one and two for the logain part until fHiLoLast
    //
    Byte_t *l = logain;
    while (l<logain+fHiLoLast)
      {
        sum += *l;
        if (*l++ >= fSaturationLimit)
            sat++;
      }


    //
    // Calculate the i-th sum as
    //    sum_i+1 = sum_i + slice[i+8] - slice[i]
    // This is fast and accurate (because we are using int's)
    //
    sumtot=sum;
    for (p=ptr; p+fWindowSizeHiGain-fHiLoLast<end; p++)
    {
        sum += *(p+fWindowSizeHiGain-fHiLoLast) - *p;
        if (sum>sumtot)
            sumtot = sum;
    }

    for (l=logain; l<logain+fHiLoLast; l++)
    {
        sum += *l - *p++;
        if (sum>sumtot)
            sumtot = sum;
    }
    max = (Float_t)sumtot;
}


// --------------------------------------------------------------------------
//
// FindSignalLoGain:
//
// - Loop from ptr to (ptr+fWindowSizeLoGain)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// - Loop from (ptr+fWindowSizeLoGain) to (ptr+fLoGainLast-fLoGainFirst)
// - Sum the content of *(ptr+fWindowSizeLoGain) and subtract *ptr
// - Check if the sum has become bigger and store it in case yes.
// 
void MExtractSlidingWindow::FindSignalLoGain(Byte_t *ptr, Float_t &max, Byte_t &sat) const
{
    const Byte_t *end = ptr + fLoGainLast - fLoGainFirst + 1;

    Int_t sum=0;
    Int_t sumtot=0;
    //
    // Calculate the sum of the first fWindowSize slices
    //
    sat = 0;
    Byte_t *p = ptr;
    while (p<ptr+fWindowSizeLoGain)
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
    sumtot=sum;
    for (p=ptr; p+fWindowSizeLoGain<end; p++)
    {
        sum += *(p+fWindowSizeLoGain) - *p;
        if (sum>sumtot)
            sumtot = sum;
    }
    max = (Float_t)sumtot;
}


