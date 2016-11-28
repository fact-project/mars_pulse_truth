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
!   Author(s): Thomas Bretz, 3/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MSrcPosRndm
//
// Input Container:
//   MHSrcPosCam
//
// Output Container:
//   MSrcPosCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MSrcPosRndm.h"

#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MHSrcPosCam.h"

ClassImp(MSrcPosRndm);

using namespace std;

// --------------------------------------------------------------------------
//
//  Search for 'MPointingPos'. Create if not found.
//
Int_t MSrcPosRndm::PreProcess(MParList *plist)
{
    fGeom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "ERROR - MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fSrcPos = (MSrcPosCam*)plist->FindCreateObj("MSrcPosCam");
    if (!fSrcPos)
        return kFALSE;

    fSrcPosAnti = (MSrcPosCam*)plist->FindCreateObj("MSrcPosCam", "MSrcPosAnti");
    if (!fSrcPosAnti)
        return kFALSE;

    if (fDistOfSource<0)
    {
        fHist = (MHSrcPosCam*)plist->FindObject("MHSrcPosCam");
        if (!fHist)
        {
            *fLog << inf << "MHSrcPosCam not found... skipping." << endl;
            return kSKIP;
        }

        *fLog << inf << "MHSrcPosCam found... taken to produce a random distribution." << endl;
    }
    else
        *fLog << inf << "Source position will be produced randomly in a distance of " << fDistOfSource << "° from the camera center!" << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  See class description.
//
Int_t MSrcPosRndm::Process()
{
    Axis_t x, y;
    if (fDistOfSource<0)
        fHist->GetHist().GetRandom2(x, y);
    else
    {
        const Double_t phi = gRandom->Uniform(TMath::TwoPi());
        x = TMath::Cos(phi)*fDistOfSource;
        y = TMath::Sin(phi)*fDistOfSource;
    }

    const Double_t f = fGeom->GetConvMm2Deg();

    fSrcPos->SetXY(x/f, y/f);
    if (fDistOfSource>=0)
        fSrcPosAnti->SetXY(-x/f, -y/f);
    return kTRUE;
}
