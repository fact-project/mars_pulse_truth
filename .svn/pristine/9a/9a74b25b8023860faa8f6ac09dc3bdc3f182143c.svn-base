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
!   Author(s): Thomas Bretz  04/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MDataValue
//
//   An MData object which corresponds to a simple value like 5.5, or 7.9
//   it can be set as variable object by giving it an index in the
//   constructor, only in this case SetVariables takes action
//
/////////////////////////////////////////////////////////////////////////////
#include "MDataValue.h"

#include <TArrayD.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MDataValue);

using namespace std;

// --------------------------------------------------------------------------
//
// Return the value as a string
//
TString MDataValue::GetRule() const
{
    if (fIndex>=0)
    {
        TString str("[");
        str += fIndex;
        str += "]";
        return str;
    }

    TString str;
    str += fValue;
    return str.Strip(TString::kBoth);
}

void MDataValue::SetVariables(const TArrayD &arr)
{
    if (fIndex>=0 && fIndex<arr.GetSize())
        fValue = arr[fIndex];
}
