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
!   Author(s): Thomas Bretz 8/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MSrcPosFromModel
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MSrcPosFromModel.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MRawRunHeader.h"
#include "MPointing.h"
#include "MSrcPosCam.h"
#include "MAstro.h"
#include "MAstroCatalog.h" // MVector3
#include "MAstroSky2Local.h"
#include "MPointingPos.h"
#include "MGeomCam.h"
#include "MReportDrive.h"

ClassImp(MSrcPosFromModel);

using namespace std;

// --------------------------------------------------------------------------
//
// Initialize fY and fY with 0
//
MSrcPosFromModel::MSrcPosFromModel(const char *name, const char *title)
{
    fName  = name  ? name  : "MSrcPosFromModel";
    fTitle = title ? title : "";

    fPoint0401 = new MPointing("bending-fit.txt");
    fPoint0405 = new MPointing("bending0405.txt");
}

MSrcPosFromModel::~MSrcPosFromModel()
{
    delete fPoint0401;
    delete fPoint0405;
}

// --------------------------------------------------------------------------
//
// Search and if necessary create MSrcPosCam in the parameter list
//
Int_t MSrcPosFromModel::PreProcess(MParList *pList)
{
    fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fPointPos = (MPointingPos*)pList->FindObject("MPointingPos");
    if (!fPointPos)
    {
        *fLog << err << "MPointPos not found... aborting." << endl;
        return kFALSE;
    }

    fRun = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRun)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fObservatory = (MObservatory*)pList->FindObject("MObservatory");
    if (!fObservatory)
    {
        *fLog << err << "MObservatory not found... aborting." << endl;
        return kFALSE;
    }

    fTime = (MTime*)pList->FindObject("MTime");
    if (!fTime)
    {
        *fLog << err << "MTime not found... aborting." << endl;
        return kFALSE;
    }

    fSrcPos = (MSrcPosCam*)pList->FindCreateObj("MSrcPosCam");
    if (!fSrcPos)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Derotate fX/fY by the current rotation angle, set MSrcPosCam
//
Int_t MSrcPosFromModel::Process()
{
    MPointing *conv = 0;

    if (!conv && fRun->GetRunNumber()<26237)
        conv = fPoint0401;
    if (!conv && fRun->GetRunNumber()<31684)
        conv = fPoint0405;

    TVector2 d;

    if (conv)
    {
        MVector3 sky;
        sky.SetRaDec(fPointPos->GetRaRad(), fPointPos->GetDecRad());

        // Get current star position (off center)
        MVector3 vl2 = MAstroSky2Local(*fTime, *fObservatory)*sky;
        // Get corrected position camera center)
        TVector3 vl1 = conv->Correct(vl2);

        // Calculate x,y of za2
        d = MAstro::GetDistOnPlain(vl1, vl2, fGeom->GetCameraDist()*1000);
    }

    // Set Source position
    fSrcPos->SetXY(d);

    return kTRUE;
}
