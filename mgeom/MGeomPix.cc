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
// MGeomPix
//
// This container stores the geometry (position) information of
// a single pixel together with the information about next neighbors.
//
//
// Version 1:
// ----------
//  - first implementation
//
// Version 2:
// ----------
//  - added fA
//
// Version 3:
// ----------
//  - added fAidx
//
// Version 4:
// ----------
//  - added fUserBits
//
// Version 5:
// ----------
//  - now derives from MGeom to which some data members have been moved
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomPix.h"

#include <TMath.h>
#include <TVector2.h>
#include <TVirtualPad.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MHexagon.h"

ClassImp(MGeomPix);

using namespace std;

const Float_t MGeomPix::gsTan30 = TMath::Sqrt(3)/3;  //TMath::Tan(TMath::DegToRad()*30);
const Float_t MGeomPix::gsTan60 = TMath::Sqrt(3);    //TMath::Tan(TMath::DegToRad()*60);

const Float_t MGeomPix::gsCos60 = 0.5;               //TMath::Cos(TMath::DegToRad()*60);
const Float_t MGeomPix::gsSin60 = TMath::Sqrt(3)/2;  //TMath::Sin(TMath::DegToRad()*60);

// --------------------------------------------------------------------------
//
// Initializes one pixel
//
MGeomPix::MGeomPix(Float_t x, Float_t y, Float_t r, UInt_t s, UInt_t a)
    : MGeom(x, y, s, a), fCosPhi(1), fSinPhi(0)
{
    SetD(r);
    SetNeighbors();
}

void MGeomPix::SetPhi(Double_t phi)
{
    fCosPhi = TMath::Cos(phi);
    fSinPhi = TMath::Sin(phi);
}

// --------------------------------------------------------------------------
//
// Print the geometry information of one pixel.
//
void MGeomPix::Print(Option_t *opt) const
{
    MGeom::Print(opt);
    gLog << " d=" << fD << "mm" << endl;
}

// ------------------------------------------------------------------------
//
// Implementation of PaintPrimitive drwaing a hexagonal pixel
//
void MGeomPix::PaintPrimitive(const TAttLine &line, const TAttFill &fill, Double_t scalexy, Double_t scaled) const
{
    const_cast<TAttLine&>(line).Modify();  //Change line attributes only if necessary
    const_cast<TAttFill&>(fill).Modify();  //Change fill area attributes only if necessary

    const Double_t fx       = fX*scalexy;
    const Double_t fy       = fY*scalexy;
    const Double_t fd       = fD*scaled;

    const Double_t c        = fd*fCosPhi;
    const Double_t s        = fd*fSinPhi;

    const Double_t gsCos60c = gsCos60*c;
    const Double_t gsCos60s = gsCos60*s;

    const Double_t gsSin60c = gsSin60*c/3;
    const Double_t gsSin60s = gsSin60*s/3;

    const Double_t gsTan30c = gsTan30*c;
    const Double_t gsTan30s = gsTan30*s;


    Double_t x[7] = {
         gsCos60c-gsSin60s,  -gsTan30s,  -gsCos60c-gsSin60s,
        -gsCos60c+gsSin60s,   gsTan30s,   gsCos60c+gsSin60s
    };

    Double_t y[7] = {
         gsCos60s+gsSin60c,   gsTan30c,  -gsCos60s+gsSin60c,
        -gsCos60s-gsSin60c,  -gsTan30c,   gsCos60s-gsSin60c,
    };

    for (Int_t i=0; i<6; i++)
    {
        // FIXME: Do not rotate fX/fY.
        //        Instead implement MGeomCam::Rotate or similar.
        x[i] += fx;
        y[i] += fy;
    }

    x[6] = x[0];
    y[6] = y[0];

    //
    //   paint the pixel
    //
    if (fill.GetFillColor())
        gPad->PaintFillArea(6, x, y);

    if (line.GetLineStyle())
        gPad->PaintPolyLine(7, x, y);
}

