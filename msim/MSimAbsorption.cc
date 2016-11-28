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
//  MSimAbsorption
//
//  Task to calculate wavelength or incident angle dependent absorption
//
//  Input Containers:
//   fParName [MParSpline]
//   MPhotonEvent
//   MCorsikaEvtHeader
//   MCorsikaRunHeader
//
//  Output Containers:
//   MPhotonEvent
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimAbsorption.h"

#include <fstream>

#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MParSpline.h"

#include "MCorsikaEvtHeader.h"
#include "MCorsikaRunHeader.h"
#include "MPhotonEvent.h"
#include "MPhotonData.h"

ClassImp(MSimAbsorption);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimAbsorption::MSimAbsorption(const char* name, const char *title)
    : fEvt(0), fRunHeader(0), fHeader(0), fSpline(0), fParName("MParSpline"), fUseTheta(kFALSE), fForce(kFALSE)
{
    fName  = name  ? name  : "MSimAbsorption";
    fTitle = title ? title : "Task to calculate wavelength dependent absorption";
}

// --------------------------------------------------------------------------
//
// Search for the needed parameter containers. Read spline from file
// calling ReadFile();
//
Int_t MSimAbsorption::PreProcess(MParList *pList)
{
    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }

    fSpline = (MParSpline*)pList->FindObject(fParName, "MParSpline");
    if (!fSpline)
    {
        *fLog << err << fParName << " [MParSpline] not found... aborting." << endl;
        return kFALSE;
    }

    if (!fSpline->IsValid())
    {
        *fLog << err << "Spline in " << fParName << " is inavlid... aborting." << endl;
        return kFALSE;
    }

    fHeader = (MCorsikaEvtHeader*)pList->FindObject("MCorsikaEvtHeader");
    if (!fHeader)
    {
        *fLog << err << "MCorsikaEvtHeader not found... aborting." << endl;
        return kFALSE;
    }

    *fLog << inf << "Using " << (fUseTheta?"Theta":"Wavelength") << " for absorption." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Bool_t MSimAbsorption::ReInit(MParList *pList)
{
    if (fUseTheta)
        return kTRUE;

    fRunHeader = (MCorsikaRunHeader*)pList->FindObject("MCorsikaRunHeader");
    if (!fRunHeader)
    {
        *fLog << err << "MCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    if (fRunHeader->Has(MCorsikaRunHeader::kCeffic) && !fForce)
        *fLog << inf << "CEFFIC enabled... task will be skipped." << endl;

    if (fRunHeader->GetWavelengthMin()<fSpline->GetXmin())
        *fLog << warn << "WARNING - Lower bound of wavelength bandwidth exceeds lower bound of spline." << endl;

    if (fRunHeader->GetWavelengthMax()>fSpline->GetXmax())
        *fLog << warn << "WARNING - Upper bound of wavelength bandwidth exceeds upper bound of spline." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Throw all events out of the MPhotonEvent which don't survive.
// Depending on fUseTheta either the wavelength or the incident angle
// is used.
//
Int_t MSimAbsorption::Process()
{
    // Skip the task if the CEFFIC option has been enabled and
    // its excution has not been forced by the user
    if (!fForce && !fUseTheta && fRunHeader->Has(MCorsikaRunHeader::kCeffic))
        return kTRUE;

    // Get the number of photons in the list
    const Int_t num = fEvt->GetNumPhotons();

    // Counter for number of total and final events
    Int_t cnt = 0;
    for (Int_t i=0; i<num; i++)
    {
        // Get i-th photon from the list
        const MPhotonData &ph = (*fEvt)[i];

        // Depending on fUseTheta get the incident angle of the wavelength
        const Double_t wl = fUseTheta ? ph.GetTheta()*TMath::RadToDeg() : ph.GetWavelength();

        // Get the efficiency (the probability that this photon will survive)
        // from the spline.
        const Double_t eff = fSpline->Eval(wl);

        // Get a random value between 0 and 1 to determine whether the photn will survive
        // gRandom->Rndm() = [0;1[
        if (gRandom->Rndm()>=eff)
            continue;

        // Copy the surviving events bakc in the list
        (*fEvt)[cnt++] = ph;
    }

    // Now we shrink the array to the number of new entries.
    fEvt->Shrink(cnt);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// FileName: reflectivity.txt
// UseTheta: No
//
Int_t MSimAbsorption::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "ParName", print))
    {
        rc = kTRUE;
        SetParName(GetEnvValue(env, prefix, "ParName", fParName));
    }

    if (IsEnvDefined(env, prefix, "UseTheta", print))
    {
        rc = kTRUE;
        SetUseTheta(GetEnvValue(env, prefix, "UseTheta", fUseTheta));
    }

    if (IsEnvDefined(env, prefix, "Force", print))
    {
        rc = kTRUE;
        SetForce(GetEnvValue(env, prefix, "Force", fForce));
    }

    return rc;
}
