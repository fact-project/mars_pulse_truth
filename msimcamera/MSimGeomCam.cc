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
//  MSimGeomCam
//
//  This task takes a photon list from a MPhotonEvent and checks which pixel
// from a MGeomCam is hit. The photons are tagged with the corresponding
// index.
//
// Additionally (and provisionally) it also calculates the photon event
// statistics.
//
//  Input Containers:
//   MPhotonEvent
//   fNameGeomCam [MGeomCam]
//   MRawRunHeader
//   [IntendedPulsePos [MParameterD]]
//   [MPulseShape]
//
//  Output Containers:
//   MPhotonStatistics
//   -/-
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimGeomCam.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

#include "MParameters.h"
#include "MParSpline.h"
#include "MRawRunHeader.h"

ClassImp(MSimGeomCam);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimGeomCam::MSimGeomCam(const char* name, const char *title)
    : fGeom(0), fEvt(0), fStat(0), fPulsePos(0), fHeader(0), fPulse(0),
    fNameGeomCam("MGeomCam")
{
    fName  = name  ? name  : "MSimGeomCam";
    fTitle = title ? title : "Task to tag each photon in a MPhotonEvent with a pixel index from a MGeomCam";
}

// --------------------------------------------------------------------------
//
// Search for the needed parameter containers.
//
Int_t MSimGeomCam::PreProcess(MParList *pList)
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

    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }

    fPulse = (MParSpline*)pList->FindObject("PulseShape", "MParSpline");
/*
    if (!fPulse)
    {
        *fLog << err << "MPulsShape not found... aborting." << endl;
        return kFALSE;
    }
 */
    fPulsePos = (MParameterD*)pList->FindObject("IntendedPulsePos", "MParameterD");
/*
    if (!fPulsePos)
    {
        *fLog << err << "IntendedPulsePos [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }
*/
    fHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fHeader)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fStat = (MPhotonStatistics*)pList->FindCreateObj("MPhotonStatistics");
    if (!fStat)
        return kFALSE;

    return kTRUE;
}


// --------------------------------------------------------------------------
//
Int_t MSimGeomCam::Process()
{
    const Int_t num = fEvt->GetNumPhotons();

    Int_t cnt = 0;
    for (Int_t i=0; i<num; i++)
    {
        MPhotonData &ph = (*fEvt)[i];

        //
        // sum the photons content in each pixel
        //
        for (UInt_t idx=0; idx<fGeom->GetNumPixels(); idx++)
        {
            // FIXME: Improve search algorithm (2D Binary search?)
            // Here we convert the photons from the ceres-coordinate
            // system which is viewed from the camera to the mirror
            // into the camera coordinates which are viewed from
            // the mirror to the camera.
            // (x on the right, y upwards, right-handed)
            if (!(*fGeom)[idx].IsInside(-ph.GetPosX()*10, ph.GetPosY()*10))
                continue;

            ph.SetTag(idx);

            (*fEvt)[cnt++] = ph;

            break;
        }
    }

    fEvt->Shrink(cnt);
    //fEvt->Sort();

    // ------ FIXME: Move somewhere else? MSimCalibrationSignal ------
/*
    if (!fEvt->IsSorted())
    {
        *fLog << err << "ERROR - MSimGeomCam: MPhotonEvent must be sorted!" << endl;
        return kERROR;
    }
 */
    const Float_t freq = fHeader->GetFreqSampling()/1000.;

    // We add an additional sample at the end to support a possible shift
    // of the start time of the first event by 0 to 1 sample.
    const Int_t   ns   = fHeader->GetNumSamplesHiGain()+1;

    const Float_t first = cnt>0 ? fEvt->GetTimeFirst() :  0;
    const Float_t last  = cnt>0 ? fEvt->GetTimeLast()  : ns*freq;

    // Length (ns), Pulse position (Units ns)
    const Float_t pp   = fPulsePos ? fPulsePos->GetVal() : 0;
    const Float_t pw   = fPulse    ? fPulse->GetWidth()  : 0;

    fStat->SetTimeMedDev(fEvt->GetTimeMedianDev());
    fStat->SetTime(first-pp-pw, last-pp+pw + ns*freq);
    fStat->SetLength(last-first);
    fStat->SetMaxIndex(fGeom->GetNumPixels()-1);
    fStat->SetReadyToSave();

    // ----------------------------------------------------------------------

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read the parameters from the resource file.
//
//    NameGeometry: MGeomCamDwarf
//
Int_t MSimGeomCam::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NameGeometry", print))
    {
        rc = kTRUE;
        SetNameGeomCam(GetEnvValue(env, prefix, "NameGeometry", fNameGeomCam));
        // FIXME: What about setup of this container?
    }

    return rc;
}
