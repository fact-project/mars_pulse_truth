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
!   Author(s): Thomas Bretz, 3/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MGeomRectangle
//
// This container describes the geometry of a rectangualr shaped pixel
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomRectangle.h"

#include <TBox.h>
#include <TMath.h>
#include <TVirtualPad.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MGeomRectangle);

using namespace std;

// --------------------------------------------------------------------------
//
// Initializes one pixel
//
MGeomRectangle::MGeomRectangle(Float_t x, Float_t y, Float_t w, Float_t h, UInt_t s, UInt_t a)
    : MGeom(x, y, s, a)
{
    //  default constructor
    SetSize(w, h<0 ? w : h);
    SetNeighbors();
}

// ------------------------------------------------------------------------
//
// compute the distance of a point (px,py) to the Hexagon center in
// MGeomPix coordinates. Return kTRUE if inside.
//
Bool_t MGeomRectangle::IsInside(Float_t px, Float_t py) const
{
    if (TMath::Abs(px-fX)>fW/2)
        return kFALSE;

    if (TMath::Abs(py-fY)>fH/2)
        return kFALSE;

    return kTRUE;
}

// ------------------------------------------------------------------------
//
// Implementation of PaintPrimitive drwaing a rectangular pixel
//
void MGeomRectangle::PaintPrimitive(const TAttLine &line, const TAttFill &fill, Double_t scalexy, Double_t scaled) const
{
    const Double_t w = fW*scaled/2;
    const Double_t h = fH*scaled/2;
    const Double_t x = fX*scalexy;
    const Double_t y = fY*scalexy;

    const_cast<TAttLine&>(line).Modify();  //Change line attributes only if necessary
    const_cast<TAttFill&>(fill).Modify();  //Change fill area attributes only if necessary

#if ROOT_VERSION_CODE < ROOT_VERSION(5,18,00)
    if (fill.GetFillColor())
        gPad->PaintBox(x-w, y-h, x+w, y+h);
    if (line.GetLineStyle())
        gPad->PaintBox(x-w, y-h, x+w, y+h, "s");
#else
    gPad->PaintBox(x-w, y-h, x+w, y+h, "l");
#endif
}

// ------------------------------------------------------------------------
//
// Return the distance from the center to one of the two opposite edges.
//
Float_t MGeomRectangle::GetT() const
{
    return TMath::Hypot(fW, fH)/2;
}

// ------------------------------------------------------------------------
//
// Return the average length of the sides
//
Float_t MGeomRectangle::GetL() const
{
    return (fW+fH)/2;
}

// --------------------------------------------------------------------------
//
// Print the geometry information of one pixel.
//
void MGeomRectangle::Print(Option_t *opt) const
{
    MGeom::Print(opt);
    gLog << " w=" << fW << "mm h=" << fH << "mm" << endl;
}
