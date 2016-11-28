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
!   Author(s): Hendrik Bartko 02/2004 <mailto:hbartko@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MArrivalTimePix
//
// This is the storage container to hold informations about the ArrivalTime
//
/////////////////////////////////////////////////////////////////////////////
#include "MArrivalTimePix.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MArrivalTimePix);

using namespace std;

static const Float_t gkSignalInitializer = 99999.9;

// ------------------------------------------------------------------------
//
// MArrivalTimePix holds the ArrivalTime (HiGain and LoGain) 
// of the FADC slices and its error. 
//
// Additionally, the number of saturated HiGain and LoGain Slices are stored. 
// 
// Default values for the ArrivalTime are: 99999.9 
//
MArrivalTimePix::MArrivalTimePix(const char* name, const char* title)
{
  fName  = name  ? name  : "MArrivalTimePix";
  fTitle = title ? title : "Container of the Extracted Signals";

  Clear();
}

// ------------------------------------------------------------------------
//
// Invalidate values
//
void MArrivalTimePix::Clear(Option_t *o)
{
  fArrivalTimeHiGain      = gkSignalInitializer;
  fArrivalTimeHiGainError = -1;
  fArrivalTimeLoGain      = gkSignalInitializer;
  fArrivalTimeLoGainError = -1;
  
  fNumHiGainSaturated = 0;
  fNumLoGainSaturated = 0;
}

void MArrivalTimePix::SetArrivalTime(Float_t sig, Float_t sigerr)   
{
  fArrivalTimeHiGain      = sig; 
  fArrivalTimeHiGainError = sigerr;
}

void MArrivalTimePix::SetArrivalTime(Float_t sighi, Float_t sighierr,
                                     Float_t siglo, Float_t sigloerr)   
{
  fArrivalTimeHiGain = sighi;
  fArrivalTimeHiGainError = sighierr;
  fArrivalTimeLoGain = siglo;
  fArrivalTimeLoGainError = sigloerr;
}

Float_t MArrivalTimePix::GetArrivalTime() const
{
    // If hi-gain is not saturated and has successfully been
    // extracted use the hi-gain arrival time
    if (!IsHiGainSaturated() && IsHiGainValid())
        return GetArrivalTimeHiGain();

    // If hi-gain could not be used ans the lo-gain could
    // not be extracted return
    if (!IsLoGainValid())
        return 0;

    // in all other cases use the lo-gain arrival time
    return GetArrivalTimeLoGain();
}

Bool_t MArrivalTimePix::IsArrivalTimeValid() const
{
    // If hi-gain is not saturated and has successfully been
    // extracted use the hi-gain arrival time
    if (!IsHiGainSaturated() && IsHiGainValid())
        return kTRUE;

    // If hi-gain could not be used ans the lo-gain could
    // not be extracted return kFALSE (no valid arrival time)
    if (!IsLoGainValid())
        return kFALSE;

    // in all other cases use the lo-gain arrival time
    return kTRUE;
}

void MArrivalTimePix::SetGainSaturation(Byte_t higain, Byte_t logain)
{
    fNumHiGainSaturated = higain;
    fNumLoGainSaturated = logain;
}

void MArrivalTimePix::Print(Option_t *o) const
{
    *fLog << " Signal: " << fArrivalTimeHiGain
        << " +- " << fArrivalTimeHiGainError
        << " Nr. Sat. Hi Gain: " <<  fNumHiGainSaturated
        << " Nr. Sat. Lo Gain: " <<  fNumLoGainSaturated
        << endl;
}
