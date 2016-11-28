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
!   Author(s): Markus Gaug, 02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractBlindPixel
//
//  Extracts the signal from a fixed window in a given range.
//
//    Call: SetRange(fHiGainFirst, fHiGainLast, fLoGainFirst, fLoGainLast) 
//    to modify the ranges. The "low-gain" ranges are used for the NSB rejection 
//    whereas the high-gain ranges for blind pixel signal extraction. "High-gain" 
//    ranges can extend to the slices stored as "low-gain" in MRawEvtPixelIter
//
//    Defaults are: 
// 
//     fHiGainFirst =  fgHiGainFirst =  10 
//     fHiGainLast  =  fgHiGainLast  =  29
//     fLoGainFirst =  fgLoGainFirst =  0 
//     fLoGainLast  =  fgLoGainLast  =  7
//
//  The switches: 
//  - SetExtractionType ( kAmplitude ) and  SetExtractionType ( kIntegral ) 
//    can be used to choose between amplitude extraction (using a spline) and
//    summed integral. 
//  - SetExtractionType ( kFilter    ) 
//    can be used to apply a filter discarding events passing over a threshold 
//    defined in fNSBFilterLimit
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractBlindPixel.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtData.h"
#include "MRawRunHeader.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalSubtractedEvt.h"
#include "MExtractedSignalBlindPixel.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MCalibrationBlindCam.h"
#include "MCalibrationBlindPix.h"

#include "MExtralgoSpline.h"

ClassImp(MExtractBlindPixel);

using namespace std;

const UInt_t  MExtractBlindPixel::fgBlindPixelIdx    = 559;
const Byte_t  MExtractBlindPixel::fgHiGainFirst      =  10;
const Byte_t  MExtractBlindPixel::fgHiGainLast       =  19;
const Byte_t  MExtractBlindPixel::fgLoGainFirst      =   0;
const Byte_t  MExtractBlindPixel::fgLoGainLast       =   7;
const Int_t   MExtractBlindPixel::fgNSBFilterLimit   =  70;
const Float_t MExtractBlindPixel::fgResolution       = 0.003;
const Float_t MExtractBlindPixel::gkOverflow         = 300.;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Initializes:
// - fBlindPixelIdx to fgBlindPixelIdx
// - fNSBFilterLimit to fgNSBFilterLimit
// - fResolution to fgResolution
// - fExtractionType to 0.
//
// Calls:
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
//
MExtractBlindPixel::MExtractBlindPixel(const char *name, const char *title)
    : fBlindPixel(0), /*fHiLoLast(0),*/ fDataType(0)
{
  
  fName  = name  ? name  : "MExtractBlindPixel";
  fTitle = title ? title : "Task to extract the signal from the FADC slices";
  
  SetResolution();
  SetNSBFilterLimit();
  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
  
//  SetNumBlindPixels();
  fBlindPixelIdx.Set(1);
  fBlindPixelIdx[0] = fgBlindPixelIdx;

  Clear();

}

// --------------------------------------------------------------------------
//
// Clear
//
// Initializes:
// - fBlindPixelIdx to 0
// - fExtractionType to 0
// 
// Calls:
// - SetBlindPixelIdx()
// 
// Deletes and sets to NULL (if exists):
// - fHiGainSignal
// - fHiGainFirstDeriv
// - fHiGainSecondDeriv
//
void MExtractBlindPixel::Clear( const Option_t *o)
{
    fExtractionType = 0;

    fBlindPixelIdx.Set(1);
    fBlindPixelIdx[0] = fgBlindPixelIdx;
}

void MExtractBlindPixel::SetBlindPixels(const MCalibrationBlindCam &cam)
{
    const Int_t n = cam.GetSize();

    fBlindPixelIdx.Set(n);
    for (Int_t i=0; i<n; i++)
        fBlindPixelIdx[i] = cam[i].GetPixId();
}

void MExtractBlindPixel::SetRange(UShort_t hifirst, UShort_t hilast, Int_t lofirst, Byte_t lolast)
{

  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  fNumHiGainSamples = (Float_t)(fHiGainLast-fHiGainFirst+1);
//  if (lolast)
//    fNumLoGainSamples = (Float_t)(fLoGainLast-fLoGainFirst+1);
//  else
    fNumLoGainSamples = 0.;

  fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
  fSqrtLoGainSamples = TMath::Sqrt(fNumLoGainSamples);  
  
  //fHiLoFirst = 0;
  //fHiLoLast  = 0;
}

