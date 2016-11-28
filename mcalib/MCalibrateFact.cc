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
!   Author(s): Thomas Bretz 10/2002 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2012
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCalibrateFact
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrateFact.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MTaskList.h"
#include "MSignalCam.h"
#include "MBadPixelsCam.h"
#include "MExtractedSignalPix.h"
#include "MExtractedSignalCam.h"

ClassImp(MCalibrateFact);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MCalibrateFact::MCalibrateFact(const char *name, const char *title)
    : fIn(0), fOut(0), fBadPixels(0),
    fNameExtractedSignalCam("MExtractedSignalCam"),
    fNameSignalCam("MSignalCam"),
    fScale(1)
{
    fName  = name  ? name  : "MCalibrateFact";
    fTitle = title ? title : "";
}

// --------------------------------------------------------------------------
//
//  Delete the filter if it was created automatically
//
MCalibrateFact::~MCalibrateFact()
{
}

// --------------------------------------------------------------------------
//
Int_t MCalibrateFact::PreProcess(MParList *plist)
{
    fIn = (MExtractedSignalCam*)plist->FindObject(fNameExtractedSignalCam, "MExtractedSignalCam");
    if (!fIn)
    {
        *fLog << err << fNameExtractedSignalCam << " [MExtractedSignalCam] not found... abort." << endl;
        return kFALSE;
    }

    fBadPixels = (MBadPixelsCam*)plist->FindCreateObj("MBadPixelsCam");
    if (!fBadPixels)
        return kFALSE;

    fOut = (MSignalCam*)plist->FindCreateObj("MSignalCam", fNameSignalCam);
    if (!fOut)
        return kFALSE;

    if (fScale!=1)
        *fLog << inf << "Using additional scale factor of " << fScale << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Bool_t MCalibrateFact::ReInit(MParList *plist)
{
    if (fCalibConst.GetSize()==0)
    {
        fCalibConst.Set(fIn->GetSize());
        fCalibConst.Reset(1);
    }

    if (UInt_t(fIn->GetSize()) != fCalibConst.GetSize())
    {
        *fLog << err << "Size mismatch." << endl;
        return kFALSE;
    }

    return kTRUE;
}

Int_t MCalibrateFact::Process()
{
    const UInt_t npix = fIn->GetSize();

    for (UInt_t idx=0; idx<npix; idx++)
    {
        const MExtractedSignalPix &sig = (*fIn)[idx];

        const Double_t signal = sig.GetExtractedSignalHiGain();
        const Bool_t   ok     = /*!sig.IsHiGainSaturated() &&*/ sig.IsHiGainValid();

        const Double_t nphot = signal * fCalibConst[idx] * fScale;
        fOut->AddPixel(idx, nphot, 0);

        if (!ok)
            (*fBadPixels)[idx].SetUnsuitable(MBadPixelsPix::kUnsuitableEvt);

        /*
        if (!ok)
        {
            if (sig.IsHiGainSaturated())
                numsathi++;

            if (sig.IsLoGainSaturated())
                numsatlo++;
        }*/
    }

    //fOut->SetNumPixelsSaturated(numsathi, numsatlo);
    //fOut->SetReadyToSave();

    return kTRUE;

}

