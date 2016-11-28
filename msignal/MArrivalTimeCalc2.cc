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
!              Hendrik Bartko, 02/2004 <mailto:hbartko@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MArrivalTimeCalc2
//
//  Calculates the arrival time as the mean time of the fWindowSize time slices
//  which have the highest integral content.
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MArrivalTimeCalc2.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"


ClassImp(MArrivalTimeCalc2);

using namespace std;

const Byte_t MArrivalTimeCalc2::fgSaturationLimit = 254;
const Byte_t MArrivalTimeCalc2::fgFirst  =  0;
const Byte_t MArrivalTimeCalc2::fgLast   = 14;
const Byte_t MArrivalTimeCalc2::fgWindowSize = 6;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MArrivalTimeCalc2::MArrivalTimeCalc2(const char *name, const char *title)
  : fSaturationLimit(fgSaturationLimit)
{

    fName  = name  ? name  : "MArrivalTimeCalc2";
    fTitle = title ? title : "Task to extract the signal from the FADC slices";

    AddToBranchList("MRawEvtData.*");

    SetRange();
}

void MArrivalTimeCalc2::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast, Byte_t windowsize)
{
    fNumHiGainSamples = hilast-hifirst+1;
    fNumLoGainSamples = lolast-lofirst+1;

    fHiGainFirst = hifirst;
    fLoGainFirst = lofirst;

    fWindowSize = windowsize & ~1;

    if (fWindowSize != windowsize)
      *fLog << warn << "MArrivalTimeCalc2::SetRange - window size has to be even, set to: " << int(fWindowSize) << " samples " << endl;

    if (fWindowSize<2) 
    {
      fWindowSize = 2;
      *fLog << warn << "MArrivalTimeCalc2::SetRange - window size set to two samples" << endl;
    }

    if (fWindowSize > fNumHiGainSamples)
    {
      fWindowSize = fNumLoGainSamples & ~1;
      *fLog << warn << "MArrivalTimeCalc2::SetRange - window size set to " << int(fWindowSize) << " samples " << endl;
    }

    if (fWindowSize > fNumLoGainSamples)
    {
      fWindowSize = fNumLoGainSamples & ~1;
      *fLog << warn << "MArrivalTimeCalc2::SetRange - window size set to " << int(fWindowSize) << " samples " << endl;
    }

    fWindowSizeSqrt = TMath::Sqrt((Float_t)fWindowSize);
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
Int_t MArrivalTimeCalc2::PreProcess(MParList *pList)
{
    fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData"));
    if (!fRawEvt)
    {
        *fLog << err << AddSerialNumber("MRawEvtData") << " not found... aborting." << endl;
        return kFALSE;
    }

    fPedestals = (MPedestalCam*)pList->FindObject(AddSerialNumber("MPedestalCam"));
    if (!fPedestals)
    {
        *fLog << err << AddSerialNumber("MPedestalCam") << " not found... aborting" << endl;
        return kFALSE;
    }

    fArrivalTime = (MArrivalTimeCam*)pList->FindCreateObj(AddSerialNumber("MArrivalTimeCam"));
    if (!fArrivalTime)
        return kFALSE;

    return kTRUE;
}

void MArrivalTimeCalc2::FindSignalTime(Byte_t *ptr, Byte_t size, Float_t &time, Float_t &deltatime, Int_t &sat, Float_t pedes, Float_t pedrms) const
{
    const Byte_t *end = ptr + size;

    Int_t sum=0;    // integral content of the actual window
    Int_t max = 0;  // highest integral content of all windows

    //
    // Calculate the sum of the first fWindowSize slices
    //
    sat = 0;
    Byte_t *p = ptr;

    while (p<ptr+fWindowSize)
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
    Byte_t *ptrmax=ptr;  // pointer to the first slice of the maximum window

    for (p=ptr; p+fWindowSize<end; p++)
    {
        sum += *(p+fWindowSize) - *p;
		
	if (sum>max)
	{
	  max = sum;
	  ptrmax = p+1;
	}
    }

    // now calculate the time for the maximum window
    Int_t timesignalsum = 0;
    Int_t timesquaredsum =0;
    Int_t timesum =0;

    for (p=ptrmax; p < ptrmax + fWindowSize; p++)
    {
        timesignalsum += *p*(p-ptr);
        timesum += p-ptr;
        timesquaredsum  += (p-ptr)*(p-ptr);
    }

    const Float_t pedsubsum = max - fWindowSize*pedes;
    const Float_t pedsubtimesignalsum = timesignalsum - timesum*pedes;

    time      = pedsubsum != 0 ? pedsubtimesignalsum / pedsubsum : 1;
    deltatime = pedsubsum != 0 ? pedrms / pedsubsum * sqrt(timesquaredsum - fWindowSize*time) : 1;
}

// --------------------------------------------------------------------------
//
//  Calculates the arrival time as the mean time of the fWindowSize time slices
//  which have the highest integral content.
//
Int_t MArrivalTimeCalc2::Process()
{
    MRawEvtPixelIter pixel(fRawEvt);

    Int_t sat=0;
    while (pixel.Next())
    {
        //
        // Find signal in hi- and lo-gain
        //
        Float_t timehi, timelo, deltatimehi, deltatimelo;
        Int_t sathi, satlo;

	//
        // Take correspodning pedestal
        //
        const Int_t pixid = pixel.GetPixelId();

	const MPedestalPix  &ped = (*fPedestals)[pixid];

	MArrivalTimePix &pix = (*fArrivalTime)[pixid];

	const Float_t pedes  = ped.GetPedestal();
	const Float_t pedrms = ped.GetPedestalRms();

	FindSignalTime(pixel.GetHiGainSamples()+fHiGainFirst, fNumHiGainSamples, timehi, deltatimehi, sathi, pedes, pedrms);
        FindSignalTime(pixel.GetLoGainSamples()+fLoGainFirst, fNumLoGainSamples, timelo, deltatimelo, satlo, pedes, pedrms);

        if (satlo)
            sat++;

        pix.SetArrivalTime(timehi+ Float_t(fHiGainFirst), deltatimehi, timelo + Float_t(fLoGainFirst), deltatimelo); 
	pix.SetGainSaturation(sathi, satlo);
    }

    fArrivalTime->SetReadyToSave();

    //
    // Print a warning if event has saturationg lo-gains
    //
    //    if (sat)
    //        *fLog << warn << "WARNING - Lo Gain saturated in " << sat << " pixels." << endl;

    return kTRUE;
}
