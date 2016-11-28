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
!   Author(s): Markus Gaug  12/2003 <mailto:markus@ifae.es>
!   Author(s): Thomas Bretz 12/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Hendrik Bartko 02/2004 <mailto:hbartko@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MArrivalTimeCam
//
// Hold the ArrivalTime information for all pixels in the camera
//
//
// Class Version 2:
// ----------------
//  - Byte_t fFirstUsedSliceHiGain;
//  - Byte_t fFirstUsedSliceLoGain;
//  - Byte_t fLastUsedSliceHiGain;
//  - Byte_t fLastUsedSliceLoGain;
//
/////////////////////////////////////////////////////////////////////////////
#include "MArrivalTimeCam.h"

#include <TClonesArray.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MArrivalTimePix.h"

ClassImp(MArrivalTimeCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Creates a MArrivalTimePix object for each pixel
//
MArrivalTimeCam::MArrivalTimeCam(const char *name, const char *title)
{
    fName  = name  ? name  : "MArrivalTimeCam";
    fTitle = title ? title : "Storage container for all Extracted Signal Information in the camera";

    fArray = new TClonesArray("MArrivalTimePix", 1);
}

// --------------------------------------------------------------------------
//
// Delete the array conatining the pixel pedest information
//
MArrivalTimeCam::~MArrivalTimeCam()
{
    delete fArray;
}

// --------------------------------------------------------------------------
//
// Distribute logging stream to all childs
//
void MArrivalTimeCam::SetLogStream(MLog *lg)
{
    fArray->R__FOR_EACH(MParContainer, SetLogStream)(lg);
    MParContainer::SetLogStream(lg);
}

// --------------------------------------------------------------------------
//
// Set the size of the camera
//
void MArrivalTimeCam::InitSize(const UInt_t i)
{
    fArray->ExpandCreate(i);
}

// --------------------------------------------------------------------------
//
// Get the size of the MArrivalTimeCam
//
Int_t MArrivalTimeCam::GetSize() const
{
    return fArray->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel index)
//
MArrivalTimePix &MArrivalTimeCam::operator[](Int_t i)
{
    return *static_cast<MArrivalTimePix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel index)
//
const MArrivalTimePix &MArrivalTimeCam::operator[](Int_t i) const
{
    return *static_cast<MArrivalTimePix*>(fArray->UncheckedAt(i));
}

void MArrivalTimeCam::Clear(Option_t *o)
{
    fArray->R__FOR_EACH(TObject, Clear)();
}

void MArrivalTimeCam::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << ":" << endl;
    int idx = -1;

    TIter Next(fArray);
    MArrivalTimePix *pix;
    while ((pix=(MArrivalTimePix*)Next()))
    {
        idx++;

        if (!pix->IsArrivalTimeValid())
            continue;

        *fLog << idx << ": ";
	pix->Print();
    }
}

Bool_t MArrivalTimeCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (idx>=GetSize())
        return kFALSE;

    const MArrivalTimePix &pix = (*this)[idx];

    switch (type)
    {
    case 0:
        val = pix.GetArrivalTimeHiGain();
        return pix.IsHiGainValid() && !pix.IsHiGainSaturated();

    case 1:
        val = pix.GetArrivalTimeHiGainError();
        return val>0;

    case 2:
        val = pix.GetArrivalTimeLoGain();
        return pix.IsLoGainValid() && !pix.IsLoGainSaturated();

    case 3:
        val = pix.GetArrivalTimeLoGainError();
        return val>0;

    case 4:
    case 6:
        val = pix.GetArrivalTime();
        return pix.IsArrivalTimeValid();

        // This is for the case the signal has been
        // extracted from lo- and hi-gain
    case 7:
        // Lo- and hi-gain must be successfully extracted
        if (!pix.IsLoGainValid() || !pix.IsHiGainValid())
            return kFALSE;

        // Lo- and hi-gain must not be saturated
        if (pix.IsLoGainSaturated() || pix.IsHiGainSaturated())
            return kFALSE;

//        if (pix.GetArrivalTimeHiGain()<3 || pix.GetArrivalTimeHiGain()>12 ||
//            pix.GetArrivalTimeLoGain()<3 || pix.GetArrivalTimeLoGain()>12)
//            return kFALSE;

        val = pix.GetArrivalTimeLoGain()-pix.GetArrivalTimeHiGain();
        return TMath::Abs(val)<2; // FIXME: Is this a good value?

    default:
	return kFALSE;
    }

    return kFALSE;
}

void MArrivalTimeCam::DrawPixelContent(Int_t num) const
{
    *fLog << warn << "MArrivalTimeCam::DrawPixelContent - not available." << endl;
}
