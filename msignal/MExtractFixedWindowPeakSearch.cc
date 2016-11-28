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
!   Author(s): Abelardo Moralejo, 04/2004 <mailto:moralejo@pd.infn.it>
!   Author(s): Markus Gaug, 04/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractFixedWindowPeakSearch
//
//  Calculate the signal integrating fWindowSize time slices, the same for 
//  all pixels. The integrated slices change from event to event, since the
//  pulse positions in the FADC jump between events, but apparently in a 
//  "coherent" fashion. We first loop over all pixels and find the one 
//  which has the highest sum of fPeakSearchWindowSize (default: 4) consecutive 
//  non-saturated high gain slices. The position of the peak in this pixel 
//  determines the integration window position for all pixels of this event. 
//  For the moment we neglect time delays between pixels (which are in most 
//  cases small). The class is based on MExtractSlidingWindow.
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
//  Call: SetWindows(windowhigain, windowlogain,peaksearchwindow) 
//  to modify the sliding window widths. Windows have to be an even number. 
//  In case of odd numbers, the window will be modified.
//
//  Call: SetOffsetFromWindow() to adjust the positionning of the extraction
//  window w.r.t. the peak search window. An fOffsetFromWindow of 0 means that
//  the starting slice of the extraction window is equal to the starting slice
//  of the maximizing peak search window. fOffsetFromWindow equal to 1 (default)
//  means that the extraction window starts one slice earlier than the peak 
//  search window result. It is recommanded to always use a smaller peak search
//  window than the extraction window.
//
//  Defaults are:
//
//   fHiGainWindowSize     = fgHiGainWindowSize     = 6
//   fLoGainWindowSize     = fgLoGainWindowSize     = 6
//   fPeakSearchWindowSize = fgPeakSearchWindowSize = 4
//   fLoGainPeakShift      = fgLoGainPeakShift      = 0
//   fOffsetFromWindow     = fgOffsetFromWindow     = 1
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractFixedWindowPeakSearch.h"
#include "MExtractor.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MRawEvtPixelIter.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

ClassImp(MExtractFixedWindowPeakSearch);

using namespace std;

const Byte_t MExtractFixedWindowPeakSearch::fgHiGainFirst          = 0;
const Byte_t MExtractFixedWindowPeakSearch::fgHiGainLast           = 16;
const Byte_t MExtractFixedWindowPeakSearch::fgLoGainFirst          = 3;
const Byte_t MExtractFixedWindowPeakSearch::fgLoGainLast           = 14;
const Byte_t MExtractFixedWindowPeakSearch::fgHiGainWindowSize     = 6;
const Byte_t MExtractFixedWindowPeakSearch::fgLoGainWindowSize     = 6;
const Byte_t MExtractFixedWindowPeakSearch::fgPeakSearchWindowSize = 4;
const Byte_t MExtractFixedWindowPeakSearch::fgOffsetFromWindow     = 1;
const Byte_t MExtractFixedWindowPeakSearch::fgLoGainPeakShift      = 1;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets:
// - fHiGainWindowSize to fgHiGainWindowSize
// - fLoGainWindowSize to fgLoGainWindowSize
// - fPeakSearchWindowSize to fgPeakSearchWindowSize
// - fLoGainPeakShift to fgLoGainPeakShift
//
// Calls: 
// - SetOffsetFromWindow()
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
//
MExtractFixedWindowPeakSearch::MExtractFixedWindowPeakSearch(const char *name, const char *title)
    : fHiGainWindowSize(fgHiGainWindowSize), 
      fLoGainWindowSize(fgLoGainWindowSize),
      fPeakSearchWindowSize(fgPeakSearchWindowSize),
      fLoGainPeakShift(fgLoGainPeakShift)
{

  fName  = name  ? name  : "MExtractFixedWindowPeakSearch";
  fTitle = title ? title : "Task to extract the signal from the FADC slices";

  SetOffsetFromWindow();
  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
}

