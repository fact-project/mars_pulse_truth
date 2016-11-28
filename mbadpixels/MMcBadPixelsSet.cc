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
!   Author(s): Oscar Blanch    12/2001 <mailto:blanch@ifae.es>
!   Author(s): Thomas Bretz    08/2002 <mailto:tbretz@astro.uni.wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MMcBadPixelsSet
//
//  This task tries to identify the starfield from the
//  MMcRunHeader container and tries to identifies it. If it is known
//  (eg. Crab) the fixed build in pixel numbers are used as blind
//  pixels.
//
//  Implemented star fields (for the MAGIC camera only):
//   - Crab: 400, 401, 402, 437, 438, 439
//
//  Input Containers:
//   -/-
//
//  Output Containers:
//   MBadPixels
//
/////////////////////////////////////////////////////////////////////////////
#include "MMcBadPixelsSet.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MMcRunHeader.hxx"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

ClassImp(MMcBadPixelsSet);

using namespace std;

static const TString gsDefName  = "MMcBadPixelsSet";
static const TString gsDefTitle = "Set predefined star fields";

// --------------------------------------------------------------------------
//
// Default constructor.
//
MMcBadPixelsSet::MMcBadPixelsSet(const char *name, const char *title)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
Int_t MMcBadPixelsSet::PreProcess (MParList *pList)
{
    fBadPixels = (MBadPixelsCam*)pList->FindCreateObj(AddSerialNumber("MBadPixelsCam"));
    if (!fBadPixels)
        return kFALSE;

    fGeomCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
    if (!fGeomCam)
    {
        *fLog << err << dbginf << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  - Check whether pixels to disable are available. If pixels are
//    given by the user nothing more is done.
//  - Otherwise try to determin the blind pixels from the starfield
//    given in MMcRunHeader.
//
Bool_t MMcBadPixelsSet::ReInit(MParList *pList)
{
    if (!fGeomCam->InheritsFrom("MGeomCamMagic"))
    {
        *fLog << warn << "MMcBadPixelsSet::ReInit: Warning - Starfield only implemented for Magic standard Camera... no action." << endl;
        return kTRUE;
    }

    //
    // Set as blind some particular pixels because of a particular
    // Star Field of View.
    //
    MMcRunHeader *mcrun = (MMcRunHeader*)pList->FindObject("MMcRunHeader");
    if (!mcrun)
    {
        *fLog << warn << "MMcBadPixelsSet::ReInit: Warning - No run header available... no action." << endl;
        return kTRUE;
    }

    Int_t rah, ram, ras;
    Int_t ded, dem, des;
    mcrun->GetStarFieldRa(&rah, &ram, &ras);
    mcrun->GetStarFieldDec(&ded, &dem, &des);

    if (rah!=5 || ram!=34 || ras!=32 || ded!=22 || dem!=0 || des!=55)
    {
        *fLog << warn << "Warning - Starfield unknown..." << endl;
        return kTRUE;
    }

    //
    // Case for Crab Nebula FOV
    //
    (*fBadPixels)[400].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
    (*fBadPixels)[401].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
    (*fBadPixels)[402].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
    (*fBadPixels)[437].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
    (*fBadPixels)[438].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
    (*fBadPixels)[439].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);

    *fLog << inf;
    *fLog << "FOV is centered at CRAB NEBULA: Setting 6 blind pixels" << endl;
    *fLog << "to avoid bias values of analysis due to CRAB NEBULA:" << endl;
    *fLog << "   Pixels: 400, 401, 402, 437, 438, 439" << endl;

    return kTRUE;
}

