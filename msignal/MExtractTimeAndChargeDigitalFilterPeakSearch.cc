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
!   Author(s): Markus Gaug, 02/2005 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
//   MExtractTimeAndChargeDigitalFilterPeakSearch
//
//   An extractor using the digital filter in combination with a global
//   peak search, analogue to the class MExtractFixedWindowPeakSearch.
//
//   The extractor returns kFALSE if too many events lie outside the 
//   range: (peak - fOffsetLeftFromPeak, peak - fOffsetRightFromPeak)
//
//   Input Containers:
//    MRawEvtData
//    MRawRunHeader
//    MPedestalCam
//
//   Output Containers:
//    MArrivalTimeCam
//    MExtractedSignalCam
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractTimeAndChargeDigitalFilterPeakSearch.h"

#include <fstream>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TString.h>
#include <TMatrix.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MPedestalPix.h"
#include "MPedestalCam.h"

#include "MRawEvtPixelIter.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

ClassImp(MExtractTimeAndChargeDigitalFilterPeakSearch);

using namespace std;

const Byte_t MExtractTimeAndChargeDigitalFilterPeakSearch::fgHiGainFirst             =  0;
const Byte_t MExtractTimeAndChargeDigitalFilterPeakSearch::fgHiGainLast              = 20;
const Byte_t MExtractTimeAndChargeDigitalFilterPeakSearch::fgLoGainFirst             =  0;
const Byte_t MExtractTimeAndChargeDigitalFilterPeakSearch::fgLoGainLast              = 14;
const Byte_t MExtractTimeAndChargeDigitalFilterPeakSearch::fgOffsetLeftFromPeak      =  3;
const Byte_t MExtractTimeAndChargeDigitalFilterPeakSearch::fgOffsetRightFromPeak     =  3;
const Byte_t MExtractTimeAndChargeDigitalFilterPeakSearch::fgPeakSearchWindowSize    =  2;
const Int_t  MExtractTimeAndChargeDigitalFilterPeakSearch::fgHiGainFailureLimit      = 10;
const Int_t  MExtractTimeAndChargeDigitalFilterPeakSearch::fgLoGainFailureLimit      = 25;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets:
// - fOffsetLeftFromPeak    to fgOffsetLeftFromPeak
// - fOffsetRightFromPeak   to fgOffsetRightFromPeak
// - fPeakSearchWindowSize  to fgPeakSearchWindowSize
// - fHiGainFailureLimit to fgHiGainFailureLimit
// - fLoGainFailureLimit to fgLoGainFailureLimit
//
MExtractTimeAndChargeDigitalFilterPeakSearch::MExtractTimeAndChargeDigitalFilterPeakSearch(const char *name, const char *title) 
    :   fHiGainOutOfRangeLeft(0), fHiGainOutOfRangeRight(0),
        fLoGainOutOfRangeLeft(0), fLoGainOutOfRangeRight(0),
        fBadPixels(NULL)
{
    fName  = name  ? name  : "MExtractTimeAndChargeDigitalFilterPeakSearch";
    fTitle = title ? title : "Digital Filter";

    SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);

    SetOffsetLeftFromPeak();
    SetOffsetRightFromPeak();
    SetPeakSearchWindowSize();
    SetHiGainFailureLimit();
    SetLoGainFailureLimit();
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawEvtData
//  - MRawRunHeader
//  - MPedestalCam
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MExtractedSignalCam
//  - MArrivalTimeCam    
//
// The following variables are set to 0:
//
//  - fHiGainOutOfRangeLeft
//  - fHiGainOutOfRangeRight
//  - fLoGainOutOfRangeLeft
//  - fLoGainOutOfRangeRight
//
Int_t MExtractTimeAndChargeDigitalFilterPeakSearch::PreProcess(MParList *pList)
{

  if (!MExtractTimeAndCharge::PreProcess(pList))
    return kFALSE;
  
  fHiGainOutOfRangeLeft  = 0;
  fHiGainOutOfRangeRight = 0;
  fLoGainOutOfRangeLeft  = 0;
  fLoGainOutOfRangeRight = 0;
  
  fBadPixels = (MBadPixelsCam*)pList->FindObject(AddSerialNumber("MBadPixelsCam"));
  if (!fBadPixels)
    {
      *fLog << err << "Cannot find MBadPixelsCam ... abort." << endl;
      return kFALSE;
    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// FindPeak
//
// Finds highest sum of "window" consecutive FADC slices in a pixel, and store
// in "startslice" the first slice of the group which yields the maximum sum.
// In "max" the value of the maximum sum is stored, in "sat" the number of 
// saturated slices.
//
void MExtractTimeAndChargeDigitalFilterPeakSearch::FindPeak(Byte_t *ptr, Byte_t *logain, Byte_t &startslice, Int_t &max, 
							    Int_t &sat, Byte_t &satpos) const
{

  Byte_t *end = ptr + fHiGainLast - fHiGainFirst + 1;

  sat = 0;
  satpos = 0;
  
  startslice = 0;
  Int_t sum=0;

  //
  // Calculate the sum of the first "fPeakSearchWindowSize" slices
  //
  sat = 0;
  Byte_t *p = ptr;

  while (p<ptr+fPeakSearchWindowSize)
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
  //    sum_i+1 = sum_i + slice[i+fPeakSearchWindowSize] - slice[i]
  // This is fast and accurate (because we are using int's)
  //
  max=sum;
  for (p=ptr; p+fPeakSearchWindowSize<end; p++)
    {
      sum += *(p+fPeakSearchWindowSize) - *p;
      if (sum > max)
	{
	  max = sum;
	  startslice = p-ptr+1;
	}
    }

  if (!fHiLoLast)
    return;

  Byte_t *l = logain;

  while (++p < end && l < logain+fHiLoLast)
    {
      sum += *l - *p;
      if (sum > max)
	{
	  max = sum;
	  startslice = p-ptr+1;
	}

      if (*l++ >= fSaturationLimit)
        {
          if (sat == 0)
            satpos = l-logain + fHiGainLast - fHiGainFirst;
          sat++;
        }
    }
  
  if (fHiLoLast <= fPeakSearchWindowSize)
    return;

  while (l < logain+fHiLoLast)
    {
      if (*l++ >= fSaturationLimit)
        {
          if (sat == 0)
            satpos = l-logain+fHiGainLast-fHiGainFirst;
          sat++;
        }

      sum += *l - *(l-fPeakSearchWindowSize);
      if (sum > max)
	{
	  max = sum;
	  startslice = l-logain + fHiGainLast - fHiGainFirst - fPeakSearchWindowSize + 1;
	}
    }

  return;
}


// --------------------------------------------------------------------------
//
// Process
//
// First find the pixel with highest sum of fPeakSearchWindowSize slices (default:2)
// "startslice" will mark the slice at which the highest sum begins for that pixel.
//
// Then define the beginning of the digital filter search window for ALL pixels as the slice
// before that: startslice-fOffsetLeftFromPeak until: startslice+fOffsetRightFromPeak
//
Int_t MExtractTimeAndChargeDigitalFilterPeakSearch::Process()
{

  MRawEvtPixelIter pixel(fRawEvt);

  Int_t   sat        = 0;
  Byte_t  satpos     = 0;
  ULong_t gsatpos    = 0;

  Int_t   maxsumhi   = -1000000;
  Int_t   numsat     = 0;
  Byte_t  startslice = 0;

  Byte_t hiGainFirstsave = fHiGainFirst;
  Byte_t hiGainLastsave  = fHiGainLast;
  Byte_t loGainFirstsave = fLoGainFirst;
  Byte_t loGainLastsave  = fLoGainLast; 
  Byte_t hiLoLastsave    = fHiLoLast;

  Byte_t higainfirst     = fHiGainFirst;
  UInt_t peakpixel       = 0;

  while (pixel.Next())
    {

      Int_t sumhi;
      sat = 0;

      MBadPixelsPix &bad = (*fBadPixels)[pixel.GetPixelId()];
      
      if (!bad.IsUnsuitable())
        FindPeak(pixel.GetHiGainSamples()+fHiGainFirst, pixel.GetLoGainSamples(), startslice, sumhi, sat, satpos);

      if (sumhi > maxsumhi && sat == 0)
	{
	  maxsumhi     = sumhi;
	  higainfirst  = fHiGainFirst + startslice;
          peakpixel    = pixel.GetPixelId();
	}
      else if (sat)
        {
          numsat++;
          gsatpos += satpos;
        }
    }

  //
  // Check necessary for calibration events
  //
  if (numsat > fSignals->GetSize()*0.9)
    higainfirst = gsatpos/numsat - 1;

  //
  // Shift the start slice to the left:
  //
  if (higainfirst >= fHiGainFirst + fOffsetLeftFromPeak)
    fHiGainFirst = higainfirst - fOffsetLeftFromPeak;
  else
    {
      //      *fLog << err << (Int_t)higainfirst << "   " << peakpixel << endl;      
      fHiGainOutOfRangeLeft++;
    }
  
  //
  // Shift the last slice to the right:
  //
  const Byte_t rlim = higainfirst + fOffsetRightFromPeak + fWindowSizeHiGain - 1;
  if (rlim <= fHiGainLast+fHiLoLast)
    if (rlim > fHiGainLast)
      {
        fHiLoLast   = rlim - fHiGainLast;
        fHiGainLast = pixel.GetNumHiGainSamples() - 1;
      }
    else
      {
        fHiLoLast   = 0;
        fHiGainLast = rlim;
      }
  else 
    {
      fHiGainOutOfRangeRight++;
      //      *fLog << err << (Int_t)higainfirst << "   " << peakpixel << endl;      
    }
  
  const Byte_t llim = higainfirst + (Int_t)fOffsetLoGain;
  if ( llim  >= fLoGainFirst + fOffsetLeftFromPeak) 
    fLoGainFirst = llim - fOffsetLeftFromPeak;
  else
    fLoGainOutOfRangeLeft++;
  
  //
  // Make sure we will not integrate beyond the lo gain limit:
  //
  const Byte_t lolast = fLoGainFirst+fWindowSizeLoGain+fOffsetRightFromPeak-1;  
  if (lolast < pixel.GetNumLoGainSamples())
    fLoGainLast = lolast;
  else
    {
      fLoGainOutOfRangeRight++;
      //      *fLog << err << (Int_t)higainfirst << "   " << peakpixel << "  " << (int)fLoGainFirst << endl;      
    }
  
  //  *fLog << inf << (int)fHiGainFirst << "  " << (int)higainfirst << "  " << (int)fHiGainLast 
  //        << "  " << (int)fLoGainFirst << "  " << (int)fLoGainLast << endl;

  pixel.Reset();

  while (pixel.Next())
    {

      //
      // Find signal in hi- and lo-gain
      //
      Float_t sumhi =0., deltasumhi =0; // Set hi-gain of MExtractedSignalPix valid
      Float_t timehi=0., deltatimehi=0; // Set hi-gain of MArrivalTimePix valid
      Byte_t sathi=0;

      // Initialize fMaxBinContent for the case, it gets not set by the derived class
      fMaxBinContent = fLoGainSwitch + 1; 

      const Int_t pixidx = pixel.GetPixelId();
      const MPedestalPix  &ped = (*fPedestals)[pixidx];
      const Bool_t higainabflag = pixel.HasABFlag();

      FindTimeAndChargeHiGain(pixel.GetHiGainSamples()+fHiGainFirst, pixel.GetLoGainSamples(), 
                              sumhi, deltasumhi, 
                              timehi, deltatimehi, 
                              sathi, ped, higainabflag);

      //
      // Make sure that in cases the time couldn't be correctly determined
      // more meaningfull default values are assigned
      //
      if (timehi<0)
          timehi = -1;
      if (timehi>pixel.GetNumHiGainSamples())
          timehi = pixel.GetNumHiGainSamples();
      
      Float_t sumlo =0., deltasumlo =-1.; // invalidate logain of MExtractedSignalPix
      Float_t timelo=0., deltatimelo=-1;  // invalidate logain of MArrivalTimePix
      Byte_t satlo=0;
      
      //
      // Adapt the low-gain extraction range from the obtained high-gain time
      //
      if (pixel.HasLoGain() && (fMaxBinContent > fLoGainSwitch) )
      {
          deltasumlo  = 0; // make logain of MExtractedSignalPix valid
          deltatimelo = 0; // make logain of MArrivalTimePix valid

          fLoGainFirstSave = fLoGainFirst;
          const Byte_t logainstart = sathi 
            ? sathi + (Int_t)fLoGainStartShift
            : (Byte_t)(timehi + fLoGainStartShift);
        
	  fLoGainFirst = logainstart > fLoGainFirstSave ? logainstart : fLoGainFirstSave;

          if ( fLoGainFirst < fLoGainLast )
            {
              const Bool_t logainabflag = (higainabflag + pixel.GetNumHiGainSamples()) & 0x1;
              FindTimeAndChargeLoGain(pixel.GetLoGainSamples()+fLoGainFirst,
                                      sumlo, deltasumlo,
                                      timelo, deltatimelo,
                                      satlo, ped, logainabflag);
          }
          fLoGainFirst = fLoGainFirstSave;

          // Make sure that in cases the time couldn't be correctly determined
          // more meaningfull default values are assigned
          if (timelo<0)
              timelo = -1;
          if (timelo>pixel.GetNumLoGainSamples())
              timelo = pixel.GetNumLoGainSamples();
      }

      MExtractedSignalPix &pix = (*fSignals)[pixidx];
      MArrivalTimePix     &tix = (*fArrTime)[pixidx];
      pix.SetExtractedSignal(sumhi, deltasumhi,sumlo, deltasumlo);
      pix.SetGainSaturation(sathi, satlo);

      tix.SetArrivalTime(timehi, deltatimehi, timelo-fOffsetLoGain, deltatimelo);
      tix.SetGainSaturation(sathi, satlo);
 
    } /* while (pixel.Next()) */

  fArrTime->SetReadyToSave();
  fSignals->SetReadyToSave();

  fHiGainFirst  = hiGainFirstsave;
  fHiGainLast   = hiGainLastsave ; 
  fLoGainFirst  = loGainFirstsave;
  fLoGainLast   = loGainLastsave ; 
  fHiLoLast     = hiLoLastsave;

  return kTRUE;
}

Int_t MExtractTimeAndChargeDigitalFilterPeakSearch::PostProcess()
{

  if (GetNumExecutions() < 1)
    return kTRUE;

  *fLog << warn << endl;

  const Int_t higainfailure = ((fHiGainOutOfRangeLeft+fHiGainOutOfRangeRight)*100)/GetNumExecutions();
  const Int_t logainfailure = ((fLoGainOutOfRangeLeft+fLoGainOutOfRangeRight)*100)/GetNumExecutions();

  if (fHiGainOutOfRangeLeft > 0)
    *fLog << Form("%5.1f",((Float_t)fHiGainOutOfRangeLeft*100)/GetNumExecutions())
          << "% ranging out of hi-gain window to the left!" << endl;
  if (fHiGainOutOfRangeRight > 0)
    *fLog << Form("%5.1f",((Float_t)fHiGainOutOfRangeRight*100)/GetNumExecutions())
	  << "% ranging out of hi-gain window to the right!" << endl;
  if (fLoGainOutOfRangeLeft > 0)
    *fLog << Form("%5.1f",((Float_t)fLoGainOutOfRangeLeft*100)/GetNumExecutions())
          << "% ranging out of lo-gain window to the left!" << endl;
  if (fLoGainOutOfRangeRight > 0)
    *fLog << Form("%5.1f",((Float_t)fLoGainOutOfRangeRight*100)/GetNumExecutions())
	  << "% ranging out of lo-gain window to the right!" << endl;

  
  if (higainfailure > fHiGainFailureLimit)
    {
      *fLog << err << higainfailure << "% range failures in high gain above limit of: " << fHiGainFailureLimit << "%." << endl;
      return kFALSE;
    }
  
  if (logainfailure > fLoGainFailureLimit)
    {
      *fLog << err << logainfailure << "% range failures in low gain above limit of: " << fLoGainFailureLimit << "%." << endl;
      return kFALSE;
    }

  return kTRUE;

}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MJPedestal.MExtractor.WindowSizeHiGain: 6
//   MJPedestal.MExtractor.WindowSizeLoGain: 6
//   MJPedestal.MExtractor.BinningResolutionHiGain: 10
//   MJPedestal.MExtractor.BinningResolutionLoGain: 10
//   MJPedestal.MExtractor.WeightsFile: filename
//
Int_t MExtractTimeAndChargeDigitalFilterPeakSearch::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

  Bool_t rc = kFALSE;

  if (IsEnvDefined(env, prefix, "OffsetLeftFromPeak", print))
    {
      fOffsetLeftFromPeak = GetEnvValue(env, prefix, "OffsetLeftFromPeak", fOffsetLeftFromPeak);
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "OffsetRightFromPeak", print))
    {
      fOffsetRightFromPeak = GetEnvValue(env, prefix, "OffsetRightFromPeak", fOffsetRightFromPeak);
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "PeakSearchWindowSize", print))
    {
      fPeakSearchWindowSize = GetEnvValue(env, prefix, "PeakSearchWindowSize", fPeakSearchWindowSize);
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "HiGainFailureLimit", print))
    {
      fHiGainFailureLimit = GetEnvValue(env, prefix, "HiGainFailureLimit", fHiGainFailureLimit);
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "LoGainFailureLimit", print))
    {
      fLoGainFailureLimit = GetEnvValue(env, prefix, "LoGainFailureLimit", fLoGainFailureLimit);
      rc = kTRUE;
    }
  
  return MExtractTimeAndChargeDigitalFilter::ReadEnv(env, prefix, print) ? kTRUE : rc;
}


void MExtractTimeAndChargeDigitalFilterPeakSearch::Print(Option_t *o) const
{
    if (IsA()==Class())
        *fLog << GetDescriptor() << ":" << endl;

    MExtractTimeAndChargeDigitalFilter::Print(o);
    *fLog << " Offset from Peak left:   " << (int)fOffsetLeftFromPeak   << endl;
    *fLog << " Offset from Peak right:  " << (int)fOffsetRightFromPeak  << endl;
    *fLog << " Peak search window size: " << (int)fPeakSearchWindowSize << endl;
    *fLog << " High Gain Failure limit: " << fHiGainFailureLimit << endl;
    *fLog << " Low Gain Failure limit:  " << fLoGainFailureLimit << endl;
}
