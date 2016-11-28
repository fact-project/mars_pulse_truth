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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MEventRate
//
// Storage Container for the event rate.
//
//    rate = Number of events / time in which the events were recorded
//       r = N / t
//       s = t / N  = 1 / r    mean time interval between consecutive events
//
//    for an exponential distribution of the time differences d between
//    consecutive events:
//
//       s = <d>
//       sigma(d)     = <d> = s
//       delta(s)     = sigma(d) /sqrt(N) = s / sqrt(N)
//       delta(s) / s = 1 / sqrt(N)
//
//       delta(r) / r = delta(s) / s = 1 / sqrt(N)
//
/////////////////////////////////////////////////////////////////////////////
#include "MEventRate.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MEventRate);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MEventRate::MEventRate(const char *name, const char *title) : fRate(-1)
{
    fName  = name  ? name  : "MEventRate";
    fTitle = title ? title : "Storage container for the event rate [Hz]";
}

Double_t MEventRate::GetError() const
{
    return TMath::Sqrt(1./fNumEvents);
}