// --------------------------------------------------------------------------
//
// Calls: 
// - MExtractor::PreProcess(pList)
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MExtractedBlindPixel
//
Int_t MExtractBlindPixel::PreProcess(MParList *pList)
{

  if (!MExtractor::PreProcess(pList))
    return kFALSE;

  fBlindPixel = (MExtractedSignalBlindPixel*)pList->FindCreateObj(AddSerialNumber("MExtractedSignalBlindPixel"));
  if (!fBlindPixel)
    return kFALSE;

  const TString raw = IsDataType(kRawEvt2) ? "MRawEvtData2" : "MRawEvtData";
  const TString sig = IsDataType(kRawEvt2) ? "MPedestalSubtractedEvt2" : "MPedestalSubtractedEvt";

  fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber(raw), "MRawEvtData");
  if (!fRawEvt)
    {
      *fLog << err << raw << " [MRawEvtData] not found... aborting." << endl;
      return kFALSE;
    }

  fSignal = (MPedestalSubtractedEvt*)pList->FindObject(AddSerialNumber(sig), "MPedestalSubtractedEvt");
  if (!fSignal)
    {
      *fLog << err << sig << " [MPedestalSubtractedEvt] not found... aborting." << endl;
      return kFALSE;
    }

  return kTRUE;
}

// -------------------------------------------------------------------------- //
//
// The ReInit searches for:
// -  MRawRunHeader::GetNumSamplesHiGain()
// -  MRawRunHeader::GetNumSamplesLoGain()
//
// In case that the variables fHiGainLast and fLoGainLast are smaller than 
// the even part of the number of samples obtained from the run header, a
// warning is given an the range is set back accordingly. A call to:  
// - SetRange(fHiGainFirst, fHiGainLast-diff, fLoGainFirst, fLoGainLast) or 
// - SetRange(fHiGainFirst, fHiGainLast, fLoGainFirst, fLoGainLast-diff) 
// is performed in that case. The variable diff means here the difference 
// between the requested range (fHiGainLast) and the available one. Note that 
// the functions SetRange() are mostly overloaded and perform more checks, 
// modifying the ranges again, if necessary.
//
Bool_t MExtractBlindPixel::ReInit(MParList *pList)
{
  
  for (UInt_t i=0;i<fBlindPixelIdx.GetSize();i++)
      fBlindPixel->SetBlindPixelIdx(fBlindPixelIdx[i], i);

  fBlindPixel->SetExtractionType(fExtractionType);
/*
  for (UInt_t i=0;i<fBlindPixelIdx.GetSize();i++)
  {

      MPedestalPix &pedpix  = (*fPedestals)[fBlindPixelIdx.At(i)];    
      
      if (&pedpix)
      {
	  fBlindPixel->SetPed      ( pedpix.GetPedestal()   * fNumLoGainSamples, i );
	  fBlindPixel->SetPedErr   ( pedpix.GetPedestalRms()* fNumLoGainSamples 
                                 / TMath::Sqrt((Float_t)fPedestals->GetNumSlices()*pedpix.GetNumEvents()), i );
	  fBlindPixel->SetPedRms   ( pedpix.GetPedestalRms()* TMath::Sqrt((Float_t)fNumLoGainSamples), i );
	  fBlindPixel->SetPedRmsErr( fBlindPixel->GetPedErr()/2., i );
      }
  }

  const Int_t higainsamples = fRunHeader->GetNumSamplesHiGain();
  const Int_t logainsamples = fRunHeader->GetNumSamplesLoGain();
  Int_t lastavailable       =  higainsamples-1;

  if (logainsamples)
    {
      //
      // If the signal is searched entirely in the low-gain range, have 
      // to skip the higain completely. This is steered by the variable fHiLoFirst
      // 
      const Int_t firstdesired   = (Int_t)fHiGainFirst;
  
      if (firstdesired > lastavailable)
        {
          const Int_t diff = firstdesired - lastavailable;
          *fLog << endl;
          *fLog << warn << "First Hi Gain slice " << (int)fHiGainFirst << " out of range [0,";
          *fLog << lastavailable << "]... start at slice " << diff << " of the Lo Gain " << endl;
          
          fHiLoFirst   = diff;
        }
    }
  
  const Int_t lastdesired = (Int_t)fHiGainLast;

  if (lastdesired > lastavailable)
    {
      Int_t diff     = lastdesired - lastavailable;
      lastavailable += logainsamples ? logainsamples-1 : 0;
      
      if (lastdesired > lastavailable)
        {
          *fLog << endl;
          *fLog << "Last Hi Gain slice " << (int)fHiGainLast << " out of range [0,";
          *fLog << lastavailable << "]... reduce upper limit by " << diff << endl;
          diff = logainsamples;
        }

      fHiGainLast = higainsamples - 1;
      fHiLoLast   = logainsamples ? diff : 0;
    }
 */
  const Int_t range = fHiGainLast-fHiGainFirst+1; //fHiLoFirst ? fHiLoLast - fHiLoFirst + 1 : fHiGainLast - fHiGainFirst + fHiLoLast + 1;

//  fHiGainSignal.Set(range);
//  fHiGainSignal.Reset();

  fHiGainFirstDeriv.Set(range);
  fHiGainFirstDeriv.Reset();

  fHiGainSecondDeriv.Set(range);
  fHiGainSecondDeriv.Reset();

  *fLog << endl;
  *fLog << inf << "Extracting "
      << (IsExtractionType(kAmplitude) ? "Amplitude" : " Integral")
      << " using " << range << " FADC samples from slice "
      << (Int_t)fHiGainFirst << " to " << (Int_t)fHiGainLast << " (incl)" << endl;

  if (IsExtractionType(kFilter))
    *fLog << inf << "Will use Filter using "
	  << (Int_t)(fLoGainLast-fLoGainFirst+1) << " FADC slices"
	  << " from slice " << (Int_t)fLoGainFirst
	  << " to " << (Int_t)fLoGainLast << endl;

  fBlindPixel->SetUsedFADCSlices(fHiGainFirst, range);

  return kTRUE;

}

