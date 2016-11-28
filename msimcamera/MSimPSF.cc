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
//  MSimPSF
//
//  This task makes a naiv simulation of the psf by smearing out the photons
//  in a plane (camera plane) by a 2D-Gaussian with fSigma
//
//  Input Containers:
//   MPhotonEvent
//
//  Output Containers:
//   MPhotonEvent
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimPSF.h"

#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

ClassImp(MSimPSF);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimPSF::MSimPSF(const char* name, const char *title)
    : fEvt(0), fSigma(-1)
{
    fName  = name  ? name  : "MSimPSF";
    fTitle = title ? title : "Task to do a naiv simulation of the psf by smearout in the camera plane";
}

// --------------------------------------------------------------------------
//
// Search for MPhotonEvent
//
Int_t MSimPSF::PreProcess(MParList *pList)
{
    if (fSigma<=0)
        return kSKIP;

    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Smear out all photons by a gaussian with fSigma
//
Int_t MSimPSF::Process()
{
    const UInt_t num = fEvt->GetNumPhotons();

    // Loop over all mirrors
    for (UInt_t i=0; i<num; i++)
    {
        // Get i-th photon
        MPhotonData &ph = (*fEvt)[i];

        // Get random gaussian shift
        const TVector2 v(gRandom->Gaus(0, fSigma), gRandom->Gaus(0, fSigma));

        // Add random smear out
        ph.SetPosition(ph.GetPos2()+v);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Sigma: 10
//
Int_t MSimPSF::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "Sigma", print))
    {
        rc = kTRUE;
        fSigma = GetEnvValue(env, prefix, "Sigma", fSigma);
    }

    return rc;
}
