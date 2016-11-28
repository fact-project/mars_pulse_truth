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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// MInpuStreamID                                                            //
//                                                                          //
// This is a ID which is assigned to tasks and to a task list.              //
// It depends on this ID whether a task is executed by the                  //
// MTaskList::Process member funtion or not.                                //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include "MInputStreamID.h"

ClassImp(MInputStreamID);

// --------------------------------------------------------------------------
//
// (default) constructor
//
MInputStreamID::MInputStreamID(const char *name, const char *title)
{
    fName = name ? name : ClassName();
    fTitle = title;

    fStreamId = "All";
}

