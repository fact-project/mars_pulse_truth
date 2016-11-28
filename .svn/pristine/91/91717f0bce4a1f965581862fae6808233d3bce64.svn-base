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

//////////////////////////////////////////////////////////////////////////////
//
//  MMirrorHex
//
// A hexagonal spherical mirror
//
//////////////////////////////////////////////////////////////////////////////
#include "MMirrorHex.h"

#include <stdlib.h> // atof (Ubuntu 8.10)

#include <TMath.h>
#include <TObjArray.h>

#include <TEllipse.h>
#include <TVirtualPad.h>

#include "MLog.h"

#include "MHexagon.h"
#include "MQuaternion.h"

ClassImp(MMirrorHex);

using namespace std;

const Double_t MMirrorHex::fgCos30 = TMath::Cos(30*TMath::DegToRad());
const Double_t MMirrorHex::fgCos60 = TMath::Cos(60*TMath::DegToRad());
const Double_t MMirrorHex::fgSin60 = TMath::Sin(60*TMath::DegToRad());

// --------------------------------------------------------------------------
//
// Return the aread of the hexagon: d^2*sin(60)
//
Double_t MMirrorHex::GetA() const
{
    return fD*fD*fgSin60*4;
}

// ------------------------------------------------------------------------
//
// This is a very rough estimate of whether a photon at a position p
// can hit a mirror. The position might be off in z and the photon
// still has to follow its trajectory. Nevertheless we can fairly assume
// the the way to travel in x/y is pretty small so we can give a rather
// good estimate of whether the photon can hit.
//
// never throw away a photon whihc can hit the mirror!
//
Bool_t MMirrorHex::CanHit(const MQuaternion &p) const
{
    // p is given in the reflectors coordinate frame. This is meant
    // to be a fast check to sort out all mirrors which we can omit
    // without time consuming calculations.

    return TMath::Hypot(p.X()-X(), p.Y()-Y())<1.05*fMaxR;
}

// ------------------------------------------------------------------------
//
// Check if the given position coincides with the mirror. The position
// is assumed to be the incident point on the mirror's surface.
//
// The coordinates are in the mirrors coordinate frame.
//
// The action should coincide with what is painted in Paint()
//
Bool_t MMirrorHex::HasHit(const MQuaternion &p) const
{
    // p is the incident point in the mirror in the mirror's
    // coordinate frame

    // Black spot in the mirror center (here we can fairly ignore
    // the distance from the plane to the mirror surface, as long
    // as the black spot does not become too large)
    if (p.R2()<0.5*0.5)
        return kFALSE;

    //
    // Now check if point is outside of hexagon; just check x coordinate
    // in three coordinate systems: the default one, in which two sides of
    // the hexagon are paralel to the y axis (see camera displays) and two 
    // more, rotated with respect to that one by +- 60 degrees.
    //
    if (TMath::Abs(p.X())>fD)
        return kFALSE;

    const Double_t dxc = p.X()*fgCos60;
    const Double_t dys = p.Y()*fgSin60;

    if (TMath::Abs(dxc+dys)>fD)
        return kFALSE;

    if (TMath::Abs(dxc-dys)>fD)
        return kFALSE;

    return kTRUE;
}

// ------------------------------------------------------------------------
//
// Paint the mirror in x/y.
//
// The graphic should coincide with the action in HasHit
//
void MMirrorHex::Paint(Option_t *opt)
{
    MHexagon h;
    TEllipse e;
    h.SetFillColor(18);
    if (!TString(opt).Contains("line", TString::kIgnoreCase))
    {
        h.SetFillColor(17);
        h.SetLineStyle(0);
        e.SetLineStyle(0);
        e.SetFillColor(gPad->GetFillColor());
    }

    if (TString(opt).Contains("same", TString::kIgnoreCase))
    {
        h.SetFillStyle(0);
        e.SetFillStyle(0);
    }

    h.PaintHexagon(X(), Y(), fD*2);

    if (!TString(opt).Contains("border", TString::kIgnoreCase))
        e.PaintEllipse(X(), Y(), 0.5, 0.5, 0, 360, 0);
}

// ------------------------------------------------------------------------
//
// Print the contents of the mirror
//
void MMirrorHex::Print(Option_t *o) const
{
    MMirror::Print(o);
    gLog << " " << fD*2 << endl;
}

// ------------------------------------------------------------------------
//
// Read the mirror's setup from a file. The first eight tokens should be
// ignored. (This could be fixed!)
//
// Here we read: D
//
Int_t MMirrorHex::ReadM(const TObjArray &tok)
{
    if (tok.GetEntries()!=1)
        return -1;

    const Double_t d = atof(tok[0]->GetName());

    if (d<=0)
        return -1;

    SetD(d);

    return 1;
}

