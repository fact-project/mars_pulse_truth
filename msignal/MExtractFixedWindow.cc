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
!   Author(s): Markus Gaug, 04/2004 <mailto:markus@ifae.es>
!              Thomas Bretz, 01/2004 
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MExtractFixedWindow
//
//  Extracts the signal from a fixed window in a given range by summing up the 
//  slice contents.
//
//  Call: SetRange(fHiGainFirst, fHiGainLast, fLoGainFirst, fLoGainLast) 
//  to modify the ranges. Ranges have to be an even number. In case of odd 
//  ranges, the last slice will be reduced by one.
//  Defaults are: 
// 
//   fHiGainFirst =  fgHiGainFirst =  3 
//   fHiGainLast  =  fgHiGainLast  =  14
//   fLoGainFirst =  fgLoGainFirst =  3 
//   fLoGainLast  =  fgLoGainLast  =  14
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractFixedWindow.h"
#include "MExtractor.h"

#include <fstream>

#include "MExtractedSignalCam.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MExtractFixedWindow);

using namespace std;

const Byte_t MExtractFixedWindow::fgHiGainFirst =  3;
const Byte_t MExtractFixedWindow::fgHiGainLast  =  14;
const Byte_t MExtractFixedWindow::fgLoGainFirst =  3;
const Byte_t MExtractFixedWindow::fgLoGainLast  =  14;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Calls:
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
//
MExtractFixedWindow::MExtractFixedWindow(const char *name, const char *title)
{
  fName  = name  ? name  : "MExtractFixedWindow";
  fTitle = title ? title : "Signal Extractor for a fixed FADC window";

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
void MExtractFixedWindow::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{

  const Byte_t windowhi = hilast-hifirst+1;
  const Byte_t whieven = windowhi & ~1;

  if (whieven != windowhi)
    {
      *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i",": Hi Gain window size has to be even, set last slice from "
                    ,(int)hilast," to ",(int)(hilast-1)) << endl;
      hilast -= 1;
    }
  
  if (whieven<2) 
    {
      *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i",": Hi Gain window is smaller than 2 FADC sampes, set last slice from" 
                    ,(int)hilast," to ",(int)(hifirst+1)) << endl;
      hilast = hifirst+1;
    }
  

  if (lolast != 0)
    {
      const Byte_t windowlo = lolast-lofirst+1;
      const Byte_t wloeven = windowlo & ~1;

      if (wloeven != windowlo)
        {
          *fLog << warn << GetDescriptor() 
                << Form("%s%2i%s%2i",": Lo Gain window size has to be even, set last slice from "
                        ,(int)lolast," to ",(int)(lolast-1)) << endl;
          lolast -= 1;
        }
      
      if (wloeven<2) 
        {
          *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i",": Lo Gain window is smaller than 2 FADC sampes, set last slice from" 
                    ,(int)lolast," to ",(int)(lofirst+1)) << endl;
          lolast = lofirst+1;        
        }
    }

  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  fNumHiGainSamples = (Float_t)(fHiGainLast-fHiGainFirst+1);
  if (fLoGainLast != 0)
    fNumLoGainSamples = (Float_t)(fLoGainLast-fLoGainFirst+1);  
  else
    fNumLoGainSamples = 0.;

  fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
  fSqrtLoGainSamples = TMath::Sqrt(fNumLoGainSamples);  

  const Int_t wshigain = fHiGainLast-fHiGainFirst+1;
  const Int_t wslogain = fLoGainLast-fLoGainFirst+1;

  switch (wshigain)
    {
    case 2:
      SetResolutionPerPheHiGain(0.021);
      break;
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
      SetResolutionPerPheHiGain(0.011);      
      break;
    default:
        *fLog << warn << GetDescriptor() << ": Could not set the hi-gain extractor resolution/phe for window size " << wshigain << endl;
    }
  
  switch (wslogain)
    {
    case 4:
      SetResolutionPerPheLoGain(0.063);
      break;
    case 6:
      SetResolutionPerPheLoGain(0.017);
      break;
    case 8:
    case 10:
      SetResolutionPerPheLoGain(0.011);      
      break;
    default:
      *fLog << warn << GetDescriptor() << ": Could not set the lo-gain extractor resolution/phe for window size " << wslogain << endl;
      SetResolutionPerPheLoGain(0.011);
    }
}

