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
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MPedestalSubtractedEvt
//
//  Storage container to store the raw FADC values.
//
/////////////////////////////////////////////////////////////////////////////
#include "MPedestalSubtractedEvt.h"

#include <TMath.h>

#include "MLogManip.h"

ClassImp(MPedestalSubtractedEvt);

using namespace std;

// --------------------------------------------------------------------------
//
// Initialize number of samples (per pixel) and number of pixels.
//
// Initialize the correct length of fSamples and fSamplesRaw
//
// And reset its contents to 0.
//
void MPedestalSubtractedEvt::InitSamples(UInt_t samples, UInt_t pixels)
{
    fNumSamples = samples;

    if (pixels>0)
        fNumPixels = pixels;

    fSamples.Set(fNumPixels*fNumSamples);
    fSamplesRaw.Set(fNumPixels*fNumSamples);

    fSamples.Reset();
    fSamplesRaw.Reset();
}

// --------------------------------------------------------------------------
//
// Return a pointer to the first slice with subtracted pedestal of
// the samples of the pixel with given pixel-index. If the number
// exceeds the number of pixels NULL is returned.
//
// The user is responsible not to exceed the slices for one pixel!
//
Float_t *MPedestalSubtractedEvt::GetSamples(UInt_t pixel) const
{
    return pixel>=fNumPixels ? NULL : fSamples.GetArray()+pixel*fNumSamples;
}

// --------------------------------------------------------------------------
//
// Return a pointer to the first slice of the raw-data samples of the pixel
// with given pixel-index. If the number exceeds the number of
// pixels NULL is returned.
//
// The user is responsible not to exceed the slices for one pixel!
//
USample_t *MPedestalSubtractedEvt::GetSamplesRaw(UInt_t pixel) const
{
    return pixel>=fNumPixels ? NULL : fSamplesRaw.GetArray()+pixel*fNumSamples;
}

// --------------------------------------------------------------------------
//
// Return some information about saturation in the raw-data of pixel idx.
//
// The search range is defined by [first,last]. Saturation is considered if
// contents is >= limit.
//
// The number of saturating slices are returned and first/last are filled
// with the first and last saturating slice index w.r.t. the beginning of
// the raw-data of this pixel not first.
//
// Warning: No range checks and no sanity checks are done!
//
Int_t MPedestalSubtractedEvt::GetSaturation(const Int_t idx, Int_t limit, Int_t &first, Int_t &last) const
{
    // Determin saturation of hi-gains
    USample_t *p0 = GetSamplesRaw(idx);

    USample_t *sat0 = 0; // first saturating slice
    USample_t *sat1 = 0; // last  saturating slice

    Int_t num = 0;

    const USample_t *end = p0+last;
    for (USample_t *ptr=p0+first; ptr<=end; ptr++)
    {
        if (*ptr>=limit)
        {
            sat1 = ptr;
            if (!sat0)
                sat0 = ptr;
            num++;
        }
    }

    last  = sat1 ? sat1-p0 : -1;
    first = sat0 ? sat0-p0 : -1;

    return num;
}

void MPedestalSubtractedEvt::GetStat(const Int_t idx, Float_t &mean, Float_t &rms) const
{
    if (fNumSamples<20)
        return;

    // Get pointer to first slice to be considered
    Float_t const *sam = GetSamples(idx);
    Float_t const *beg = sam;

    Double_t sum  = 0;
    Double_t sum2 = 0;

    for (const Float_t *ptr=beg+10; ptr<sam+fNumSamples-10; ptr++)
    {
        sum  += *ptr;
        sum2 += *ptr * *ptr;
    }

    sum  /= fNumSamples-20;
    sum2 /= fNumSamples-20;

    mean = sum;
    rms  = TMath::Sqrt(sum2 - sum * sum);
}

// --------------------------------------------------------------------------
//
// Get the maximum of the pedestal subtracted slices [first,last] of
// pixel with index idx.
//
// The position returned is the index of the position of the pedestal
// subtracted maximum w.r.t. to first.
// The value returned is the maximum corresponding to this index.
//
// Warning: No range checks and no sanity checks are done!
//
Int_t MPedestalSubtractedEvt::GetMax(const Int_t idx, const Int_t first, const Int_t last, Float_t &val) const
{
    // Get pointer to first slice to be considered
    Float_t const *sam = GetSamples(idx);

    Float_t const *beg = sam+first;

    // The best information so far: the first slice is the maximum
    const Float_t *max = beg;

    for (const Float_t *ptr=beg+1; ptr<=sam+last; ptr++)
        if (*ptr>*max)
            max = ptr;

    val = *max;
    return max-beg;
}

// --------------------------------------------------------------------------
//
// Get the maximum of the raw slices [first,last] of pixel with index idx.
//
// The position returned is the index of the position of the raw-data
// w.r.t. to first.
// The value returned is the maximum corresponding to this index.
//
// Warning: No range checks and no sanity checks are done!
//
Int_t MPedestalSubtractedEvt::GetRawMax(const Int_t idx, const Int_t first, const Int_t last, UInt_t &val) const
{
    // Get pointer to first slice to be considered
    USample_t const *sam = GetSamplesRaw(idx);

    USample_t const *beg = sam+first;

    // The best information so far: the first slice is the maximum
    const USample_t *max = beg;

    for (const USample_t *ptr=beg+1; ptr<=sam+last; ptr++)
        if (*ptr>*max)
            max = ptr;

    val = *max;
    return max-beg;
}

void MPedestalSubtractedEvt::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << endl;
    *fLog << " Num Pixels:  " << fNumPixels << " (" << fNumSamples << " samples)" << endl;
    *fLog << " Samples raw:" << hex << endl;;
    for (UInt_t idx=0; idx<fNumPixels; idx++)
    {
        *fLog << setw(4) << dec << idx << hex << ":";
        for (UInt_t i=0; i<fNumSamples; i++)
            *fLog << " " << fSamplesRaw[idx*fNumSamples+i];
        *fLog << endl;
    }
    *fLog << dec << endl;
    *fLog << " Samples:" << endl;;
    for (UInt_t idx=0; idx<fNumPixels; idx++)
    {
        *fLog << setw(4) << idx << ":";
        for (UInt_t i=0; i<fNumSamples; i++)
            *fLog << " " << fSamples[idx*fNumSamples+i];
        *fLog << endl;
    }
    *fLog << endl;
}

Bool_t MPedestalSubtractedEvt::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    switch (type)
    {
    case 0:
    case 1:
        {
            Float_t mean, rms;
            GetStat(idx, mean, rms);
            val = type==0 ? mean : rms;
        }
        break;
    case 2:
        {
            Float_t flt;
            GetMaxPos(idx, flt);
            val = flt;
        }
        break;
    }

    return kTRUE;
}

/*
#include <TSpline.h>
#include "MArrayD.h"
void  MPedestalSubtractedEvt::InterpolateSaturation(const Int_t idx, Int_t limit, Int_t first, Int_t last) const
{
    MArrayD x(GetNumSamples());
    MArrayD y(GetNumSamples());

    Float_t *s = GetSamples(idx);

    Int_t n = 0;
    for (unsigned int i=0; i<GetNumSamples(); i++)
    {
        if (s[i]>limit)
            continue;
        x[n] = i;
        y[n] = s[i];
        n++;
    }

    TSpline5 sp("", x.GetArray(), y.GetArray(), n);

    for (unsigned int i=0; i<GetNumSamples(); i++)
    {
        if (s[i]>limit)
            s[i] = sp.Eval(i);
    }
}
*/