// --------------------------------------------------------------------------
//
// SetRange: 
//
// Calls:
// - MExtractor::SetRange(hifirst,hilast,lofirst,lolast);
// - SetWindows(fHiGainWindowSize,fLoGainWindowSize,fPeakSearchWindowSize);
//
void MExtractFixedWindowPeakSearch::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{

  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  //
  // Redo the checks if the window is still inside the ranges
  //
  SetWindows(fHiGainWindowSize,fLoGainWindowSize,fPeakSearchWindowSize);
  
}

// --------------------------------------------------------------------------
//
// Checks:
// - if a window is odd, subtract one
// - if a window is bigger than the one defined by the ranges, set it to the available range
// - if a window is smaller than 2, set it to 2
// 
// Sets:
// - fNumHiGainSamples to: (Float_t)fHiGainWindowSize
// - fNumLoGainSamples to: (Float_t)fLoGainWindowSize
// - fSqrtHiGainSamples to: TMath::Sqrt(fNumHiGainSamples)
// - fSqrtLoGainSamples to: TMath::Sqrt(fNumLoGainSamples)  
//  
void MExtractFixedWindowPeakSearch::SetWindows(Byte_t windowh, Byte_t windowl, Byte_t peaksearchwindow)
{

  fHiGainWindowSize     = windowh & ~1;
  fLoGainWindowSize     = windowl & ~1;
  fPeakSearchWindowSize = peaksearchwindow & ~1;

  if (fHiGainWindowSize != windowh)
    *fLog << warn << GetDescriptor() << ": Hi Gain window size has to be even, set to: " 
          << int(fHiGainWindowSize) << " samples " << endl;
  
  if (fLoGainWindowSize != windowl)
    *fLog << warn << GetDescriptor() << ": Lo Gain window size has to be even, set to: " 
          << int(fLoGainWindowSize) << " samples " << endl;
    
  if (fPeakSearchWindowSize != peaksearchwindow)
    *fLog << warn << GetDescriptor() << ": Peak Search window size has to be even, set to: " 
          << int(fPeakSearchWindowSize) << " samples " << endl;

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
      fLoGainWindowSize = availlorange;
    }
  
  if (fHiGainWindowSize<2) 
    {
      fHiGainWindowSize = 2;
      *fLog << warn << GetDescriptor() << ": Hi Gain window size set to two samples" << endl;
    }
  
  if (fLoGainWindowSize<2) 
    {
      fLoGainWindowSize = 2;
      *fLog << warn << GetDescriptor() << ": Lo Gain window size set to two samples" << endl;
    }

  if (fPeakSearchWindowSize<2) 
    {
      fPeakSearchWindowSize = 2;
      *fLog << warn << GetDescriptor() 
            << ": Peak Search window size set to two samples" << endl;
    }

  fNumHiGainSamples = (Float_t)fHiGainWindowSize;
  fNumLoGainSamples = (Float_t)fLoGainWindowSize;

  fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
  fSqrtLoGainSamples = TMath::Sqrt(fNumLoGainSamples);
}


// --------------------------------------------------------------------------
//
// The ReInit calls:
// -  MExtractor::ReInit()
// -  fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast+fHiLoLast, fNumHiGainSamples,
//                                fLoGainFirst, fLoGainLast, fNumLoGainSamples);
//
Bool_t MExtractFixedWindowPeakSearch::ReInit(MParList *pList)
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
// FindPeak
// Finds highest sum of "window" consecutive FADC slices in a pixel, and store
// in "startslice" the first slice of the group which yields the maximum sum.
// In "max" the value of the maximum sum is stored, in "sat" the number of 
// saturated slices.
//
void MExtractFixedWindowPeakSearch::FindPeak(Byte_t *ptr, Byte_t window, Byte_t &startslice, Int_t &max, 
                                             Int_t &sat, Byte_t &satpos) const
{

  const Byte_t *end = ptr + fHiGainLast - fHiGainFirst + 1;

  sat = 0;
  satpos = 0;
  
  startslice = 0;
  Int_t sum=0;

  //
  // Calculate the sum of the first "window" slices
  //
  sat = 0;
  Byte_t *p = ptr;

  while (p<ptr+window)
    {
      sum += *p;
      if (*p++ >= fSaturationLimit)
        {
          if (sat == 0)
              satpos = p-ptr;
          sat++;
        }
    }

  //
  // Check for saturation in all other slices
  //
  while (p<end)
    if (*p++ >= fSaturationLimit)
      {
        if (sat == 0)
          satpos = p-ptr;
        sat++;
      }
  
  //
  // Calculate the i-th sum of n as
  //    sum_i+1 = sum_i + slice[i+window] - slice[i]
  // This is fast and accurate (because we are using int's)
  //
  max=sum;
  for (p=ptr; p+window<end; p++)
    {
      sum += *(p+window) - *p;
      if (sum > max)
	{
	  max = sum;
	  startslice = p-ptr+1;
	}
    }

  return;
}

