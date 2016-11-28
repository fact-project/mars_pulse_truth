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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MGeom
//
// This container stores the geometry (position) information of
// a single pixel together with the information about next neighbors.
//
//
// Version 1:
// ----------
//  - first implementation (previously part of MGeomPix)
//
////////////////////////////////////////////////////////////////////////////
#include "MGeom.h"

#include <TMath.h>
#include <TVector2.h>
#include <TVirtualPad.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"

using namespace std;

// --------------------------------------------------------------------------
//
// Initializes one pixel
//
MGeom::MGeom(Float_t x, Float_t y, UInt_t s, UInt_t a)
    : fX(x), fY(y), fA(0), fSector(s), fAidx(a), fUserBits(0)
{
    //  default constructor
    SetNeighbors();
}

// --------------------------------------------------------------------------
//
// Return position as TVector2(fX, fY)
//
TVector2 MGeom::GetP() const
{
    return TVector2(fX, fY);
}

void MGeom::SetP(const TVector2 &v)
{
    fX = v.X();
    fY = v.Y();
}

// --------------------------------------------------------------------------
//
// Initializes Next Neighbors.
//
// WARNING: This function is public, but it is not meant for user access.
// It should only be used from geometry classes (like MGeomCam)
//
void MGeom::SetNeighbors(Short_t i0, Short_t i1, Short_t i2,
                         Short_t i3, Short_t i4, Short_t i5)
{
    fNeighbors[0] = i0;
    fNeighbors[1] = i1;
    fNeighbors[2] = i2;
    fNeighbors[3] = i3;
    fNeighbors[4] = i4;
    fNeighbors[5] = i5;

    int i;
    for (i=0; i<6; i++)
        if (fNeighbors[i]<0)
            break;

    fNumNeighbors = i;

    // FIXME
    fNumNeighbors<5 ? SETBIT(fUserBits, kIsInOutermostRing) : CLRBIT(fUserBits, kIsInOutermostRing);
}

// --------------------------------------------------------------------------
//
//  Set the kIsOuterRing flag if this pixel has a outermost pixel
//  as Next Neighbor and don't have the kIsOutermostRing flag itself.
//
void MGeom::CheckOuterRing(const MGeomCam &cam)
{
    if (IsInOutermostRing())
        return;

    CLRBIT(fUserBits, kIsInOuterRing);

    for (int i=0; i<fNumNeighbors; i++)
        if (cam[fNeighbors[i]].IsInOutermostRing())
        {
            SETBIT(fUserBits, kIsInOuterRing);
            return;
        }
}

// --------------------------------------------------------------------------
//
// Print the geometry information of one pixel.
//
void MGeom::Print(Option_t *opt) const
{
    //   information about a pixel
    gLog << all << MParContainer::GetDescriptor(*this) << ":  x/y=" << fX << "/" << fY << "mm ";
    gLog << "A= " << fA << "mm" << UTF8::kSquare << " (";
    for (int i=0; i<fNumNeighbors; i++)
        gLog << fNeighbors[i] << (i<fNumNeighbors-1?",":"");
    gLog << ")";
}


// ------------------------------------------------------------------------
//
// Return distance of center to coordinate origin: hypot(fX,fY)
//
Float_t MGeom::GetDist() const
{
    return TMath::Hypot(fX, fY);
}

// ------------------------------------------------------------------------
//
// Return distance of center to center of pix: hypot(fX-pix.fX,fY-pix.fY)
//
Float_t MGeom::GetDist(const MGeom &pix) const
{
    return TMath::Hypot(fX-pix.fX, fY-pix.fY);
}

// ------------------------------------------------------------------------
//
// Return distance squared of center to center of pix:
//  (fX-pix.fX)^2+(fY-pix.fY)^2
//
Float_t MGeom::GetDist2(const MGeom &pix) const
{
    const Double_t dx = fX-pix.fX;
    const Double_t dy = fY-pix.fY;
    return dx*dx + dy*dy;
}

// ------------------------------------------------------------------------
//
// Return angle defined by the center and the center of pix:
//  atan2(fX-pix.fX,fY-pix.fY)
//
Float_t MGeom::GetAngle(const MGeom &pix) const
{
    return TMath::ATan2(fX - pix.GetX(), fY - pix.GetY());
}

// ------------------------------------------------------------------------
//
// Return the direction of the pixel pix w.r.t. this pixel.
// Remark: This function assumes a simple geometry.
//
Int_t MGeom::GetDirection(const MGeom &pix) const
{
    const Double_t phi = GetAngle(pix)*TMath::RadToDeg();

    if (phi<-165) return kTop;
    if (phi<-105) return kRightTop;
    if (phi< -75) return kRight;
    if (phi< -15) return kRightBottom;
    if (phi<  15) return kBottom;
    if (phi<  75) return kLeftBottom;
    if (phi< 105) return kLeft;
    if (phi< 165) return kLeftTop;

    return kTop;
}

// ------------------------------------------------------------------------
//
// This implementation is based on the use of DistanceToPrimitive
// (capital T) which should be implemented in user coordinates.
//
Int_t MGeom::DistancetoPrimitive(Int_t px, Int_t py)
{
    const Double_t x = gPad->AbsPixeltoX(px);
    const Double_t y = gPad->AbsPixeltoY(py);

    return IsInside(x, y) ? -1 : 999999;
}

