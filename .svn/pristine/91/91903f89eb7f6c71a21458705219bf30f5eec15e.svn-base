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
!   Author(s): Thomas Bretz 08/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  MCameraSmooth                                                          //
//                                                                         //
//  This task fills each pixel in the camera with the average of the       //
//  number of cerenkov photons from its surrounding pixels. This can       //
//  be done using the central pixel or ignoring the central pixel.         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MCameraSmooth.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomPix.h"
#include "MGeomCam.h"
#include "MSignalPix.h"
#include "MSignalCam.h"

ClassImp(MCameraSmooth);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. You can specify the numer of loops how often a
// smoothing should be done. The default is 1.
//
MCameraSmooth::MCameraSmooth(Byte_t n, const char *name, const char *title)
    : fCounts(n), fUseCentralPixel(kTRUE)
{
    fName  = name  ? name  : "MCameraSmooth";
    fTitle = title ? title : "Task to smooth the camera";
}

// --------------------------------------------------------------------------
//
//  - get the MSignalCam from the parlist (abort if missing)
//  - get MGeomCam from the parameter list
//
Int_t MCameraSmooth::PreProcess (MParList *pList)
{
    fEvt = (MSignalCam*)pList->FindObject("MSignalCam");
    if (!fEvt)
    {
        *fLog << err << dbginf << "MSignalCam not found... aborting." << endl;
        return kFALSE;
    }

    fGeomCam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fGeomCam)
    {
        *fLog << warn << dbginf << "No camera geometry available... aborting." << endl;

        return kFALSE;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Do the smoothing
//
Int_t MCameraSmooth::Process()
{
    const UShort_t entries = fEvt->GetNumPixels();

    //
    // remove the pixels in fPixelsID if they are set to be used,
    // (set them to 'unused' state)
    //

    Double_t *photons = new Double_t[entries];
    Double_t *errors  = new Double_t[entries];

    for (int n=0; n<fCounts; n++)
    {
        for (UShort_t i=0; i<entries; i++)
        {
            MSignalPix &pix = (*fEvt)[i];

            //const Int_t id = pix.GetPixId();

            const MGeomPix &gpix = (*fGeomCam)[i];

            const Int_t nn = gpix.GetNumNeighbors();

            Int_t num  = fUseCentralPixel ? 1 : 0;
            photons[i] = fUseCentralPixel ? pix.GetNumPhotons() : 0;
            errors[i]  = fUseCentralPixel ? pix.GetErrorPhot()  : 0;

            for (int j=0; j<nn; j++)
            {
                const UShort_t nid = gpix.GetNeighbor(j);

                const MSignalPix &evtpix = (*fEvt)[nid];
                photons[i] += evtpix.GetNumPhotons();
                errors[i]  += evtpix.GetErrorPhot();
                num++;
            }

            photons[i] /= num;
            errors[i]  /= num;
        }

        for (UShort_t i=0; i<entries; i++)
            (*fEvt)[i].Set(photons[i], errors[i]);
    }

    delete photons;
    delete errors;

    return kTRUE;
}

