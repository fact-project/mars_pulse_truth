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
!   Author(s): Thomas Bretz, 03/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2010
!
! Version 2:
! ----------
!
! Made persistent (the correct contets is not guranteed, because it is
! calculated from the outside (e.g. in MImgCleanStd))
!
!    Short_t       fNumIslands;                  //!
!    Short_t       fNumSinglePixels;             //!
!    Float_t       fSizeSinglePixels;            //!
!    Float_t       fSizeSubIslands;              //!
!    Float_t       fSizeMainIsland;              //!
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//            
// MSignalCam
//
// Class Version 1:
// ----------------
//  - first version
//
/////////////////////////////////////////////////////////////////////////////
#include "MSignalCam.h"

#include <math.h>
#include <limits.h>
#include <fstream>

#include <TArrayI.h>
#include <TArrayD.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeom.h"

ClassImp(MSignalCam);
ClassImp(MSignalCamIter);

using namespace std;

// --------------------------------------------------------------------------
//
// Creates a MSignalPix object for each pixel in the event
//
MSignalCam::MSignalCam(const char *name, const char *title) /*: fNumPixels(0)*/
{
    fName  = name  ? name  : "MSignalCam";
    fTitle = title ? title : "(Number of Photon)-Event Information";

    fPixels = new TClonesArray("MSignalPix", 0);

    Reset();
}

// --------------------------------------------------------------------------
//
// Copy contants of obj into this instance.
//
void MSignalCam::Copy(TObject &obj) const
{
    MSignalCam &cam = static_cast<MSignalCam&>(obj);

    cam.fNumIslands = fNumIslands;
    cam.fNumSinglePixels = fNumSinglePixels;
    cam.fSizeSinglePixels = fSizeSinglePixels;
    cam.fSizeSubIslands = fSizeSubIslands;
    cam.fSizeMainIsland = fSizeMainIsland;

    cam.fNumPixelsSaturatedHiGain = fNumPixelsSaturatedHiGain;
    cam.fNumPixelsSaturatedLoGain = fNumPixelsSaturatedLoGain;

    cam.fPixels->Delete();

    const UInt_t n = GetNumPixels();

    cam.InitSize(n);

    for (UInt_t i=0; i<n; i++)
        new ((*cam.fPixels)[i]) MSignalPix((*this)[i]);
}

// --------------------------------------------------------------------------
//
// reset counter and delete netries in list.
//
void MSignalCam::Reset()
{
    fNumSinglePixels  =  0;
    fSizeSinglePixels =  0;
    fSizeSubIslands   =  0;
    fSizeMainIsland   =  0;
    fNumIslands       = -1;

    fNumPixelsSaturatedHiGain = -1;
    fNumPixelsSaturatedLoGain = -1;

    fPixels->R__FOR_EACH(TObject, Clear)();
}

// --------------------------------------------------------------------------
//
//  Dump the cerenkov photon event to *fLog
//
void MSignalCam::Print(Option_t *) const
{
    const Int_t entries = fPixels->GetEntries();

    *fLog << GetDescriptor() << dec << endl;
    *fLog << " Number of Pixels: " << GetNumPixels() << "(" << entries << ")" << endl;

    for (Int_t i=0; i<entries; i++ )
        (*this)[i].Print();
}

// --------------------------------------------------------------------------
//
//   Count and return the number of unmapped pixels
//
Int_t MSignalCam::GetNumPixelsUnmapped() const
{
    const UInt_t n = GetNumPixels();

    if (n <= 0)
        return -1;

    Int_t cnt=0;
    for (UInt_t i=0; i<n; i++)
    {
        if ((*this)[i].IsPixelUnmapped())
            cnt++;
    }

    return cnt;
}

