/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz,  1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomCamSquare
//
// This class stores the geometry information of a camera with square
// pixels
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomCamSquare.h"

#include "MMath.h"

#include "MGeomRectangle.h"

ClassImp(MGeomCamSquare);

// --------------------------------------------------------------------------
//
//  Square camera with 144 pixels in a distance of 4.96m with a
// layout of 12x12
//
MGeomCamSquare::MGeomCamSquare(const char *name)
    : MGeomCam(144, 4.96, name, "Geometry information of a square camera")
{
    CreateCam(12, 12, 7);
    InitGeometry();
}

// --------------------------------------------------------------------------
//
//  For geometry and Next Neighbor info see
//  CreateCam
//
MGeomCamSquare::MGeomCamSquare(Short_t x, Short_t y, Double_t diameter, Double_t dist, const char *name)
    : MGeomCam(x*y, dist, name, "Geometry information of a square camera")
{
    CreateCam(x, y, diameter);
    InitGeometry();
}

// --------------------------------------------------------------------------
//
// Check if the photon which is flying along the trajectory u has passed
// (or will pass) the frame of the camera (and consequently get
// absorbed). The position p and direction u must be in the
// telescope coordinate frame, which is z parallel to the focal plane,
// x to the right and y upwards, looking from the mirror towards the camera.
//
Bool_t MGeomCamSquare::HitFrame(MQuaternion p, const MQuaternion &u) const
{
    // z is defined from the mirror (0) to the camera (z>0).
    // Thus we just propagate to the focal plane (z=fDist)
    //p -= 1700./u.Z()*u;
    p.PropagateZ(u, GetCameraDist()*100);

    return TMath::Abs(p.X())*10<GetMaxRadius() && TMath::Abs(p.Y())*10<GetMaxRadius();
}

// --------------------------------------------------------------------------
//
//  This fills the geometry information from a table into the pixel objects.
//
void MGeomCamSquare::CreateCam(Short_t nx, Short_t ny, Double_t diameter)
{
    for (Short_t x=0; x<nx; x++)
        for (Short_t y=0; y<ny; y++)
            SetAt(x*ny+y, MGeomRectangle((0.5*nx-x-0.5)*diameter, (0.5*ny-y-0.5)*diameter, diameter));
}
