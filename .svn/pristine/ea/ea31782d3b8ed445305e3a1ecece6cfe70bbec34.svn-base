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
!   Author(s): Thomas Bretz  4/2007 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMoviePrepare
//
// Prepare the splines for displaying of a movie. The data is shifted by
// the calculated time offset from MCalibrationRelTimeCam. It is
// flat-fielded by multiplication with the calibration factor from
// MCalibrateData divided by the median of the calibration factors of the
// pixels with area index 0 (for MAGIC: inner pixels).
//
// The splines are initialized for the first slice given to the last
// slice given (included).
//
// By default (MJCalibrateSignal.cc) the same slices than for the hi-gain
// extraction range are used. To overwrite this behaviour use
//
//    MMoviewPrepare.FirstSlice: 10
//    MMoviewPrepare.LastSlice:  50
//
// The maximum y-value set as knot is stored together with the splines
// in MMovieData
//
// Input:
//   MGeomCam
//   MPedestalSubtractedEvt
//   MPedestalFundamental [MPedestalCam]
//   MCalibConstCam
//   MCalibrationRelTimeCam
//   MRawRunHeader
//
// Output:
//   MMovieData
//
/////////////////////////////////////////////////////////////////////////////
#include "MMoviePrepare.h"

#include <TGraph.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"

#include "MParList.h"
#include "MTaskList.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MCalibConstCam.h"
#include "MCalibConstPix.h"

#include "MCalibrationRelTimeCam.h"
#include "MCalibrationRelTimePix.h"

#include "MExtractor.h"
#include "MMovieData.h"
#include "MRawRunHeader.h"
#include "MPedestalSubtractedEvt.h"

ClassImp(MMoviePrepare);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Initialize fData with default rule "MMcEvt.fEnergy"
//
MMoviePrepare::MMoviePrepare(const char *name, const char *title)
    : fFirstSlice(2), fLastSlice(12)
{
    fName  = name  ? name  : "MMoviePrepare";
    fTitle = title ? title : "Task to calculate a MParameterD";
}

// --------------------------------------------------------------------------
//
// Set moview range from extractor
//
void MMoviePrepare::SetRangeFromExtractor(const MExtractor &ext)
{
    fFirstSlice = ext.GetHiGainFirst();
    fLastSlice  = ext.GetHiGainLast();
}

// --------------------------------------------------------------------------
//
// Input:
//   MGeomCam
//   MPedestalSubtractedEvt
//   MPedestalFundamental [MPedestalCam]
//   MCalibrateData
//   MCalibrationRelTimeCam
//   MRawRunHeader
//
// Output:
//   MMovieData
//
Int_t MMoviePrepare::PreProcess(MParList *plist)
{
    fCam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fCam)
    {
        *fLog << err << "MGeomCam not found ... aborting." << endl;
        return kFALSE;
    }
    fEvt = (MPedestalSubtractedEvt*)plist->FindObject("MPedestalSubtractedEvt");
    if (!fEvt)
    {
        *fLog << err << "MPedestalSubtractedEvt not found ... aborting." << endl;
        return kFALSE;
    }
    fPed = (MPedestalCam*)plist->FindObject("MPedestalFundamental", "MPedestalCam");
    if (!fPed)
    {
        *fLog << err << "MPedestalFundamental [MPedestalCam] not found ... aborting." << endl;
        return kFALSE;
    }
    fCal = (MCalibConstCam*)plist->FindObject("MCalibConstCam");
    if (!fCal)
    {
        *fLog << err << "MCalibConstCam not found ... aborting." << endl;
        return kFALSE;
    }
    fRel = (MCalibrationRelTimeCam*)plist->FindObject("MCalibrationRelTimeCam");
    if (!fRel)
    {
        *fLog << err << "MCalibrationRelTimeCam not found ... aborting." << endl;
        return kFALSE;
    }
    fRun = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!fRun)
    {
        *fLog << err << "MRawRunHeader not found ... aborting." << endl;
        return kFALSE;
    }

    fOut = (MMovieData*)plist->FindCreateObj("MMovieData");
    if (!fOut)
        return kFALSE;

    return kTRUE;
}

