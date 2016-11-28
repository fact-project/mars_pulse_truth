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
// MArrayD
//
// Array of Double_t. It is almost the same than TArrayD but derives from
// TObject
//
// Another advantage is: operator[] has no range check!
//
//////////////////////////////////////////////////////////////////////////////
#include "MArrayD.h"
#include "TArrayD.h"

ClassImp(MArrayD);

// --------------------------------------------------------------------------
//
//  Cuts the last entries of an array containing only zeros.
//
void MArrayD::StripZeros(TArrayD &arr)
{

  const Int_t n = arr.GetSize();
  
  for (Int_t i=n-1; i>=0; i--)
    if (arr[i] != 0)
        {
          arr.Set(i+1);
          break;
        }
}

// --------------------------------------------------------------------------
//
//  Cuts the last entries of an array containing only zeros.
//
void MArrayD::StripZeros()
{
    const Int_t n = GetSize();

    for (Int_t i=n-1; i>=0; i--)
      if ((*this)[i] != 0)
        {
          Set(i+1);
          break;
        }
}

