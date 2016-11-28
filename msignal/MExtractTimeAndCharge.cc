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
!   Author(s): Markus Gaug, 05/2004 <mailto:markus@ifae.es>
!   Author(s): Thomas Bretz, 05/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractTimeAndCharge
//
//   Base class for the signal extractors which extract the arrival time 
//   and the signal at the same time. Uses the functions 
//   FindTimeAndChargeHiGain() and FindTimeAndChargeLoGain() to extract
//   the signal.
//
//   The following figure gives and example of possible inheritance trees. 
//   An extractor class can inherit from each of the following base classes:
//    - MExtractor
//    - MExtractTime
//    - MExtractTimeAndCharge
//
//Begin_Html
/*
<img src="images/ExtractorClasses.gif">
*/
//End_Html
//
//   The following variables have to be set by the derived class and 
//   do not have defaults:
//   - fNumHiGainSamples
//   - fNumLoGainSamples
//   - fSqrtHiGainSamples
//   - fSqrtLoGainSamples
//
//
// Class Version 3:
// ----------------
//   - Byte_t fMaxBinContent;
//
// Class Version 4:
// ----------------
//   - Byte_t fLoGainSwitch
//   + UInt_t fLoGainSwitch
//
//
// Input Containers:
//   MRawEvtData
//   MRawRunHeader
//   MPedestalCam
//
// Output Containers:
//   MArrivalTimeCam
//   MExtractedSignalCam
//
// For weired events check: Run 94127 Event 672, 1028
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractTimeAndCharge.h"

#include <TRandom.h>
#include <TVector3.h>

#include "MMath.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawRunHeader.h"
#include "MRawEvtPixelIter.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

#include "MPedestalSubtractedEvt.h"

ClassImp(MExtractTimeAndCharge);

using namespace std;

