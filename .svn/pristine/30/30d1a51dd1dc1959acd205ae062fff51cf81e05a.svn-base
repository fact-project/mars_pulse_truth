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
!   Author(s): Thomas Bretz, 10/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MFRealTimePeriod
//
//  This filter allows the execution of tasks in real time intervals. If
//  a timeout (given in milliseconds in the constructor) was exceeded
//  the return value for IsExpression is set to kTRUE and the timer is
//  reset.
//
/////////////////////////////////////////////////////////////////////////////
#include "MFRealTimePeriod.h"

#include <TTime.h>
#include <TSystem.h>

ClassImp(MFRealTimePeriod);

// --------------------------------------------------------------------------
//
// Check, whether the current time is greater than the stored time plus
// the timeout time. If this is the case the return value of
// IsExpressionTrue is set to kTRUE and the stored time is reset to the
// current time. To get the current time gSystem->Now() is used.
//
Int_t MFRealTimePeriod::Process()
{
    const ULong_t t = (ULong_t)gSystem->Now();

    fResult = t>fTime+fMilliSec;

    if (fResult)
        fTime=t;

    return kTRUE;
}
