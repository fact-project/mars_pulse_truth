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
!   Author(s): Markus Gaug  12/2003 <mailto:markus@ifae.es>
!   Author(s): Thomas Bretz 12/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MExtractedSignalPix
//
// This is the storage container to hold informations about the pedestal
// (offset) value of one Pixel (PMT).
//
// Class Version 3:
// ----------------
//  - fIsLoGainUsed
//  - fChisquare
//
/////////////////////////////////////////////////////////////////////////////
#include "MExtractedSignalPix.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MExtractedSignalPix);

using namespace std;

static const Float_t gkSignalInitializer = -99999.9;

// ------------------------------------------------------------------------
//
// MExtractedSignalPix holds the extracted signal (HiGain and LoGain) 
// of the FADC slices and its error. 
//
// Additionally, the number of saturated HiGain and LoGain Slices are stored. 
// 
// Default values for the extracted signals are: 99999.9 
//
MExtractedSignalPix::MExtractedSignalPix(const char* name, const char* title)
{
  fName  = name  ? name  : "MExtractedSignalPix";
  fTitle = title ? title : "Container of the Extracted Signals";

  Clear();
}


// ------------------------------------------------------------------------
//
// Invalidate values
//
void MExtractedSignalPix::Clear(Option_t *o)
{
  fExtractedSignalHiGain      = gkSignalInitializer;
  fExtractedSignalHiGainError = -1;
  fExtractedSignalLoGain      = gkSignalInitializer;
  fExtractedSignalLoGainError = -1;
  
  fNumHiGainSaturated = 0;
  fNumLoGainSaturated = 0;
}

void MExtractedSignalPix::SetExtractedSignal(Float_t sig, Float_t sigerr)
{
  fExtractedSignalHiGain      = sig; 
  fExtractedSignalHiGainError = sigerr;
}

void MExtractedSignalPix::SetExtractedSignal(Float_t sighi, Float_t sighierr,
                                             Float_t siglo, Float_t sigloerr)   
{
  fExtractedSignalHiGain = sighi;
  fExtractedSignalHiGainError = sighierr;
  fExtractedSignalLoGain = siglo;
  fExtractedSignalLoGainError = sigloerr;
}

void MExtractedSignalPix::SetGainSaturation(Byte_t higain, Byte_t logain)
{
    fNumHiGainSaturated = higain;
    fNumLoGainSaturated = logain;
}

void MExtractedSignalPix::Print(Option_t *o) const
{
    *fLog << " Signal: " << fExtractedSignalHiGain
        << " +- " << fExtractedSignalHiGainError
        << " Nr. Sat. Hi Gain: " <<  fNumHiGainSaturated
        << " Nr. Sat. Lo Gain: " <<  fNumLoGainSaturated
        << endl;
}
