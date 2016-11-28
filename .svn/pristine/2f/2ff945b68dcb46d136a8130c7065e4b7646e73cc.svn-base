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
!   Author(s): Thomas Bretz, 02/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!              Hendrik Bartko, 01/2004 <mailto:hbartko@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractSignal2
//
//  Calculate the signal as the fWindowSize time slices which have the highest
// integral contents.
//
// 
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractSignal2.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

//#include "MArrivalTime.h"

ClassImp(MExtractSignal2);

using namespace std;

const Byte_t MExtractSignal2::fgSaturationLimit = 254;
const Byte_t MExtractSignal2::fgFirst      =  3;
const Byte_t MExtractSignal2::fgLast       = 14;
const Byte_t MExtractSignal2::fgWindowSize = 6;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
MExtractSignal2::MExtractSignal2(const char *name, const char *title)
  : fSaturationLimit(fgSaturationLimit)
{

    fName  = name  ? name  : "MExtractSignal2";
    fTitle = title ? title : "Task to extract the signal from the FADC slices";

    AddToBranchList("MRawEvtData.*");

    SetRange();
}

void MExtractSignal2::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t windowh, Byte_t lofirst, Byte_t lolast, Byte_t windowl)
{

    fNumHiGainSamples = hilast-hifirst+1;
    fNumLoGainSamples = lolast-lofirst+1;

    fHiGainFirst = hifirst;
    fLoGainFirst = lofirst;

    fWindowSizeHiGain = windowh & ~1;
    fWindowSizeLoGain = windowl & ~1;

    if (fWindowSizeHiGain != windowh)
      *fLog << warn << "MExtractSignal2::SetRange - Hi Gain window size has to be even, set to: " 
            << int(fWindowSizeHiGain) << " samples " << endl;
    
    if (fWindowSizeLoGain != windowl)
      *fLog << warn << "MExtractSignal2::SetRange - Lo Gain window size has to be even, set to: " 
            << int(fWindowSizeLoGain) << " samples " << endl;
    
    if (fWindowSizeHiGain<2) 
    {
      fWindowSizeHiGain = 2;
      *fLog << warn << "MExtractSignal2::SetRange - Hi Gain window size set to two samples" << endl;
    }

    if (fWindowSizeLoGain<2) 
    {
      fWindowSizeLoGain = 2;
      *fLog << warn << "MExtractSignal2::SetRange - Lo Gain window size set to two samples" << endl;
    }

    if (fWindowSizeHiGain > fNumHiGainSamples)
    {
      fWindowSizeHiGain = fNumHiGainSamples & ~1;
      *fLog << warn << "MExtractSignal2::SetRange - Hi Gain window size set to " 
            << int(fWindowSizeHiGain) << " samples " << endl;
    }
    
    if (fWindowSizeLoGain > fNumLoGainSamples)
    {
      fWindowSizeLoGain = fNumLoGainSamples & ~1;
      *fLog << warn << "MExtractSignal2::SetRange - Lo Gain window size set to " 
            << int(fWindowSizeLoGain) << " samples " << endl;
    }

    fWindowSqrtHiGain = TMath::Sqrt((Float_t)fWindowSizeHiGain);
    fWindowSqrtLoGain = TMath::Sqrt((Float_t)fWindowSizeLoGain);

}



// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawEvtData
//  - MPedestalCam
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MExtractedSignalCam
//
Int_t MExtractSignal2::PreProcess(MParList *pList)
{
    fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData"));
    if (!fRawEvt)
    {
        *fLog << err << AddSerialNumber("MRawEvtData") << " not found... aborting." << endl;
        return kFALSE;
    }


    fSignals = (MExtractedSignalCam*)pList->FindCreateObj(AddSerialNumber("MExtractedSignalCam"));
    if (!fSignals)
        return kFALSE;

    fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainFirst+fNumHiGainSamples-1, (Float_t)fWindowSizeHiGain,
                                fLoGainFirst, fLoGainFirst+fNumLoGainSamples-1, (Float_t)fWindowSizeLoGain);

    fPedestals = (MPedestalCam*)pList->FindObject(AddSerialNumber("MPedestalCam"));
    if (!fPedestals)
    {
        *fLog << err << AddSerialNumber("MPedestalCam") << " not found... aborting" << endl;
        return kFALSE;
    }
    return kTRUE;
}

void MExtractSignal2::FindSignal(Byte_t *ptr, Byte_t size, Byte_t window, Int_t &max, Int_t &sat) const
{
    const Byte_t *end = ptr + size;

    Int_t sum=0;

    //
    // Calculate the sum of the first fWindowSize slices
    //
    sat = 0;
    Byte_t *p = ptr;
    while (p<ptr+window)
    {
        sum += *p;
        if (*p++ >= fSaturationLimit)
            sat++;
    }

    //
    // Check for saturation in all other slices
    //
    while (p<end)
        if (*p++ >= fSaturationLimit)
            sat++;

    //
    // Calculate the i-th sum as
    //    sum_i+1 = sum_i + slice[i+8] - slice[i]
    // This is fast and accurate (because we are using int's)
    //
    max=sum;
    for (p=ptr; p+window<end; p++)
    {
        sum += *(p+window) - *p;
        if (sum>max)
            max = sum;
    }
}

// --------------------------------------------------------------------------
//
// Calculate the integral of the FADC of fWindowSize time slices which have the
// highest signal
//
Int_t MExtractSignal2::Process()
{
    MRawEvtPixelIter pixel(fRawEvt);
    fSignals->Clear();

    Int_t sat=0;
    while (pixel.Next())
    {
        //
        // Find signal in hi- and lo-gain
        //
        Int_t sumhi, sathi;
        FindSignal(pixel.GetHiGainSamples()+fHiGainFirst, fNumHiGainSamples, fWindowSizeHiGain, sumhi, sathi);

        Int_t sumlo=0;
        Int_t satlo=0;
        if (pixel.HasLoGain())
        {
            FindSignal(pixel.GetLoGainSamples()+fLoGainFirst, fNumLoGainSamples, fWindowSizeLoGain, sumlo, satlo);
            if (satlo)
                sat++;
        }

        //
        // Take correspodning pedestal
        //
        const Int_t pixid = pixel.GetPixelId();

        const MPedestalPix  &ped = (*fPedestals)[pixid];
        MExtractedSignalPix &pix = (*fSignals)[pixid];

        const Float_t pedes  = ped.GetPedestal();
        const Float_t pedrms = ped.GetPedestalRms();

        //
        // Set extracted signal with pedestal substracted
        //
        pix.SetExtractedSignal(sumhi - pedes*fWindowSizeHiGain, pedrms*fWindowSqrtHiGain,
                               sumlo - pedes*fWindowSizeLoGain, pedrms*fWindowSqrtLoGain);

        pix.SetGainSaturation(sathi, satlo);
    } /* while (pixel.Next()) */


    fSignals->SetReadyToSave();

    return kTRUE;
}
