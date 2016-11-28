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
//  MMirrorDisk
//
// A disk like spherical mirror.
//
//////////////////////////////////////////////////////////////////////////////
#include "MMirrorDisk.h"

#include <stdlib.h> // atof (Ubuntu 8.10)

#include <TMath.h>
#include <TObjArray.h>

#include <TEllipse.h>
#include <TVirtualPad.h>

#include "MLog.h"

#include "MQuaternion.h"

ClassImp(MMirrorDisk);

using namespace std;

// --------------------------------------------------------------------------
//
// Return the aread of the disk: Pi*r^2
//
Double_t MMirrorDisk::GetA() const
{
    return TMath::Pi()*fR*fR;
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
Bool_t MMirrorDisk::CanHit(const MQuaternion &p) const
{
    // p is given in the reflectors coordinate frame. This is meant
    // to be a fast check to sort out all mirrors which we can omit
    // without time consuming calculations.

    // Check if this mirror can be hit at all
    const Double_t dx = TMath::Abs(p.X()-X());
    if (dx>fR*1.05)
        return kFALSE;

    const Double_t dy = TMath::Abs(p.Y()-Y());
    if (dy>fR*1.05)
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
Bool_t MMirrorDisk::HasHit(const MQuaternion &p) const
{
    // p is the incident point in the mirror in the mirror's
    // coordinate frame

    // Black spot in the mirror center (here we can fairly ignore
    // the distance from the plane to the mirror surface, as long
    // as the black spot does not become too large)
    if (p.R2()<0.5*0.5)
        return kFALSE;

    // Check if the photon has really hit the square mirror
    return p.R()<fR;
}

// ------------------------------------------------------------------------
//
// Print the contents of the mirror
//
void MMirrorDisk::Print(Option_t *o) const
{
    MMirror::Print(o);
    gLog << " " << fR << endl;
}

// ------------------------------------------------------------------------
//
// Paint the mirror in x/y.
//
// The graphic should coincide with the action in HasHit
//
void MMirrorDisk::Paint(Option_t *opt)
{
    TEllipse e;
    e.SetFillColor(18);
    if (!TString(opt).Contains("line", TString::kIgnoreCase))
    {
        e.SetFillColor(17);
        e.SetLineStyle(0);
    }
    if (TString(opt).Contains("same", TString::kIgnoreCase))
        e.SetFillStyle(0);
    e.PaintEllipse(X(), Y(), fR, fR, 0, 360, 0);

    if (!TString(opt).Contains("line", TString::kIgnoreCase))
        e.SetFillColor(gPad->GetFillColor());

    if (!TString(opt).Contains("border", TString::kIgnoreCase))
        e.PaintEllipse(X(), Y(), 0.5, 0.5, 0, 360, 0);
}

// ------------------------------------------------------------------------
//
// Read the mirror's setup from a file. The first eight tokens should be
// ignored. (This could be fixed!)
//
// Here we read: fR
//
Int_t MMirrorDisk::ReadM(const TObjArray &tok)
{
    if (tok.GetEntries()!=1)
        return -1;

    const Double_t d = atof(tok[0]->GetName());

    if (d<=0)
        return -1;

    fR = d/2;

    return 1;
}

// ------------------------------------------------------------------------
//
void MMirrorDisk::WriteM(ostream &out) const
{
    out << fR*2;
}
