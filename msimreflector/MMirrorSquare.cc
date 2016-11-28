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
//  MMirror
//
// A square type spherical mirror
//
//////////////////////////////////////////////////////////////////////////////
#include "MMirrorSquare.h"

#include <stdlib.h> // atof (Ubuntu 8.10)

#include <TMath.h>
#include <TObjArray.h>

#include <TBox.h>
#include <TEllipse.h>
#include <TVirtualPad.h>

#include "MLog.h"

#include "MQuaternion.h"

ClassImp(MMirrorSquare);

using namespace std;

// --------------------------------------------------------------------------
//
// Return the max radius, i.e. the distance of the edges to zje center
//
Double_t MMirrorSquare::GetMaxR() const
{
    return TMath::Sqrt(2.)*fSideLength;
}

// --------------------------------------------------------------------------
//
// Return the aread of the square: l^2
//
Double_t MMirrorSquare::GetA() const
{
    return fSideLength*fSideLength*4;
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
Bool_t MMirrorSquare::CanHit(const MQuaternion &p) const
{
    // p is given in the reflectors coordinate frame. This is meant
    // to be a fast check to sort out all mirrors which we can omit
    // without time consuming calculations.

    // Check if this mirror can be hit at all
    const Double_t dx = TMath::Abs(p.X()-X());
    if (dx>fSideLength*1.05)
        return kFALSE;

    const Double_t dy = TMath::Abs(p.Y()-Y());
    if (dy>fSideLength*1.05)
        return kFALSE;

    return kTRUE;
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
Bool_t MMirrorSquare::HasHit(const MQuaternion &p) const
{
    // p is the incident point in the mirror in the mirror's
    // coordinate frame

    // Black spot in the mirror center (here we can fairly ignore
    // the distance from the plane to the mirror surface, as long
    // as the black spot does not become too large)
    if (p.R2()<0.5*0.5)
        return kFALSE;

    // Check if the photon has really hit the square mirror
    if (TMath::Max(fabs(p.X()), fabs(p.Y()))>fSideLength)
        return kFALSE;

    return kTRUE;
}

// ------------------------------------------------------------------------
//
// Paint the mirror in x/y.
//
// The graphic should coincide with the action in HasHit
//
void MMirrorSquare::Paint(Option_t *opt)
{
    TBox b;
    TEllipse e;
    b.SetFillColor(18);

    if (!TString(opt).Contains("line", TString::kIgnoreCase))
    {
        b.SetFillColor(17);
        b.SetLineStyle(0);
        e.SetLineStyle(0);
        e.SetFillColor(gPad->GetFillColor());
    }

    if (TString(opt).Contains("same", TString::kIgnoreCase))
    {
        b.SetFillStyle(0);
        e.SetFillStyle(0);
    }

    b.PaintBox(X()-fSideLength, Y()-fSideLength, X()+fSideLength, Y()+fSideLength);

    if (!TString(opt).Contains("border", TString::kIgnoreCase))
        e.PaintEllipse(X(), Y(), 0.5, 0.5, 0, 360, 0);
}

// ------------------------------------------------------------------------
//
// Print the contents of the mirror
//
void MMirrorSquare::Print(Option_t *o) const
{
    MMirror::Print(o);
    gLog << " " << fSideLength << endl;
}

// ------------------------------------------------------------------------
//
// Read the mirror's setup from a file. The first eight tokens should be
// ignored. (This could be fixed!)
//
// Here we read: L
//
Int_t MMirrorSquare::ReadM(const TObjArray &tok)
{
    if (tok.GetEntries()!=1)
        return -1;

    Double_t l = atof(tok[0]->GetName());

    if (l<=0)
        return -1;

    fSideLength = l/2;

    return 1;
}

// ------------------------------------------------------------------------
//
void MMirrorSquare::WriteM(ostream &out) const
{
    out << fSideLength*2;
}
