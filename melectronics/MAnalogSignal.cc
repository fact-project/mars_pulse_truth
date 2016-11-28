/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz,  1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MAnalogSignal
//
// This is the equivalent to an analog signal. The signal is stored by
// a sampling in equidistant bins.
//
//////////////////////////////////////////////////////////////////////////////
#include "MAnalogSignal.h"

#include <TF1.h>
#include <TRandom.h>
#include <TObjArray.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MSpline3.h"
#include "MDigitalSignal.h"

#include "MExtralgoSpline.h"

ClassImp(MAnalogSignal);

using namespace std;

// ------------------------------------------------------------------------
//
// Set the array length and the length of the buffers.
//
void MAnalogSignal::Set(UInt_t n)
{
    // FIXME: Maybe we move this before initializing the spline
    // with a check?
    fDer1.Set(n);
    fDer2.Set(n);

    MArrayF::Set(n);
}

// ------------------------------------------------------------------------
//
// Evaluate the spline an add the result between t+xmin and t+xmax
// (xmin and xmax are the limits of the spline) to the signal.
// The spline is evaluated at the bin-center of the analog signal
// and multiplied by f.
//
// Return kTRUE if the full range of the spline could be added to the
// analog signal, kFALSE otherwise.
//
Bool_t MAnalogSignal::AddPulse(const MSpline3 &spline, Float_t t, Float_t f)
{
    // FIXME: This could be improved using a MExtralgoSpline with
    //        the identical stepping as the signal and we could use
    //        the integral instead of the pure y-value if we want.

    // Both in units of the sampling frequency
    const Float_t start = t+spline.GetXmin();
    const Float_t end   = t+spline.GetXmax();

    Int_t  first = TMath::CeilNint(start);
    UInt_t last  = TMath::CeilNint(end); // Ceil:< Floor:<=

    Bool_t rc = kTRUE;
    if (first<0)
    {
        first=0;
        rc = kFALSE;
    }
    if (last>GetSize())
    {
        last=GetSize();
        rc = kFALSE;
    }

    // FIXME: As soon as we have access to TSpline3::fPoly we can
    //        gain a lot in execution speed here.

    Float_t *arr = GetArray();
    for (UInt_t i=first; i<last; i++)
        arr[i] += spline.Eval(i-t)*f;

    return rc;
}

// ------------------------------------------------------------------------
//
// Evaluate the spline an add the result between t+xmin and t+xmax
// (xmin and xmax are the limits of the TF1) to the signal.
// The spline is evaluated at the bin-center of the analog signal
// and multiplied by f.
//
// Return kTRUE if the full range of the function could be added to the
// analog signal, kFALSE otherwise.
//
Bool_t MAnalogSignal::AddPulse(const TF1 &func, Float_t t, Float_t f)
{
    // Both in units of the sampling frequency
    const Float_t start = t+func.GetXmin();
    const Float_t end   = t+func.GetXmax();

    Int_t  first = TMath::CeilNint(start);
    UInt_t last  = TMath::CeilNint(end); // Ceil:< Floor:<=

    Bool_t rc = kTRUE;
    if (first<0)
    {
        first=0;
        rc = kFALSE;
    }
    if (last>GetSize())
    {
        last=GetSize();
        rc = kFALSE;
    }

    // FIXME: As soon as we have access to TSpline3::fPoly we can
    //        gain a lot in execution speed here.

    Float_t *arr = GetArray();
    for (UInt_t i=first; i<last; i++)
        arr[i] += func.Eval(i-t)*f;

    return rc;
}

// ------------------------------------------------------------------------
//
// Add a second analog signal. Just by addining it bin by bin.
//
void MAnalogSignal::AddSignal(const MAnalogSignal &s, Int_t delay,
                              Float_t dampingFactor )
{
    Add(s.GetArray(), s.fN, delay, dampingFactor);
}


