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
!   Author(s): Thomas Bretz, 04/2016 <mailto:tbretz@physik.rwth-aachen.de>
!
!   Copyright: FACT Software Development, 2000-2016
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MSoftwareTrigger
//
/////////////////////////////////////////////////////////////////////////////
#include "MSoftwareTrigger.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MSoftwareTrigger);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MSoftwareTrigger::MSoftwareTrigger(const char *name, const char *title) : fPatch(-1),
    fBaseline(0), fPosition(0), fAmplitude(0)

{
    fName  = name  ? name  : "MSoftwareTrigger";
    fTitle = title ? title : "Storage container for the software trigger";
}