// ------------------------------------------------------------------------
//
// compute the distance of a point (px,py) to the Hexagon center in
// MGeomPix coordinates. Return kTRUE if inside.
//
Bool_t MGeomPix::IsInside(Float_t px, Float_t py) const
{

    //const Double_t disthex = 999999;//TMath::Sqrt(dx*dx + dy*dy);

    const Double_t dh = fD/2;

    //
    //  compute the distance of the Point to the center of the Hexagon
    //
    const Double_t dx = px-fX;
    const Double_t dy = py-fY;

    // FIXME: Derotate

    // DX = dx*cos(-phi) - dy*sin(-phi)
    // DY = dx*sin(-phi) + dy*cos(-phi)

    const Double_t x =  dx*fCosPhi + dy*fSinPhi;

    if (TMath::Abs(x) > dh)
        return kFALSE; // return disthex

    const Double_t y = -dx*fSinPhi + dy*fCosPhi;

    const Double_t xc = x*gsCos60;  // 1/2
    const Double_t ys = y*gsSin60;  // sqrt(3)/2

    //
    // Now check if point is outside of hexagon; just check x coordinate
    // in three coordinate systems: the default one, in which two sides of
    // the hexagon are paralel to the y axis (see camera displays) and two 
    // more, rotated with respect to that one by +- 60 degrees.
    //

    if  (TMath::Abs(xc + ys) > dh)
        return kFALSE; // return disthex

    if  (TMath::Abs(xc - ys) > dh)
        return kFALSE; // return disthex

    return kTRUE; // return -1


}

// --------------------------------------------------------------------------
//
// Calculate in the direction 0-5 (kind of sector) in the ring-th ring
// the x and y coordinate of the i-th pixel. The unitx are unity,
// distance to (0,0) is retruned.
//
Double_t MGeomPix::CalcXY(Int_t dir, Int_t ring, Int_t i, Double_t &x, Double_t &y)
{
    switch (dir)
    {
    case kDirCenter: // Center
        x = 0;
        y = 0;
        break;

    case kDirNE: // Direction North East
        x = ring-i*0.5;
        y = i*gsSin60;
        break;

    case kDirN: // Direction North
        x = ring*0.5-i;
        y = ring*gsSin60;
        break;

    case kDirNW: // Direction North West
        x = -(ring+i)*0.5;
        y = (ring-i)*gsSin60;
        break;

    case kDirSW: // Direction South West
         x = 0.5*i-ring;
         y = -i*gsSin60;
         break;

    case kDirS: // Direction South
         x = i-ring*0.5;
         y = -ring*gsSin60;
         break;

    case kDirSE: // Direction South East
        x = (ring+i)*0.5;
        y = (-ring+i)*gsSin60;
        break;
    }
    return x*x + y*y;
}

