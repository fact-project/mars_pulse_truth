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
!   Author(s): Thomas Bretz 06/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomCamFact
//
// This class stores the geometry information of the Fact camera.
//    MGeomCamFact cam;       
//
// The idea is to have always four rectangular shaped pixels in
// a rectangle, but the rectangles orderes such that the structure
// is a closed package (hexagonal) structure. The rectangles are
// created in width and hight such that this hexagonal structure can be
// maintained.
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomCamFact.h"


#include <TMath.h>

#include "MGeomCamDwarf.h"
#include "MGeomRectangle.h"
#include "MGeomPix.h"

ClassImp(MGeomCamFact);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Fact camera
//
MGeomCamFact::MGeomCamFact(const char *name)
    : MGeomCam(0, 4.8887, name, "Geometry information of Fact Camera")
{
    CreateGeometry(MGeomCamDwarf(209.5, 13.2, 4.8887));
}

// --------------------------------------------------------------------------
//
//  Use this to create a camera with a roundish shape and a radius rad in
// millimeter containing the four-pixel centers. The four-pixel will have
// a diameter diameter in millimeters, and a distance dist in meters.
//
MGeomCamFact::MGeomCamFact(Double_t rad, Double_t diameter, Double_t dist, const char *name)
    : MGeomCam(0, dist, name, "Geometry information for a FACT camera")
{
    CreateGeometry(MGeomCamDwarf(rad, diameter, dist));
}

// --------------------------------------------------------------------------
//
//  Use this to create a camera with a hexagonal shape and rings rings.
// The first ring around the central pixel is 1. The four-pixel will have a
// diameter diameter in millimeters, and a distance dist in meters.
//
MGeomCamFact::MGeomCamFact(Int_t rings, Double_t diameter, Double_t dist, const char *name)
    : MGeomCam(0, dist, name, "Geometry information for a FACT camera")
{
    CreateGeometry(MGeomCamDwarf(rings, diameter, dist));
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
Bool_t MGeomCamFact::HitFrame(MQuaternion p, const MQuaternion &u) const
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
// Creates four rectangular pixels within a pixel of a camera with
// hexagonal pixels
//
void MGeomCamFact::CreateGeometry(const MGeomCam &hex)
{
    const Double_t dx = static_cast<MGeomPix&>(hex[0]).GetDx()/4;
    const Double_t dy = static_cast<MGeomPix&>(hex[0]).GetDy()/4;

    MGeomCam cam(hex.GetNumPixels()*4, hex.GetCameraDist());

    for (UInt_t i=0; i<hex.GetNumPixels(); i++)
    {
        const MGeom &pix = hex[i];

        for (int j=0; j<4; j++)
        {
            Double_t x = pix.GetX();
            Double_t y = pix.GetY();

            switch (j)
            {
            case 0:
                x -= dx;
                y -= dy;
                break;
            case 1:
                x += dx;
                y -= dy;
                break;
            case 2:
                x -= dx;
                y += dy;
                break;
            case 3:
                x += dx;
                y += dy;
                break;
            }

            cam.SetAt(i*4+j, MGeomRectangle(x, y, dx*2, dy*2));
        }
    }

    cam.InitGeometry();
    cam.Copy(*this);
}
