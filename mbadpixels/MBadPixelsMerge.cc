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
!   Author(s): Thomas Bretz 02/2004 <mailto:tbretz@astro.uni.wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MBadPixelsMerge
//  ===============
//
//  Merges in ReInit two bad pixel containers together:
//   1) The contents of the container given in the constructor is in ReInit
//      merged into MBadPixelsCam from the parameter list (aka run-headers)
//   2) MBadPixelsCam from the parameter list (aka run-headers) is merged
//      into the container given in the constructor. While the contents
//      to which 1) refers are still untouched.
//
//
// An explanation taken from Mantis:
// --------------------------------
// In my eyes everything works a supposed to do. We have different sources
// for bad-pixels, eg from Pedestal calculation, from the calibration
// constant calculation, manual setting and so on. If processing data we
// have to take care of all this different sources. Therefor we have to
// store the bad pixels from this sources (eg. from calibration). In
// addition MBadPixelsCam is read from the file containing the data (once
// per file). Now always after a new (data-)file has been opened the bad
// pixels from (for example) the calibration file have to be merged into
// the container loaded from the (data-)file which is stored in the
// parameter list. Copying the pointer would totally overwrite the pixels
// loaded (automatically by MReadMarsFile) from the data-file. All this is
// done using a copy of the original MBadPixelsCam (fSource). In addition
// fDest is initialized to the pointer given as argument to the
// constructor. To keep track of all bad pixels the instance this pointer
// is pointing to is used to collect all bad pixels used so far.
//
//
// ToDo:
//   - Take a setup file (ReadEnv-implementation) as input
//
//
//  Input Containers:
//   MBadPixelsCam
//
//  Output Containers:
//   MBadPixelsCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MBadPixelsMerge.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MBadPixelsCam.h"

ClassImp(MBadPixelsMerge);

using namespace std;

const TString MBadPixelsMerge::fgDefName  = "MBadPixelsMerge";
const TString MBadPixelsMerge::fgDefTitle = "Merge extra- and intra-loop pixels";

// --------------------------------------------------------------------------
//
// Constructor. A copy of the given MBadPixelsCam is created. This copy
// is - in ReInit - merged into the MBadPixelsCam which is found in the
// parameter list. In addition the pointer is stored and all MBadPixelsCam
// which are processed in ReInit are merged into this container.
//
MBadPixelsMerge::MBadPixelsMerge(MBadPixelsCam *bad, const char *name, const char *title)
    : fDest(bad)
{
    fName  = name  ? name  : fgDefName.Data();
    fTitle = title ? title : fgDefTitle.Data();

    fSource = new MBadPixelsCam(*bad);
}

// --------------------------------------------------------------------------
//
// Delete the copy of the primer MBadPixelsCam
//
MBadPixelsMerge::~MBadPixelsMerge()
{
    delete fSource;
}

// --------------------------------------------------------------------------
//
// 1) Get MBadPixelCam from the parameter list, if it doesn't yet exist,
//    it will be created.
// 2) Merge MBasPixelsCam into the primer container given in the constructor
// 3) Merge the primer container given in the constructor into MBadPixelsCam
//
Bool_t MBadPixelsMerge::ReInit(MParList *pList)
{
    MBadPixelsCam *cam = (MBadPixelsCam*)pList->FindCreateObj("MBadPixelsCam");
    if (!cam)
        return kFALSE;

    fDest->Merge(*cam);
    cam->Merge(*fSource);

    return kTRUE;
}
