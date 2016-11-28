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
//         to modify the ranges. 
//
//         The spline will then be integrated from fHiGainFirst to fHiGainLast, 
//         including half of the outer edges. The effective number of intergrated
//         slices ("range") is thus: 
// 
//         range = fHiGainLast - fHiGainFirst
//
//         Ranges have to be an even number. In case of odd ranges, the last slice 
//         will be reduced by one.
//
//  Defaults are: 
// 
//   fHiGainFirst =  fgHiGainFirst =  2 
//   fHiGainLast  =  fgHiGainLast  =  14
//   fLoGainFirst =  fgLoGainFirst =  3 
//   fLoGainLast  =  fgLoGainLast  =  13
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractFixedWindowSpline.h"

#include "MExtractedSignalCam.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MExtractFixedWindowSpline);

using namespace std;

const Byte_t  MExtractFixedWindowSpline::fgHiGainFirst  = 2;
const Byte_t  MExtractFixedWindowSpline::fgHiGainLast   = 14;
const Byte_t  MExtractFixedWindowSpline::fgLoGainFirst  = 3;
const Byte_t  MExtractFixedWindowSpline::fgLoGainLast   = 13;
// --------------------------------------------------------------------------
//
// Default constructor.
//
// Calls: 
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
// 
MExtractFixedWindowSpline::MExtractFixedWindowSpline(const char *name, const char *title) 
{

  fName  = name  ? name  : "MExtractFixedWindowSpline";
  fTitle = title ? title : "Signal Extractor for a fixed FADC window using a fast spline";

  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
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
void MExtractFixedWindowSpline::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{

  const Byte_t windowhi = hilast-hifirst;
  const Byte_t whieven  = windowhi & ~1;

  if (whieven != windowhi)
    {
      *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i",": Hi Gain window size has to be uneven, set last slice from "
                    ,(int)hilast," to ",(int)(hilast-1)) << endl;
      hilast -= 1;
    }
  
  if (whieven<2) 
    {
      *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i",": Hi Gain window is smaller than 2 FADC sampes, set last slice from" 
                    ,(int)hilast," to ",(int)(hifirst+2)) << endl;
      hilast = hifirst+2;
    }

  const Byte_t windowlo = lolast-lofirst;
  const Byte_t wloeven  = windowlo & ~1;

  if (lolast != 0)
    {
      if (wloeven != windowlo)
        {
          *fLog << warn << GetDescriptor() 
                << Form("%s%2i%s%2i",": Lo Gain window size has to be uneven, set last slice from "
                        ,(int)lolast," to ",(int)(lolast-1)) << endl;
          lolast -= 1;
        }
      
      
      if (wloeven<2) 
        {
          *fLog << warn << GetDescriptor() 
                << Form("%s%2i%s%2i",": Lo Gain window is smaller than 2 FADC sampes, set last slice from" 
                        ,(int)lolast," to ",(int)(lofirst+2)) << endl;
          lolast = lofirst+2;        
        }
    }
  

  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  //
  // Very important: Because the spline interpolates between the slices, 
  //                 the number of samples for the pedestal subtraction 
  //                 is now 1 less than with e.g. MExtractFixedWindow
  //
  fNumHiGainSamples = (Float_t)(fHiGainLast-fHiGainFirst);
  if (fLoGainLast != 0)
    fNumLoGainSamples = (Float_t)(fLoGainLast-fLoGainFirst);  
  else
    fNumLoGainSamples = 0.;

  fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
  fSqrtLoGainSamples = TMath::Sqrt(fNumLoGainSamples);  
  
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
Bool_t MExtractFixedWindowSpline::ReInit(MParList *pList)
{

  if (!MExtractor::ReInit(pList))
    return kFALSE;

  fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast+fHiLoLast, fNumHiGainSamples,
                              fLoGainFirst, fLoGainLast, fNumLoGainSamples);

  Int_t range = fHiGainLast - fHiGainFirst + 1 + fHiLoLast;

  fHiGainFirstDeriv.Set(range);
  fHiGainSecondDeriv.Set(range);

  range = fLoGainLast - fLoGainFirst + 1;

  fLoGainFirstDeriv.Set(range);
  fLoGainSecondDeriv.Set(range);

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
void MExtractFixedWindowSpline::FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat) const
{
  
  const Byte_t *end = ptr + fHiGainLast - fHiGainFirst;
  Int_t range = fHiGainLast - fHiGainFirst + fHiLoLast + 1;
  
  Float_t pp;
  //  Int_t   i = 0;

  Int_t summ = 0;
  // 
  // Take half of the first slice content
  // 
  Float_t *firstderiv = fHiGainFirstDeriv.GetArray();
  Float_t *secondderiv = fHiGainSecondDeriv.GetArray();
  sum = (Float_t)*ptr/2.;
  // 
  // The first slice has already been treated now!
  // 
  ptr++; // i++;
  firstderiv++; 
  secondderiv++;
  //
  // Check for saturation in all other slices
  //
  while (ptr<end)
    {

      summ += *ptr;

      // pp = fHiGainSecondDeriv[i-1] + 4.;
      // fHiGainSecondDeriv[i] = -1.0/pp;
      // fHiGainFirstDeriv [i] = *(ptr+1) - 2.* *(ptr) + *(ptr-1);
      // fHiGainFirstDeriv [i] = (6.0*fHiGainFirstDeriv[i]-fHiGainFirstDeriv[i-1])/pp;

      pp = *(secondderiv-1) + 4.;
      *secondderiv = -1.0/pp;
      *firstderiv  = *(ptr+1) - 2.* *(ptr) + *(ptr-1);
      *firstderiv  = (6.0* *(firstderiv) - *(firstderiv-1))/pp;

      if (*ptr++ >= fSaturationLimit)
        sat++;

      secondderiv++;
      firstderiv++;

      //      i++;
    }
  
  switch (fHiLoLast)
    {
    case 0:
      // Treat the last slice of the high-gain as half slice:
      sum += (Float_t)*ptr/2.;
      break;
    case 1:
      // Treat the last slice of the high-gain as full slice:
      summ += *ptr;
      pp    = *(secondderiv-1) + 4.;
      *secondderiv = -1.0/pp;
      *firstderiv  = *(logain) - 2.* *(ptr) + *(ptr-1);
      *firstderiv  = (6.0* *(firstderiv) - *(firstderiv-1))/pp;
      secondderiv++;
      firstderiv++;
      if (*logain >= fSaturationLimit)
        sat++;
      // Treat the first slice of the low-gain as half slice:
      sum  += (Float_t)*logain/2;
      break;
    case 2:
      // Treat the last slice of the high-gain as full slice:
      summ += *ptr;
      pp    = *(secondderiv-1) + 4.;
      *secondderiv = -1.0/pp;
      *firstderiv  = *(logain) - 2.* *(ptr) + *(ptr-1);
      *firstderiv  = (6.0* *(firstderiv) - *(firstderiv-1))/pp;
      secondderiv++;
      firstderiv++;
      // Treat the last first slice of the low-gain as full slice:
      summ += *logain;
      pp    = *(secondderiv-1) + 4.;
      *secondderiv = -1.0/pp;
      *firstderiv  = *(logain+1) - 2.* *(logain) + *(ptr);
      *firstderiv  = (6.0* *(firstderiv) - *(firstderiv-1))/pp;
      secondderiv++;
      firstderiv++;
      if (*logain++ >= fSaturationLimit)
        sat++;
      // Treat the second slice of the low-gain as half slice:
      sum  += (Float_t)*logain/2;
      if (*logain >= fSaturationLimit)
        sat++;
      break;
    default:
      // Treat the last slice of the high-gain as full slice:
      summ += *ptr;
      pp    = *(secondderiv-1) + 4.;
      *secondderiv = -1.0/pp;
      *firstderiv  = *(logain) - 2.* *(ptr) + *(ptr-1);
      *firstderiv  = (6.0* *(firstderiv) - *(firstderiv-1))/pp;
      secondderiv++;
      firstderiv++;
      // Treat the last first slice of the low-gain as full slice:
      summ += *logain;
      pp    = *(secondderiv-1) + 4.;
      *secondderiv = -1.0/pp;
      *firstderiv  = *(logain+1) - 2.* *(logain) + *(ptr);
      *firstderiv  = (6.0* *(firstderiv) - *(firstderiv-1))/pp;
      secondderiv++;
      firstderiv++;
      if (*logain++ >= fSaturationLimit)
        sat++;
      // Treat the rest of the slices:
      const Byte_t *end = logain+fHiLoLast;
      while (logain<end)
	{
	  summ += *logain;
	  pp    = *(secondderiv-1) + 4.;
	  *secondderiv = -1.0/pp;
	  *firstderiv  = *(logain+1) - 2.* *(logain) + *(logain-1);
	  *firstderiv  = (6.0* *(firstderiv) - *(firstderiv-1))/pp;
	  //	  pp = fHiGainSecondDeriv[i-1] + 4.;
	  //	  fHiGainSecondDeriv[i] = -1.0/pp;
	  //	  fHiGainFirstDeriv [i] = *(logain+1) - 2.* *(logain) + *(logain-1);
	  //	  fHiGainFirstDeriv [i] = (6.0*fHiGainFirstDeriv[i]-fHiGainFirstDeriv[i-1])/pp;
	  secondderiv++;
	  firstderiv++;
	  if (*logain++ >= fSaturationLimit)
	    sat++;
	}
      break;
    }
  
  //
  // Go back to last but one element:
  //
  secondderiv--;
  firstderiv--;

  for (Int_t k=range-2;k>0;k--)
    {
      *secondderiv = *secondderiv * *(secondderiv+1) + *firstderiv;
      sum += 0.25* *secondderiv;
      firstderiv--;
      secondderiv--;
      //      fHiGainSecondDeriv[k] = fHiGainSecondDeriv[k]*fHiGainSecondDeriv[k+1] + fHiGainFirstDeriv[k];
      //      sum += 0.25*fHiGainSecondDeriv[k];
    }

  sum += (Float_t)summ;
}