// --------------------------------------------------------------------------
//
// FindSignalHiGain:
//
// - Loop from ptr to (ptr+fHiGainLast-fHiGainFirst)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// - If fHiLoLast is set, loop from logain to (logain+fHiLoLast)
// - Add contents of *logain to sum
//
/*
void MExtractBlindPixel::FindIntegral(Byte_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat)
{
    const Byte_t *end = ptr + fHiGainLast - fHiGainFirst + 1;

    Int_t summ = 0;
    while (p<end)
    {
        summ += *ptr;

        if (*ptr++ >= fSaturationLimit)
            sat++;
    }

    sum = (Float_t)summ;
}
*/

// --------------------------------------------------------------------------
//
// FindSignalPhe:
//
// - Loop from ptr to (ptr+fHiGainLast-fHiGainFirst)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// - If fHiLoLast is set, loop from logain to (logain+fHiLoLast)
// - Add contents of *logain to sum
// 
//void MExtractBlindPixel::FindAmplitude(Float_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat)
Float_t MExtractBlindPixel::FindAmplitude(Int_t idx, Int_t numsat) const
{
    Int_t sat0 = fHiGainFirst; // First slice to extract and first saturating slice
    Int_t sat1 = fHiGainLast;  // Last  slice to extract and last saturating slice

    Int_t maxpos = fSignal->GetMaxPos(idx, sat0, sat1);

//    numsat = fSignal->GetSaturation(idx, fSaturationLimit, sat0, sat1);

    const Float_t *ptr = fSignal->GetSamples(idx);
    const Int_t    num = fHiGainLast-fHiGainFirst+1;

    MExtralgoSpline s(ptr, num, fHiGainFirstDeriv.GetArray(), fHiGainSecondDeriv.GetArray());

    s.SetExtractionType(MExtralgoSpline::kAmplitude);

    s.Extract(numsat, maxpos);

    return s.GetSignal();
/*
  Int_t range   = 0;
  Int_t count   = 0;
  Float_t abmaxpos = 0.;
  Byte_t *p     = ptr;
  Byte_t *end;
  Byte_t max    = 0;
  Byte_t maxpos = 0;
  Int_t summ   = 0;

  if (fHiLoFirst == 0)
    {

      range = fHiGainLast - fHiGainFirst + 1;

      end   = ptr + range;
      //
      // Check for saturation in all other slices
      //
      while (p++<end)
        {
          
          fHiGainSignal[count] = (Float_t)*p;
          summ += *p;

          if (*p > max)
            {
              max    = *p;
              maxpos =  count;
            }
          
          count++;

          if (*p >= fSaturationLimit)
              sat++;
        }
    }
  
  if (fHiLoLast != 0)
    {
      
      p    = logain + fHiLoFirst;
      end  = logain + fHiLoLast;
      
      while (p<end)
        {
          
          fHiGainSignal[count] = (Float_t)*p;
          summ += *p;

          if (*p > max)
            {
              max    = *p;
              maxpos =  count;
            }
          
          range++;
          count++;

          if (*p++ >= fSaturationLimit)
              sat++;
        }
    }

  //
  // allow one saturated slice 
  //
  if (sat > 1)
  {
    sum = gkOverflow;
    return;
  }

  //
  // Don't start if the maxpos is too close to the left limit.
  //
  if (maxpos < 2)
  {
    sum = (Float_t)max;
    return;
  }

  Float_t pp;

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
  sum       = (Float_t)khicont;
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
        {
          sum    = y;
          abmaxpos = x;
        }
    }
  */
}