/*
Bool_t MMoviePrepare::ReInit(MParList *plist)
{
    MExtractedSignalCam *cam = (MExtractedSignalCam*)plist->FindObject("MExtractedSignalCam");
    if (!cam)
    {
        *fLog << err << "MExtractedSignalCam not found ... aborting." << endl;
        return kFALSE;
    }
    fSlope = cam->GetNumUsedHiGainFADCSlices();

    cout << "---> " << fSlope << " <---" << endl;

    return kTRUE;
}
*/

// --------------------------------------------------------------------------
//
// Return the median of the calibration constants of all pixels
// with area index 0
//
Double_t MMoviePrepare::GetMedianCalibConst() const
{
    Int_t n = fCam->GetNumPixWithAidx(0);

    MArrayF arr(n);

    for (UInt_t i=0; i<fCam->GetNumPixels(); i++)
        if ((*fCam)[i].GetAidx()==0)
            arr[--n] = (*fCal)[i].GetCalibConst();

    return TMath::Median(arr.GetSize(), arr.GetArray());
}

// --------------------------------------------------------------------------
//
// Return the median of the pedestal rms of all pixels with area index 0
//
Double_t MMoviePrepare::GetMedianPedestalRms() const
{
    Int_t n = fCam->GetNumPixWithAidx(0);

    MArrayF rms(n);

    for (UInt_t i=0; i<fCam->GetNumPixels(); i++)
        if ((*fCam)[i].GetAidx()==0)
            rms[--n] = (*fPed)[i].GetPedestalRms();

    return TMath::Median(rms.GetSize(), rms.GetArray());
}

// --------------------------------------------------------------------------
//
// Get value from fData and set it to fEnergy. SetReadyToSave for fEnergy.
// Return kCONTINUE if value is NaN (Not a Number)
//
Int_t MMoviePrepare::Process()
{
    if (fLastSlice>=fEvt->GetNumSamples())
    {
        fLastSlice = fEvt->GetNumSamples()-1;
        *fLog << inf << "Cannot go beyond last slice... set last=" << fLastSlice << "." << endl;
    }

    // ---------------- Setup ------------------
    const UInt_t  width = fLastSlice-fFirstSlice+1;     // [slices] width of window shown
    const Float_t freq = fRun->GetFreqSampling()/1000.; // [GHz] Sampling frequency
    const Float_t len  = width/freq;                    // [ns]  length of data stream in data-time

    // ---------------- Prepare data ------------------

    fOut->Reset();

    // For invalid calib constants we can use the sector average
    const UShort_t npix  = fEvt->GetNumPixels();
    const Double_t slope = 1./GetMedianCalibConst();

    TGraph g(width);

    Float_t max = 0;
    for (UShort_t p=0; p<npix; p++)
    {
        const MCalibrationRelTimePix &tpix = (MCalibrationRelTimePix&)(*fRel)[p];

        const Float_t  offset = tpix.GetTimeOffset();
        const Float_t  cal    = (*fCal)[p].GetCalibConst();
        const Float_t *ptr    = fEvt->GetSamples(p)+fFirstSlice;
        const Float_t  scale  = cal*slope*fCam->GetPixRatio(p);

        for (UInt_t s=0; s<width; s++)
        {
            const Double_t x = (s-offset)/freq;
            const Double_t y = ptr[s]*scale;

            g.SetPoint(s, x, y);

            // Check for maximum in the visible time range only
            if (x>0 && x<len && y>max)
                max=y;
        }

        g.SetName(MString::Format("%d", p));
        fOut->Add(g);
    }

    fOut->SetMax(max);
    fOut->SetNumSlices(width);
    fOut->SetMedianPedestalRms(GetMedianPedestalRms());

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Check for corresponding entries in resource file and setup
//
// Example:
//   MMoviePrepare.FirstSlice: 10
//   MMoviePrepare.LastSlice:  50
//
Int_t MMoviePrepare::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "FirstSlice", print))
    {
        rc = kTRUE;
        fFirstSlice = GetEnvValue(env, prefix, "FirstSlice", (Int_t)fFirstSlice);
    }
    if (IsEnvDefined(env, prefix, "LastSlice", print))
    {
        rc = kTRUE;
        fLastSlice = GetEnvValue(env, prefix, "LastSlice", (Int_t)fLastSlice);
    }

    return rc;
}