// --------------------------------------------------------------------------
//
// FindSignalLoGain:
//
// - Loop from ptr to (ptr+fLoGainLast-fLoGainFirst)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// 
void MExtractFixedWindowSpline::FindSignalLoGain(Byte_t *ptr, Float_t &sum, Byte_t &sat) const
{
  
  const Byte_t *end = ptr + fLoGainLast - fLoGainFirst;
  Int_t range = fLoGainLast - fLoGainFirst + 1;
  
  Float_t pp;
  //  Int_t   i = 0;

  Int_t summ = 0;
  // 
  // Take half of the first slice content
  // 
  Float_t *firstderiv = fLoGainFirstDeriv.GetArray();
  Float_t *secondderiv = fLoGainSecondDeriv.GetArray();
  sum = (Float_t)*ptr/2.;
  // 
  // The first slice has already been treated now!
  // 
  ptr++; // i++;
  secondderiv++;
  firstderiv++;
  //
  // Check for saturation in all other slices
  //
  while (ptr<end)
    {

      summ += *ptr;
      //      i++;

      // pp = fLoGainSecondDeriv[i-1] + 4.;
      // fLoGainSecondDeriv[i] = -1.0/pp;
      // fLoGainFirstDeriv [i] = *(ptr+1) - 2.* *(ptr) + *(ptr-1);
      // fLoGainFirstDeriv [i] = (6.0*fLoGainFirstDeriv[i]-fLoGainFirstDeriv[i-1])/pp;

      pp = *(secondderiv-1) + 4.;
      *secondderiv = -1.0/pp;
      *firstderiv  = *(ptr+1) - 2.* *(ptr) + *(ptr-1);
      *firstderiv  = (6.0* *(firstderiv) - *(firstderiv-1))/pp;

      if (*ptr++ >= fSaturationLimit)
        sat++;

      secondderiv++;
      firstderiv++;
    }
  
  sum += (Float_t)*ptr/2.;
  
  //
  // Go back to last but one element:
  //
  secondderiv--;
  firstderiv--;

  for (Int_t k=range-2;k>0;k--)
    {
      *secondderiv = *secondderiv * *(secondderiv+1) + *firstderiv;
      sum += 0.25* *secondderiv;
      firstderiv--;
      secondderiv--;
      //      fLoGainSecondDeriv[k] = fLoGainSecondDeriv[k]*fLoGainSecondDeriv[k+1] + fLoGainFirstDeriv[k];
      //      sum += 0.25*fLoGainSecondDeriv[k];
    }
  
  sum += (Float_t)summ;
}

