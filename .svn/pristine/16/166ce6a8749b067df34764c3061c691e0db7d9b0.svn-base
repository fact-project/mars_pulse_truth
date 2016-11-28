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
!   Author(s): Thomas Bretz  11/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MQuaternion
//
// The MQuaternion is derived from TQuaternion. A quaternion is a four vector
// which can store space and time (like a lorentz vector).
//
// There are a few advantages of the TQuaternion class over the
// TLorentzVector, namely the implementation of a direct algebra with
// just the space part of the vector keeping the time as it is.
// (This is useful, e.g, for rotations and shift just in space).
//
//  - You can construct the MQuaternion from a TQuaternion or a TVector3 and
//    time.
//  - Multiplying the MQuaternion with a TRotation with rotate just the
//    space-part.
//  - You can access the data members with X(), Y(), Z() and T()
//  - To get the length or squared-length of the space-vector use R() and R2()
//  - Access the 2D vector (x/y) with XYvector()
//  - Thera are a few new function to propagate a MQuaternion along a trajectory
//    in space and time (also expressed as an MQuaternion with a direction
//    vector and a speed) Here we assume v>0.
//
//    + PropagateDz(MQuaternion &w, Double_t dz)
//
//      If dz is positive the position is propagated along the given trajectory
//      in space (such that the z-component will increase by dz) and
//      forward in time. If dz<0 the result is vice versa.
//
//    + PropagateZ0(MQuaternion &w)
//
//      This is an abbreviation for Propagate(w, -Z()). It propagates the
//      position such that its z-component will vanish. If this is along
//      the given trajectory time will increase if it is backward time
//      will decrease.
//
//    + PropagateZ(MQuaternion &w, Double_t z)
//
//      This is an abbreviation for Propagate(w, z-Z()). It propagates the
//      position such that its z-component will become z. If this is along
//      the given trajectory time will increase if it is backward time
//      will decrease.
//
//////////////////////////////////////////////////////////////////////////////
#include "MQuaternion.h"

ClassImp(MQuaternion);

using namespace std;

