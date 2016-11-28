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
!   Author(s): Markus Gaug  04/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MCalibrateRelTimes
//
//   This task takes the extracted arrival times from MArrivalTimeCam for each  
//   pixel and applies the offset calibrated in MCalibrationRelTimeCam 
//   The calibrated arrival time and its error gets stored in MSignalCam. 
//
//   Input Containers:
//    MArrivalTimeCam 
//    MCalibrationRelTimeCam
//
//   Output Containers:
//    MSignalCam
//
//////////////////////////////////////////////////////////////////////////////
#include "MCalibrateRelTimes.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"

#include "MCalibrationRelTimeCam.h"
#include "MCalibrationRelTimePix.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MSignalCam.h"
#include "MSignalPix.h"

#include "MRawRunHeader.h"

ClassImp(MCalibrateRelTimes);

using namespace std;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrateRelTimes::MCalibrateRelTimes(const char *name, const char *title) 
    : fCalibrations(NULL), fBadPixels(NULL), fSignals(NULL), fArrivalTime(NULL)
{
    fName  = name  ? name  : "MCalibrateRelTimes";
    fTitle = title ? title : "Task to calculate the calibrated arrival times of photons in one event";
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MGeomCam
//  - MCalibrationRelTimesCam
//  - MArrivalTimeCam
//  - MBadPixelsCam
//
Int_t MCalibrateRelTimes::PreProcess(MParList *pList)
{
    fSignals = (MArrivalTimeCam*)pList->FindObject(AddSerialNumber("MArrivalTimeCam"));
    if (!fSignals)
    {
        *fLog << err << AddSerialNumber("MArrivalTimeCam") << " not found ... aborting" << endl;
        return kFALSE;
    }

    fBadPixels = (MBadPixelsCam*)pList->FindObject(AddSerialNumber("MBadPixelsCam"));
    if (!fBadPixels)
        *fLog << warn << AddSerialNumber("MBadPixelsCam") << " not found ... ignoring." << endl;

    fCalibrations = (MCalibrationRelTimeCam*)pList->FindObject(AddSerialNumber("MCalibrationRelTimeCam"));
    if (!fCalibrations)
    {
        *fLog << err << AddSerialNumber("MCalibrationRelTimeCam") << " not found ... aborting." << endl;
        return kFALSE;
    }

    fArrivalTime = (MSignalCam*)pList->FindCreateObj(AddSerialNumber("MSignalCam"));
    if (!fArrivalTime)
        return kFALSE;
    
    return kTRUE;
}

Bool_t MCalibrateRelTimes::ReInit(MParList *pList)
{
    MRawRunHeader *runheader = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
    if (!runheader)
    {
        *fLog << err << AddSerialNumber("MRawRunHeader") << " not found ... aborting." << endl;
        return kFALSE;
    }

    fFreq = runheader->GetFreqSampling();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Apply the calibration factors to the extracted signal according to the 
// selected calibration method
//
Int_t MCalibrateRelTimes::Process()
{
    const UInt_t npix = fSignals->GetSize();

    for (UInt_t idx=0; idx<npix; idx++)
    {
        if (fBadPixels && (*fBadPixels)[idx].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
            continue;

        const MCalibrationRelTimePix &pix = (MCalibrationRelTimePix&)(*fCalibrations)[idx];
        const MArrivalTimePix        &sig = (*fSignals)[idx];

        const Float_t signal = sig.GetArrivalTime();
        const Float_t offset = pix.GetTimeOffset();

        // convert from slices to ns
        const Float_t time = 1000*(signal-offset)/fFreq; // [ns]

        (*fArrivalTime)[idx].SetArrivalTime(time);

        // FIXME: Is this necessary or explicitly done by the signal-
        // extractor (may depend on the fact whether independent
        // extractor are or can be used for signal and arrival time)
        if (fBadPixels && !sig.IsArrivalTimeValid())
            (*fBadPixels)[idx].SetUnsuitable(MBadPixelsPix::kUnsuitableEvt);
    }

    fArrivalTime->SetReadyToSave();

    return kTRUE;
}
