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
//  MSimCalibrationSignal
//
// This task is a MRead task which produces events instead of reading them
// from a file. To be more precise, calibration events are produced.
// (Note, that pedestal events are also a kind of calibration events).
//
// Whether pedestal or calibration events are produced is defined by
// the RunType stored in MRawRunheader. The number of pixels which are
// "enlightened" are determined from a MGeomCam.
//
//
//  Input Containers:
//   fNameGeomCam [MGeomCam]
//   MRawRunHeader
//   IntendedPulsePos [MParameterD]
//
//  Output Containers:
//   [MPhotonEvent]
//   [TriggerPos [MParameterD]]
//   MPhotonStatistics
//   MRawEvtHeader
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimCalibrationSignal.h"

#include <TRandom.h>

#include "MParList.h"
#include "MTaskList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParameters.h"

#include "MGeomCam.h"
#include "MTriggerPattern.h"

#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

#include "MParSpline.h"

ClassImp(MSimCalibrationSignal);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimCalibrationSignal::MSimCalibrationSignal(const char* name, const char *title)
    : fParList(0), fGeom(0), fPulse(0), fPulsePos(0), fTrigger(0),
    fRunHeader(0), fEvtHeader(0),  fEvt(0), fStat(0),
    fNumEvents(1000), fNumPhotons(5), fTimeJitter(1)
{
    fName  = name  ? name  : "MRead";//"MSimCalibrationSignal";
    fTitle = title ? title : "Task to create a fake signal (derives from MRead)";
}

// --------------------------------------------------------------------------
//
//  Check for the needed parameter containers.
//
Int_t MSimCalibrationSignal::PreProcess(MParList *pList)
{
    fGeom = (MGeomCam*)pList->FindObject(fNameGeomCam, "MGeomCam");
    if (!fGeom)
    {
        *fLog << inf << fNameGeomCam << " [MGeomCam] not found..." << endl;

        fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
        if (!fGeom)
        {
            *fLog << err << "MGeomCam not found... aborting." << endl;
            return kFALSE;
        }
    }

    fRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRunHeader)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fEvt = (MPhotonEvent*)pList->FindCreateObj("MPhotonEvent");
    if (!fEvt)
        return kFALSE;

    fTrigger = (MParameterD*)pList->FindCreateObj("MParameterD", "TriggerPos");
    if (!fTrigger)
        return kFALSE;

    fStat = (MPhotonStatistics*)pList->FindCreateObj("MPhotonStatistics");
    if (!fStat)
        return kFALSE;

    fEvtHeader = (MRawEvtHeader*)pList->FindCreateObj("MRawEvtHeader");
    if (!fEvtHeader)
        return kFALSE;

    fPulsePos = (MParameterD*)pList->FindObject("IntendedPulsePos", "MParameterD");
    if (!fPulsePos)
    {
        *fLog << err << "IntendedPulsePos [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    fPulse = (MParSpline*)pList->FindObject("PulseShape", "MParSpline");
    if (!fPulse)
    {
        *fLog << err << "PulsShape [MParSpline] not found... aborting." << endl;
        return kFALSE;
    }

    *fLog << all << "Number of Events: " << fNumEvents << endl;

    //
    // Search for MTaskList
    //
    fParList = pList;

    //
    // A new file has been opened and new headers have been read.
    //  --> ReInit tasklist
    //
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// ReInit the task-list if this is the first "event from this fake file"
//
Bool_t MSimCalibrationSignal::CallReInit()
{
    if (GetNumExecutions()!=1)
        return kTRUE;

    MTask *tlist = (MTask*)fParList->FindObject("MTaskList");
    if (!tlist)
    {
        *fLog << err << dbginf << "MTaskList not found... abort." << endl;
        return kFALSE;
    }

    // FIXME: Is there a way to write them as LAST entry in the file?
    fRunHeader->SetReadyToSave();

    return tlist->ReInit(fParList);
}

// --------------------------------------------------------------------------
//
// Produce the events.
//
Int_t MSimCalibrationSignal::Process()
{
    if (GetNumExecutions()>fNumEvents)
        return kFALSE;

    if (!CallReInit())
        return kERROR;

    Int_t cnt = 0;
    if (fRunHeader->IsCalibrationRun())
    {
        for (UInt_t idx=0; idx<fGeom->GetNumPixels(); idx++)
        {
            // FIXME: Scale number of photons with the pixel size!
            const Int_t num = gRandom->Poisson(fNumPhotons);

            // FIXME: How does the distribution look like? Poissonian?
            for (Int_t i=0; i<num; i++)
            {
                MPhotonData &ph = fEvt->Add(cnt++);

                // FIMXE: Is this the correct distribution
                const Float_t tm = gRandom->Gaus(0, fTimeJitter);

                ph.SetPrimary(MMcEvtBasic::kArtificial);
                ph.SetTag(idx);
                ph.SetWeight();
                ph.SetTime(tm);
            }
        }
    }

    fEvt->Shrink(cnt);
    fEvt->Sort(kTRUE);

    // ------------ FIMXE: Move somewhere else? -------------
    // -------------------- MSimGeomCam ---------------------

    // =====> Move to MSimReadoutWindow ?

    const Double_t freq = fRunHeader->GetFreqSampling()/1000.;

    // We add an additional sample at the end to support a possible shift
    // of the start time of the first event by 0 to 1 sample.
    const Int_t   ns = fRunHeader->GetNumSamplesHiGain()+1;

    // Length (ns), Pulse position (Units ns)
    const Float_t pp = fPulsePos->GetVal();
    const Float_t pw = fPulse->GetWidth();

    const Float_t first = cnt>0 ? fEvt->GetFirst()->GetTime() : 0;
    const Float_t last  = cnt>0 ? fEvt->GetLast()->GetTime()  : ns*freq;

    fStat->SetTime(first-pp-pw, last-pp+pw + ns*freq);
    fStat->SetMaxIndex(fGeom->GetNumPixels()-1);
    fStat->SetReadyToSave();

    // FIXME: Jitter! (Own class?)
    fTrigger->SetVal((pp+pw)*freq);
    fTrigger->SetReadyToSave();

    // Set trigger pattern according to the trigger type
    const UInt_t p = fRunHeader->IsCalibrationRun() ? 0x100 : 0x400; 
    //const UInt_t p = fRunHeader->IsCalibrationRun() ? MTriggerPattern::kCalibration : MTriggerPattern::kPedestal;
    fEvtHeader->SetTriggerPattern(~(p | (p<<8)));
    fEvtHeader->SetCalibrationPattern(0/*BIT(16)<<16*/); // CT1 Pulser, see MCalibrationPatternDecode
    fEvtHeader->SetReadyToSave();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read the parameters from the resource file.
//
//    NumEvents:   1000
//    NumPhotons:     5
//    TimeJitter:     1
//
Int_t MSimCalibrationSignal::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NumEvents", print))
    {
        rc = kTRUE;
        fNumEvents = GetEnvValue(env, prefix, "NumEvents", (Int_t)fNumEvents);
    }

    if (IsEnvDefined(env, prefix, "NumPhotons", print))
    {
        rc = kTRUE;
        fNumPhotons = GetEnvValue(env, prefix, "NumPhotons", (Int_t)fNumPhotons);
    }

    if (IsEnvDefined(env, prefix, "TimeJitter", print))
    {
        rc = kTRUE;
        fTimeJitter = GetEnvValue(env, prefix, "TimeJitter", fTimeJitter);
    }

    return rc;
}
