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
//  MExtractPINDiode
//
//  Extracts the signal from a fixed window in a given range.
//
//  Call: SetRange(fHiGainFirst, fHiGainLast, fLoGainFirst, fLoGainLast) 
//  to modify the ranges.
//
//  Defaults are: 
// 
//   fHiGainFirst =  fgHiGainFirst =  0 
//   fHiGainLast  =  fgHiGainLast  =  29
//   fLoGainFirst =  fgLoGainFirst =  0
//   fLoGainLast  =  fgLoGainLast  =  0
//
//  The FADC slices are fit by a Gaussian around the pulse maximum. 
//  The following figures show two typical (high-intensity and low-intensity) 
//  pulses together with the applied fit:
//
//Begin_Html
/*
<img src="images/PINDiode_pulse_high.gif">
<img src="images/PINDiode_pulse_low.gif">
*/
//End_Html
//
// The fit ranges can be modified with the functions:
// - SetLowerFitLimit() 
// - SetUpperFitLimit()
//
// Defaults are:
//   - fLowerFitLimit: 2
//   - fUpperFitLimit: 5
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractPINDiode.h"

#include <fstream>

#include <TF1.h>
#include <TH1.h>
#include <TPad.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtData.h"
#include "MRawRunHeader.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MExtractedSignalPINDiode.h"

ClassImp(MExtractPINDiode);

using namespace std;

const UInt_t MExtractPINDiode::fgPINDiodeIdx   =  3;
const Byte_t MExtractPINDiode::fgHiGainFirst   =  0;
const Byte_t MExtractPINDiode::fgHiGainLast    = 29;
const Byte_t MExtractPINDiode::fgLoGainFirst   =  0;
const Byte_t MExtractPINDiode::fgLoGainLast    =  0;
const Byte_t MExtractPINDiode::fgLowerFitLimit =  2;
const Byte_t MExtractPINDiode::fgUpperFitLimit =  5;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Calls: 
// - SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast)
//
// - Set fPINDiodeIdx   to fgPINDiodeIdx
// - Set fLowerFitLimit to fgLowerFitLimit
// - Set fUpperFitLimit to fgUpperFitLimit
//
MExtractPINDiode::MExtractPINDiode(const char *name, const char *title)
    : fSlices(NULL)
{

  fName  = name  ? name  : "MExtractPINDiode";
  fTitle = title ? title : "Task to extract the signal from the FADC slices";
  
  SetRange(fgHiGainFirst, fgHiGainLast, fgLoGainFirst, fgLoGainLast);
  SetPINDiodeIdx();

  SetLowerFitLimit();
  SetUpperFitLimit();  

  fPedMean.Set(2);
}

// --------------------------------------------------------------------------
//
// - delete the histogram fSlices, if it exists
//
MExtractPINDiode::~MExtractPINDiode()
{
  if (fSlices)
    delete fSlices;
}

// --------------------------------------------------------------------------
//
// SetRange: 
//
// Checks: 
// - if the Hi Gain window is smaller than 4, set fHiGainLast to fHiGainFirst+3
// 
// Calls:
// - MExtractor::SetRange(hifirst,hilast,lofirst,lolast);
// 
// Sets:
// - fNumHiGainSamples to: (Float_t)(fHiGainLast-fHiGainFirst+1)
// - fNumLoGainSamples to: 0.
// - fSqrtHiGainSamples to: TMath::Sqrt(fNumHiGainSamples)
// - fSqrtLoGainSamples to: 0.
//  
void MExtractPINDiode::SetRange(UShort_t hifirst, UShort_t hilast, Int_t lofirst, Byte_t lolast)
{

  lofirst = 0;
  lolast  = 0;

  const Byte_t window = hilast-hifirst+1;

  if (window<4) 
    {
      *fLog << warn << GetDescriptor() 
            << Form("%s%2i%s%2i",": Total window is smaller than 4 FADC sampes, set last slice from" 
                    ,(int)lolast," to ",(int)(lofirst+3)) << endl;
      hilast = hifirst+3;
    }

  MExtractor::SetRange(hifirst,hilast,lofirst,lolast);

  fNumHiGainSamples = (Float_t)(fHiGainLast-fHiGainFirst+1);
  fNumLoGainSamples = 0.;

  fSqrtHiGainSamples = TMath::Sqrt(fNumHiGainSamples);
  fSqrtLoGainSamples = 0.;
  
}

