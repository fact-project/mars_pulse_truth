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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */


//////////////////////////////////////////////////////////////////////////////
//
// MArrayI
//
// Array of Int_t. It is almost the same than TArrayI but derives from
// TObject
//
// Another advantage is: operator[] has no range check!
//
//////////////////////////////////////////////////////////////////////////////
#include "MArrayI.h"

#include "MMath.h"

ClassImp(MArrayI);

void MArrayI::ReSort(Bool_t down)
{
    MMath::ReSort(fN, fArray, down);
}
