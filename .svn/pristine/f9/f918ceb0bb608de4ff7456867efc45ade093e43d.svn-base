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
//  MSimSignalCam
//
//  This task takes a photon list (MPhotonEvent) and converts it into
//  a MSignalCam container. Photons with kNightSky as source are not
//  considered. The arrival time is just the average arrival time
//  of the photons minus the one of the first.
//
//  Input Containers:
//   MPhotonEvent
//   MPhotonStatistics
//   [TriggerPos [MParameterD]]
//
//  Output Containers:
//   MSignalCam
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimSignalCam.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MParameters.h"

#include "MSignalCam.h"
#include "MSignalPix.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

ClassImp(MSimSignalCam);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimSignalCam::MSimSignalCam(const char* name, const char *title)
    : fEvt(0), fStat(0), fSignal(0), fTrigger(0)
{
    fName  = name  ? name  : "MSimSignalCam";
    fTitle = title ? title : "Task to convert a tagged MPhotonEvent list into MSignalCam";
}

// --------------------------------------------------------------------------
//
// Search for the necessary parameter containers
//
Int_t MSimSignalCam::PreProcess(MParList *pList)
{
    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }

    fStat = (MPhotonStatistics*)pList->FindObject("MPhotonStatistics");
    if (!fStat)
    {
        *fLog << err << "MPhotonStatistics not found... aborting." << endl;
        return kFALSE;
    }

    fTrigger = (MParameterD*)pList->FindObject("TriggerPos", "MParameterD");
/*
    if (!fTrigger)
    {
        *fLog << err << "TriggerPos [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }
*/
    fSignal = (MSignalCam*)pList->FindCreateObj("MSignalCam");
    if (!fSignal)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set the size, i.e. the number of pixels, from MSignalCam. It has to be
// preset e.g. by MGeomApply in a preceeding ReInit.
//
Bool_t MSimSignalCam::ReInit(MParList *plist)
{
    const UInt_t npix = fSignal->GetNumPixels();

    if (npix==0)
    {
        *fLog << err << "ERROR - MSignalCam has 0 entries. Presumably MGeomApply::ReInit not done." << endl;
        return kFALSE;
    }

    if (fCont.GetSize()!=npix)
    {
        fCont.Set(npix);
        fTime.Set(npix);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Loop over all photons to sum the photons and determine the average
// arrival time. Write the result into the MSignalCam.
//
// Photons from the NSB are ignored.
//
Int_t MSimSignalCam::Process()
{
    // FIXME: Check the maximum index in GetTag from the statistics container
    fCont.Reset();
    fTime.Reset();

    // Loop over all photons in the event
    const UInt_t num = fEvt->GetNumPhotons();
    for (UInt_t i=0; i<num; i++)
    {
        // Get i-th photon
        const MPhotonData &ph = (*fEvt)[i];

        // Reject photons from the night sky
        if (ph.GetPrimary()==MMcEvtBasic::kNightSky || ph.GetPrimary()==MMcEvtBasic::kArtificial)
            continue;

        // Get tag (must be the index tag!)
        const Int_t idx = ph.GetTag();

        // Reject untagged photons
        if (idx<0)
            continue;

        // Calculate sum of time and photons
        fCont[idx] += ph.GetWeight();
        fTime[idx] += ph.GetTime()*ph.GetWeight();
    }

    // Get time of start point from the statistics container
    //   FIXME: Should be the real time of the first photon

    // ====> Distance to trigger position! (if TrigPos found!)
    // What about events with trigger<0?
    const Float_t trig  = fTrigger && fTrigger->GetVal()>=0 ? fTrigger->GetVal()  : 0;
    const Float_t first = fStat->GetTimeFirst()+trig;

    // Loop over all pixels and set signal and arrival time.
    // Set the pixels valid.
    const UInt_t npix = fSignal->GetNumPixels();
    for (UInt_t idx=0; idx<npix; idx++)
    {
        MSignalPix &pix = (*fSignal)[idx];

        pix.SetNumPhotons(fCont[idx]); 
        pix.SetArrivalTime(fCont[idx]<=0 ? -1 : fTime[idx]/fCont[idx]-first);
        pix.SetRing(fCont[idx]>0); // Used==1, Unused==0
        pix.SetPixelCore(kFALSE);
    }
    fSignal->SetReadyToSave();

    return kTRUE;
}
