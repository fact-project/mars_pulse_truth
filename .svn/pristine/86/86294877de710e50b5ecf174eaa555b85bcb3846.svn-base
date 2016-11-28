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
//   MTreatSaturation
//
//////////////////////////////////////////////////////////////////////////////
#include "MTreatSaturation.h"

#include <algorithm>

#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawRunHeader.h"
#include "MExtralgoSpline.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

#include "MPedestalSubtractedEvt.h"

ClassImp(MTreatSaturation);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MTreatSaturation::MTreatSaturation(const char *name, const char *title)
    : fRaw(0), fEvt(0)
{
    fName  = name  ? name  : "MTreatSaturation";
    fTitle = title ? title : "Base class for replacing saturation with pulses";
}

// --------------------------------------------------------------------------
//
Int_t MTreatSaturation::PreProcess(MParList *pList)
{
    fRaw = (MRawEvtData*)pList->FindObject("MRawEvtData");
    if (!fRaw)
    {
        *fLog << err << "MRawEvtData not found... aborting." << endl;
        return kFALSE;
    }

    fEvt = (MPedestalSubtractedEvt*)pList->FindObject("MPedestalSubtractedEvt");
    if (!fEvt)
    {
        *fLog << err << "MPedestalSubtractedEvt not found... aborting." << endl;
        return kFALSE;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MTreatSaturation::Process()
{
    const UInt_t fHiGainFirst =  50;
    const UInt_t fHiGainLast  = 225;

    const UInt_t npix = fEvt->GetNumPixels();

    const UInt_t rangehi = fHiGainLast - fHiGainFirst - 1;

    if (fDev1.GetSize()!=rangehi)
    {
        fDev1.Set(rangehi);
        fDev2.Set(rangehi);
    }

    for (UInt_t ch=0; ch<npix; ch++)
    {
        Float_t *ptr = fEvt->GetSamples(ch);

        const Float_t *pbeg = ptr+fHiGainFirst+1;
        const Float_t *pend = ptr+fHiGainLast-1;

        // Find maximum
        const Float_t *pmax = pbeg;
        for (const Float_t *p=pbeg; p<pend; p++)
            if (*p>*pmax)
                pmax = p;

        // Is there any chance for saturation?
        if (*pmax<1900)
            continue;

        // Determine a rough estimate for the average baseline
        double baseline = 0;
        for (const Float_t *p=ptr+10; p<ptr+50; p++)
            baseline += *ptr;
        baseline /= 40;

        // Do a spline interpolation to find the crossing of 1.8V
        // before and after the maximum (This is a level, I think
        // there is no saturation effect at all yet)
        MExtralgoSpline s(pbeg, rangehi, fDev1.GetArray(), fDev2.GetArray());
        s.SetExtractionType(MExtralgoSpline::kAmplitudeRel);

        const double leading = s.SearchYdn(pmax-pbeg, 1800);
        const double falling = s.SearchYup(pmax-pbeg, 1800);
        const double width   = falling-leading;

        // If the width above 1.8V is below 3 samples... go ahead as usual.
        if (width<3)
            continue;

        // Estimate the amplitude and the arrival time from the width
        const double amplitude = (1800-baseline)/(0.898417 - 0.0187633*width + 0.000163919*width*width - 6.87369e-7*width*width*width + 1.13264e-9*width*width*width*width);
        const double deltat    = -1.41371-0.0525846*width + 93.2763/(width+13.196);
        const double time0     = leading-deltat-1;

        const Int_t beg = TMath::FloorNint(leading);
        const Int_t end = TMath::CeilNint(falling);

        ptr += fHiGainFirst+1;
        for (Int_t i=beg-5; i<end+5; i++)
        {
            const double x = i-time0;
            const double v = amplitude*(1-1/(1+exp(x/2.14)))*exp(-x/38.8)+baseline;
            if (v>ptr[i])
                ptr[i] = v;
        }
    }

    return kTRUE;
}

