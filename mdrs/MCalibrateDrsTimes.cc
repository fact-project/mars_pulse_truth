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
!   Author(s): Thomas Bretz 2013 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2013
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MCalibrateDrsTimes
//
//////////////////////////////////////////////////////////////////////////////
#include "MCalibrateDrsTimes.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MSignalCam.h"

#include "MRawRunHeader.h"
#include "MRawEvtData.h"

#include "MDrsCalibrationTime.h"

ClassImp(MCalibrateDrsTimes);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrateDrsTimes::MCalibrateDrsTimes(const char *name, const char *title) 
    : fRunHeader(NULL), fCalib(NULL), fBadPixels(NULL), fSignals(NULL),
    fArrivalTime(NULL), fArrivalTimeU(NULL), fNameArrivalTime("MArrivalTimeCam"),
    fNameCalibrated("MSignalCam"), fNameUncalibrated(""), fIsTimeMarker(kFALSE)
{
    fName  = name  ? name  : "MCalibrateDrsTimes";
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
Int_t MCalibrateDrsTimes::PreProcess(MParList *pList)
{
    fSignals = (MArrivalTimeCam*)pList->FindObject(AddSerialNumber(fNameArrivalTime), "MArrivalTimeCam");
    if (!fSignals)
    {
        *fLog << err << AddSerialNumber("MArrivalTimeCam") << " not found ... aborting" << endl;
        return kFALSE;
    }

    fRaw = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData"));
    if (!fRaw)
    {
        *fLog << err << AddSerialNumber("MRawEvtData") << " not found ... aborting" << endl;
        return kFALSE;
    }

    fBadPixels = (MBadPixelsCam*)pList->FindObject(AddSerialNumber("MBadPixelsCam"));
    if (!fBadPixels)
        *fLog << warn << AddSerialNumber("MBadPixelsCam") << " not found ... ignoring." << endl;

    fCalib = (MDrsCalibrationTime*)pList->FindObject("MDrsCalibrationTime");
    if (!fCalib)
        *fLog << warn << "MDrsCalibrationTime not found... no calibratuon will be applied." << endl;

    fArrivalTime = (MSignalCam*)pList->FindCreateObj(AddSerialNumber("MSignalCam"), fNameCalibrated);
    if (!fArrivalTime)
        return kFALSE;

    if (!fNameUncalibrated.IsNull())
    {
        fArrivalTimeU = (MSignalCam*)pList->FindCreateObj("MSignalCam", fNameUncalibrated);
        if (!fArrivalTimeU)
            return kFALSE;
    }

    return kTRUE;
}

Bool_t MCalibrateDrsTimes::ReInit(MParList *pList)
{
    fRunHeader = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
    if (!fRunHeader)
    {
        *fLog << err << AddSerialNumber("MRawRunHeader") << " not found ... aborting." << endl;
        return kFALSE;
    }

    fFreq = fRunHeader->GetFreqSampling();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Apply the calibration factors to the extracted signal according to the 
// selected calibration method
//
Int_t MCalibrateDrsTimes::Process()
{
    const UInt_t npix = fSignals->GetSize();

    const UShort_t *idx = fRaw->GetPixelIds();
    const int16_t  *start = reinterpret_cast<int16_t*>(fRaw->GetStartCells());

    for (UInt_t hw=(fIsTimeMarker?8:0); hw<npix; hw+=(fIsTimeMarker?9:1))
    //for (UInt_t hw=8; hw<npix; hw+=9)
    {
        const UInt_t sw = idx[hw];

        if (start[hw]<0)
        {
            if (fBadPixels)
                (*fBadPixels)[sw].SetUnsuitable(MBadPixelsPix::kUnsuitableEvt);
            continue;
        }

        if (fBadPixels && !fIsTimeMarker && (*fBadPixels)[sw].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
            continue;

        const Float_t signal = (*fSignals)[sw].GetArrivalTimeHiGain();
        const Float_t slope  = (*fSignals)[sw].GetArrivalTimeHiGainError();
        const Float_t offset = fCalib ? fCalib->GetOffset(hw, start[hw], signal) : 0;
        const Float_t offset2 = (fCalib && (signal-slope)>=0) ? fCalib->GetOffset(hw, start[hw], signal-slope) : 0;
        const Float_t delay  = fCalib ? fCalib->GetDelay(hw) : 0;

        //if (fIsTimeMarker)
        //    offset = fCalib ? fCalib->GetDelay(hw, start[hw], signal) : 0;

        // convert from slices to ns
        const Float_t utime      = 1000*(signal       )/fFreq-delay;  // [ns]
        const Float_t time       = 1000*(signal-offset)/fFreq-delay;  // [ns]
        const Float_t slopecal   = (slope-offset+offset2)<0 ? -1 : 1000*(slope-offset+offset2)/fFreq; // [ns]
        const Float_t uslope     = slope<0 ? -1 : 1000*(slope)/fFreq;                // [ns]

        /*
        (*fArrivalTime)[sw].SetArrivalTime(time);
        if (fArrivalTimeU)
            (*fArrivalTimeU)[sw].SetArrivalTime(utime);
            */

        for (UInt_t j=hw-(fIsTimeMarker?8:0); j<=hw; j++)
        {
            (*fArrivalTime)[idx[j]].SetArrivalTime(time);
            (*fArrivalTime)[idx[j]].SetTimeSlope(slopecal);
            if (fArrivalTimeU)
            {
                (*fArrivalTimeU)[idx[j]].SetArrivalTime(utime);
                (*fArrivalTimeU)[idx[j]].SetTimeSlope(uslope);
            }
        }
    }

    fArrivalTime->SetReadyToSave();

    return kTRUE;
}
