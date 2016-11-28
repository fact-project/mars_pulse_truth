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
//  MSimExcessNoise
//
//  This task adds the noise (usually signal height, i.e. excess, dependent)
//  to the photon signal.
//
//  Input Containers:
//   MCorsikaEvent
//
//  Output Containers:
//   MCorsikaEvent
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimExcessNoise.h"

#include <TMath.h>
#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

ClassImp(MSimExcessNoise);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimExcessNoise::MSimExcessNoise(const char* name, const char *title)
: fEvt(0), fExcessNoise(0.2)
{
    fName  = name  ? name  : "MSimExcessNoise";
    fTitle = title ? title : "Task to simulate the excess dependant noise (conversion photon to signal height)";
}

// --------------------------------------------------------------------------
//
// Check for the necessary parameter containers.
//
Int_t MSimExcessNoise::PreProcess(MParList *pList)
{
    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }

    *fLog << inf << "Excess Noise Factor in use " << fExcessNoise << "%" << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Change the weight of each signal according to the access noise
//
Int_t MSimExcessNoise::Process()
{
    const UInt_t num = fEvt->GetNumPhotons();
    for (UInt_t i=0; i<num; i++)
    {
        MPhotonData &ph = (*fEvt)[i];

        const Float_t oldw = ph.GetWeight();
        if (oldw<0)
            continue;

        const Float_t neww = gRandom->Gaus(oldw, fExcessNoise*TMath::Sqrt(oldw));
        ph.SetWeight(neww);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// ExcessNoise: 0.2
//
Int_t MSimExcessNoise::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "ExcessNoise", print))
    {
        rc = kTRUE;
        fExcessNoise = GetEnvValue(env, prefix, "ExcessNoise", fExcessNoise);
    }

    return rc;
}
