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
!   Copyright: CheObs Software Development, 2000-2010
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MSimReflector
//
//  fDetectorFrame is a radius in centimeter, defining a disk in the focal
//  plane around the focal point, in which photons are absorbed. If
//  fDetectorFrame<=0 the virtual HitFrame function of the camera
//  geometry container is used instead.
//
//  fDetectorMargin is a margin (in mm) which is given to the
//  MGeomCam::HitDetector. It should define a margin around the area
//  defined in HitDetector on the focal plane in which photons are kept.
//  Usually this can be 0 because photons not hitting the detector are
//  obsolete except they can later be "moved" inside the detector, e.g.
//  if you use MSimPSF to emulate a PSF by moving photons randomly
//  on the focal plane. To switch off this check set detector margin to -1.
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimReflector.h"

#include <TMath.h>
#include <TRandom.h>

#include "MGeomCam.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MQuaternion.h"
#include "MMirror.h"
#include "MReflector.h"
#include "MReflection.h"

#include "MCorsikaEvtHeader.h"
//#include "MCorsikaRunHeader.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

#include "MPointingPos.h"

ClassImp(MSimReflector);

using namespace std;

// USEFUL CORSIKA OPTIONS:
//  NOCLONG

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimReflector::MSimReflector(const char* name, const char *title)
    : fEvt(0), fMirror0(0), fMirror1(0), fMirror2(0), fMirror3(0),
    fMirror4(0), /*fRunHeader(0),*/ fEvtHeader(0), fReflector(0),
    fGeomCam(0), fPointing(0), fNameReflector("MReflector"),
    fDetectorFrame(0), fDetectorMargin(0)
{
    fName  = name  ? name  : "MSimReflector";
    fTitle = title ? title : "Task to calculate reflection os a mirror";
}