const Float_t MExtractTimeAndCharge::fgLoGainStartShift = -1.0;  // was -2.5
const UInt_t  MExtractTimeAndCharge::fgLoGainSwitch     =  120;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets: 
// - fWindowSizeHiGain and fWindowSizeLoGain to 0
// - fLoGainStartShift to fgLoGainStartShift
// - fLoGainSwitch     to fgLoGainSwitch
//
MExtractTimeAndCharge::MExtractTimeAndCharge(const char *name, const char *title)
    : fWindowSizeHiGain(0), fWindowSizeLoGain(0)
{
    fName  = name  ? name  : "MExtractTimeAndCharge";
    fTitle = title ? title : "Base class for signal and time extractors";

    SetLoGainStartShift();
    SetLoGainSwitch();
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
//  - MExtractedSignalCam
//  - MArrivalTimeCam    
//
Int_t MExtractTimeAndCharge::PreProcess(MParList *pList)
{
    if (!MExtractTime::PreProcess(pList))
        return kFALSE;

    fSignals = (MExtractedSignalCam*)pList->FindCreateObj("MExtractedSignalCam",AddSerialNumber(fNameSignalCam));
    if (!fSignals)
        return kFALSE;

    *fLog << flush << inf;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The ReInit calls:
// -  MExtractor::ReInit()
//
// Call: 
// - MArrivalTimeCam::SetUsedFADCSlices(fHiGainFirst, fHiGainLast, fNumHiGainSamples,
//                                      fLoGainFirst, fLoGainLast, fNumLoGainSamples);
// - InitArrays();
//
Bool_t MExtractTimeAndCharge::ReInit(MParList *pList)
{
    if (!MExtractTime::ReInit(pList))
        return kFALSE;

    if (!InitArrays(fRunHeader->GetNumSamplesHiGain()+fRunHeader->GetNumSamplesLoGain()))
        return kFALSE;

    if (fSignals)
        fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast, fNumHiGainSamples,
                                    fLoGainFirst, fLoGainLast, fNumLoGainSamples);

    if (!HasLoGain())
    {
        *fLog << inf << "No lo-gains... resetting lo-gain switch." << endl;
        fLoGainSwitch=0xff;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return the x-value lower than sat0 at which the signal has been
// fallen bwlow maxcont/2. This time is determined using a simple second
// order polynomial interpolation.
//
Double_t MExtractTimeAndCharge::GetSaturationTime(Int_t sat0, const Float_t *sig, Int_t maxconthalf) const
{
    const Int_t p = sat0>1 ? sat0-2 : sat0-1;
    if (sat0<=0)
        return 0;

    if (sat0==1)
        return sig[0]>maxconthalf ? 0 : 0.5;

    if (sig[p]>sig[p+1] || sig[p+1]>sig[p+2])
        return sig[p+1]>maxconthalf ? sat0-1 : sat0-0.5;

    // Find the place at which the signal is maxcont/2
    const TVector3 vx(sig[p], sig[p+1], sig[p+2]);
    const TVector3 vy(p, p+1, p+2);

    return MMath::InterpolParabLin(vx, vy, maxconthalf);
}

// --------------------------------------------------------------------------
//
// Calculate the integral of the FADC time slices and store them as a new
// pixel in the MArrivalTimeCam container.
// Calculate the integral of the FADC time slices and store them as a new
// pixel in the MExtractedSignalCam container. 
// The functions FindTimeAndChargeHiGain() and FindTimeAndChargeLoGain() are 
// supposed to extract the signal themselves.
//
Int_t MExtractTimeAndCharge::Process()
{
    const Int_t nums = fSignal->GetNumSamples();

    const Int_t numh = fRunHeader->GetNumSamplesHiGain();
    const Int_t numl = fRunHeader->GetNumSamplesLoGain();

    // Some sanity checks to get rid of some weird behaviour of the simulation
    if (nums!=numh+numl)
    {
        *fLog << err << "MExtractTimeAndCharge::Process: ERROR - Number of samples in event ";
        *fLog << "(" << nums << ")" << endl << " doesn't match number in run-header ";
        *fLog << "(" << numh+numl << ")" << endl;
        *fLog << " In case you are processing real data you have just found a bug." << endl;
        *fLog << " If you process Monte Carlo data, it means probably that the length" << endl;
        *fLog << " of the arrays in MRawEvtData are inconsistent with the run-header." << endl;
        return kERROR;
    }

    // Start extraction
    const UInt_t satlim = fSaturationLimit*fRunHeader->GetScale();

    // Do not even try to extract the hi-gain if we have
    // more than one saturating slice
    const Int_t rangehi = fHiGainLast - fHiGainFirst + 1;

    MRawEvtPixelIter pixel(fRawEvt);
    while (pixel.Next())
    {
        const Int_t pixidx = pixel.GetPixelId();

        const Float_t *sig = fSignal->GetSamples(pixidx);

        // Would it be better to take lastsat-firstsat?
        Int_t sathi0   = fHiGainFirst;  // First slice to extract and first saturating slice
        Int_t sathi1   = fHiGainLast;   // Last  slice to extract and last saturating slice
        Int_t numsathi = fSignal->GetSaturation(pixidx, satlim, sathi0, sathi1);

        Float_t sumhi =0., deltasumhi =-1; // Set hi-gain of MExtractedSignalPix valid
        Float_t timehi=0., deltatimehi=-1; // Set hi-gain of MArrivalTimePix valid

        if (numsathi<2)
        {
            const Int_t maxposhi = fSignal->GetMaxPos(pixidx, fHiGainFirst, fHiGainLast);
            FindTimeAndChargeHiGain2(sig+fHiGainFirst, rangehi,
                                     sumhi, deltasumhi, timehi, deltatimehi,
                                     numsathi, maxposhi);
        }

        // If we have saturating slices try to get a better estimate
        // of the arrival time than timehi or sathi0. This is
        // usefull to know where to start lo-gain extraction.
        const UInt_t maxcont = fSignal->GetRawMaxVal(pixidx, fHiGainFirst, fHiGainLast);
        if (numsathi>1)
        {
            timehi = GetSaturationTime(sathi0, sig, maxcont/2)-fHiGainFirst;
            deltatimehi = 0;
        }

        // Make sure that in cases the time couldn't be correctly determined
        // more meaningfull default values are assigned.
        // For extractors like the digital filter and the spline
        // we allow extracpolation by one slice.
        // FIXME: Defined Out-Of-Range better so that the extractors
        //        know what to return!
        // e.g. timehi=-2 in sline extractor means: leading edge not found not found
        if (deltatimehi>-0.5 && (timehi<-1 || timehi>=rangehi))
        {
            // Flag this as unreliable!
            timehi = gRandom->Uniform(rangehi);
            // deltatimehi=-1; // Set PIXEL to UNRELIABLE?
        }

        timehi += fHiGainFirst;

        Float_t sumlo =0, deltasumlo =-1;  // invalidate logain of MExtractedSignalPix
        Float_t timelo=0, deltatimelo=-1;  // invalidate logain of MArrivalTimePix
        Int_t numsatlo=0;

        //
        // Adapt the low-gain extraction range from the obtained high-gain time
        //

        // IN THIS CASE THE PIXEL SHOULD BE MARKED BAD!!!!
        // MEANS: Hi gain has saturated, but the signal is to dim
        // to extract the lo-gain properly
        // This could happen because the maxcont was not extracted from
        // all slices!
        // THIS produces pulse positions ~= -1
        // The signal might be handled in MCalibrateData, but hwat's about
        // the arrival times in MCalibrateRelTime
        if (numsathi>0 && maxcont<=fLoGainSwitch)
            deltasumlo=deltasumhi=deltatimelo=deltatimehi=-1;

        // If more than 8 hi-gain slices have saturated this is
        // no physical event. We just assume that something with
        // the extraction is wrong
        if (numsathi>8) // FIXME: Should be something like 2?
            deltasumhi=deltatimehi=-1;

        // FIXME: What to do with the pixel if it saturates too early???
        if (numl>0 && maxcont>fLoGainSwitch)
        {
            Int_t first = numh+fLoGainFirst;
            Int_t last  = numh+fLoGainLast;

            // To determin the window in which the lo-gain is extracted
            // clearly more information about the relation between the
            // extraction window and the reslting time is necessary.
            //
            // numh + fLoGainStartShift == 14 / fLoGainStartShift=-1
            //
            // The lo-gain is expected to have its raising edge
            // at timehi+numh+fOffsetLoGain. We start to search for the
            // lo-gain fLoGainStartShift slices earlier.
            //
            // Instead of fLoGainStartShift the extractor should now how many
            // slices before the expected raising edge the start of the
            // search must be placed and from there we can step 1.5 slices
            // back to be on the safe side.
            //
            // The jitter in the hi-/lo-gain offset ssems to be around +/-0.5
            const Float_t tm = deltatimehi<0 ? -1.+fHiGainFirst : timehi;
            first = TMath::FloorNint(tm+numh+fOffsetLoGain+fLoGainStartShift);

            if (first<0)
                first = 0;
            if (first>last)
                first=last;

            /*
            // Currently we have to stick to this check because at least
            // the spline has arrays of this size...
            if (first>last)
                first = last;
            if (first<numh+fLoGainFirst)
                first = numh+fLoGainFirst;
             */
            // Would it be better to take lastsat-firstsat?
            Int_t satlo0 = first;   // First slice to extract and first saturating slice
            Int_t satlo1 = last;    // Last  slice to extract and last saturating slice
            numsatlo = fSignal->GetSaturation(pixidx, satlim, satlo0, satlo1);

            //if (satlo0>first && satlo1<last && numsatlo>2)
            //{
            //    fSignal->InterpolateSaturation(pixidx, fSaturationLimit, satlo0, satlo1);
            //    numsatlo = 0;
            //}

            const Int_t rangelo  = last-first+1;
            const Int_t maxposlo = fSignal->GetMaxPos(pixidx, first, last);
            FindTimeAndChargeLoGain2(sig+first, rangelo,
                                     sumlo, deltasumlo, timelo, deltatimelo,
                                     numsatlo, maxposlo);

            // If we have saturating slices try to get a better estimate
            // of the arrival time than timehi or sathi0. This is
            // usefull to know where to start lo-gain extraction.
            if (numsatlo>1)
            {
                const UInt_t maxcontlo = fSignal->GetRawMaxVal(pixidx, fHiGainFirst, fHiGainLast);
                timelo = GetSaturationTime(satlo0, sig, maxcontlo/2)-numh-first;
                deltatimelo = 0;
            }

            // Make sure that in cases the time couldn't be correctly determined
            // more meaningfull default values are assigned
            // For extractors like the digital filter and the spline
            // we allow extracpolation by one slice.
            if (deltatimelo>-0.5 && (timelo<-1 || timelo>=rangelo))
            {
                // Flag this as unreliable!
                timelo = gRandom->Uniform(rangelo);
                //deltatimelo=-1; // Set PIXEL to UNRELIABLE?
            }

            timelo += first-numh;

            // If more than 6 lo-gain slices have saturated this is
            // no physical event. We just assume that something with
            // the extraction is wrong
            if (numsatlo>6)
                deltasumlo=deltatimelo=-1;

            // The extracted lo-gain signal cannot be zero or
            // negative at all, so it must be wrong
            if (sumlo<=0)
                deltasumlo=-1;

            //if (TMath::Abs(timelo-fOffsetLoGain - timehi)>1.0)
            //    deltatimelo = -1;
        }

        // Now store the result in the corresponding containers
        MExtractedSignalPix &pix = (*fSignals)[pixidx];
        MArrivalTimePix     &tix = (*fArrTime)[pixidx];
        pix.SetExtractedSignal(sumhi, deltasumhi, sumlo, deltasumlo);
        pix.SetGainSaturation(numsathi, numsatlo);

        tix.SetArrivalTime(timehi, deltatimehi, timelo-fOffsetLoGain, deltatimelo);
        tix.SetGainSaturation(numsathi, numsatlo);
    } /* while (pixel.Next()) */

    fArrTime->SetReadyToSave();
    fSignals->SetReadyToSave();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// In addition to the resources of the base-class MExtractor:
//   MJPedestal.MExtractor.LoGainStartShift: -2.8
//
Int_t MExtractTimeAndCharge::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = MExtractTime::ReadEnv(env, prefix, print);

    if (IsEnvDefined(env, prefix, "LoGainStartShift", print))
    {
        fLoGainStartShift = GetEnvValue(env, prefix, "LoGainStartShift", fgLoGainStartShift);
	SetLoGainStartShift(fLoGainStartShift);
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "LoGainSwitch", print))
    {
        fLoGainSwitch = GetEnvValue(env, prefix, "LoGainSwitch", (Int_t)fLoGainSwitch);
        rc = kTRUE;
    }

    return rc;
}

void MExtractTimeAndCharge::Print(Option_t *o) const
{
    MExtractTime::Print(o);

    if (HasLoGain())
    {
        *fLog << dec;
        *fLog << " LoGainStartShift:   " << fLoGainStartShift << endl;
        *fLog << " LoGainSwitch:       " << fLoGainSwitch << endl;
    }
}
