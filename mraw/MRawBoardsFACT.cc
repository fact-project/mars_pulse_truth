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
!   Author(s): Thomas Bretz 12/2000 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2012
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MRawBoardsFACT
//
//  This container stores the information about one crate. A list of this
//  informations can be find at MRawCrateArray
//
/////////////////////////////////////////////////////////////////////////////
#include "MRawBoardsFACT.h"

//#include "MLog.h"
//#include "MLogManip.h"

ClassImp(MRawBoardsFACT);

using namespace std;

MRawBoardsFACT::MRawBoardsFACT()
{
    fName = "MRawBoardsFACT";

    memset(fFadTime, 0, 160);
}

// --------------------------------------------------------------------------
//
//  print all stored information to gLog
//
void MRawBoardsFACT::Print(Option_t *t) const
{
}