// Deprecated. Use MSimRandomPhotons instead
void MAnalogSignal::AddRandomPulses(const MSpline3 &spline, Float_t num)
{
    // Average number (1./freq) of pulses per slice

    const Float_t start =   0   -spline.GetXmin();
    const Float_t end   = (fN-1)-spline.GetXmax();

    const UInt_t first = TMath::CeilNint(start);
    const UInt_t last  = TMath::CeilNint(end); // Ceil:< Floor:<=

    Double_t d = first;

    while (d<last)
    {
        d += gRandom->Exp(num);
        AddPulse(spline, d);
    }
}

// ------------------------------------------------------------------------
//
// Add a random gaussian with amplitude and offset to every bin
// of the analog signal. The default offset is 0. The default amplitude 1.
//
void MAnalogSignal::AddGaussianNoise(Float_t amplitude, Float_t offset)
{
    for (Float_t *ptr = GetArray(); ptr<GetArray()+fN; ptr++)
        *ptr += gRandom->Gaus(offset, amplitude);
}

// ------------------------------------------------------------------------
//
// The signal is evaluated using the spline MExtralgoSpline.
// Searching upwards from the beginning all points are calculated at
// which the spline is equal to threshold. After a rising edge
// a leading edge is searched. From this an MDigitalSignal is
// created and added to an newly created TObjArray. If len<0 then
// the signal length is equal to the time above threshold, otherwise
// the length is fixed to len. The start of the digital signal is the
// rising edge. If due to fixed length two digital signal overlap the
// digital signals are combined into one signal.
//
// For numerical reasons we have to avoid to find the same x-value twice.
// Therefor a "dead-time" of 1e-4 is implemented after each edge.
//
// The user is responsible of deleting the TObjArray.
//
TObjArray *MAnalogSignal::Discriminate(Float_t threshold, Double_t start, Double_t end, Float_t len) const
{
    TObjArray *ttl = new TObjArray;
    ttl->SetOwner();

    // The time after which we start searching for a falling or leading
    // edge at threshold after a leading or falling edge respectively.
    // This value has mainly numerical reasons. If starting the search
    // too early we might end up in an endless loop finding the same
    // value again and again. This just means that a glitch above or
    // below the threshold which is shorter than this value can
    // stay unnoticed. This shouldn't hurt.
    const Double_t deadtime = 1e-4;

    // FIXME: Are local maximum/minima handled correctly?

    const MExtralgoSpline sp(GetArray(), fN, fDer1.GetArray(), fDer2.GetArray());

    Double_t x1 = 0;
    Double_t x2 = start;  // Start searching at x2

    while (1)
    {
        // Search for the next rising edge (starting at x2)
        while (1)
        {
            x1 = sp.SearchYup(x2+deadtime, threshold);
            if (x1<0 || x1>=end)
                return ttl;

            const Bool_t rising = sp.Deriv1(x1)>0;
            if (rising)
                break;

            x2 = x1;
        }

        // Search for the next falling edge (starting at x1)
        while (1)
        {
            x2 = sp.SearchYup(x1+deadtime, threshold);
            if (x2<0)
                x2 = end;
            if (x2>=end)
                break;

            const Bool_t falling = sp.Deriv1(x2)<0;
            if (falling)
                break;

            x1 = x2;
        }

        // We found a rising and a falling edge
        MDigitalSignal *sig = new MDigitalSignal(x1, len>0?len:x2-x1);

        // In case of a fixed length we have to check for possible overlapping
        if (len>0 && ttl->GetEntriesFast()>0)
        {
            // FIXME: What if in such a case the electronics is just dead?
            MDigitalSignal *last = static_cast<MDigitalSignal*>(ttl->Last());
            // Combine both signals to one if they overlap
            if (last->Combine(*sig))
            {
                // Both signals overlap and have been combined into the existing one
                delete sig;
                continue;
            }
            // The signals don't overlap we add the new signal as usual
        }

        // Add the new signal to the list of signals
        ttl->Add(sig);
    }

    return ttl;
}