// --------------------------------------------------------------------------
//
// FindSignalHiGain:
//
// - Loop from ptr to (ptr+fHiGainWindowSize)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// 
void MExtractFixedWindowPeakSearch::FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat) const
{

  Byte_t *end = ptr + fHiGainWindowSize-fHiLoLast;

  Int_t summ = 0;
  //
  // Calculate the sum of the "window" slices starting in ptr
  //
  while (ptr<end)
    {
      summ += *ptr;
      if (*ptr++ >= fSaturationLimit)
	sat++;
    }

  //
  // If part of the "lo-Gain" slices are used,
  // repeat steps one and two for the logain part until fHiLoLast
  //
  Byte_t *p = logain;
  end = logain + fHiLoLast;
  while (p<end)
    {
  
      summ += *p;
      if (*p++ >= fSaturationLimit)
        sat++;
    }

  sum = (Float_t)summ;
  return;
}

// --------------------------------------------------------------------------
//
// FindSignalLoGain:
//
// - Loop from ptr to (ptr+fLoGainWindowSize)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// 
void MExtractFixedWindowPeakSearch::FindSignalLoGain(Byte_t *ptr, Float_t &sum, Byte_t &sat) const
{
  //
  // Calculate the sum of the "window" slices starting in ptr
  //
  Byte_t *p = ptr;
  Int_t summ = 0;

  while (p<ptr+fLoGainWindowSize)
    {
      summ += *p;
      if (*p++ >= fSaturationLimit)
	sat++;
    }

  sum = (Float_t)summ;
  return;
}