// --------------------------------------------------------------------------
//
// FindSignalFilter:
//
// - Loop from ptr to (ptr+fLoGainLast-fLoGainFirst)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
//
/*
void MExtractBlindPixel::FindSignalFilter(Byte_t *ptr, Int_t range, Int_t &sum, Byte_t &sat) const
{

  Byte_t *end = ptr + range;
  
  while (ptr<end)
    {
      sum += *ptr;
      
      if (*ptr++ >= fSaturationLimit)
        sat++;
    }
}
*/

// --------------------------------------------------------------------------
//
// Calculate the integral of the FADC time slices and store them as a new
// pixel in the MExtractedBlindPixel container.
//
Int_t MExtractBlindPixel::Process()
{

  MRawEvtPixelIter pixel(fRawEvt);
  
  fBlindPixel->Clear();
  
  for (UInt_t id=0;id<fBlindPixelIdx.GetSize();id++)
  {
      // Be carefull: GetSaturation changed sat0 and sat1
      Int_t sat0 = fHiGainFirst; // First slice to extract and first saturating slice
      Int_t sat1 = fHiGainLast;  // Last  slice to extract and last saturating slice

      const Int_t sat = fSignal->GetSaturation(fBlindPixelIdx[id], fSaturationLimit, sat0, sat1);
      
      if (IsExtractionType(kFilter))
      {
          // FIXME: fLoGain* is used to determine the FILTER/CHECK range
          const Int_t numh = fRunHeader->GetNumSamplesHiGain();

          const Int_t sum = fSignal->GetIntegralRaw(fBlindPixelIdx[id], fLoGainFirst+numh, fLoGainLast+numh);
	  if (sum > fNSBFilterLimit)
	  {
	      fBlindPixel->SetExtractedSignal(-1.,id);
	      fBlindPixel->SetNumSaturated(sat,id);
	      fBlindPixel->SetReadyToSave();
	      continue;
	  }

          /*
          sum = 0;
          if (pixel.HasLoGain())
            FindSignalFilter(sl+pixel.GetNumLoGainSamples()+fLoGainFirst, fLoGainLast - fLoGainFirst + 1, sum, sat);

          if (fModified)
            {
              if (sum > fNSBFilterLimit)
                {
                  fBlindPixel->SetExtractedSignal(-1.,id);
                  fBlindPixel->SetNumSaturated(sat,id);
                  fBlindPixel->SetReadyToSave();
                  continue;
                }
            }
          */
      }


      Float_t newsum = 0.;
      if (IsExtractionType(kAmplitude))
          newsum = FindAmplitude(fBlindPixelIdx[id], sat);
      else
          newsum = fSignal->GetIntegralRaw(fBlindPixelIdx[id], fHiGainFirst, fHiGainLast);
  
      fBlindPixel->SetExtractedSignal(newsum, id);
      fBlindPixel->SetNumSaturated(sat, id);
  }

  fBlindPixel->SetReadyToSave();
  return kTRUE;
}

// ------------------------------------------------------------------------------------
//
// Returns true if the Data type. Available are: kAmplitude, kIntegral and kFilter
// The flags kIntegral and kFilter may be set both. 
//
Bool_t MExtractBlindPixel::IsDataType( const DataType_t typ )
{
  return TESTBIT( fDataType, typ );
}

// ------------------------------------------------------------------------------------
//
// Returns true if the extraction type. Available are: kAmplitude, kIntegral and kFilter
// The flags kIntegral and kFilter may be set both. 
//
Bool_t MExtractBlindPixel::IsExtractionType( const ExtractionType_t typ )
{
  return TESTBIT( fExtractionType, typ );
}

// --------------------------------------------------------------------------
//
// Sets the Data type. Available are: kAmplitude and kIntegral
//
void MExtractBlindPixel::SetDataType( const DataType_t typ )
{
  SETBIT( fDataType, typ );
}

// --------------------------------------------------------------------------
//
// Sets the extraction type. Available are: kAmplitude and kIntegral
//
void MExtractBlindPixel::SetExtractionType( const ExtractionType_t typ )
{
  SETBIT( fExtractionType, typ );
}
