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
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCameraAUX
//
// Class Version 2:
//  + Float_t fTempCountingHouse1; // [deg C] Temperature sensor in the counting house 1
//  + Float_t fTempCountingHouse2; // [deg C] Temperature sensor in the counting house 2
//  + Float_t fTempCountingHouse3; // [deg C] Temperature sensor in the counting house 3
//
/////////////////////////////////////////////////////////////////////////////
#include "MCameraAUX.h"

ClassImp(MCameraAUX);

using namespace std;