// --------------------------------------------------------------------------
//
// Calls: 
// - MExtractor::PreProcess
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MRawEvtData2
//  - MExtractedPINDiode
//
// Initializes fPedMean to:
// - fPedMean[0]: pedestal + AB-offset
// - fPedMean[1]: pedestal - AB-offset
//
// Initializes TH1F fSlices to [fHiGainFirst-0.5,fHiGainLast+0.5]
//
Int_t MExtractPINDiode::PreProcess(MParList *pList)
{

  if (!MExtractor::PreProcess(pList))
    return kFALSE;
  
  fRawEvt = NULL;
  fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData2"));
  if (!fRawEvt)
    {
      *fLog << err << AddSerialNumber("MRawEvtData2") << " not found... aborting." << endl;
      return kFALSE;
    }

  fPINDiode = (MExtractedSignalPINDiode*)pList->FindCreateObj(AddSerialNumber("MExtractedSignalPINDiode"));
  if (!fPINDiode)
    return kFALSE;

  fPedMean.Reset();
/*
  const MPedestalPix &ped   = (*fPedestals)[fPINDiodeIdx]; 

  if (&ped)
    {
      fPedMean[0] = ped.GetPedestal() + ped.GetPedestalABoffset();
      fPedMean[1] = ped.GetPedestal() - ped.GetPedestalABoffset();      
    }
  else
    {
      *fLog << err << " Cannot find MPedestalPix of the PIN Diode (idx=" 
            << fPINDiodeIdx << ")" << endl;
        return kFALSE;
    }
  */
  if (fSlices)
    delete fSlices;

  fSlices = new TH1F("PINDiode","PIN Diode fitted slices",(Int_t)(fHiGainLast-fHiGainFirst+1),
                     fHiGainFirst-0.5,fHiGainLast+0.5);
  fSlices->SetDirectory(NULL);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// The ReInit calls:
// -  MExtractor::ReInit()
// -  fPINDiode->SetUsedFADCSlices(fHiGainFirst, fLoGainLast);
//
Bool_t MExtractPINDiode::ReInit(MParList *pList)
{

  MExtractor::ReInit(pList);
  
  fPINDiode->SetUsedFADCSlices(fHiGainFirst, fLoGainLast);

  *fLog << endl;
  *fLog << inf << "Taking " << fNumHiGainSamples
        << " HiGain samples from slice " << (Int_t)fHiGainFirst
        << " to " << (Int_t)(fHiGainLast/*+fHiLoLast*/) << " incl" << endl;

  return kTRUE;
}




// ----------------------------------------------------------------------------------
//
// Extracts the (pedestal-subtracted) FADC slices between fHiGainFirst and fHiGainLast 
// and fills them into the histogram fSlices. Memorizes the position of maximum at 
// maxpos.
//
// Checks for saturation
// 
// Fits fSlices to a Gaussian in the ranges: maxpos-fLowerFitLimit, maxpos+fUpperFitLimit
//
// Writes fit results into MExtractedSignalPINDiode
//
Int_t MExtractPINDiode::Process()
{

/*
  MRawEvtPixelIter pixel(fRawEvt);

  fPINDiode->Clear();
  fSlices->Reset();

  pixel.Jump(fPINDiodeIdx);

  Byte_t sat  = 0;

  const Int_t higainsamples = fRunHeader->GetNumSamplesHiGain();
  const Int_t logainsamples = fRunHeader->GetNumSamplesLoGain();

  const Bool_t higainabflag = pixel.HasABFlag();
  Byte_t *ptr = pixel.GetHiGainSamples()+fHiGainFirst;
  Byte_t *end = ptr+higainsamples;

  Int_t cnt=0;

  Float_t max = 0.;
  Int_t maxpos = 0;

  while (ptr<end)
    {

      if (*ptr >= fSaturationLimit)
        {
          sat++;
          break;
        }

      const Float_t cont = (Float_t)*ptr - fPedMean[(cnt + higainabflag) & 0x1];
      fSlices->Fill(cnt,cont);

      if (cont > max)
      {
        max = cont;
        maxpos = cnt;
      }

      ptr++;
      cnt++;
    }
  
  cnt = 0;
  
  if (logainsamples>0 && !sat)
    {
      
      ptr = pixel.GetLoGainSamples();
      end = ptr+logainsamples;
      
      const Bool_t logainabflag = (higainabflag + higainsamples) & 0x1;
      
      while (ptr<end)
        {
          
          if (*ptr >= fSaturationLimit)
            {
              sat++;
              break;
            }

          const Float_t cont = (Float_t)*ptr - fPedMean[(cnt + logainabflag) & 0x1];
          
          fSlices->Fill(cnt+ higainsamples,cont);
          
          if (cont > max) 
            {
              max    = cont;
              maxpos = cnt+higainsamples;
            }
          ptr++;
          cnt++;
        }
    }
          
  if (sat)
    {
      fPINDiode->SetSaturation(1);
      return kTRUE;
    }

  fSlices->Fit("gaus", "Q0", "", maxpos-fLowerFitLimit,maxpos+fUpperFitLimit);

  TF1 &gaus = *fSlices->GetFunction("gaus");

  fPINDiode->SetExtractedSignal(gaus.GetParameter(0), gaus.GetParError(0));
  fPINDiode->SetExtractedTime  (gaus.GetParameter(1), gaus.GetParError(1));
  fPINDiode->SetExtractedSigma (gaus.GetParameter(2), gaus.GetParError(2));
  fPINDiode->SetExtractedChi2  (gaus.GetChisquare());
  fPINDiode->SetReadyToSave();
  */
  return kTRUE;
}

// ----------------------------------------------------------------------------------
//
// deletes fSlices and sets pointer to NULL
//
Int_t MExtractPINDiode::PostProcess()
{
  
  delete fSlices;
  fSlices = NULL;
  
  return kTRUE;
}
