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
!   Author(s): Thomas Bretz,  2/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MSimMMCS
//
// Task to copy the Mars CheObs MC headers to the old Mars style
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimMMCS.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MMcEvt.hxx"

#include "MRawRunHeader.h"

#include "MCorsikaRunHeader.h"
#include "MCorsikaEvtHeader.h"

#include "MMcRunHeader.hxx"
#include "MMcCorsikaRunHeader.h"

#include "MPointingPos.h"

ClassImp(MSimMMCS);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimMMCS::MSimMMCS(const char* name, const char *title)
{
    fName  = name  ? name  : "MSimMMCS";
    fTitle = title ? title : "Task to copy the Mars CheObs MC headers to the old Mars style";
}

// --------------------------------------------------------------------------
//
Int_t MSimMMCS::PreProcess(MParList *plist)
{
    fMcRunHeader = (MMcRunHeader*)plist->FindCreateObj("MMcRunHeader");
    if (!fMcRunHeader)
        return kFALSE;

    if (!plist->FindCreateObj("MMcCorsikaRunHeader"))
        return kFALSE;

    if (!plist->FindCreateObj("MRawRunHeader"))
        return kFALSE;

    fMcEvtBasic = (MMcEvtBasic*)plist->FindCreateObj("MMcEvtBasic");
    if (!fMcEvtBasic)
        return kFALSE;

    fMcEvt = (MMcEvt*)plist->FindCreateObj("MMcEvt");
    if (!fMcEvt)
        return kFALSE;

    fPointingTel = (MPointingPos*)plist->FindObject("MPointingPos");
    if (!fPointingTel)
    {
        *fLog << err << "MPointingPos not found... aborting." << endl;
        return kFALSE;
    }

    fEvtHeader = (MCorsikaEvtHeader*)plist->FindObject("MCorsikaEvtHeader");
    if (!fEvtHeader)
    {
        *fLog << err << "MCorsikaEvtHeader not found... aborting." << endl;
        return kFALSE;
    }

    fRunHeader = (MCorsikaRunHeader*)plist->FindObject("MCorsikaRunHeader");
    if (!fRunHeader)
    {
        *fLog << err << "MCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Bool_t MSimMMCS::ReInit(MParList *plist)
{
    MMcCorsikaRunHeader *mch = (MMcCorsikaRunHeader*)plist->FindObject("MMcCorsikaRunHeader");
    if (!mch)
    {
        *fLog << err << "MMcCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    mch->SetSpectrum(fRunHeader->GetSlopeSpectrum(),
                     fRunHeader->GetEnergyMin(), fRunHeader->GetEnergyMax());
    mch->SetViewCone(fRunHeader->GetViewConeInnerAngle(),
                     fRunHeader->GetViewConeOuterAngle());
    mch->SetReadyToSave();

    // ----------------------------------------------------

    //    fNumPheFromDNSB        MMcPedestalNSBAdd   // Number of phe/ns from diffuse NSB

    // FIXME: Is there a way to write them as LAST entry in the file?
    fMcRunHeader->SetNumSimulatedShowers(fRunHeader->GetNumEvents()*fRunHeader->GetNumReuse());
    fMcRunHeader->SetCorsikaVersion(TMath::Nint(fRunHeader->GetProgramVersion()*100));

    if (fRunHeader->GetImpactMax()>0)
        fMcRunHeader->SetImpactMax(fRunHeader->GetImpactMax());

    // ----------------------------------------------------

    MRawRunHeader *rh = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!rh)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    const UInt_t id = fRunHeader->GetParticleID();

    // FIXME: Is there a way to write them as LAST entry in the file?
    rh->SetFileNumber(fRunHeader->GetRunNumber()%1000);
    rh->SetSourceInfo(MMcEvtBasic::GetParticleName(id));
    rh->SetReadyToSave();

    // ----------------------------------------------------

    fMcEvtBasic->SetPartId(MMcEvtBasic::ParticleId_t(id));

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// We have two scenarios for the ponting:
//
//  1) Diffuse flux (view cone option)
//
//     Azimuth and Zenith angle are fixed values (min==max).
//     The diffuse flux is produced in a cone around this angle.
//
//     That means that the telescope orientation is aligned and fixed
//     along the cone axis.
//
//     To analyse a diffuse flux the source position should be fixed to
//     the camera center.
//     In some cases (e.g. training of RF) the source depending parameters
//     might be needed w.r.t. the real origin of the primary particle.
//     In this case the primary shower direction is needed.
//
//  2) Directed flux
//
//     Particles are produced between a min and max Azimuth and Zenith angle.
//     The telescope axis is either parallel (on source) or off axis
//     (wobble) to the primary particle direction.
//     This is used to "fake" the trajectory of the source.
//
//  3) Flux along a trajectory
//
//     [...]
//
Int_t MSimMMCS::Process()
{
    fMcEvtBasic->SetEnergy(fEvtHeader->GetTotalEnergy());
    fMcEvtBasic->SetImpact(fEvtHeader->GetImpact());

    // Pointing direction of the telescope (telescope axis)
    fMcEvtBasic->SetTelescopeTheta(fPointingTel->GetZdRad());
    fMcEvtBasic->SetTelescopePhi(fPointingTel->GetAzRad());

    // Direction of primary particle
    // Convert from corsika frame to telescope frame, taking
    // the magnetic field into account: tel = corsika+offset
    fMcEvtBasic->SetParticleTheta(fEvtHeader->GetZd());
    fMcEvtBasic->SetParticlePhi(fEvtHeader->GetAz()+fRunHeader->GetMagneticFieldAz());

    fMcEvtBasic->SetReadyToSave();


    static_cast<MMcEvtBasic&>(*fMcEvt) = *fMcEvtBasic;


    fMcEvt->SetEvtNumber(fEvtHeader->GetEvtNumber());
    fMcEvt->SetEventReuse(fEvtHeader->GetNumReuse());
    fMcEvt->SetPhotElfromShower(0);

    if (fRunHeader->GetImpactMax()<0 &&
        fEvtHeader->GetImpact()>fMcRunHeader->GetImpactMax())
        fMcRunHeader->SetImpactMax(fEvtHeader->GetImpact());

    return kTRUE;
}
