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
!   Author(s): Oscar Blanch 11/2002 <mailto:blanch@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomMirror
//
// This is the base class of the Mirror geometry. 
//
/////////////////////////////////////////////////////////////////////////////
#include "MGeomMirror.h"

#include <TMath.h>
#include <TRotation.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MGeomMirror);

using namespace std;

// --------------------------------------------------------------------------
//
// Initializes a Mirror geometry with 0 values, except for fMirrorID.
//
MGeomMirror::MGeomMirror(Int_t mir, const char *name, const char *title)
    : fReflector(0)
{
    fName  = name  ? name  : "MGeomMirror";
    fTitle = title ? title : "Storage container for  a mirror geometry";

    fMirrorId=mir;   // the Mirror Id

    fFocalDist=0.;   //  focal distance of that mirror [cm]
    fSX=0.;          // curvilinear coordinate of mirror's center in X[cm]
    fSY=0.;          // curvilinear coordinate of mirror's center in X[cm]
    fX=0.;           // x coordinate of the center of the mirror [cm]
    fY=0.;           // y coordinate of the center of the mirror [cm]
    fZ=0.;           // z coordinate of the center of the mirror [cm]
    fThetaN=0.;      // polar theta angle of the direction 
                     //  where the mirror points to
    fPhiN=0.;        // polar phi angle of the direction
                     // where the mirror points to
    fXN=0.;          // xn coordinate of the normal vector
                     // in the center
    fYN=0.;          // yn coordinate of the normal vector
                     // in the center
    fZN=0.;          // zn coordinate of the normal vector
                     // in the center
                     // Note: fXN^2*fYN^2*fZN^2 = 1
    fDeviationX=0.;  // deviation in x [cm]
    fDeviationY=0.;  // deviation in y [cm]
                     // of the spot of a single mirror on the camera plane

}

// --------------------------------------------------------------------------
//
// DESCRIPTION MISSING: Please contact Oscar
//
void MGeomMirror::SetMirrorContent(Int_t mir, Float_t focal, Float_t curv_x,
                                   Float_t curv_y, Float_t lin_x, Float_t lin_y,
				   Float_t lin_z, Float_t theta, Float_t phi,
				   Float_t x_n, Float_t y_n, Float_t z_n){
    fMirrorId=mir;   // the Mirror Id

    fFocalDist=focal;   //  focal distance of that mirror [cm]
    fSX=curv_x;          // curvilinear coordinate of mirror's center in X[cm]
    fSY=curv_y;          // curvilinear coordinate of mirror's center in X[cm]
    fX=lin_x;           // x coordinate of the center of the mirror [cm]
    fY=lin_y;           // y coordinate of the center of the mirror [cm]
    fZ=lin_z;           // z coordinate of the center of the mirror [cm]
    fThetaN=theta;      // polar theta angle of the direction 
                     //  where the mirror points to
    fPhiN=phi;        // polar phi angle of the direction
                     // where the mirror points to
    fXN=x_n;          // xn coordinate of the normal vector
                     // in the center
    fYN=y_n;          // yn coordinate of the normal vector
                     // in the center
    fZN=z_n;          // zn coordinate of the normal vector
                     // in the center
                     // Note: fXN^2*fYN^2*fZN^2 = 1
}

void MGeomMirror::SetMirrorDeviations(Float_t dev_x, Float_t dev_y)
{
    fDeviationX=dev_x;  // deviation in x [cm]
    fDeviationY=dev_y;  // deviation in y [cm]
                     // of the spot of a single mirror on the camera plane
}

// --------------------------------------------------------------------------
//
// Setting function to store the mirror reflectivity (ref) at different
// wavelength (wav).
//
void MGeomMirror::SetReflectivity(const TArrayF &wav, const TArrayF &ref)
{
    if (fWavelength.GetSize()!=wav.GetSize() ||
        fReflectivity.GetSize()!=ref.GetSize())
    {
        *fLog << err << dbginf << " fWavelength or fQE do not have ";
        *fLog << "size of setting arrays" << endl;
        return;
    }

    fWavelength = wav;
    fReflectivity = ref;
}

TVector3 MGeomMirror::GetReflection(const TVector3 &star, Double_t dist)
{
    if (!fReflector)
    {
        fReflector = new TRotation; // unit matrix
        fReflector->Rotate(TMath::Pi(), GetMirrorNorm());
    }

    // Reflect star on the mirror (done by a rotation
    // around the normal vector of the mirror center
    TVector3 light(star);
    light *= *fReflector;

    if (dist<0)
        return light;

    // calculate distance to the camera (stretch vector such, that
    // its Z component is the distance between the camera and
    // the mirror
    const TVector3 &pos = GetMirrorCenter();
    const Double_t d = (dist - pos.Z()) / light.Z();

    return light*d + pos;
}
