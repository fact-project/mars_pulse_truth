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
!   Author(s): Thomas Bretz 03/2007 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Michael Backes 03/2007 <mailto:michael.backes@udo.edu>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomCamDwarf
//
// This class stores the geometry information of the Dwarf camera.
//    MGeomCamDwarf cam;        // Creates the 313 pixel dwarf camera
//
// It can also be used to create a hexagonal camera with identical sized
// pixels and n rings (while the central pixel is counted as ring 0).
//    MGeomCamDwarf cam(9, 21); // Creates the CT3 camera
//
// Or it can be used to create a roundish camera, similar to a
// hexagonal camera, but the edges filled with additional pixels
// inside a circle.
//    MGeomCamDwarf cam(209.5, 13.2);
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomCamDwarf.h"

#include <TMath.h>

#include "MGeomPix.h"

ClassImp(MGeomCamDwarf);

using namespace std;

// --------------------------------------------------------------------------
//
//  Dwarf camera has 313 pixels. For geometry and Next Neighbor info see
//  CreateCam and CreateNN
//
MGeomCamDwarf::MGeomCamDwarf(const char *name)
    : MGeomCam(CalcNumPix(9.5), 4.57, name, "Geometry information of Dwarf Camera")
{
    CreateCam(21, 9.5);
    InitGeometry();
}

// --------------------------------------------------------------------------
//
//  Use this to create a camera with a roundish shape and a radius rad in
// millimeter containing the pixel centers. The pixel will have a diameter
// diameter in millimeters, and a distance dist in meters.
//
MGeomCamDwarf::MGeomCamDwarf(Double_t rad, Double_t diameter, Double_t dist, const char *name)
    : MGeomCam(CalcNumPix(diameter<=0 ? rad : rad/diameter), dist, name, "Geometry information for a roundish camera")
{
    CreateCam(diameter, diameter<=0 ? rad : rad/diameter);
    InitGeometry();
}

// --------------------------------------------------------------------------
//
//  Use this to create a camera with a hexagonal shape and rings rings.
// The first ring around the central pixel is 1. The pixel will have a
// diameter diameter in millimeters, and a distance dist in meters.
//
MGeomCamDwarf::MGeomCamDwarf(Int_t rings, Double_t diameter, Double_t dist, const char *name)
    : MGeomCam(CalcNumPix(rings), dist, name, "Geometry information for a hexagonal camera")
{
    CreateCam(diameter, rings);
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
Bool_t MGeomCamDwarf::HitFrame(MQuaternion p, const MQuaternion &u) const
{
    // Add 10% to the max radius and convert from mm to cm
    // FIXME: Why does the compiler complain without this cast?
    return static_cast<const MGeomCam*>(this)->HitFrame(p, u, GetMaxRadius()*0.11);
}

// --------------------------------------------------------------------------
//
// Calculate in the direction 0-5 (kind of sector) in the ring-th ring
// the x and y coordinate of the i-th pixel. The units are unity,
// distance to (0,0) is retruned.
//
Int_t MGeomCamDwarf::CalcNumPix(Int_t rings)
{
    //
    //  add the first pixel to the list
    //
    Int_t cnt = 1;

    for (Int_t ring=0; ring<rings; ring++)
        cnt += 6*(ring+1);

    return cnt;
}

// --------------------------------------------------------------------------
//
// Calculate in the direction 0-5 (kind of sector) in the ring-th ring
// the x and y coordinate of the i-th pixel. The unitx are unity,
// distance to (0,0) is retruned.
//
// Due to possible rounding errors we need to use exactly the same
// algorithm as for creating the pixels!
//
Int_t MGeomCamDwarf::CalcNumPix(Double_t rad)
{
    const Double_t r2 = rad*rad;

    //
    //  add the first pixel to the list
    //
    Int_t cnt  = 1;
    Int_t ring = 1;
    while (1)
    {
        Int_t n = 0;

        //
        // calc. coords for this ring counting from the
        // starting number to the ending number
        //
        for (Int_t dir=MGeomPix::kDirNE; dir<=MGeomPix::kDirSE; dir++)
        {
            for (int i=0; i<ring; i++)
            {
                Double_t x, y;
                if (MGeomPix::CalcXY(dir, ring, i, x, y)<r2)
                    n++;
            }
        }

        if (n==0)
            return cnt;

        ring++;
        cnt += n;
    }

    return cnt;
}


// --------------------------------------------------------------------------
//
//  This fills the geometry information for a hexagonal camera
//
void MGeomCamDwarf::CreateCam(Double_t diameter, Int_t rings)
{
    //    units for diameter are mm

    //
    //  add the first pixel to the list
    //
    SetAt(0, MGeomPix(0, 0, diameter));

    Int_t cnt  = 1;

    for (int ring=1; ring<=rings; ring++)
    {
        for (Int_t dir=MGeomPix::kDirNE; dir<=MGeomPix::kDirSE; dir++)
        {
            for (int i=0; i<ring; i++)
            {
                Double_t x, y;
                MGeomPix::CalcXY(dir, ring, i, x, y);
                SetAt(cnt++, MGeomPix(x*diameter, y*diameter, diameter));
            }
        }
    }
}

// --------------------------------------------------------------------------
//
//  This fills the geometry information for a roundish camera
//
void MGeomCamDwarf::CreateCam(Double_t diameter, Double_t rad)
{
    //    units for diameter are mm

    const Double_t r2 = rad*rad;

    //
    //  add the first pixel to the list
    //
    SetAt(0, MGeomPix(0, 0, diameter));

    Int_t cnt  = 1;
    Int_t ring = 1;

    while (1)
    {
        Int_t n = 0;

        for (Int_t dir=MGeomPix::kDirNE; dir<=MGeomPix::kDirSE; dir++)
        {
            for (int i=0; i<ring; i++)
            {
                Double_t x, y;
                if (MGeomPix::CalcXY(dir, ring, i, x, y)<r2)
                    SetAt(cnt+n++, MGeomPix(x*diameter, y*diameter, diameter));
            }
        }

        if (n==0)
            return;

        ring++;
        cnt += n;
    }
}