// ==============================================================================
/*
#include <TOrdCollection.h>
#include "MMath.h" 
#include "../mgui/MHexagon.h" // MHexagon::fgDx

// ------------------------------------------------------------------------
//
// Small helper class to allow fast sorting of TVector2 by angle
//
class HVector2 : public TVector2
{
    Double_t fAngle;
public:
    HVector2() : TVector2()  { }
    HVector2(const TVector2 &v) : TVector2(v)  { }
    HVector2(Double_t x, Double_t y) : TVector2 (x, y) { }
    void CalcAngle() { fAngle = Phi(); }
    Bool_t IsSortable() const { return kTRUE; }
    Int_t Compare(const TObject *obj) const { return ((HVector2*)obj)->fAngle>fAngle ? 1 : -1; }
    Double_t GetAngle() const { return fAngle; }
};

// ------------------------------------------------------------------------
//
// Calculate the edge points of the intersection area of two hexagons.
// The points are added as TVector2 to the TOrdCollection.
// The user is responsible of delete the objects.
//
void MGeomPix::GetIntersectionBorder(TOrdCollection &col, const MGeomPix &hex) const
{
//    if (fPhi!=0)
//    {
//        gLog << warn << "MGeomPix::GetIntersectionBorder: Only phi=0 supported yet." << endl;
//        return;
//    }

    Bool_t inside0[6], inside1[6];

    HVector2 v0[6];
    HVector2 v1[6];

    Int_t cnt0=0;
    Int_t cnt1=0;

    // Calculate teh edges of each hexagon and whether this edge
    // is inside the other hexgon or not
    for (int i=0; i<6; i++)
    {
        const Double_t x0 = fX+MHexagon::fgDx[i]*fD;
        const Double_t y0 = fY+MHexagon::fgDy[i]*fD;

        const Double_t x1 = hex.fX+MHexagon::fgDx[i]*hex.fD;
        const Double_t y1 = hex.fY+MHexagon::fgDy[i]*hex.fD;

        v0[i].Set(x0, y0);
        v1[i].Set(x1, y1);

        inside0[i] = hex.DistanceToPrimitive(x0, y0) < 0;
        inside1[i] = DistanceToPrimitive(x1, y1)     < 0;

        if (inside0[i])
        {
            col.Add(new HVector2(v0[i]));
            cnt0++;
        }
        if (inside1[i])
        {
            col.Add(new HVector2(v1[i]));
            cnt1++;
        }
    }

    if (cnt0==0 || cnt1==0)
        return;

    // No calculate which vorder lines intersect
    Bool_t iscross0[6], iscross1[6];
    for (int i=0; i<6; i++)
    {
        iscross0[i] = (inside0[i] && !inside0[(i+1)%6]) || (!inside0[i] && inside0[(i+1)%6]);
        iscross1[i] = (inside1[i] && !inside1[(i+1)%6]) || (!inside1[i] && inside1[(i+1)%6]);
    }

    // Calculate the border points of our intersection area
    for (int i=0; i<6; i++)
    {
        // Skip non intersecting lines
        if (!iscross0[i])
            continue;

        for (int j=0; j<6; j++)
        {
            // Skip non intersecting lines
            if (!iscross1[j])
                continue;

            const TVector2 p = MMath::GetIntersectionPoint(v0[i], v0[(i+1)%6], v1[j], v1[(j+1)%6]);
            if (hex.DistanceToPrimitive(p.X(), p.Y())<1e-9)
                col.Add(new HVector2(p));
        }
    }
}

// ------------------------------------------------------------------------
//
// Calculate the overlapping area of the two hexagons.
//
Double_t MGeomPix::CalcOverlapArea(const MGeomPix &cam) const
{
//    if (fPhi!=0)
//    {
//        gLog << warn << "MGeomPix::CalcOverlapArea: Only phi=0 supported yet." << endl;
//        return -1;
//    }

    TOrdCollection col;
    col.SetOwner();

    GetIntersectionBorder(col, cam);

    // Check if there is an intersection to proceed with
    const Int_t n = col.GetEntries();
    if (n==0)
        return 0;

    // Calculate the center of gravity
    TVector2 cog;

    TIter Next(&col);
    HVector2 *v=0;
    while ((v=(HVector2*)Next()))
        cog += *v;
    cog /= n;

    // Shift the figure to its center-og-gravity and
    // calculate the angle of the connection line between the
    // border points of our intersesction area and its cog
    Next.Reset();
    while ((v=(HVector2*)Next()))
    {
        *v -= cog;
        v->CalcAngle();
    }

    // Sort these points by this angle
    col.Sort();

    // Now sum up the area of all the triangles between two
    // following points and the cog.
    Double_t A = 0;
    for (int i=0; i<n; i++)
    {
        // Vectors from cog to two nearby border points
        const HVector2 *v1 = (HVector2*)col.At(i);
        const HVector2 *v2 = (HVector2*)col.At((i+1)%n);

        // Angle between both vectors
        const Double_t a = fmod(v1->GetAngle()-v2->GetAngle()+TMath::TwoPi(), TMath::TwoPi());

        // Length of both vectors
        const Double_t d1 = v1->Mod();
        const Double_t d2 = v2->Mod();

        A += d1*d2/2*sin(a);
    }
    return A;
}
*/