// ------------------------------------------------------------------------
//
void MMirrorHex::WriteM(ostream &out) const
{
    out << fD*2;
}

// ------------------------------------------------------------------------
//
// Check if the given position coincides with the mirror. The position
// is assumed to be the incident point on the mirror's surface.
//
// The coordinates are in the mirrors coordinate frame.
//
// The action should coincide with what is painted in Paint()
//
Bool_t MMirrorHex15::HasHit(const MQuaternion &p) const
{
    // p is the incident point in the mirror in the mirror's
    // coordinate frame

    // Black spot in the mirror center (here we can fairly ignore
    // the distance from the plane to the mirror surface, as long
    // as the black spot does not become too large)
//    if (p.R2()<0.5*0.5)
//        return kFALSE;

    static const TVector2 rot(cos(-15*TMath::DegToRad()), sin(-15*TMath::DegToRad()));

    const TVector2 &v = p.XYvector();

    const Double_t x = v^rot;
    const Double_t y = v*rot;

    //
    // Now check if point is outside of hexagon; just check x coordinate
    // in three coordinate systems: the default one, in which two sides of
    // the hexagon are paralel to the y axis (see camera displays) and two 
    // more, rotated with respect to that one by +- 60 degrees.
    //
    if (TMath::Abs(x)>fD)
        return kFALSE;

    const Double_t dxc = x*fgCos60;
    const Double_t dys = y*fgSin60;

    if (TMath::Abs(dxc+dys)>fD)
        return kFALSE;

    if (TMath::Abs(dxc-dys)>fD)
        return kFALSE;

    return kTRUE;
}

// ------------------------------------------------------------------------
//
// Paint the mirror in x/y.
//
// The graphic should coincide with the action in HasHit
//
void MMirrorHex15::Paint(Option_t *opt)
{
    MHexagon h;
    TEllipse e;
    h.SetFillColor(18);
    if (!TString(opt).Contains("line", TString::kIgnoreCase))
    {
        h.SetFillColor(17);
        h.SetLineStyle(0);
        e.SetLineStyle(0);
        e.SetFillColor(gPad->GetFillColor());
    }

    if (TString(opt).Contains("same", TString::kIgnoreCase))
    {
        h.SetFillStyle(0);
        e.SetFillStyle(0);
    }

    h.PaintHexagon(X(), Y(), fD*2, 15*TMath::DegToRad());

    if (!TString(opt).Contains("border", TString::kIgnoreCase))
        e.PaintEllipse(X(), Y(), 0.5, 0.5, 0, 360, 0);
}

// ------------------------------------------------------------------------
//
// Check if the given position coincides with the mirror. The position
// is assumed to be the incident point on the mirror's surface.
//
// The coordinates are in the mirrors coordinate frame.
//
// The action should coincide with what is painted in Paint()
//
Bool_t MMirrorHex90::HasHit(const MQuaternion &p) const
{
    // p is the incident point in the mirror in the mirror's
    // coordinate frame

    // Black spot in the mirror center (here we can fairly ignore
    // the distance from the plane to the mirror surface, as long
    // as the black spot does not become too large)
    if (p.R2()<0.5*0.5)
        return kFALSE;

    //
    // Now check if point is outside of hexagon; just check x coordinate
    // in three coordinate systems: the default one, in which two sides of
    // the hexagon are parallel to the y axis (see camera displays) and two
    // more, rotated with respect to that one by +- 60 degrees.
    //
    if (TMath::Abs(p.Y())>fD)
        return kFALSE;

    const Double_t dxs = p.X()*fgSin60;
    const Double_t dyc = p.Y()*fgCos60;

    if (TMath::Abs(dxs+dyc)>fD)
        return kFALSE;

    if (TMath::Abs(dxs-dyc)>fD)
        return kFALSE;

    return kTRUE;
}

// ------------------------------------------------------------------------
//
// Paint the mirror in x/y.
//
// The graphic should coincide with the action in HasHit
//
void MMirrorHex90::Paint(Option_t *opt)
{
    MHexagon h;
    TEllipse e;
    h.SetFillColor(18);
    if (!TString(opt).Contains("line", TString::kIgnoreCase))
    {
        h.SetFillColor(17);
        h.SetLineStyle(0);
        e.SetLineStyle(0);
        e.SetFillColor(gPad->GetFillColor());
    }

    if (TString(opt).Contains("same", TString::kIgnoreCase))
    {
        h.SetFillStyle(0);
        e.SetFillStyle(0);
    }

    h.PaintHexagon(X(), Y(), fD*2, TMath::Pi()/2);

    if (!TString(opt).Contains("border", TString::kIgnoreCase))
        e.PaintEllipse(X(), Y(), 0.5, 0.5, 0, 360, 0);
}

