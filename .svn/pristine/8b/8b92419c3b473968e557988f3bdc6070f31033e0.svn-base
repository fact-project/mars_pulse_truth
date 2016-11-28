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
// MSimPointingPos
//
//
// This task is meant to simulate the pointing position (mirror orientation).
// This depends on the direction from which the shower is coming but also
// on the user request (e.g. Wobble mode).
//
//
// If fOffTargetDistance==0 the telescope is oriented depending on the
// view cone option. If a view cone was given the orientation is fixed to
// the main direction around the view cone was produced. If no view
// cone was given the telescope is oriented parallel to the shower axis.
//
// If no view cone option was given and off-target observations are switched
// on by setting fOffTargetDistance!=0 the pointing position is calculated:
//
//  1) fOffTargetDistance < 0:
//     The pointing position is randomly distributed in a disk of radius
//     -fOffTargetDistance. fOffTargetDistance is silently ignored.
//
//  2) fOffTargetDistance > 0:
//     The pointing position is set to a position in the given distance
//     away from the shower axis. If fOffTargetPhi>=0 it is fixed at
//     this phi value. For phi<0 it is randomly distributed at distances
//     fOffTargetDistance. (phi==0 is the direction of positive theta)
//
// The original zenith and azimuth coordinates of the shower axis are stored in
// the MSimSourcePos container.
//
// If the view cone option was given and off-target observations are switched on
// the orientation is fixed to the main direction around the view cone was
// produced.
// In addition a 'quasi'-simulated source position is calculated,
// depending on fOffTargetDistance and fOffTargetPhi (see 1) and 2) above).
// The corresponding zenith and azimuth coordinates are stored in the
// MSimSourcePos container. This is of course not a physical source position,
// but it can be used to determine the performance of wobble analysis on
// background events (which are homogenous distributed).
//
//
//
//  Input Containers:
//   MCorsikaRunHeader
//   MCorsikaEvtHeader
//
//  Output Containers:
//   MPointingPos
//   MSimSourcePos
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimPointingPos.h"

#include <TRandom.h>
#include <TVector3.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MCorsikaEvtHeader.h"
#include "MCorsikaRunHeader.h"

#include "MPointingPos.h"

ClassImp(MSimPointingPos);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimPointingPos::MSimPointingPos(const char* name, const char *title)
    : fRunHeader(0), fEvtHeader(0), fPointing(0), fSimSourcePosition(0),
    fOffTargetDistance(0), fOffTargetPhi(-1)

{
    fName  = name  ? name  : "MSimPointingPos";
    fTitle = title ? title : "Task to simulate the pointing position (mirror orientation)";
}

// --------------------------------------------------------------------------
//
// Get the distance from the real source to the poitning position.
//
Double_t MSimPointingPos::GetOffTargetDistance() const
{
    return fOffTargetDistance==0 ? 0 : fOffTargetDistance*TMath::RadToDeg();
}

// --------------------------------------------------------------------------
//
// Get the phi angle counted from the upward direction the source position
// is rotated. distance from the real source to the pointing position.
// A negative value refers to a random distribution.
//
Double_t MSimPointingPos::GetOffTargetPhi() const
{
    return fOffTargetPhi<0 ? -1 : fOffTargetPhi*TMath::RadToDeg();
}

// --------------------------------------------------------------------------
//
// Set fOffTargetDistance, see also GetOffTargetDistance
//
void MSimPointingPos::SetOffTargetDistance(Double_t d)
{
    fOffTargetDistance = d==0 ? 0 : d*TMath::DegToRad();
}

// --------------------------------------------------------------------------
//
// Set fOffTargetPhi, see also GetOffTargetPhi
//
void MSimPointingPos::SetOffTargetPhi(Double_t p)
{
    fOffTargetPhi = p<0 ? -1 : p*TMath::DegToRad();
}



