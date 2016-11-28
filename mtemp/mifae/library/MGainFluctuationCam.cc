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
!   Author(s): Oscar Blanch 1/2005 <mailto:blanch@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//            
// MGainFluctuationCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MGainFluctuationCam.h"

#include <math.h>
#include <limits.h>
#include <fstream>

#include <TArrayD.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

ClassImp(MGainFluctuationCam);
ClassImp(MGainFluctuationCamIter);

using namespace std;

// --------------------------------------------------------------------------
//
// Creates a MGainFluctuationPix object for each pixel in the event
//
MGainFluctuationCam::MGainFluctuationCam(const char *name, const char *title) : fNumPixels(0)
{
    fName  = name  ? name  : "MGainFluctuationCam";
    fTitle = title ? title : "(Gain Fluctuation)-Event Information";

    fPixels = new TClonesArray("MGainFluctuationPix", 0);
}


// --------------------------------------------------------------------------
//
// reset counter and delete netries in list.
//
void MGainFluctuationCam::Reset()
{
    fNumPixels  =  0;
    fMaxIndex   = -1;
    fLut.Set(0);
    // fPixels->Delete();
}

void MGainFluctuationCam::FixSize()
{
    fLut.Set(fMaxIndex+1);

    if (fPixels->GetEntriesFast() == (Int_t)fNumPixels)
        return;

    fPixels->ExpandCreateFast(fNumPixels);
}

// --------------------------------------------------------------------------
//
//  Dump the gain fluctuation event to *fLog
//
void MGainFluctuationCam::Print(Option_t *) const
{
    const Int_t entries = fPixels->GetEntries();

    *fLog << GetDescriptor() << dec << endl;
    *fLog << " Number of Pixels: " << fNumPixels << "(" << entries << ")" << endl;

    for (Int_t i=0; i<entries; i++ )
        (*this)[i].Print();
}

Float_t MGainFluctuationCam::GetGain(int i) const
{
  const MGainFluctuationPix &pix = (*this)[i];
  
  return pix.GetGain();
}

// --------------------------------------------------------------------------
//
// get the minimum number of photons  of all valid pixels in the list
// If you specify a geometry the number of photons is weighted with the
// area of the pixel
//
Float_t MGainFluctuationCam::GetGainMin(const MGeomCam *geom) const
{
    if (fNumPixels <= 0)
        return -5.;

    const UInt_t n = geom->GetNumPixels();

    Float_t minval = FLT_MAX;

    for (UInt_t i=0; i<fNumPixels; i++)
    {
        const MGainFluctuationPix &pix = (*this)[i];

        const UInt_t id = pix.GetPixId();
        if (id<0 || id>=n)
            continue;

        Float_t testval = pix.GetGain();

        if (testval < minval)
            minval = testval;
    }

    return minval;
}

// --------------------------------------------------------------------------
//
// get the maximum number of photons of all valid pixels in the list
// If you specify a geometry the number of photons is weighted with the
// area of the pixel
//
Float_t MGainFluctuationCam::GetGainMax(const MGeomCam *geom) const
{
    if (fNumPixels <= 0)
        return 50.;

    const UInt_t n = geom->GetNumPixels();

    Float_t maxval = -FLT_MAX;

    for (UInt_t i=0; i<fNumPixels; i++)
    {
        const MGainFluctuationPix &pix = (*this)[i];

        const UInt_t id = pix.GetPixId();
        if (id<0 || id>=n)
            continue;

        Float_t testval = pix.GetGain();

        if (testval > maxval)
            maxval = testval;
    }
    return maxval;
}


// --------------------------------------------------------------------------
//
// Return a pointer to the pixel with the requested idx. NULL if it doesn't
// exist. The Look-up-table fLut is used. If its size is zero (according
// to Rene this will happen if an old class object is loaded) we still
// try to search in the array.
//
MGainFluctuationPix *MGainFluctuationCam::GetPixById(Int_t idx) const
{
    if (idx<0)
        return 0;

    if (fLut.GetSize()>0)
    {
        if (idx>=fLut.GetSize())
            return 0;
        return fLut[idx]<0 ? 0 : (MGainFluctuationPix*)(fPixels->UncheckedAt(fLut[idx]));
    }

    TIter Next(fPixels);
    MGainFluctuationPix *pix = NULL;

    while ((pix=(MGainFluctuationPix*)Next()))
        if (pix->GetPixId()==idx)
            return pix;

    return NULL;
}

MGainFluctuationPix *MGainFluctuationCam::AddPixel(Int_t idx, Float_t gain)
    {
        //
        // If this is too slow or takes to much space we might use
        // MGeomApply and an InitSize member function instead.
        //
        if (idx>=fLut.GetSize())
        {
            const Int_t n = fLut.GetSize();
            fLut.Set(idx*2+1); //idx+1 is slower than idx*2+1
            for (int i=n; i<idx*2+1; i++)
                fLut[i] = -1;
        }

        fLut[idx] = fNumPixels;
        if (idx>fMaxIndex)
            fMaxIndex=idx;

        return new ((*fPixels)[fNumPixels++]) MGainFluctuationPix(idx, gain);
    }

// --------------------------------------------------------------------------
//
// Returns, depending on the type flag:
//
//  0: Number of Photons*PixRatio
//  1: Error*sqrt(PixRatio)
//  2: Cleaning level = Num Photons*sqrt(PixRatio)/Error
//  3: Number of Photons
//  4: Error
//  5: Island index
//
Bool_t MGainFluctuationCam::GetPixelContent(Double_t &val, Int_t idx,const MGeomCam&, Int_t type) const
{
    MGainFluctuationPix *pix = GetPixById(idx);

    switch (type)
    {
    case 1:
        val = pix->GetGain();
        return kTRUE;
    }
    return kTRUE;
}

void MGainFluctuationCam::DrawPixelContent(Int_t num) const
{
    *fLog << warn << "MGainFluctuationCam::DrawPixelContent - not available." << endl;
}

TObject *MGainFluctuationCamIter::Next()
{
    if (!fUsedOnly)
        return TObjArrayIter::Next();

    MGainFluctuationPix *pix;
    while ((pix = (MGainFluctuationPix*)TObjArrayIter::Next()))
            return pix;
    return pix;
}
