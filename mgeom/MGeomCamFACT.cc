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
!   Author(s): Thomas Bretz 08/2010 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomCamFACT
//
// This class stores the geometry information of the final FACT camera.
//    MGeomCamFACT cam;        // Creates the final FACT camera
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomCamFACT.h"

#include <TMath.h>

#include "MGeomPix.h"

ClassImp(MGeomCamFACT);

using namespace std;

// --------------------------------------------------------------------------
//
MGeomCamFACT::MGeomCamFACT(const char *name)
    : MGeomCam(1440, 4.889, name, "Geometry information of the final FACT Camera")
{
    CreateCam();
    InitGeometry();
}

// --------------------------------------------------------------------------
//
MGeomCamFACT::MGeomCamFACT(Double_t dist, const char *name)
    : MGeomCam(1440, dist, name, "Geometry information of the final FACT Camera")
{
    CreateCam();
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
// The units are cm.
//
Bool_t MGeomCamFACT::HitFrame(MQuaternion p, const MQuaternion &u) const
{
    // z is defined from the mirror (0) to the camera (z>0).
    // Thus we just propagate to the focal plane (z=fDist)
    //p -= 1700./u.Z()*u;
    p.PropagateZ(u, GetCameraDist()*100); // m->cm

    // Add 10% to the max radius and convert from mm to cm
    return p.R()<GetMaxRadius()*0.11;//TMath::Abs(p.X())<65 && TMath::Abs(p.Y())<65;
}

// --------------------------------------------------------------------------
//
void MGeomCamFACT::CreateCam()
{
    const static Int_t dir[6] =
    {
        MGeomPix::kDirSE,
        MGeomPix::kDirNE,
        MGeomPix::kDirN,
        MGeomPix::kDirNW,
        MGeomPix::kDirSW,
        MGeomPix::kDirS
    };

    const Int_t    rings    = 23;
    const Double_t diameter = 9.5;

    //
    //  add the first pixel to the list
    //
    const MGeomPix pix0(-0.5*diameter, 0, diameter);
    SetAt(0, pix0);

    Int_t cnt  = 1;
    for (int ring=1; ring<=rings; ring++)
    {
        for (Int_t s=0; s<6; s++)
        {
            for (int i=1; i<=ring; i++)
            {
                Double_t x, y;
                const Double_t d2 = MGeomPix::CalcXY(dir[s], ring, i, x, y);
                if (d2 - x > 395.75)
                    continue;

                const MGeomPix pix((x-0.5)*diameter, -y*diameter, diameter);
                SetAt(cnt++, pix);

                if (cnt!=1416)
                    continue;

                const MGeomPix pix1(6.5*diameter, -y*diameter, diameter);
                const MGeomPix pix2(6.5*diameter,  y*diameter, diameter);
                SetAt(1438, pix1);
                SetAt(1439, pix2);
            }
        }
    }
}