// --------------------------------------------------------------------------
//
// The ReInit calls:
// -  MExtractor::ReInit()
// -  fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast+fHiLoLast, fNumHiGainSamples,
//                                fLoGainFirst, fLoGainLast, fNumLoGainSamples);
//
Bool_t MExtractFixedWindow::ReInit(MParList *pList)
{

  MExtractor::ReInit(pList);
  
  fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast+fHiLoLast, fNumHiGainSamples,
                              fLoGainFirst, fLoGainLast, fNumLoGainSamples);


  *fLog << endl;
  *fLog << inf << "Taking " << fNumHiGainSamples
        << " HiGain samples from slice " << (Int_t)fHiGainFirst
        << " to " << (Int_t)(fHiGainLast+fHiLoLast) << " incl" << endl;
  *fLog << inf << "Taking " << fNumLoGainSamples
        << " LoGain samples from slice " << (Int_t)fLoGainFirst
        << " to " << (Int_t)fLoGainLast << " incl" << endl;
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
void MExtractFixedWindow::FindSignalHiGain(Byte_t *ptr, Byte_t *logain, Float_t &sum, Byte_t &sat) const
{

  Int_t summ = 0;

  Byte_t *end = ptr + fHiGainLast - fHiGainFirst + 1;

  while (ptr<end)
    {
      summ += *ptr;
      if (*ptr++ >= fSaturationLimit)
        sat++;
    }

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
// - Loop from ptr to (ptr+fLoGainLast-fLoGainFirst)
// - Sum up contents of *ptr
// - If *ptr is greater than fSaturationLimit, raise sat by 1
// - If fHiLoLast is set, loop from logain to (logain+fHiLoLast)
// - Add contents of *logain to sum
// 
void MExtractFixedWindow::FindSignalLoGain(Byte_t *ptr, Float_t &sum, Byte_t &sat) const
{

  Int_t summ = 0;

  Byte_t *end = ptr + fLoGainLast - fLoGainFirst + 1;
  
  while (ptr<end)
    {
      summ += *ptr;
      
      if (*ptr++ >= fSaturationLimit)
        sat++;
    }

  sum = (Float_t)summ;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MExtractFixedWindow::StreamPrimitive(ostream &out) const
{

  out << "   " << ClassName() << " " << GetUniqueName() << "(\"";
  out << "\"" << fName << "\", \"" << fTitle << "\");" << endl;
  
  if (fSaturationLimit!=fgSaturationLimit)
    {
      out << "   " << GetUniqueName() << ".SetSaturationLimit(";
      out << (int)fSaturationLimit << ");" << endl;
    }
  
  const Bool_t arg4 = fNumLoGainSamples+fLoGainFirst-1 != fgLoGainLast;
  const Bool_t arg3 = arg4 || fLoGainFirst != fgLoGainFirst;
  const Bool_t arg2 = arg3 || fNumHiGainSamples+fHiGainFirst-1 != fgHiGainLast;
  const Bool_t arg1 = arg2 || fHiGainFirst != fgHiGainFirst;
  
  if (!arg1)
    return;
  
  out << "   " << GetUniqueName() << ".SetRange(";
  out << (int)fLoGainFirst;
  if (arg2)
    {
      out << ", " << (int)(fNumHiGainSamples+fHiGainFirst-1);
      if (arg3)
        {
          out << ", " << (int)fLoGainFirst;
          if (arg4)
            out << ", " << (int)(fNumLoGainSamples+fLoGainFirst-1);
        }
    }
  out << ");" << endl;
}

void MExtractFixedWindow::Print(Option_t *o) const
{
    *fLog << all;
    *fLog << GetDescriptor() << ":" << endl;
    MExtractor::Print(o);
}
