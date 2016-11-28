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
!   Author(s): Thomas Bretz 2012 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2013
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHDrsCalibration
//
///////////////////////////////////////////////////////////////////////
#include "MHDrsCalibrationCheck.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawRunHeader.h"
#include "MHCamera.h"
#include "MPedestalSubtractedEvt.h"

ClassImp(MHDrsCalibrationCheck);

using namespace std;

// --------------------------------------------------------------------------
//
//  default constructor
//  creates an a list of histograms for all pixels and both gain channels
//
MHDrsCalibrationCheck::MHDrsCalibrationCheck(const char *name, const char *title) :
    fEvt(0)
{
    //
    // set the name and title of this object
    //
    fName  = name  ? name  : "MHDrsCalibrationCheck";
    fTitle = title ? title : "Container for ADC spectra histograms";
}

// --------------------------------------------------------------------------
//
// To setup the object we get the number of pixels from a MGeomCam object
// in the Parameter list.
//
Bool_t MHDrsCalibrationCheck::SetupFill(const MParList *pList)
{
    SetTitle("Noise (DRS calibration applied);;ADC signal [mV];");

    if (!MHCamEvent::SetupFill(pList))
        return kFALSE;

    fSum->ResetBit(MHCamera::kProfile);

    *fLog << warn << "WARNING --- oulier detection needed!" << endl;

    return kTRUE;
}

Bool_t MHDrsCalibrationCheck::ReInit(MParList *pList)
{
    MRawRunHeader *h = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!h)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fNumSamples = h->GetNumSamples();
    fNumPixels  = h->GetNumPixel();

    fEvt = (MPedestalSubtractedEvt*)pList->FindObject("MPedestalSubtractedEvt");
    if (!fEvt)
    {
        *fLog << err << "MPedestalSubtractedEvt not found... aborting." << endl;
        return kFALSE;
    }

    fSum1.resize(fNumPixels*fNumSamples);
    fSum2.resize(fNumPixels*fNumSamples);

    return kTRUE;
}

// --------------------------------------------------------------------------

Int_t MHDrsCalibrationCheck::Fill(const MParContainer *par, const Stat_t w)
{
    for (size_t ch=0; ch<fNumPixels*fNumSamples; ch++)
    {
        const double val = fEvt->GetSamples(0)[ch];

        fSum1[ch] += val;
        fSum2[ch] += val*val;
    }

    return kTRUE;
}


void MHDrsCalibrationCheck::InitHistogram()
{
    if (!fEvt)
        return;

    fSum->Reset();

    for (size_t ch=0; ch<fNumPixels; ch++)
    {
        double m = 0;
        double r = 0;
        for (size_t i=0; i<fNumSamples; i++)
        {
            const int idx = i + ch*fNumSamples;

            const double avg  = fSum1[idx] / GetNumExecutions();
            const double avg2 = fSum2[idx] / GetNumExecutions();
            const double rms  = sqrt(avg2 - avg*avg);

            m += avg;
            r += rms;
        }

        fSum->AddBinContent(ch+1, m);
        fSum->SetBinError(ch+1,   r);
    }

    fSum->Scale(1./fNumSamples);  // 0.5mV/ADC count
    fSum->SetEntries(GetNumExecutions());
    fSum->SetAllUsed();
}

Bool_t MHDrsCalibrationCheck::Finalize()
{
    InitHistogram();

    return kTRUE;
}

void MHDrsCalibrationCheck::Paint(Option_t *o)
{
    InitHistogram();
    MHCamEvent::Paint(o);
}
