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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MAstroSky2Local
// ---------------
//
// Rotation Matrix to convert sky coordinates to ideal local coordinates
// for example:
//
//   const Double_t ra  = MAstro::Hms2Rad(5, 34, 31.9);
//   const Double_t dec = MAstro::Dms2Rad(22, 0, 52.0);
//
//   MTime time;
//   time.Set(2004, 1, 26, 00, 20, 00);
//
//   MObservatory obs(MObservatory::kMagic1);
//
//   TVector3 v;
//   v.SetMagThetaPhi(1, TMath::Pi()/2-dec, ra);
//
//   v *= MAstroSky2Local(time, obs);
//
//   Double_t azimuth   = v.Phi();
//   Double_t zdistance = v.Theta();
//
// To get the inverse matrix for an inverse transformation you can use:
//
//   MAstroSky2Local::Invert()
//
// or simply do:
//
//   v *= MAstroSky2Local(time, obs).Inverse();
//
// Reminder: This tranformation only does a simple coordinate
//           transformation. It completely ignores all other atrometric
//           effects, like nutation, abberation, precission, ...
//
////////////////////////////////////////////////////////////////////////////
#include "MAstroSky2Local.h"

#include "MAstro.h"
#include "MTime.h"
#include "MObservatory.h"

using namespace std;

ClassImp(MAstroSky2Local);

// --------------------------------------------------------------------------
//
// Initialize the rotation matrix R as:
//
//   R = A*B*C*D
//
// with
//
//       |1  0  0|
//   A = |0 -1  0|    (Change counting direction of rotation angle)
//       |0  0  1|
//
//   B = RotZ(r1)     (Change rotation angle such, that phi=0 is identical
//                     for both coordinate systems)
//
//   C = RotY(r2)     (Make zenith and sky pole the same point)
//
//   D = RotZ(180deg) (Align rotation angle to N=0, E=90)
//
// with
//
//   r1 = gmst + longitude   with   gmst fraction of day, see MTime::GetGmst
//                                  logitude of observers location
//
//   r2 = latitude-90deg     with   latitude of observers location
//
void MAstroSky2Local::Init(Double_t gmst, const MObservatory &obs)
{
    RotateZ(gmst + obs.GetElong());
    RotateY(obs.GetPhi()-TMath::Pi()/2);
    RotateZ(TMath::Pi());
}

// --------------------------------------------------------------------------
//
// Initialize MAstroSky2Local for a given time an a defined observatory
// For more information see class description
// For more information about gmst see MTime::GetGmst()
//
MAstroSky2Local::MAstroSky2Local(Double_t gmst, const MObservatory &obs) : TRotation(1, 0, 0, 0, -1, 0, 0, 0, 1)
{
    Init(gmst, obs);
}

// --------------------------------------------------------------------------
//
// Initialize MAstroSky2Local for a given time an a defined observatory
// For more information see class description
//
MAstroSky2Local::MAstroSky2Local(const MTime &t, const MObservatory &obs) : TRotation(1, 0, 0, 0, -1, 0, 0, 0, 1)
{
    Init(t.GetGmst(), obs);
}

// --------------------------------------------------------------------------
//
// Get the corresponding rotation angle of the sky coordinate system
// seen with an Alt/Az telescope.
//
// dzd and daz is a pointing offset, which is subtracted from the
// calculated local coordinates correspoding to time, observatory
// and ra/dec.
//
// For more information see MAstro::RotationAngle
//
Double_t MAstroSky2Local::RotationAngle(Double_t ra, Double_t dec, Double_t dzd, Double_t daz) const
{
    TVector3 v;
    v.SetMagThetaPhi(1, TMath::Pi()/2-dec, ra);
    v *= *this;

    return MAstro::RotationAngle(ZZ(), XZ(), v.Theta()-dzd, v.Phi()-daz);
}
