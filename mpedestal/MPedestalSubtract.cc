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
!   Author(s): Thomas Bretz, 10/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MPedestalSubtract
//
//  This class merges hi- and lo-gain samples into one array and
//  subtracts the pedestal (including the AB-offset) from the
//  data and stores the result in MPedestalSubtractedEvt.
//
// Input Containers:
//   MRawEvtData
//   MRawRunHeader
//   MPedestalCam
//
// Output Containers:
//   MPedestalSubtractedEvt
//
//////////////////////////////////////////////////////////////////////////////
#include "MPedestalSubtract.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MArrayB.h"

#include "MRawRunHeader.h"
#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MPedestalSubtractedEvt.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

ClassImp(MPedestalSubtract);

using namespace std;

const char *MPedestalSubtract::fgNamePedestalCam           = "MPedestalCam";
const char *MPedestalSubtract::fgNamePedestalSubtractedEvt = "MPedestalSubtractedEvt";

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MPedestalSubtract::MPedestalSubtract(const char *name, const char *title)
    : fRawEvt(NULL), fPedestals(NULL), fSignal(NULL)
{
    fName  = name  ? name  : "MPedestalSubtract";
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
//  - MPedestalSubtractedEvt
//
Int_t MPedestalSubtract::PreProcess(MParList *pList)
{
    fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData"));
    if (!fRawEvt)
    {
        *fLog << err << AddSerialNumber("MRawEvtData") << " not found... aborting." << endl;
        return kFALSE;
    }

    fSignal = (MPedestalSubtractedEvt*)pList->FindCreateObj("MPedestalSubtractedEvt");//, AddSerialNumber(fNamePedestalSubtractedEvt));
    if (!fSignal)
        return kFALSE;

    if (fPedestals)
    {
        *fLog << inf << "Pedestals given by pointer will be subtracted." << endl;
        return kTRUE;
    }

    if (fNamePedestalCam.IsNull())
    {
        *fLog << inf << "No name for MPedestalCam given, pedestal subtraction will be skipped." << endl;
        return kTRUE;
    }

    fPedestals = (MPedestalCam*)pList->FindObject(AddSerialNumber(fNamePedestalCam), "MPedestalCam");
    if (!fPedestals)
    {
        *fLog << err << AddSerialNumber(fNamePedestalCam) << " [MPedestalCam] not found... aborting" << endl;
        return kFALSE;
    }

    *fLog << inf << "Pedestals " << fNamePedestalCam << " will be subtracted." << endl;

    return kTRUE;
}

void MPedestalSubtract::Memcpy(void *dest, void *src, Int_t cnt) const
{
    if (fRawEvt->GetNumBytesPerSample()==2)
        memcpy(dest, src, cnt*2);
    else
    {
        const Byte_t *b = (Byte_t*)src;
        for (USample_t *ptr=(USample_t*)dest; ptr<(USample_t*)dest+cnt; ptr++)
            *ptr = *b++;
    }
}

Bool_t MPedestalSubtract::ReInit(MParList *pList)
{
    fRunHeader = (MRawRunHeader*)pList->FindObject(AddSerialNumber("MRawRunHeader"));
    if (!fRunHeader)
    {
        *fLog << err << AddSerialNumber("MRawRunHeader") << " not found... aborting." << endl;
        return kFALSE;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Int_t MPedestalSubtract::Process()
{
    // Please note:
    //   - for data with only hi-gain samples numl is 0
    //   - for data with hi- and lo-gain samples
    //     numl is 0 if read from a raw-data file or a new MC root-file(?)
    //     numl is not 0 if read from an old MC root-file

    // Total number of samples
    const Int_t numh = fRawEvt->GetNumHiGainSamples();
    const Int_t numl = fRawEvt->GetNumLoGainSamples();

    // Check if event is empty (presumably MC event -- sanity check)
    if (numh+numl==0)
        return kCONTINUE;

    // Check for consistency (our simulation can do weird things!)
    if (numh+numl!=fRunHeader->GetNumSamplesHiGain()+fRunHeader->GetNumSamplesLoGain())
    {
        *fLog << err << "MPedestalSubtract::Process: ERROR - Number of samples in event ";
        *fLog << "(hi+lo=" << numh+numl << ")" << endl << " doesn't match number in run-header ";
        *fLog << "(" << fRunHeader->GetNumSamplesHiGain()+fRunHeader->GetNumSamplesLoGain() << ")" << endl;
        *fLog << " In case you are processing real data you have just found a bug." << endl;
        *fLog << " If you process Monte Carlo data, it means probably that the length" << endl;
        *fLog << " of the arrays in MRawEvtData are inconsistent with the run-header." << endl;
        return kERROR;
    }

    // Get scale between FADC units and 256 ;-)
    const UInt_t scale = fRawEvt->GetScale();

    // initialize fSignal
    fSignal->InitSamples(numh+numl);//, fRawEvt->GetNumPixels(), numh+numl);

    // iterate over all pixels
    MRawEvtPixelIter pixel(fRawEvt);
    while (pixel.Next())
    {
        // Get index ofthis pixel
        const Int_t pixidx = pixel.GetPixelId();

        if (pixidx>=fSignal->GetNumPixels())
        {
            *fLog << err << "ERROR - Pixel index " << pixidx << " out of bounds... abort." << endl;
            *fLog <<        "        Index from MRawEvtPixelIter is " << pixidx << ", max in MPedestalSubtractedEvt is " << fSignal->GetNumPixels() << endl;
            return kERROR;
        }
        // Get pointer were to store merged raw data
        USample_t *sample = fSignal->GetSamplesRaw(pixidx);

        // copy hi- and lo-gains samples together
        Memcpy(sample,      pixel.GetHiGainSamples(), numh);
        Memcpy(sample+numh, pixel.GetLoGainSamples(), numl);

        // start of destination array, end of hi-gain destination array
        // and start of hi-gain samples
        Float_t *beg = fSignal->GetSamples(pixidx);
        Float_t *end = beg + fSignal->GetNumSamples();

        const USample_t *src = sample;

        // if no pedestals are given just convert the data into
        // floats and we are finished
        if (!fPedestals)
        {
            while (beg<end)
                *beg++ = *src++;//Float_t(*src++)/scale;
            continue;
        }

        // get pedestal information for this pixel
        const MPedestalPix &pedpix = (*fPedestals)[pixidx];

        // pedestal information
        const Int_t   ab  = pixel.HasABFlag() ? 1 : 0;
        const Float_t ped = pedpix.GetPedestal();

        // determine with which pedestal (+/- AB offset) to start
        const Bool_t  swap    = (ab&1)==1;
        const Float_t offh    = swap ? -pedpix.GetPedestalABoffset() : pedpix.GetPedestalABoffset();
        const Float_t mean[2] = { ped + offh, ped - offh };

        // Copy hi-gains into array and substract pedestal
        // FIXME: Shell we really subtract the pedestal from saturating slices???
        for (Float_t *ptr=beg; ptr<end; ptr++)
            *ptr = Float_t(*src++)/scale - mean[(ptr-beg)&1];
    }

    return kTRUE;
}
