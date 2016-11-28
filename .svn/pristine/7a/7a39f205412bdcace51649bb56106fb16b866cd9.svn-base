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
// Note, that we could use basic geometry classes instead, but especially
// CanHit is time critical. So this class is (should be) optimized for
// execution speed.
//
// This base class provides the code to calculate a spherical mirror
// (ExecuteMirror) and to scatter in a way to get a proper PSF.
// Furthermore it stored the geometry of a mirror.
//
// ------------------------------------------------------------------------
//
// Bool_t CanHit(const MQuaternion &p) const;
//
//    This is a very rough estimate of whether a photon at a position p
//    can hit a mirror. The position might be off in z and the photon
//    still has to follow its trajectory. Nevertheless we can fairly assume
//    the the way to travel in x/y is pretty small so we can give a rather
//    good estimate of whether the photon can hit.
//
//    Never throw away a photon whihc can hit the mirror!
//
// ------------------------------------------------------------------------
//
// Bool_t HasHit(const MQuaternion &p) const;
//
//    Check if the given position coincides with the mirror. The position
//    is assumed to be the incident point on the mirror's surface.
//
//    The coordinates are in the mirrors coordinate frame.
//
//    The action should coincide with what is painted in Paint()
//
// ------------------------------------------------------------------------
//
// Double_t GetA() const
//
//     Return the reflective area of the mirror
//
// ------------------------------------------------------------------------
//
// Double_t GetMaxR() const
//
//     Return the maximum distance of a reflective point from the
//     mirror center
//
// ------------------------------------------------------------------------
//
// void Paint(Option_t *opt)
//
//    Paint the mirror in x/y.
//
//    The graphic should coincide with the action in HasHit
//
// ------------------------------------------------------------------------
//
// Int_t ReadM(const TObjArray &tok);
//
//    Read the mirror's setup from a file. The first eight tokens should be
//    ignored. (This could be fixed!)
//
//////////////////////////////////////////////////////////////////////////////
#include "MMirror.h"

#include <TRandom.h>

#include "MLog.h"

#include "MQuaternion.h"

ClassImp(MMirror);

using namespace std;

void MMirror::SetShape(Char_t c)
{
    switch (toupper(c))
    {
    case 'S':
        fShape = 0;
        break;

    case 'P':
        fShape = 1;
        break;

    default:
        fShape = 0;
    }
}

// --------------------------------------------------------------------------
//
// Return the TVector2 which is the x/y position of the mirror minus
// q.XYvector/(;
//
TVector2 MMirror::operator-(const MQuaternion &q) const
{
    return TVector2(X()-q.X(), Y()-q.Y());
}

// --------------------------------------------------------------------------
//
// Return the TVector2 which is the difference of this mirror and the
// given mirror
//
TVector2 MMirror::operator-(const MMirror &m) const
{
    return TVector2(X()-m.X(), Y()-m.Y());
}

// --------------------------------------------------------------------------
//
// Simulate the PSF. Therefor we smear out the given normal vector
// with a gaussian.
//
// Returns a vector which can be added to the normal vector.
//
// FIXME: What is the correct focal distance to be given here?
//        Can the smearing be imporved?
//
TVector3 MMirror::SimPSF(const TVector3 &n, Double_t F, Double_t psf) const
{
    //const TVector3 n( x, y, -d)         // Normal vector of the mirror
    const TVector3 xy(-n.Y(), n.X(), 0);  // Normal vector in x/y plane

    Double_t gx, gy;
    gRandom->Rannor(gx, gy);         // 2D random Gauss distribution

    psf /= 2;                        // The factor two because of the doubleing of the angle in the reflection
    psf /= F;                        // Scale the Gauss to the size of the PSF
    //psf *= n.Z();                  // 
    psf *= n.Mag();                  // This means that the PSF is measured in the focal distance

    TVector3 dn(gx*psf, gy*psf, 0);  // Instead of psf/F also atan(psf/F) might make sense

    dn.Rotate(-n.Theta(), xy);       // Tilt the gauss-vector to the normal vector

    return dn;  // Return the vector to be added to the normal vector
}

// --------------------------------------------------------------------------
//
void MMirror::Print(Option_t *o) const
{
    gLog << fPos.X()  << " " << fPos.Y()  << " " << fPos.Z() << " ";
    gLog << fNorm.X() << " " << fNorm.Y() << " " << fNorm.Z() << " ";
    gLog << fFocalLength << " ";
    if (fSigmaPSF>0)
        gLog << fSigmaPSF << " ";

    const TString n = ClassName();

    gLog << n(7, n.Length());
}