// --------------------------------------------------------------------------
//
// Process
// First find the pixel with highest sum of fPeakSearchWindowSize slices (default:4)
// "startslice" will mark the slice at which the highest sum begins for that pixel.
// Then define the beginning of the integration window for ALL pixels as the slice
// before that: startslice-fOffsetFromWindow, unless of course startslice-fOffsetFromWindow<=0,
// in which case we start at 0. We will also check that the integration window does not 
// go beyond the FADC limits.
//
Int_t MExtractFixedWindowPeakSearch::Process()
{

  MRawEvtPixelIter pixel(fRawEvt);

  Int_t sat;
  Byte_t  satpos;
  ULong_t gsatpos = 0;

  Int_t maxsumhi = -1000000;
  Int_t numsat   = 0;
  Byte_t startslice;
  Byte_t hiGainFirst = 0;
  Byte_t loGainFirst = 0;
  fHiLoLast = 0;

  while (pixel.Next())
    {
      Int_t sumhi;
      sat = 0;

      FindPeak(pixel.GetHiGainSamples()+fHiGainFirst, fPeakSearchWindowSize, startslice, sumhi, sat, satpos);

      if (sumhi > maxsumhi && sat == 0)
	{
	  maxsumhi = sumhi;
	  if (startslice > fOffsetFromWindow)
            hiGainFirst = fHiGainFirst + startslice - fOffsetFromWindow;
	  else
	    hiGainFirst = fHiGainFirst;
	}
      else if (sat)
        {
          numsat++;
          gsatpos += satpos;
        }
    }

  // Check necessary for calibration events
  if (numsat > fSignals->GetSize()*0.9)
    hiGainFirst = gsatpos/numsat - 1;

  loGainFirst = ( hiGainFirst+fLoGainPeakShift > fLoGainFirst ) 
    ? hiGainFirst+fLoGainPeakShift 
    : fLoGainFirst;

  // Make sure we will not integrate beyond the hi gain limit:
  if (hiGainFirst+fHiGainWindowSize > pixel.GetNumHiGainSamples())
    fHiLoLast = hiGainFirst+fHiGainWindowSize - pixel.GetNumHiGainSamples();
  //    hiGainFirst = pixel.GetNumHiGainSamples()-fHiGainWindowSize;

  // Make sure we will not integrate beyond the lo gain limit:
  if (loGainFirst+fLoGainWindowSize > pixel.GetNumLoGainSamples())
    loGainFirst = pixel.GetNumLoGainSamples()-fLoGainWindowSize;

  pixel.Reset();

  while (pixel.Next())
    {
      //
      // Find signal in hi- and lo-gain
      //
      Float_t sumhi=0.;
      Byte_t sathi=0;

      FindSignalHiGain(pixel.GetHiGainSamples()+hiGainFirst, pixel.GetLoGainSamples(), sumhi, sathi);

      Float_t sumlo=0.;
      Byte_t satlo=0;
      if (pixel.HasLoGain())
        FindSignalLoGain(pixel.GetLoGainSamples()+loGainFirst, sumlo, satlo);

      //
      // Take corresponding pedestal
      //
      const Int_t pixid = pixel.GetPixelId();

      const MPedestalPix  &ped = (*fPedestals)[pixid];
      MExtractedSignalPix &pix = (*fSignals)[pixid];

      const Float_t pedes  = ped.GetPedestal();
      const Float_t pedrms = ped.GetPedestalRms();
      //
      // Set extracted signal with pedestal substracted
      //
      pix.SetExtractedSignal(sumhi - pedes*fNumHiGainSamples, pedrms*fSqrtHiGainSamples,
                             sumlo - pedes*fNumLoGainSamples, pedrms*fSqrtLoGainSamples);

      pix.SetGainSaturation(sathi, satlo);

      //      pix.SetNumHiGainSlices(fNumHiGainSamples);
      //      pix.SetNumLoGainSlices(fNumLoGainSamples);      
      
    } /* while (pixel.Next()) */


  fSignals->SetReadyToSave();

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// In addition to the resources of the base-class MExtractor:
//   MJPedestal.MExtractor.WindowSizeHiGain: 6
//   MJPedestal.MExtractor.WindowSizeLoGain: 6
//   MJPedestal.MExtractor.PeakSearchWindow: 4
//   MJPedestal.MExtractor.OffsetFromWindow: 1
//   MJPedestal.MExtractor.LoGainPeakShift:  1
//
Int_t MExtractFixedWindowPeakSearch::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Byte_t hw = fHiGainWindowSize;
    Byte_t lw = fLoGainWindowSize;
    Byte_t pw = fPeakSearchWindowSize;

    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "PeakSearchWindow", print))
    {
        pw = GetEnvValue(env, prefix, "PeakSearchWindow", pw);
        rc = kTRUE;
    }
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
        SetWindows(hw, lw, pw);


    if (IsEnvDefined(env, prefix, "OffsetFromWindow", print))
    {
        SetOffsetFromWindow(GetEnvValue(env, prefix, "OffsetFromWindow", fOffsetFromWindow));
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "LoGainPeakShift", print))
    {
        SetLoGainPeakShift(GetEnvValue(env, prefix, "LoGainPeakShift", fLoGainPeakShift));
        rc = kTRUE;
    }

    rc = MExtractor::ReadEnv(env, prefix, print) ? kTRUE : rc;

    return rc;
}

void MExtractFixedWindowPeakSearch::Print(Option_t *o) const
{
    *fLog << all;
    *fLog << GetDescriptor() << ":" << endl;
    *fLog << " Windows: Hi-Gain=" << (int)fHiGainWindowSize << "  Lo-Gain=" << (int)fLoGainWindowSize;
    *fLog << "  Peak-Search=" << (int)fPeakSearchWindowSize << endl;
    *fLog << " Offset From Window: " << (int)fOffsetFromWindow << endl;
    *fLog << " Lo-Gain peak Shift: " << (int)fLoGainPeakShift << endl;
    MExtractor::Print(o);
}
