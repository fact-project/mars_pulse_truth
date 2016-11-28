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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MMcPedestalCopy
//
//  This task looks for the ìnformation about FADC pedestals in
//  MMcFadcHeader and translates it to the pedestal values in
//  MPedestalCam
//
//  Input Containers:
//   MMcFadcHeader
//   [MMcRunHeader]
//   [MRawRunHeader]
//
//  Output Containers:
//   MPedestalCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MMcPedestalCopy.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MPedestalPix.h"
#include "MPedestalCam.h"

#include "MRawRunHeader.h"
#include "MMcRunHeader.hxx"
#include "MMcFadcHeader.hxx"

ClassImp(MMcPedestalCopy);

using namespace std;

MMcPedestalCopy::MMcPedestalCopy(const char *name, const char *title)
{
    fName  = name  ? name  : "MMcPedestalCopy";
    fTitle = title ? title : "Copy MC pedestals into MPedestal Container";

    //
    // This is not needed here using MReadMarsFile because for the
    // RunHeader tree the auto scheme is disabled by default
    //
    AddToBranchList("MMcFadcHeader.fPedesMean");
    AddToBranchList("MMcFadcHeader.fElecNoise");
}

// --------------------------------------------------------------------------
//
// Make sure that there is a MPedestalCam object in the parameter list.
//
Int_t  MMcPedestalCopy::PreProcess(MParList *pList)
{
    return pList->FindCreateObj(AddSerialNumber("MPedestalCam")) ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
// Check for the runtype.
// Search for MPedestalCam and MMcFadcHeader.
//
Bool_t MMcPedestalCopy::ReInit(MParList *pList)
{
    const MRawRunHeader *run = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
    if (!run)
    {
        *fLog << warn << dbginf << "Warning - cannot check file type, " << AddSerialNumber("MRawRunHeader") << " not found." << endl;
        return kTRUE;
    }

    //
    // If it is no MC file skip this function...
    //
    if (!run->IsMonteCarloRun())
    {
        *fLog << inf << "This is no MC file... skipping." << endl;
        return kTRUE;
    }

    MPedestalCam *pedcam = (MPedestalCam*)pList->FindObject(AddSerialNumber("MPedestalCam"));
    if (!pedcam)
    {
        *fLog << err << "MPedestalCam not found... aborting." << endl;
        return kFALSE;
    }

    // Get MMcRunHeader to check camera version
    MMcRunHeader *mcrun = (MMcRunHeader*)pList->FindObject(AddSerialNumber("MMcRunHeader"));

    // Check if it is a ceres file
    if (mcrun && mcrun->IsCeres())
    {
        *fLog << inf << "This is a ceres file... subtracting Baseline from ElectronicNoise [MPedestalCam]." << endl;

        MPedestalCam *noise = (MPedestalCam*)pList->FindObject(AddSerialNumber("ElectronicNoise"), "MPedestalCam");
        if (!noise)
        {
            *fLog << err << "ElectronicNoise [MPedestalCam] not found... aborting." << endl;
            return kFALSE;
        }

        const int num = pedcam->GetSize();
        for (int i=0; i<num; i++)
        {
            const MPedestalPix &n = (*noise)[i];

            (*pedcam)[i].Set(n.GetPedestal()/run->GetScale(),
                             n.GetPedestalRms()/run->GetScale());
        }

        pedcam->SetReadyToSave();

        return kTRUE;
    }

    //
    // Now check the existance of all necessary containers. This has
    // to be done only if this is a MC file.
    //
    MMcFadcHeader *fadc = (MMcFadcHeader*)pList->FindObject(AddSerialNumber("MMcFadcHeader"));
    if (!fadc)
    {
        *fLog << err << "MMcFadcHeader not found... aborting." << endl;
        return kFALSE;
    }

    if (!mcrun)
        *fLog << warn << dbginf << AddSerialNumber("MMcRunHeader") << " not found... assuming camera<0.7" << endl;

    const Bool_t camver70 = mcrun && mcrun->GetCamVersion()>=70;

    const int num = pedcam->GetSize();
    for (int i=0; i<num; i++)
    {
        // Here one should compute the Pedestal taking into account how
        // the MC makes the transformation analogic-digital for the FADC.
        // This is done only once per file -> not time critical.
        const Float_t pedest = fadc->GetPedestal(i);
        const Float_t sigma  = camver70 ? fadc->GetPedestalRmsHigh(i) : fadc->GetElecNoise(i);

        (*pedcam)[i].Set(pedest/run->GetScale(), sigma/run->GetScale());
    }

    if (camver70)
        pedcam->SetReadyToSave();

    return kTRUE;
}