// --------------------------------------------------------------------------
//
// get the minimum number of photons  of all valid pixels in the list
// If you specify a geometry the number of photons is weighted with the
// area of the pixel
//
Float_t MSignalCam::GetNumPhotonsMin(const MGeomCam *geom) const
{
    const UInt_t n = GetNumPixels();

    if (n <= 0)
        return -5.;

    Float_t minval = FLT_MAX;

    for (UInt_t i=0; i<n; i++)
    {
        const MSignalPix &pix = (*this)[i];
        if (!pix.IsPixelUsed())
            continue;

        Float_t testval = pix.GetNumPhotons();

        if (geom)
            testval *= geom->GetPixRatio(i);

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
Float_t MSignalCam::GetNumPhotonsMax(const MGeomCam *geom) const
{
    const UInt_t n = GetNumPixels();

    if (n <= 0)
        return 50.;

    Float_t maxval = -FLT_MAX;

    for (UInt_t i=0; i<n; i++)
    {
        const MSignalPix &pix = (*this)[i];
        if (!pix.IsPixelUsed())
            continue;

        Float_t testval = pix.GetNumPhotons();
        if (geom)
            testval *= geom->GetPixRatio(i);

        if (testval > maxval)
            maxval = testval;
    }
    return maxval;
}

// --------------------------------------------------------------------------
//
// get the minimum ratio of photons/error
//
Float_t MSignalCam::GetRatioMin(const MGeomCam *geom) const
{
    const UInt_t n = GetNumPixels();

    if (n <= 0)
        return -5.;

    Float_t minval = FLT_MAX;

    for (UInt_t i=0; i<n; i++)
    {
        const MSignalPix &pix = (*this)[i];
        if (!pix.IsPixelUsed())
            continue;

        Float_t testval = pix.GetNumPhotons()/pix.GetErrorPhot();
        if (geom)
            testval *= geom->GetPixRatioSqrt(i);

        if (testval < minval)
            minval = testval;
    }

    return minval;
}

// --------------------------------------------------------------------------
//
// get the maximum ratio of photons/error
//
Float_t MSignalCam::GetRatioMax(const MGeomCam *geom) const
{
    const UInt_t n = GetNumPixels();

    if (n <= 0)
        return -5.;

    Float_t maxval = -FLT_MAX;

    for (UInt_t i=0; i<n; i++)
    {
        const MSignalPix &pix = (*this)[i];
        if (!pix.IsPixelUsed())
            continue;

        Float_t testval = pix.GetNumPhotons()/pix.GetErrorPhot();
        if (geom)
            testval *= geom->GetPixRatioSqrt(i);

        if (testval > maxval)
            maxval = testval;
    }

    return maxval;
}

// --------------------------------------------------------------------------
//
// get the minimum of error
// If you specify a geometry the number of photons is weighted with the
// area of the pixel
//
Float_t MSignalCam::GetErrorPhotMin(const MGeomCam *geom) const
{
    const UInt_t n = GetNumPixels();

    if (n <= 0)
        return 50.;

    Float_t minval = FLT_MAX;

    for (UInt_t i=0; i<n; i++)
    {
        const MSignalPix &pix = (*this)[i];
        if (!pix.IsPixelUsed())
            continue;

        Float_t testval = pix.GetErrorPhot();

        if (geom)
            testval *= geom->GetPixRatio(i);

        if (testval < minval)
            minval = testval;
    }
    return minval;
}

// --------------------------------------------------------------------------
//
// get the maximum ratio of photons/error
// If you specify a geometry the number of photons is weighted with the
// area of the pixel
//
Float_t MSignalCam::GetErrorPhotMax(const MGeomCam *geom) const
{
    const UInt_t n = GetNumPixels();

    if (n <= 0)
        return 50.;

    Float_t maxval = -FLT_MAX;

    for (UInt_t i=0; i<n; i++)
    {
        const MSignalPix &pix = (*this)[i];
        if (!pix.IsPixelUsed())
            continue;

        Float_t testval = pix.GetErrorPhot();

        if (geom)
            testval *= geom->GetPixRatio(i);

        if (testval > maxval)
            maxval = testval;
    }
    return maxval;
}

MSignalPix *MSignalCam::AddPixel(Int_t idx, Float_t nph, Float_t er)
{
    MSignalPix *pix = static_cast<MSignalPix*>((*fPixels)[idx]);
    pix->Set(nph, er);
    return pix;
}

// --------------------------------------------------------------------------
//
// This function recursively finds all pixels of one island and assigns
// the number num as island number to the pixel.
//
//  1) Check whether a pixel with the index idx exists, is unused
//     and has not yet a island number assigned.
//  2) Assign the island number num to the pixel
//  3) Loop over all its neighbors taken from the geometry geom. For all
//     neighbors recursively call this function (CalcIsland)
//  4) Sum the size of the pixel and all neighbors newly assigned
//     (by CalcIsland) to this island
//
// Returns the sum of the pixel size.
//
Double_t MSignalCam::CalcIsland(const MGeomCam &geom, Int_t idx, Int_t num)
{
    // Get the pixel information of a pixel with this index
    MSignalPix &pix = (*this)[idx];

    // If an island number was already assigned to this pixel... do nothing.
    if (pix.GetIdxIsland()>=0)
        return 0;

    // If the pixel is an unused pixel... do nothing.
    if (!pix.IsPixelUsed())
        return 0;

    // Assign the new island number num to this used pixel
    pix.SetIdxIsland(num);

    // Get the geometry information (neighbors) of this pixel
    const MGeom &gpix = geom[idx];

    // Get the size of this pixel
    Double_t size = pix.GetNumPhotons();

    // Now do the same with all its neighbors and sum the
    // sizes which they correspond to
    const Int_t n = gpix.GetNumNeighbors();
    for (int i=0; i<n; i++)
        size += CalcIsland(geom, gpix.GetNeighbor(i), num);

    // return size of this (sub)cluster
    return size;
}

// --------------------------------------------------------------------------
//
// Each pixel which is maked as used is assigned an island number
// (starting from 0). A pixel without an island number assigned
// has island number -1.
//
// The index 0 corresponds to the island with the highest size (sum
// of GetNumPhotons() in island). The size is decreasing with
// increasing indices.
//
// The information about pixel neighbory is taken from the geometry
// MGeomCam geom;
//
// You can access this island number of a pixel with a call to
// MSignalPix->GetIdxIsland. The total number of islands available
// can be accessed with MSignalCam->GetNumIslands.
//
// CalcIslands returns the number of islands found. If an error occurs,
// eg the geometry has less pixels than the highest index stored, -1 is
// returned.
//
Int_t MSignalCam::CalcIslands(const MGeomCam &geom)
{
    const UInt_t numpix = GetNumPixels();

    if (/*fMaxIndex<0 ||*/ numpix==0)
    {
        *fLog << err << "ERROR - MSignalCam doesn't contain pixels!" << endl;
        fNumIslands = 0;
        return -1;
    }
/*
    if ((UInt_t)fMaxIndex>=geom.GetNumPixels())
    {
        *fLog << err << "ERROR - MSignalCam::CalcIslands: Size mismatch - geometry too small!" << endl;
        return -1;
    }
  */
    // Create a list to hold the sizes of the islands (The maximum
    // number of islands possible is roughly fNumPixels/4)
    TArrayD size(numpix/3);

    // Calculate Islands
    Int_t   n=0;
    Float_t totsize = 0;

    for (UInt_t idx=0; idx<numpix; idx++)
    {
        const MSignalPix &pix = (*this)[idx];
        if (!pix.IsPixelUsed())
            continue;

        // This 'if' is necessary (although it is done in GetIsland, too)
        // because otherwise the counter (n) would be wrong.
        // So only 'start' a new island for used pixels (selected by
        // using the Iterator) which do not yet belong to another island.
        if (pix.GetIdxIsland()<0)
        {
            // Don't put this in one line! n is used twice...
            const Double_t sz = CalcIsland(geom, idx/*pix->GetPixId()*/, n);
            size[n++] = sz;
            totsize += sz;
        }
    }

    // Create an array holding the indices
    TArrayI idxarr(n);

    // Sort the sizes descending
    TMath::Sort(n, size.GetArray(), idxarr.GetArray(), kTRUE);

    // Replace island numbers by size indices -- After this
    // islands indices are sorted by the island size
    for (UInt_t idx=0; idx<numpix; idx++)
    {
        MSignalPix &pix = (*this)[idx];
        if (!pix.IsPixelUsed())
            continue;

        const Short_t i = pix.GetIdxIsland();

        // Find new index
        Short_t j;
        for (j=0; j<n; j++)
            if (idxarr[j]==i)
                break;

        pix.SetIdxIsland(j==n ? -1 : j);
    }

    // Now assign number of islands found
    fNumIslands     = n;
    fSizeSubIslands = n>0 ? totsize-size[idxarr[0]] : 0;
    fSizeMainIsland = n>0 ? size[idxarr[0]] : 0;

    // return number of island
    return fNumIslands;
}

// --------------------------------------------------------------------------
//
// Compares the cleaning (fRing and fIsCore) of this object and the
// argument. Return the result (kFALSE if different).
//
// If differences are found the pixels (the pixel from this object first)
// is printed.
//
Bool_t MSignalCam::CompareCleaning(const MSignalCam &cam) const
{
    const UInt_t n = GetNumPixels();

    if (n != cam.GetNumPixels())
    {
        *fLog << warn << "MSignalCam::CompareCleaning - Number of pixels mismatch." << endl;
        return kFALSE;
    }

    for (UInt_t i=0; i<n; i++)
    {
        const MSignalPix &p = (*this)[i];
        const MSignalPix &q = cam[i];

        if (p.GetRing()==q.GetRing() && p.IsPixelCore()==q.IsPixelCore())
            continue;

        *fLog << warn << "MSignalCam::CompareCleaning - Pixel #" << i << " mismatch." << endl;
        p.Print();
        q.Print();
        return kFALSE;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Compares the islands (fIdxIsland) of this object and the
// argument. Return the result (kFALSE if different).
//
// If differences are found the pixels (the pixel from this object first)
// is printed.
//
Bool_t MSignalCam::CompareIslands(const MSignalCam &cam) const
{
    const UInt_t n = GetNumPixels();

    if (n != cam.GetNumPixels())
    {
        *fLog << warn << "MSignalCam::CompareIslands - Number of pixels mismatch." << endl;
        return kFALSE;
    }

    for (UInt_t i=0; i<n; i++)
    {
        const MSignalPix &p = (*this)[i];
        const MSignalPix &q = cam[i];

        if (p.GetIdxIsland()==q.GetIdxIsland())
            continue;

        *fLog << warn << "MSignalCam::CompareIslands - Pixel #" << i << " mismatch." << endl;
        p.Print();
        q.Print();
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns, depending on the type flag:
//
//  0: Number of Photons*PixRatio <default>
//  1: Error*sqrt(PixRatio)
//  2: Cleaning level = Num Photons*sqrt(PixRatio)/Error
//  3: Number of Photons
//  4: Error
//  5: Island index
//  6: arrival time of mapped pixels
//  7: arrival time if signa avove 20phe
//  8: arrival time
// 10: as 0, but returns kFALSE if signal <=0
// 11: as 8, but returns kFALSE if signal <=0
// 12: time slope
//
Bool_t MSignalCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (idx<0 || (UInt_t)idx>GetNumPixels())
        return kFALSE;

    const MSignalPix &pix = (*this)[idx];

    // Used inlcudes status unampped
    if (!pix.IsPixelUsed() && (type<6 || type==8 || type==14))
        return kFALSE;

    const Double_t ratio = cam.GetPixRatio(idx);

    switch (type)
    {
    case 1: // scaled error of phtoons
        val = pix.GetErrorPhot()*TMath::Sqrt(ratio);
        return kTRUE;

    case 2: // significance of number of photons
        if (pix.GetErrorPhot()<=0)
            return kFALSE;
        val = pix.GetNumPhotons()*TMath::Sqrt(ratio)/pix.GetErrorPhot();
        return kTRUE;

    case 3: // number of photo electrons
        val = pix.GetNumPhotons();
        break;

    case 4: // error of signal
        val = pix.GetErrorPhot();
        break;

    case 5: // index of island
        val = pix.GetIdxIsland();
        break;

    case 6: // arrival time of mapped pixels only
        if (pix.IsPixelUnmapped())
            return kFALSE;
        val = pix.GetArrivalTime();
        break;

    case 7: // pulse position above 50phe
        // The number of photons is not scaled with the ratio because
        // otherwise to many large pixels survive (maybe because the
        // fluctuations scale different than expected)
        if (pix.IsPixelUnmapped() || pix.GetNumPhotons()<50)
            return kFALSE;
        val = pix.GetArrivalTime();
        break;

    case 8: // arrival time
        val = pix.GetArrivalTime();
        break;

        /*
    case 10: // lo gain time
        if (pix.IsPixelUnmapped() || !pix.IsLoGainUsed() ||
            pix.GetNumPhotons()<320)
            return kFALSE;
        val = pix.GetArrivalTime();
        return kTRUE;

    case 11: // hi gain time
        // The number of photons is not scaled with the ratio because
        // otherwise to many large pixels survive (maybe because the
        // fluctuations scale different than expected)
        if (pix.IsPixelUnmapped() || pix.IsLoGainUsed() ||
            pix.GetNumPhotons()<50)
            return kFALSE;
        val = pix.GetArrivalTime();
        return kTRUE;
        */

    case 10:
        val = pix.GetNumPhotons()*ratio;
        return val>0;

    case 11:
        val = pix.GetArrivalTime();
        return pix.GetNumPhotons()>0;

    case 13: // time slope
        val = pix.GetTimeSlope();
        break;

    case 14: // time slope
        if (pix.IsPixelUnmapped())
            return kFALSE;
        val = pix.GetTimeSlope();
        break;

    case 9:
    default:
        val = pix.GetNumPhotons()*ratio;
        return kTRUE;
    }
    return kTRUE;
}

void MSignalCam::DrawPixelContent(Int_t num) const
{
    *fLog << warn << "MSignalCam::DrawPixelContent - not available." << endl;
}

TObject *MSignalCamIter::Next()
{
    if (!fUsedOnly)
    {
        fIdx++;
        return TObjArrayIter::Next();
    }

    MSignalPix *pix;
    while ((pix = (MSignalPix*)TObjArrayIter::Next()))
    {
        fIdx++;
        if (pix->IsPixelUsed())
            return pix;
    }
    return pix;
}