// --------------------------------------------------------------------------
//
// Search for all necessary containers
//
Int_t MSimPointingPos::PreProcess(MParList *pList)
{
    fPointing = (MPointingPos*)pList->FindCreateObj("MPointingPos");
    if (!fPointing)
        return kFALSE;

    fSimSourcePosition = (MPointingPos*)pList->FindCreateObj("MPointingPos","MSimSourcePos");
    if (!fSimSourcePosition)
        return kFALSE;

    fRunHeader = (MCorsikaRunHeader*)pList->FindObject("MCorsikaRunHeader");
    if (!fRunHeader)
    {
        *fLog << err << "MCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fEvtHeader = (MCorsikaEvtHeader*)pList->FindObject("MCorsikaEvtHeader");
    if (!fEvtHeader)
    {
        *fLog << err << "MCorsikaEvtHeader not found... aborting." << endl;
        return kFALSE;
    }

    if (!IsOffTargetObservation())
        return kTRUE;

    *fLog << inf;
    *fLog << "Off target observations switched on with" << endl;
    if (fOffTargetDistance>0)
    {
        *fLog <<"   a pointing distance of " << GetOffTargetDistance() << "deg ";
        if (fOffTargetPhi<0)
            *fLog << "randomly distributed in phi." << endl;
        else
            *fLog << "and phi=" << GetOffTargetPhi() << "deg." << endl;
    }
    else
        *fLog << "   a homogenous distribution up to a distance of " << -GetOffTargetDistance() << "deg " << endl;

    return kTRUE;
}

Bool_t MSimPointingPos::ReInit(MParList *pList)
{
    if (fRunHeader->HasViewCone() && IsOffTargetObservation())
    {
        *fLog << warn;
        *fLog << "WARNING - Combining the view cone option with off-target pointing can lead to not homogenous events." << endl;
        *fLog << "          A simulated source position according to the off-target parameters will be created instead." << endl;
    }
    // FIXME: Check also the enlightened region on the ground!
    return kTRUE;
}

void MSimPointingPos::GetDelta(Double_t &dtheta, Double_t &dphi) const
{
    if (fOffTargetDistance>0)
    {
        dtheta = fOffTargetDistance;
        dphi   = fOffTargetPhi>=0 ? fOffTargetPhi : gRandom->Uniform(TMath::TwoPi());
    }
    else
    {
        dtheta = TMath::Sqrt(gRandom->Uniform(fOffTargetDistance));
        dphi   = gRandom->Uniform(TMath::TwoPi());
    }
}

// --------------------------------------------------------------------------
//
Int_t MSimPointingPos::Process()
{
    // If a view cone is given use the fixed telescope orientation
    const Bool_t viewcone = fRunHeader->HasViewCone();

    // Local sky coordinates (direction of telescope axis)
    Double_t zdCorsika = viewcone ? fRunHeader->GetZdMin() : fEvtHeader->GetZd()*TMath::RadToDeg();  // x==north
    Double_t azCorsika = viewcone ? fRunHeader->GetAzMin() : fEvtHeader->GetAz()*TMath::RadToDeg();  // y==west

    // Calculate off target position in local sky coordinates
    Double_t dtheta, dphi;
    GetDelta(dtheta, dphi);

    const Double_t theta = zdCorsika*TMath::DegToRad();
    const Double_t phi   = azCorsika*TMath::DegToRad();

    TVector3 originalVector, wobbleVector;
    originalVector.SetMagThetaPhi(1, theta,        phi);
    wobbleVector.SetMagThetaPhi(1, theta+dtheta, phi);

    wobbleVector.Rotate(dphi, originalVector);

    Double_t zdWobble, azWobble;
    zdWobble = wobbleVector.Theta()*TMath::RadToDeg();
    azWobble = wobbleVector.Phi()  *TMath::RadToDeg();

    // Transform the corsika coordinate system (north is magnetic north)
    // into the telescopes local coordinate system. Note, that all vectors
    // are already correctly oriented.
    azCorsika += fRunHeader->GetMagneticFieldAz()*TMath::RadToDeg();
    azWobble += fRunHeader->GetMagneticFieldAz()*TMath::RadToDeg();

    // Setup the pointing and the simulated source position
    if (!viewcone)
    {
        fPointing->SetLocalPosition(zdWobble, azWobble);
        fSimSourcePosition->SetLocalPosition(zdCorsika, azCorsika);
    }
    else
    {
        fPointing->SetLocalPosition(zdCorsika, azCorsika);
        fSimSourcePosition->SetLocalPosition(zdWobble, azWobble);
    }

    return kTRUE;
}

Int_t MSimPointingPos::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "OffTargetDistance", print))
    {
        rc = kTRUE;
        SetOffTargetDistance(GetEnvValue(env, prefix, "OffTargetDistance", GetOffTargetDistance()));
    }

    if (IsEnvDefined(env, prefix, "OffTargetPhi", print))
    {
        rc = kTRUE;
        SetOffTargetPhi(GetEnvValue(env, prefix, "OffTargetPhi", GetOffTargetPhi()));
    }

    return rc;
}

