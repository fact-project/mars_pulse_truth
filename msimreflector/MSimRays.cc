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
!   Author(s): Thomas Bretz,  1/2009 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: CheObs Software Development, 2000-2010
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MSimRays
//
// Task to produce rays from a light source at either infinity or a given
// height from a given local sky position.
//
// The sky position is defined by an MPointingPos object in the parameter
// list (if none exists, the source is at the reflector axis). Its
// default name is "MPointingPos".
//
// The height of the light/point source is set by SetHeight in units of km.
// A value <= 0 means infinity.
//
// The number of rays produced per event is defined by SetNumPhotons(n).
// The default is 1000.
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimRays.h"

#include <TMath.h>       // root >=5.20
#include <TRandom.h>
#include <TRotation.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MQuaternion.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

#include "MReflector.h"
#include "MPointingPos.h"

ClassImp(MSimRays);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimRays::MSimRays(const char* name, const char *title)
    : fEvt(0), fReflector(0), fPointPos(0), fSource(0),
    fNumPhotons(1000), fHeight(-1),
    fNameReflector("MReflector"), fNamePointPos("MPointingPos"),
    fNameSource("Source")
{
    fName  = name  ? name  : "MSimRays";
    fTitle = title ? title : "Task to calculate reflection os a mirror";
}

// --------------------------------------------------------------------------
//
// Search for the necessary parameter containers.
//
Int_t MSimRays::PreProcess(MParList *pList)
{
    fEvt = (MPhotonEvent*)pList->FindCreateObj("MPhotonEvent");
    if (!fEvt)
        return kFALSE;

    if (!pList->FindCreateObj("MCorsikaEvtHeader"))
        return kFALSE;

    fReflector = (MReflector*)pList->FindObject(fNameReflector, "MReflector");
    if (!fReflector)
    {
        *fLog << inf << fNameReflector << " [MReflector] not found..." << endl;
        return kFALSE;
    }

    fSource = (MPointingPos*)pList->FindObject(fNameSource, "MPointingPos");
    if (!fSource)
    {
//        *fLog << inf << fNameSource << " [MPointingPos] not found..." << endl;
//        return kFALSE;
    }

    fPointPos = (MPointingPos*)pList->FindObject(fNamePointPos, "MPointingPos");
    if (!fPointPos)
    {
        *fLog << inf << fNamePointPos << " [MPointingPos] not found..." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Converts the photons into the telscope coordinate frame using the
// pointing position from MPointingPos.
//
// Reflects all photons on all mirrors and stores the final photons on
// the focal plane. Also intermediate photons are stored for debugging.
//
Int_t MSimRays::Process()
{
    // Get arrays from event container
    fEvt->Resize(fNumPhotons);

    TClonesArray &arr  = fEvt->GetArray();

    const Int_t num = arr.GetEntriesFast();

    const Double_t maxr = fReflector->GetMaxR();

    const Double_t deltazd = fSource ? fSource->GetZdRad() : 0;
    const Double_t deltaaz = fSource ? fSource->GetAzRad() : 0;

    const Double_t zd = fPointPos->GetZdRad() + deltazd;
    const Double_t az = fPointPos->GetAzRad() + deltaaz;


    // cm     -> m
    // s      -> ns
    // length -> time
    const Double_t conv = 1./(TMath::C()*100/1e9);

    // Local sky coordinates (direction of telescope axis)
    //const Double_t zd = fPointing->GetZdRad();  // x==north
    //const Double_t az = fPointing->GetAzRad();

    // Height of point source [cm] (0 means infinity)
    const Double_t h = fHeight * 100000;

    // Rotation matrix to derotate sky
    // For the new coordinate system see the Wiki
    TRotation rot;    // The signs are positive because we align the incident point on ground to the telescope axis
    rot.RotateX( zd); // Rotate point on ground to align it with the telescope axis
    rot.RotateZ(-az); // tilt the point from ground to make it parallel to the mirror plane

    Int_t idx = 0;
    while (idx<num)
    {
        MPhotonData &dat = *static_cast<MPhotonData*>(arr.UncheckedAt(idx));

        Double_t x, y;
        const Double_t r = gRandom->Uniform();
        gRandom->Circle(x, y, maxr*TMath::Sqrt(r));
/*
        Double_t ra = gRandom->Uniform(maxr);
        Double_t ph = gRandom->Uniform(TMath::TwoPi());


        // Get radom incident point on the mirror plane.
        //const Double_t x = gRandom->Uniform(-maxr, maxr);
        //const Double_t y = gRandom->Uniform(-maxr, maxr);

        Double_t x = ra*sin(ph);
        Double_t y = ra*cos(ph);

//        if (x*x + y*y > maxr*maxr)
//            continue;
  */
        // The is the incident direction of the photon
        // h==0 means infinitiy
        const TVector3 u = fHeight>0 ? TVector3(x, y, -h).Unit() :  TVector3(0, 0, -1);

        // w is pointing away from the direction the photon comes from
        // CORSIKA-orig: x(north), y(west),  z(up), t(time)
        // NOW:          x(east),  y(north), z(up), t(time)
        MQuaternion p(TVector3(x, y, 0), fHeight>0 ? TMath::Sqrt(x*x + y*y + h*h): 0);
        MQuaternion w(u, conv);

        // Rotate the coordinates into the reflector's coordinate system.
        // It is assumed that the z-plane is parallel to the focal plane.
        // (The reflector coordinate system is defined by the telescope orientation)
        p *= rot;
        w *= rot;

        // Now propagate the photon to the z-plane in the new coordinate system
        p.PropagateZ0(w);

        // Shift the coordinate system to the telescope. Corsika's
        // coordinate system is always w.r.t. to the particle axis
        //p += impact;

        // Store new position and direction in the reflector's coordinate frame
        dat.SetPosition(p);
        dat.SetDirection(w);

        idx++;
    }

    // Doesn't seem to be too time consuming. But we could also sort later!
    //  (after cones, inside the camera)
    // fEvt->Sort(kTRUE);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Height: -1
// NumPhotons: 1000
//
Int_t MSimRays::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "Height", print))
    {
        rc = kTRUE;
        fHeight = GetEnvValue(env, prefix, "Height", fHeight);
    }
    if (IsEnvDefined(env, prefix, "NumPhotons", print))
    {
        rc = kTRUE;
        fNumPhotons = GetEnvValue(env, prefix, "NumPhotons", (Int_t)fNumPhotons);
    }

    return rc;
}
