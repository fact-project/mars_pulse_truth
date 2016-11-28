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
!   Author(s): Markus Gaug  02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MExtractedSignalPINDiode
//
// This is the storage container to hold informations about the extracted signal 
// (offset) value of the calibration PIN Diode
//
/////////////////////////////////////////////////////////////////////////////
#include "MExtractedSignalPINDiode.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MExtractedSignalPINDiode);

using namespace std;
// ------------------------------------------------------------------------
//
// MExtractedSignalPINDiode holds the extracted signal
// of the FADC slices and its error. 
//
// Additionally, the number of saturated Slices are stored. 
// 
// Default values for the extracted signals are: -1. 
//
MExtractedSignalPINDiode::MExtractedSignalPINDiode(const char* name, const char* title)
{

  fName  = name  ? name  : "MExtractedSignalPINDiode";
  fTitle = title ? title : "Container of the Extracted Signals";

  Clear();
}

// ------------------------------------------------------------------------
//
// Invalidate values to -1.
//
void MExtractedSignalPINDiode::Clear(Option_t *o)
{

  fExtractedSignal          = -1.;
  fExtractedSignalErr       = -1.;
  fExtractedTime            = -1.;
  fExtractedTimeErr         = -1.; 
  fExtractedSigma           = -1.; 
  fExtractedSigmaErr        = -1.; 
  fExtractedChi2            = -1.; 

  fSaturated  = kFALSE;
  
}

void MExtractedSignalPINDiode::SetUsedFADCSlices(const Byte_t first, const Byte_t num)   
{
  fFirst          = first; 
  fNumFADCSamples = num;
}


void MExtractedSignalPINDiode::SetExtractedSignal(const Float_t sig, const Float_t sigerr)   
{
  fExtractedSignal    = sig; 
  fExtractedSignalErr = sigerr;
}

void MExtractedSignalPINDiode::SetExtractedSigma(const Float_t sig, const Float_t sigerr)   
{
  fExtractedSigma    = sig; 
  fExtractedSigmaErr = sigerr;
}

void MExtractedSignalPINDiode::SetExtractedTime(const Float_t sig, const Float_t sigerr)   
{
  fExtractedTime    = sig; 
  fExtractedTimeErr = sigerr;
}

Bool_t MExtractedSignalPINDiode::IsValid() const
{
  if (fSaturated)
    return kFALSE;
  
  return fExtractedSignal >= 0. || fExtractedSignalErr >= 0.;
}

void MExtractedSignalPINDiode::Print(Option_t *o) const
{
  *fLog << Form(" Signal: %4.2f+-%4.2f",fExtractedSignal,fExtractedSignalErr) 
        << Form(" Arr.Time: %4.2f+-%4.2f",fExtractedTime,fExtractedTimeErr) 
        << Form(" Sigma: %4.2f+-%4.2f",fExtractedSigma,fExtractedSigmaErr) 
        << Form(" Chi2: %5.2f",fExtractedChi2) 
        << Form(" Saturation: %s",fSaturated ? "yes" : "no")
        << endl;
}
