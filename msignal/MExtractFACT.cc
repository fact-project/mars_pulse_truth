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
!   Author(s): Thomas Bretz, 2014 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2014
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractFACT
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractFACT.h"

#include <algorithm>

#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawRunHeader.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

#include "MPedestalSubtractedEvt.h"

ClassImp(MExtractFACT);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets: 
// - fWindowSizeHiGain and fWindowSizeLoGain to 0
// - fLoGainStartShift to fgLoGainStartShift
// - fLoGainSwitch     to fgLoGainSwitch
//
MExtractFACT::MExtractFACT(const char *name, const char *title)
{
    fName  = name  ? name  : "MExtractFACT";
    fTitle = title ? title : "Base class for signal and time extractors";
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
Int_t MExtractFACT::PreProcess(MParList *pList)
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
Bool_t MExtractFACT::ReInit(MParList *pList)
{
    if (!MExtractTime::ReInit(pList))
        return kFALSE;

    //if (fSignals)
    //    fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainLast, fNumHiGainSamples,
    //                                fLoGainFirst, fLoGainLast, fNumLoGainSamples);

    return kTRUE;
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
Int_t MExtractFACT::Process()
{
    // FIXME: Check ranges
    //const UInt_t nroi = fSignal->GetNumSamples();
    const UInt_t npix = fSignal->GetNumPixels();

    const UInt_t rangehi = fHiGainLast - fHiGainFirst - 1;

    for (UInt_t ch=0; ch<npix; ch++)
    {
        const Float_t *ptr = fSignal->GetSamples(ch);

        const Float_t *pbeg = ptr+fHiGainFirst+1;
        const Float_t *pend = ptr+fHiGainLast-1;

        // Find maximum
        const Float_t *pmax = pbeg;

        if (!IsNoiseCalculation())
        {
            for (const Float_t *p=pbeg; p<pend; p++)
                if (*p>*pmax)
                    pmax = p;
        }

        //Float_t max = *pmax;

        // -------------------------------------------------------------------------
        // Calculate a parabola through this and the surrounding points
        // on logarithmic values (that's a gaussian)
        //
        // Quadratic interpolation
        //
        // calculate the parameters of a parabula such that
        //    y(i)  = a + b*x(i) +   c*x(i)^2
        //    y'(i) =     b      + 2*c*x(i)
        //
        // y = exp( - (x-k)^2 / s^2 / 2 )
        //
        // -2*s^2 * log(y) = x^2 - 2*k*x + k^2
        //
        // a = (k/s0)^2/2
        // b = k/s^2
        // c = -1/(2s^2)      -->    s = sqrt(-1/2c)
        //
        Float_t max  = -1;
        Float_t tmax  = -1;
        if (pmax>pbeg && pmax<pend-1)
        {
            const Float_t &v1 = pmax[-1];
            const Float_t &v2 = pmax[ 0];
            const Float_t &v3 = pmax[+1];
            if (v1>0 && v2>0 && v3>0)
            {
                const double y1 = log(v1);
                const double y2 = log(v2);
                const double y3 = log(v3);

                const double a = y2;
                const double b = (y3-y1)/2;
                const double c = (y1+y3)/2 - y2;
                if (c<0)
                {
                    const double w  = -1./(2*c);
                    const double dx =  b*w;

                    if (dx>=-1 && dx<=1)
                    {
                        max = exp(a + b*dx + c*dx*dx);

                        // tmax is position of maximum
                        tmax = dx;
                        tmax += Int_t(pmax-ptr);
                    }
                }
            }
        }

        Float_t time = -1;
        Float_t slope = -1;
        if (max>=0)
        {
            // -10: maximum search window

            // Time is position of half hight leading edge
            pend = std::max(pbeg, pmax-15);
            for (;pmax>=pend; pmax--)
                if (*pmax<max/2)
                    break;

            if (pmax>pend && pmax[0]!=pmax[1])
            {
                time = (max/2-pmax[0])/(pmax[1]-pmax[0]);
                time += Int_t(pmax-ptr);
                slope = tmax - time;
            }
        }

        if (time<0)
        {
            time = gRandom->Uniform(rangehi)+fHiGainFirst+1;
            max  = ptr[uint16_t(time)];
        }

        // Now store the result in the corresponding containers
        MExtractedSignalPix &pix = (*fSignals)[ch];
        MArrivalTimePix     &tix = (*fArrTime)[ch];
        pix.SetExtractedSignal(max);
        pix.SetGainSaturation(0);

        tix.SetArrivalTime(time, slope);
        tix.SetGainSaturation(0);
    }

    fArrTime->SetReadyToSave();
    fSignals->SetReadyToSave();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// In addition to the resources of the base-class MExtractor:
//   MJPedestal.MExtractor.LoGainStartShift: -2.8
//
Int_t MExtractFACT::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = MExtractTime::ReadEnv(env, prefix, print);
    /*
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
    */
    return rc;
}