// --------------------------------------------------------------------------
//
// Search for the necessary parameter containers.
//
Int_t MSimReflector::PreProcess(MParList *pList)
{
    fMirror0 = (MPhotonEvent*)pList->FindCreateObj("MPhotonEvent", "MirrorPlane0");
    if (!fMirror0)
        return kFALSE;
    fMirror1 = (MPhotonEvent*)pList->FindCreateObj("MPhotonEvent", "MirrorPlane1");
    if (!fMirror1)
        return kFALSE;
    fMirror2 = (MPhotonEvent*)pList->FindCreateObj("MPhotonEvent", "MirrorPlane2");
    if (!fMirror2)
        return kFALSE;
    fMirror3 = (MPhotonEvent*)pList->FindCreateObj("MPhotonEvent", "MirrorPlane3");
    if (!fMirror3)
        return kFALSE;
    fMirror4 = (MPhotonEvent*)pList->FindCreateObj("MPhotonEvent", "MirrorPlane4");
    if (!fMirror4)
        return kFALSE;

    fReflector = (MReflector*)pList->FindObject(fNameReflector, "MReflector");
    if (!fReflector)
    {
        *fLog << err << fNameReflector << " [MReflector] not found..." << endl;
        return kFALSE;
    }

    if (fReflector->GetNumMirrors()==0)
    {
        *fLog << err << "ERROR - Reflector '" << fNameReflector << "' doesn't contain a single mirror." << endl;
        return kFALSE;
    }

    fGeomCam = (MGeomCam*)pList->FindObject(fNameGeomCam, "MGeomCam");
    if (!fGeomCam)
    {
        if (!fNameGeomCam.IsNull())
            *fLog << inf << fNameGeomCam << " [MGeomCam] not found..." << endl;

        fGeomCam = (MGeomCam*)pList->FindObject("MGeomCam");
        if (!fGeomCam)
        {
            *fLog << err << "MGeomCam not found... aborting." << endl;
            return kFALSE;
        }
    }

    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }
    /*
    fRunHeader = (MCorsikaRunHeader*)pList->FindObject("MCorsikaRunHeader");
    if (!fRunHeader)
    {
        *fLog << err << "MCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }
    */
    fEvtHeader = (MCorsikaEvtHeader*)pList->FindObject("MCorsikaEvtHeader");
    if (!fEvtHeader)
    {
        *fLog << err << "MCorsikaEvtHeader not found... aborting." << endl;
        return kFALSE;
    }

    fPointing = (MPointingPos*)pList->FindObject(/*"PointingCorsika",*/ "MPointingPos");
    if (!fPointing)
    {
        *fLog << err << "MPointingPos not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The main point of calculating the reflection is to determine the
// coincidence point of the particle trajectory on the mirror surface.
//
// If the position and the trajectory of a particle is known it is enough
// to calculate the z-value of coincidence. x and y are then well defined.
//
//  Since the problem (mirror) has a rotational symmetry we only have to care
//  about the distance from the z-axis.
//
// Given:
//
//    p:  position  vector of particle (z=0)
//    u:  direction vector of particle
//    F:  Focal distance of the mirror
//
//  We define:
//
//    q   :=   (px,    py   )
//    v   :=   (ux/uz, uy/uz)
//    r^2 := x^2 + y^2
//
//
// Distance from z-axis:
// ---------------------
//
//          q'        =  q - z*v        (z>0)
//
//    Calculate distance r (|q|)
//
//          r^2       = (px-z*ux)^2 + (py-z*uy)^2
//          r^2       = px^2+py^2 + z^2*(ux^2+uy^2) - 2*z*(px*ux+py*uy)
//          r^2       =   |q|^2   + z^2*|v|^2       - 2*z* q*v
//
//
// Spherical Mirror Surface:  (distance of surface point from 0/0/0)
// -------------------------
//
//   Sphere:  r^2 +   z^2    = R^2               | Parabola: z = p*r^2
//   Mirror:  r^2 +  (z-R)^2 = R^2               | Mirror:   z = p*r^2
//                                               |
//    Focal length: F=R/2                        |  Focal length: F = 1/4p
//                                               |
//   r^2 + (z-2*F)^2 = (2*F)^2                   |           z = r^2/4F
//                                               |
//          z        = -sqrt(4*F*F - r*r) + 2*F  |
//          z-2*F    = -sqrt(4*F*F - r*r)        |
//         (z-2*F)^2 = 4*F*F - r*r               |
//   z^2-4*F*z+4*F^2 = 4*F*F - r*r  (4F^2-r^2>0) |  z - r^2/4F = 0
//   z^2-4*F*z+r^2   = 0
//
//    Find the z for which our particle has the same distance from the z-axis
//    as the mirror surface.
//
//    substitute r^2
//
//
// Equation to solve:
// ------------------
//
//   z^2*(1+|v|^2) - 2*z*(2*F+q*v) + |q|^2 =  0  |  z^2*|v|^2 - 2*z*(2*F+q*v) + |q|^2 = 0
//
//                                   z = (-b +- sqrt(b*b - 4ac))/(2*a)
//
//              a =   1+|v|^2                    |             a = |v|^2
//              b = - 2*b'  with  b' = 2*F+q*v   |             b = - 2*b'  with  b' = 2*F+q*v
//              c =   |q|^2                      |             c = |q|^2
//                                               |
//
//                                        substitute b := 2*b'
//
//                            z = (2*b' +- 2*sqrt(b'*b' - ac))/(2*a)
//                            z = (  b' +-   sqrt(b'*b' - ac))/a
//                            z = (b'/a +-   sqrt(b'*b' - ac))/a
//
//                                        substitute f := b'/a
//
//                                      z = f +- sqrt(f^2 - c/a)
//
// =======================================================================================
//
// After z of the incident point has been determined the position p is
// propagated along u to the plane with z=z. Now it is checked if the
// mirror was really hit (this is implemented in HasHit).
// From the position on the surface and the mirrors curvature we can
// now calculate the normal vector at the incident point.
// This normal vector is smeared out with MMirror::PSF (basically a
// random gaussian) and then the trajectory is reflected on the
// resulting normal vector.
//
Bool_t MMirror::ExecuteReflection(MQuaternion &p, MQuaternion &u) const
{
    // If the z-componenet of the direction vector is normalized to 1
    // the calculation of the incident points becomes very simple and
    // the resulting z is just the z-coordinate of the incident point.
    const TVector2 v(u.XYvector()/u.Z());
    const TVector2 q(p.XYvector());

    // Radius of curvature
    const Double_t G = 2*fFocalLength;

    // Find the incident point of the vector to the mirror
    // u corresponds to downward going particles, thus we use -u here
    const Double_t b = G - q*v;
    const Double_t a = v.Mod2();
    const Double_t c = q.Mod2();

    // Solution for q spherical (a+1) (parabolic mirror (a) instead of (a+1))
    const Double_t A = fShape ? a : a+1;

    const Double_t f = b/A;
    const Double_t g = c/A;

    // Solution of second order polynomial (transformed: a>0)
    // (The second solution can be omitted, it is the intersection
    //  with the upper part of the sphere)
    const Double_t z = a==0 ? c/(2*b) : f - TMath::Sqrt(f*f - g);

    // Move the photon along its trajectory to the x/y plane of the
    // mirror's coordinate frame. Therefor stretch the vector
    // until its z-component is the distance from the vector origin
    // until the vector hits the mirror surface.
    // p += z/u.Z()*u;
    // p is at the mirror plane and we want to propagate back to the mirror surface
    p.PropagateZ(u, z);

    // MirrorShape: Now check if the photon really hit the mirror or just missed it
    if (!HasHit(p))
        return kFALSE;

    // Get normal vector for reflection by calculating the derivatives
    // of a the mirror's surface along x and y
    const Double_t d = fShape ? G : TMath::Sqrt(G*G - p.R2());

    // The solution for the normal vector is
    //    TVector3 n(-p.X()/d, -p.Y()/d, 1));
    // Since the normal vector doesn't need to be of normal
    // length we can avoid an obsolete division
    TVector3 n(p.X(), p.Y(), -d);

    if (fSigmaPSF>0)
        n += SimPSF(n);

    // Changes also the sign of the z-direction of flight
    // This is faster giving identical results
    u *= MReflection(n);
    //u *= MReflection(p.X(), p.Y(), -d);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Converts the coordinates into the coordinate frame of the mirror.
// Executes the reflection calling ExecuteReflection and converts
// the coordinates back.
// Depending on whether the mirror was hit kTRUE or kFALSE is returned.
// It the mirror was not hit the result coordinates are wrong.
//
Bool_t MMirror::ExecuteMirror(MQuaternion &p, MQuaternion &u) const
{
    // Move the mirror to the point of origin and rotate the position into
    // the individual mirrors coordinate frame.
    // Rotate the direction vector into the mirror's coordinate frame
    p -= fPos;
    p *= fTilt;
    u *= fTilt;

    // Move the photon along its trajectory to the x/y plane of the
    // mirror's coordinate frame. Therefor stretch the vector
    // until its z-component vanishes.
    //p -= p.Z()/u.Z()*u;

    // p is at the reflector plane and we want to propagate back to the mirror plane
    p.PropagateZ0(u);

    // Now try to  propagate the photon from the plane to the mirror
    // and reflect its direction vector on the mirror.
    if (!ExecuteReflection(p, u))
        return kFALSE;

    // Derotate from mirror coordinates and shift the photon back to
    // reflector coordinates.
    // Derotate the direction vector
    u *= fTilt.Inverse();
    p *= fTilt.Inverse();
    p += fPos;

    return kTRUE;
}

// Jeder Spiegel sollte eine Liste aller andern Spiegel in der
// reihenfolge Ihrer Entfernung enthalten. Wir starten mit der Suche
// immer beim zuletzt getroffenen Spiegel!
//
// --------------------------------------------------------------------------
//
// Loops over all mirrors of the reflector. After doing a rough check
// whether the mirror can be hit at all the reflection is executed
// calling the ExecuteMirror function of the mirrors.
//
// If a mirror was hit its index is retuened, -1 otherwise.
//
// FIXME: Do to lopping over all mirrors for all photons this is the
// most time consuming function in teh reflector simulation. By a more
// intelligent way of finding the right mirror then just testing all
// this could be accelerated a lot.
//
Int_t MReflector::ExecuteReflector(MQuaternion &p, MQuaternion &u) const
{
    //static const TObjArray *arr = &((MMirror*)fMirrors[0])->fNeighbors;

    // This way of access is somuch faster than the program is
    // a few percent slower if accessed by UncheckedAt
    const MMirror **s = GetFirstPtr();
    const MMirror **e = s+GetNumMirrors();
    //const MMirror **s = (const MMirror**)fMirrors.GetObjectRef(0);
    //const MMirror **e = s+fMirrors.GetEntriesFast();
    //const MMirror **s = (const MMirror**)arr->GetObjectRef(0);
    //const MMirror **e = s+arr->GetEntriesFast();

    // Loop over all mirrors
    for (const MMirror **m=s; m<e; m++)
    {
        const MMirror &mirror = **m;

        // FIXME: Can we speed up using lookup tables or
        //        indexed tables?

        // MirrorShape: Check if this mirror can be hit at all
        // This is to avoid time consuming calculation if there is no
        // chance of a coincidence.
        // FIXME: Inmprove search algorithm (2D Binary search?)
        if (!mirror.CanHit(p))
            continue;

        // Make a local copy of position and direction which can be
        // changed by ExecuteMirror.
        MQuaternion q(p);
        MQuaternion v(u);

        // Check if this mirror is hit, and if it is hit return
        // the reflected position and direction vector.
        // If the mirror is missed we go on with the next mirror.
        if (!mirror.ExecuteMirror(q, v))
            continue;

        // We hit a mirror. Restore the local copy of position and
        // direction back into p und u.
        p = q;
        u = v;

        //arr = &mirror->fNeighbors;

        return m-s;
    }

    return -1;
}

// --------------------------------------------------------------------------
//
// Converts the photons into the telscope coordinate frame using the
// pointing position from MPointingPos.
//
// Reflects all photons on all mirrors and stores the final photons on
// the focal plane. Also intermediate photons are stored for debugging.
//
Int_t MSimReflector::Process()
{
    // Get arrays from event container
    TClonesArray &arr  = fEvt->GetArray();

    // Because we knwo in advance what the maximum storage space could
    // be we allocated it in advance (or shrink it if it was extremely
    // huge before)
    // Note, that the drawback is that an extremly large event
    //       will take about five times its storage space
    //       for a moment even if a lot from it is unused.
    //       It will be freed in the next step.
    fMirror0->Resize(arr.GetEntriesFast()); // Free memory of allocated MPhotonData
    fMirror2->Resize(arr.GetEntriesFast()); // Free memory of allocated MPhotonData
    fMirror3->Resize(arr.GetEntriesFast()); // Free memory of allocated MPhotonData
    fMirror4->Resize(arr.GetEntriesFast()); // Free memory of allocated MPhotonData

    // Initialize mirror properties
    const Double_t F = fGeomCam->GetCameraDist()*100; // Focal length [cm]

    // Local sky coordinates (direction of telescope axis)
    const Double_t zd = fPointing->GetZdRad();  // x==north
    const Double_t az = fPointing->GetAzRad();

    // Rotation matrix to derotate sky
    // For the new coordinate system see the Wiki
    TRotation rot;    // The signs are positive because we align the incident point on ground to the telescope axis
    rot.RotateZ( az); // Rotate point on ground to align it with the telescope axis
    rot.RotateX(-zd); // tilt the point from ground to make it parallel to the mirror plane

    // Now get the impact point from Corsikas output
    const TVector3 impact(fEvtHeader->GetX(), fEvtHeader->GetY(), 0);

    // Counter for number of total and final events
    UInt_t cnt[6] = { 0, 0, 0, 0, 0, 0 };

    const Int_t num = arr.GetEntriesFast();
    for (Int_t idx=0; idx<num; idx++)
    {
        MPhotonData *dat = static_cast<MPhotonData*>(arr.UncheckedAt(idx));

        // w is pointing away from the direction the photon comes from
        // CORSIKA-orig: x(north), y(west),  z(up), t(time)
        // NOW:          x(east),  y(north), z(up), t(time)
        MQuaternion p(dat->GetPosQ());  // z=0
        MQuaternion w(dat->GetDirQ());  // z<0

        // Shift the coordinate system to the telescope. Corsika's
        // coordinate system is always w.r.t. to the particle axis
        p -= impact;

        // Rotate the coordinates into the reflector's coordinate system.
        // It is assumed that the z-plane is parallel to the focal plane.
        // (The reflector coordinate system is defined by the telescope orientation)
        p *= rot;
        w *= rot;

        // ---> Simulate star-light!
        // w.fVectorPart.SetXYZ(0.2/17, 0.2/17, -(1-TMath::Hypot(0.3, 0.2)/17));

        // Now propagate the photon to the z-plane in the new coordinate system
        p.PropagateZ0(w);

        // Store new position and direction in the reflector's coordinate frame
        dat->SetPosition(p); 
        dat->SetDirection(w);

        (*fMirror0)[cnt[0]++] = *dat;
        //*static_cast<MPhotonData*>(cpy0.UncheckedAt(cnt[0]++)) = *dat;

        // Check if the photon has hit the camera housing and holding
        if (fGeomCam->HitFrame(p, w, fDetectorFrame))
            continue;

        // FIXME: Do we really need this one??
        //(*fMirror1)[cnt[1]++] = *dat;
        //*static_cast<MPhotonData*>(cpy1.UncheckedAt(cnt[1]++)) = *dat;

        // Check if the reflector can be hit at all
        if (!fReflector->CanHit(p))
            continue;

        (*fMirror2)[cnt[2]++] = *dat;
        //*static_cast<MPhotonData*>(cpy2.UncheckedAt(cnt[2]++)) = *dat;

        // Now execute the reflection of the photon on the mirrors' surfaces
        const Int_t num = fReflector->ExecuteReflector(p, w);
        if (num<0)
            continue;

        // Set new position and direction (w.r.t. to the reflector's coordinate system)
        // Set also the index of the mirror which was hit as tag.
        dat->SetTag(num);
        dat->SetPosition(p);
        dat->SetDirection(w);

        // FTemme: As dat.fTag is later changed from mirror ID to pixel ID, here
        // also dat.fMirrorTag is set to num:
        dat->SetMirrorTag(num);

        (*fMirror3)[cnt[3]++] = *dat;
        //*static_cast<MPhotonData*>(cpy3.UncheckedAt(cnt[3]++)) = *dat;

        // Propagate the photon along its trajectory to the focal plane z=F
        p.PropagateZ(w, F);

        // Store new position
        dat->SetPosition(p);

        (*fMirror4)[cnt[4]++] = *dat;
        //*static_cast<MPhotonData*>(cpy4.UncheckedAt(cnt[4]++)) = *dat;

        // FIXME: It make make sense to move this out of this class
        // It is detector specific not reflector specific
        // Discard all photons which definitly can not hit the detector surface
        if (fDetectorMargin>=0 && !fGeomCam->HitDetector(p, fDetectorMargin))
            continue;

        // Copy this event to the next 'new' in the list
        *static_cast<MPhotonData*>(arr.UncheckedAt(cnt[5]++)) = *dat;
    }

    // Now we shrink the array to a storable size (for details see
    // MPhotonEvent::Shrink).
    fMirror0->Shrink(cnt[0]);
    //fMirror1->Shrink(cnt[1]);
    fMirror2->Shrink(cnt[2]);
    fMirror3->Shrink(cnt[3]);
    fMirror4->Shrink(cnt[4]);
    fEvt->Shrink(cnt[5]);

    // Doesn't seem to be too time consuming. But we could also sort later!
    //  (after cones, inside the camera)
    fEvt->Sort(kTRUE);

    // FIXME FIXME FIXME: Set maxindex, first and last time.
    // SetMaxIndex(fReflector->GetNumMirrors()-1)
    // if (fEvt->GetNumPhotons())
    // {
    //    SetTime(fEvt->GetFirst()->GetTime(), fEvt->GetLast()->GetTime());
    // }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// DetectorMargin: 0
//
Int_t MSimReflector::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "DetectorFrame", print))
    {
        rc = kTRUE;
        fDetectorFrame = GetEnvValue(env, prefix, "DetectorFrame", 0);
    }
    if (IsEnvDefined(env, prefix, "DetectorMargin", print))
    {
        rc = kTRUE;
        fDetectorMargin = GetEnvValue(env, prefix, "DetectorMargin", 0);
    }

    return rc;
}
