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
!   Author(s): Thomas Bretz, 2013<mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2013
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MDrsCalibApply
//
//////////////////////////////////////////////////////////////////////////////
#include "MDrsCalibApply.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MArrayB.h"

#include "MRawRunHeader.h"
#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MDrsCalibration.h"
#include "MPedestalSubtractedEvt.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

ClassImp(MDrsCalibApply);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MDrsCalibApply::MDrsCalibApply(const char *name, const char *title)
    : fRawEvt(NULL), fDrsCalib(NULL), fSignal(NULL), fResult(0),
    fMaxNumPrevEvents(5), fRemoveSpikes(3), fSlidingAverage(0)
{
    fName  = name  ? name  : "MDrsCalibApply";
    fTitle = title ? title : "Class to subtract pedestal";
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawEvtData
//  - MRawRunHeader
//  - MPedestalCam
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MDrsCalibApplyedEvt
//
Int_t MDrsCalibApply::PreProcess(MParList *pList)
{
    fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData"));
    if (!fRawEvt)
    {
        *fLog << err << AddSerialNumber("MRawEvtData") << " not found... aborting." << endl;
        return kFALSE;
    }

    fDrsCalib = (MDrsCalibration*)pList->FindObject("MDrsCalibration");
    if (!fDrsCalib)
        *fLog << warn << "No DRS calibration container (MDrsCalibration) found... skipping." << endl;

    else
        *fLog << inf << "DRS calibration will be applied." << endl;

    fResult = 0;
    if (fMaxNumPrevEvents>0)
    {
        fPrevStart.clear();
        fResult = (MDrsCalibResult*)pList->FindCreateObj("MDrsCalibResult");
        if (!fResult)
            return kFALSE;
    }

    fSignal = (MPedestalSubtractedEvt*)pList->FindCreateObj("MPedestalSubtractedEvt");//, AddSerialNumber(fNamePedestalSubtractedEvt));
    if (!fSignal)
        return kFALSE;

    return kTRUE;
}

Bool_t MDrsCalibApply::ReInit(MParList *pList)
{
    fRunHeader = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
    if (!fRunHeader)
    {
        *fLog << err << AddSerialNumber("MRawRunHeader") << " not found... aborting." << endl;
        return kFALSE;
    }

    if (!fDrsCalib)
        return kTRUE;

    if (fDrsCalib->fRoi>0 && fDrsCalib->fRoi != fRunHeader->GetNumSamplesHiGain())
    {
        *fLog << warn << "Slice number of DRS calibration " << fDrsCalib->fRoi << " and run-header " << fRunHeader->GetNumSamplesHiGain() << " don't match...\n";
        *fLog << "secondary baseline correction will not be applied." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Int_t MDrsCalibApply::Process()
{
    // Please note:
    //   - for data with only hi-gain samples numl is 0
    //   - for data with hi- and lo-gain samples
    //     numl is 0 if read from a raw-data file or a new MC root-file(?)
    //     numl is not 0 if read from an old MC root-file

    // Total number of samples
    const Int_t roi = fRawEvt->GetNumHiGainSamples();

    // Check if event is empty (presumably MC event -- sanity check)
    if (roi==0)
        return kCONTINUE;

    // Check for consistency (our simulation can do weird things!)
    if (roi!=fRunHeader->GetNumSamplesHiGain())
    {
        *fLog << err << "MDrsCalibApply::Process: ERROR - Number of samples in event ";
        *fLog << "(hi=" << roi << ")" << endl << " doesn't match number in run-header ";
        *fLog << "(" << fRunHeader->GetNumSamplesHiGain() << ")" << endl;
        *fLog << " In case you are processing real data you have just found a bug." << endl;
        *fLog << " If you process Monte Carlo data, it means probably that the length" << endl;
        *fLog << " of the arrays in MRawEvtData are inconsistent with the run-header." << endl;
        return kERROR;
    }

    if (fRawEvt->GetNumLoGainSamples()+fRunHeader->GetNumSamplesLoGain()>0)
    {
        *fLog << err << "MDrsCalibApply::Process: ERROR - Data must not contain lo gain slices." << endl;
        return kERROR;
    }

    // initialize fSignal
    fSignal->InitSamples(roi);

    if (!fDrsCalib)  // FIXME: Do a fake (none) calibration
        return kTRUE;

    const int16_t *val = reinterpret_cast<int16_t*>(fRawEvt->GetSamples());

    const int32_t *offset = fDrsCalib->fOffset.data();
    const int64_t *gain   = fDrsCalib->fGain.data();
    const int64_t *trgoff = fDrsCalib->fTrgOff.data();

    const uint64_t scaleabs  = fDrsCalib->fNumOffset;
    const uint64_t scalegain = fDrsCalib->fNumGain;
    const uint64_t scalerel  = fDrsCalib->fNumTrgOff;

    const int16_t *start = reinterpret_cast<int16_t*>(fRawEvt->GetStartCells());

    Float_t *vec = fSignal->GetSamples(0);

    const UShort_t *idx = fRawEvt->GetPixelIds();
    for (size_t ch=0; ch<fRawEvt->GetNumPixels(); ch++)
    {
        const size_t drs = ch*1024;
        const size_t hw  = ch*roi;
        const size_t sw  = idx[ch]*roi;

        DrsCalibrate::ApplyCh(vec+sw, val+hw, start[ch], roi,
                              offset+drs, scaleabs,
                              gain  +drs, scalegain,
                              trgoff+hw,  scalerel);
    }

    if (fResult)
    {
        fResult->fData.resize(fPrevStart.size()*2);

        int i=0;
        for (auto it=fPrevStart.begin(); it!=fPrevStart.end(); it++)
        {
            fResult->fData[i++] = DrsCalibrate::CorrectStep(vec, fRawEvt->GetNumPixels(), roi, it->data(), start, roi+10, idx);
            fResult->fData[i++] = DrsCalibrate::CorrectStep(vec, fRawEvt->GetNumPixels(), roi, it->data(), start, 3,      idx);
        }

        fPrevStart.push_front(vector<Short_t>(start, start+fRawEvt->GetNumPixels()));
        if (fPrevStart.size()>fMaxNumPrevEvents)
            fPrevStart.pop_back();
    }

    switch (fRemoveSpikes)
    {
    case 1:
        for (size_t ch=0; ch<fRawEvt->GetNumPixels(); ch++)
            DrsCalibrate::RemoveSpikes(vec+ch*roi, roi);
        break;
    case 2:
        for (size_t ch=0; ch<fRawEvt->GetNumPixels(); ch++)
            DrsCalibrate::RemoveSpikes2(vec+ch*roi, roi);
        break;
    case 3:
        for (size_t ch=0; ch<fRawEvt->GetNumPixels(); ch++)
            DrsCalibrate::RemoveSpikes3(vec+ch*roi, roi);
        break;
    case 4:
        for (size_t ch=0; ch<fRawEvt->GetNumPixels(); ch++)
            DrsCalibrate::RemoveSpikes4(vec+ch*roi, roi);
        break;
    }


    if (fSlidingAverage)
        DrsCalibrate::SlidingAverage(vec, roi, fSlidingAverage);

    fSignal->SetReadyToSave();

    return kTRUE;
}
