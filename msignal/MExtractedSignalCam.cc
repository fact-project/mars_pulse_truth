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
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MExtractedSignalCam
//
// Hold the Extracted Signal information for all pixels in the camera
//
// Class Version 3:
// ----------------
//  - fNdf
//
// Class Version 4:
// ----------------
//  - Byte_t fFirstUsedSliceLoGain;         // First Low Gain FADC used for extraction (incl.)
//  + Int_t  fFirstUsedSliceLoGain;         // First Low Gain FADC used for extraction (incl.)
//
/////////////////////////////////////////////////////////////////////////////
#include "MExtractedSignalCam.h"

#include <TClonesArray.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MExtractedSignalPix.h"

ClassImp(MExtractedSignalCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Creates a MExtractedSignalPix object for each pixel
//
MExtractedSignalCam::MExtractedSignalCam(const char *name, const char *title)
    : fFirstUsedSliceHiGain(0), fFirstUsedSliceLoGain(0),
      fLastUsedSliceHiGain(0), fLastUsedSliceLoGain(0), 
      fUsedWindowHiGain(0.), fUsedWindowLoGain(0.)
{
    fName  = name  ? name  : "MExtractedSignalCam";
    fTitle = title ? title : "Storage container for all Extracted Signal Information in the camera";

    fArray = new TClonesArray("MExtractedSignalPix", 1);
}

// --------------------------------------------------------------------------
//
// Delete the array conatining the pixel pedest information
//
MExtractedSignalCam::~MExtractedSignalCam()
{
    delete fArray;
}

// --------------------------------------------------------------------------
//
// Distribute logging stream to all childs
//
void MExtractedSignalCam::SetLogStream(MLog *lg)
{
    fArray->R__FOR_EACH(MParContainer, SetLogStream)(lg);
    MParContainer::SetLogStream(lg);
}

// --------------------------------------------------------------------------
//
// Set the size of the camera
//
void MExtractedSignalCam::InitSize(const UInt_t i)
{
    fArray->ExpandCreate(i);
}

// --------------------------------------------------------------------------
//
// Get the size of the MExtractedSignalCam
//
Int_t MExtractedSignalCam::GetSize() const
{
    return fArray->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel index)
//
MExtractedSignalPix &MExtractedSignalCam::operator[](Int_t i)
{
    return *static_cast<MExtractedSignalPix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel index)
//
const MExtractedSignalPix &MExtractedSignalCam::operator[](Int_t i) const
{
    return *static_cast<MExtractedSignalPix*>(fArray->UncheckedAt(i));
}

/*
Float_t MExtractedSignalCam::GetProb( const Int_t pixidx )   const
{ 
  if (pixidx > GetSize()-1)
    return -1.;
  
  return TMath::Prob((*this)[pixidx].GetChisquare(),fNdf); 
}
*/

void MExtractedSignalCam::Clear(Option_t *o)
{
    fArray->R__FOR_EACH(TObject, Clear)();
}

void MExtractedSignalCam::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << ":" << endl;
    int idx = -1;

    TIter Next(fArray);
    MExtractedSignalPix *pix;
    while ((pix=(MExtractedSignalPix*)Next()))
    {
        idx++;

        if (!pix->IsLoGainValid() && ! pix->IsHiGainValid())
            continue;

        *fLog << idx << ": ";
	pix->Print();
    }
}

Bool_t MExtractedSignalCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (idx>=GetSize())
        return kFALSE;

    const MExtractedSignalPix &pix = (*this)[idx];

    switch (type)
    {
    case 0:
        val = pix.GetExtractedSignalHiGain();
        return pix.IsHiGainValid() && !pix.IsHiGainSaturated();

    case 1:
        val = pix.GetExtractedSignalHiGainError();
        return val>0;

    case 2:
        val = pix.GetExtractedSignalLoGain();
        return pix.IsLoGainValid() && !pix.IsLoGainSaturated();

    case 3:
        val = pix.GetExtractedSignalLoGainError();
        return val>0;

        // This is for the case the signal has been
        // extracted from lo- and hi-gain
    case 4:
        // Lo- and hi-gain must be successfully extracted
        if (!pix.IsLoGainValid() || !pix.IsHiGainValid())
            return kFALSE;

        // Lo- and hi-gain must not be saturated
        if (pix.IsLoGainSaturated() || pix.IsHiGainSaturated())
            return kFALSE;

        // Both signals must have a positive value
        if (pix.GetExtractedSignalLoGain()<=0 || pix.GetExtractedSignalHiGain()<=0)
            return kFALSE;

        //val = log10(pix.GetExtractedSignalHiGain())-log10(pix.GetExtractedSignalLoGain());
        //return TMath::Abs(val-1)<0.4;
        val = pix.GetExtractedSignalHiGain()/pix.GetExtractedSignalLoGain();
        //return val>4 && val<30;
        return val>4 && val<35;

    case 5:
        val = pix.GetExtractedSignalHiGain();
        return pix.IsHiGainValid() && !pix.IsHiGainSaturated() && val>100;

    default:
	return kFALSE;
    }

    return kFALSE;
}

void MExtractedSignalCam::DrawPixelContent(Int_t num) const
{
    *fLog << warn << "MExtractedSignaCam::DrawPixelContent - not available." << endl;
}
