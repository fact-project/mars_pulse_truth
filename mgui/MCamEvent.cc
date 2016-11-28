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
!   Author(s): Thomas Bretz, 6/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MCamEvent
//
// A base class describing an event in the camera.
//
// If GetPixelContent returns kFALSE, it must not touch 'val'.
//
// You can derive a class in addition to TObject from MCamEvent, too.
//
// MCamEvent MUST be after TObject:
//  ALLOWED:   class MyClass : public TObject, public MCamEvent
//  FORBIDDEN: class MyClass : public MCamEvent, public TObject
//
//////////////////////////////////////////////////////////////////////////////
#include "MCamEvent.h"

#include <TMath.h>

#include "MGeomCam.h"
#include "MArrayD.h"

ClassImp(MCamEvent);

// --------------------------------------------------------------------------
//
// You can overwrite this function if you want the container to be
// initialized by MGeomApply with the geometry. If it is not overloaded
// it calls InitSize with the corresponding pixel number. If this information
// is enough for you it is enough to overload InitSize.
//
void MCamEvent::Init(const MGeomCam &geom)
{
    InitSize(geom.GetNumPixels());
}

// --------------------------------------------------------------------------
//
// Return the median of all corresponding GetPixelContent
//
Double_t MCamEvent::GetCameraMedian(const MGeomCam &cam, Int_t type) const
{
    Int_t num = 0;

    MArrayD arr(cam.GetNumPixels());
    for (unsigned int i=0; i<cam.GetNumPixels(); i++)
    {
        Double_t val;
        if (!GetPixelContent(val, i, cam, type))
            continue;

        arr[num++] = val;
    }

    return TMath::Median(num, arr.GetArray());
}

// --------------------------------------------------------------------------
//
// Return the mean of all corresponding GetPixelContent
//
Double_t MCamEvent::GetCameraMean(const MGeomCam &cam, Int_t type) const
{
    return GetCameraStat(cam, type)[0];
}

// --------------------------------------------------------------------------
//
// Return the rms of all corresponding GetPixelContent
//
Double_t MCamEvent::GetCameraRMS(const MGeomCam &cam, Int_t type) const
{
    return GetCameraStat(cam, type)[1];
}

// --------------------------------------------------------------------------
//
// Return the rms of all corresponding GetPixelContent
//
TArrayD MCamEvent::GetCameraStat(const MGeomCam &cam, Int_t type) const
{
    Int_t    num = 0;
    Double_t sum = 0;
    Double_t sq  = 0;

    for (unsigned int i=0; i<cam.GetNumPixels(); i++)
    {
        Double_t val;
        if (!GetPixelContent(val, i, cam, type))
            continue;

        sum += val;
        sq  += val;
        num ++;
    }

    if (num==0)
        return 0;

    sum /= num;
    sq  /= num;

    TArrayD stat(2);
    stat[0] = sum;
    stat[1] = TMath::Sqrt(sq-sum*sum);
    return stat;
}
