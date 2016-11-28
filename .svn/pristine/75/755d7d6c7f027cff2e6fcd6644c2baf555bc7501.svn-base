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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer, 1/2001
!   Author(s): Nadia Tonello, 4/2003 <mailto:tonello@mppmu.mpg.de>
!   Author(s): Stefan Ruegamer, 03/2006 <mailto:snruegam@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MImgCleanStd
//
// The Image Cleaning task selects the pixels you use for the Hillas
// parameters calculation.
//
// There are two methods to make the selection: the standard one, as done
// in the analysis of CT1 data, and the democratic one, as suggested by
// W.Wittek. The number of photo-electrons of a pixel is compared with the
// pedestal RMS of the pixel itself (standard method) or with the average
// RMS of the inner pixels (democratic method).
// In both cases, the possibility to have a camera with pixels of
// different area is taken into account.
// The too noisy pixels can be recognized and eventually switched off
// (Unmap: set blind pixels to UNUSED) separately, using the
// MBlindPixelCalc Class. In the MBlindPixelCalc class there is also the
// function to replace the value of the noisy pixels with the interpolation
// of the content of the neighbors (SetUseInterpolation).
//
// Example:
// ...
// MBlindPixelCalc blind;
// blind.SetUseInterpolation();
// blind.SetUseBlindPixels();
//
// MImgCleanStd clean;
// ...
// tlist.AddToList(&blind);
// tlist.AddToList(&clean);
//
// Look at the MBlindPixelCalc Class for more details.
//
// Starting point: default values ----------------------------------------
//
// When an event is read, before the image cleaning, all the pixels that
// are in MSignalCam are set as USED and NOT CORE. All the pixels belong
// to RING number 1 (like USED pixels).
// Look at MSignalPix.h to see how these informations of the pixel are
// stored.
// The default  cleaning METHOD is the STANDARD one and the number of the
// rings around the CORE pixel it analyzes is 1. Look at the Constructor
// of the class in MImgCleanStd.cc to see (or change) the default values.
//
// Example: To modify this setting, use the member functions
// SetMethod(MImgCleanStd::kDemocratic) and SetCleanRings(UShort_t n).
//
// MImgCleanStd:CleanStep1 -----------------------------------------------
//
// The first step of cleaning defines the CORE pixels. The CORE pixels are
// the ones which contain the informations about the core of the electro-
// magnetic shower.
// The ratio  (A_0/A_i) is calculated from fCam->GetPixRatio(i). A_0 is
// the area of the central pixel of the camera, A_i is the area of the
// examined pixel. In this way, if we have a MAGIC-like camera, with the
// outer pixels bigger than the inner ones, the level of cleaning in the
// two different regions is weighted.
// This avoids problems of deformations of the shower images.
// The signal S_i and the pedestal RMS Prms_i of the pixel are called from
// the object MSignalPix.
// If (default method = kStandard)
//Begin_Html
//&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="images/MImgCleanStd-f1.png">
//End_Html
// the pixel is set as CORE pixel. L_1 (n=1) is called "first level of
// cleaning" (default: fCleanLvl1 = 3).
// All the other pixels are set as UNUSED and belong to RING 0.
// After this point, only the CORE pixels are set as USED, with RING
// number 1.
//
// MImgCleanStd:CleanStep2 ----------------------------------------------
//
// The second step of cleaning looks at the isolated CORE pixels and sets
// them to UNUSED. An isolated pixel is a pixel without CORE neighbors.
// At the end of this point, we have set as USED only CORE pixels with at
// least one CORE neighbor.
//
// MImgCleanStd:CleanStep3  ----------------------------------------------
//
// The third step of cleaning looks at all the pixels (USED or UNUSED) that
// surround the USED pixels.
// If the content of the analyzed pixel survives at the second level of
// cleaning, i.e. if
//Begin_Html
//&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="images/MImgCleanStd-f1.png">
//End_Html
// the pixel is set as USED. L_2 (n=2) is called "second level of cleaning"
// (default:fCleanLvl2 = 2.5).
//
// When the number of RINGS to analyze is 1 (default value), only the
// pixels that have a neighbor CORE pixel are analyzed.
//
// There is the option to decide the number of times you want to repeat
// this procedure (number of RINGS analyzed around the core pixels = n).
// Every time the level of cleaning is the same (fCleanLvl2) and the pixel
// will belong to ring r+1, 1 < r < n+1. This is described in
// MImgCleanStd:CleanStep4 .
//
// Dictionary and member functions ---------------------------------------
//
// Here there is the detailed description of the member functions and of
// the terms commonly used in the class.
//
//
// STANDARD CLEANING:
// =================
// This is the method used for the CT1 data analysis. It is the default
// method of the class.
// The number of photo-electrons of a pixel (S_i) is compared to the
// pedestal RMS of the pixel itself (Prms_i). To have the comparison to
// the same photon density for all the pixels, taking into account they
// can have different areas, we have to keep in mind that the number of
// photons that hit each pixel, goes linearly with the area of the pixel.
// The fluctuations of the LONS are proportional to sqrt(A_i), so when we
// compare S_i with Prms_i, only a factor  sqrt(A_0/A_i) is missing to
// have the same (N.photons/Area) threshold for all the pixels.
//
//              !!WARNING: if noise independent from the
//         pixel size (example: electronic noise) is introduced,
//         then the noise fluctuations are no longer proportional
//         to sqrt(A_i), and then the cut value (for a camera with
//         pixels of different sizes) resulting from the above
//         procedure  would not be proportional to pixel size as we 
//         intend. In that case, democratic cleaning is preferred.
//
// If
//Begin_Html
//&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="images/MImgCleanStd-f1.png">
//End_Html
// the pixel survives the cleaning and it is set as CORE (when L_n is the
// first level of cleaning, fCleanLvl1) or USED (when L_n is the second
// level of cleaning, fCleanLvl2).
//
// Example:
//
// MImgCleanStd clean;
// //creates a default Cleaning object, with default setting
// ...
// tlist.AddToList(&clean);
// // add the image cleaning to the main task list
//
//
// DEMOCRATIC CLEANING:
// ===================
// You use this cleaning method when you want to compare the number of
// photo-electrons of each pixel with the average pedestal RMS of the
// inner pixels (for the MAGIC camera they are the smaller ones):
//Begin_Html
//&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="images/MImgCleanStd-f2.png">
//End_Html
// In this case, the simple ratio (A_0/A_i) is used to weight the level of
// cleaning, because both the inner and the outer pixels (that in MAGIC
// have a different area) are compared to the same pedestal RMS, coming
// from the inner pixels.
//
// Make sure that you used a class calculating the MPedPhotCam which also
// updated the contents of the mean values (Recalc) correctly.
//
//
// PROBABILITY CLEANING
// ====================
// This method takes signal height (over signal noise) and arrival time
// into account. Instead of comparing signal/Noise with cleaning level
// one and two, we calculate
//  - P_ped: The probability that a signal is a pedestal (using
//           the signal height and the pedestal) For this probability the
//           same algorithm like in kScaled is used (which is a standard
//           cleaning which scales the noise with the mean noise of pixels
//           with the same size)
//  - P_sig: The probability that the signal corresponds to the pixel
//           with the highest signal. For this probability we use the
//           arrival time only.
//
// The cleaning now is done in levels of Probability (eg. 0.2, 0.05)
// The meaning of the cleaning levels is essentially the same (the same cleaning
// algorithm is used) but the cleaning is not done in levels of signal/noise
// but in level of this probability.
//
// This probability is calculated as (1-P_ped)*P_sig
//
// Example:
//
// MImgCleanStd clean(0.2, 0.05);
// clean.SetMethod(MImgCleanStd::kProbability);
//
//
// ABSOLUTE CLEANING
// =================
// This method takes signal height (photons) times area ratio
// ad the cleaning levels.
//
// The cleaning now is done in these levels (eg. 16, 20)
// The meaning of the cleaning levels is essentially the same (the same cleaning
// algorithm is used) but the cleaning is not done in different 'units'
//
// Example:
//
// MImgCleanStd clean(20, 16);
// clean.SetMethod(MImgCleanStd::kAbsolulte);
//
//
// Member Function:  SetMethod()
// ============================
// When you call the MImgCleanStd task, the default method is kStandard.
//
// If you want to switch to the kDemocratic method you have to
// call this member function.
//
// Example:
//
// MImgCleanStd clean;
// //creates a default Cleaning object, with default setting
//
// clean.SetMethod(MImgCleanStd::kDemocratic);
// //now the method of cleaning is changed to Democratic
//
//
// FIRST AND SECOND CLEANING LEVEL
// ===============================
// When you call the MImgCleanStd task, the default cleaning levels are
// fCleanLvl1 = 3, fCleanLvl2 = 2.5. You can change them easily when you
// create the MImgCleanStd object.
//
// Example:
//
// MImgCleanStd clean(Float_t lvl1,Float_t lvl2);
// //creates a default cleaning object, but the cleaning levels are now
// //lvl1 and lvl2.
//
// RING NUMBER
// ===========
// The standard cleaning procedure is such that it looks for the
// informations of the boundary part of the shower only on the first
// neighbors of the CORE pixels.
// There is the possibility now to look not only at the first neighbors
// (first ring),but also further away, around the CORE pixels. All the new
// pixels you can find with this method, are tested with the second level
// of cleaning and have to have at least an USED neighbor.
//
// They will be also set as USED and will be taken into account during the
// calculation of the image parameters.
// The only way to distinguish them from the other USED pixels, is the
// Ring number, that is bigger than 1.
//
// Example: You can decide how many rings you want to analyze using:
//
// MImgCleanStd clean;
// //creates a default cleaning object (default number of rings =1)
// clean.SetCleanRings(UShort_t r);
// //now it looks r times around the CORE pixels to find new pixels with
// //signal.
//
//
//  Class Version 4:
//  ----------------
//   + Float_t  fTimeLvl2;
//   - Bool_t   fKeepSinglePixels;
//   + Bool_t   fKeepIsolatedPixels;
//   + Int_t    fRecoverIsolatedPixels;
//
//
//  Input Containers:
//   MGeomCam
//   MPedPhotCam
//   MSignalCam
//
//  Output Containers:
//   MSignalCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MImgCleanStd.h"

#include <stdlib.h>       // atof					  
#include <fstream>        // ofstream, SavePrimitive

#include <TEnv.h>

#include <TGFrame.h>      // TGFrame
#include <TGLabel.h>      // TGLabel
#include <TGTextEntry.h>  // TGTextEntry

#include "MLog.h"
#include "MLogManip.h"

#include "MArrayI.h"
#include "MParList.h"
#include "MCameraData.h"

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MSignalPix.h"
#include "MSignalCam.h"

#include "MGGroupFrame.h" // MGGroupFrame

ClassImp(MImgCleanStd);

using namespace std;

enum {
    kImgCleanLvl1,
    kImgCleanLvl2
};

static const TString gsDefName  = "MImgCleanStd";
static const TString gsDefTitle = "Task to perform image cleaning";

const TString MImgCleanStd::gsNamePedPhotCam="MPedPhotCam"; // default name of the 'MPedPhotCam' container
const TString MImgCleanStd::gsNameSignalCam ="MSignalCam"; // default name of the 'MSignalCam' container
const TString MImgCleanStd::gsNameGeomCam   ="MGeomCam";    // default name of the 'MGeomCam' container

// --------------------------------------------------------------------------
//
// Default constructor. Here you can specify the cleaning method and levels. 
// If you don't specify them the 'common standard' values 3.0 and 2.5 (sigma
// above mean) are used.
// Here you can also specify how many rings around the core pixels you want 
// to analyze (with the fixed lvl2). The default value for "rings" is 1.
//
MImgCleanStd::MImgCleanStd(const Float_t lvl1, const Float_t lvl2,
                           const char *name, const char *title)
    : fCleaningMethod(kStandard), fCleanLvl0(lvl1), fCleanLvl1(lvl1),
    fCleanLvl2(lvl2), fTimeLvl1(1.5), fTimeLvl2(1.5), fCleanRings(1),
    fKeepIsolatedPixels(kFALSE), fRecoverIsolatedPixels(0),
    fPostCleanType(0), fNamePedPhotCam(gsNamePedPhotCam),
    fNameGeomCam(gsNameGeomCam), fNameSignalCam(gsNameSignalCam)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

void MImgCleanStd::ResetCleaning() const
{
    //
    // check the number of all pixels against the noise level and
    // set them to 'unused' state if necessary
    //
    const UInt_t npixevt = fEvt->GetNumPixels();
    for (UInt_t idx=0; idx<npixevt; idx++)
    {
        MSignalPix &pix = (*fEvt)[idx];
        if (pix.IsPixelUnmapped())
            continue;

        pix.SetPixelUnused();
        pix.SetPixelCore(kFALSE);
    }
}

// --------------------------------------------------------------------------
//
// The first step of cleaning defines the CORE pixels. All the other pixels
// are set as UNUSED and belong to RING 0.
// After this point, only the CORE pixels are set as USED, with RING
// number 1.
//
//  NT 28/04/2003: now the option to use the standard method or the
//  democratic method is implemented:
//
//  kStandard:   This method looks for all pixels with an entry (photons)
//               that is three times bigger than the noise of the pixel
//               (default: 3 sigma, clean level 1)
//
//  kDemocratic: this method looks for all pixels with an entry (photons)
//               that is n times bigger than the noise of the mean of the
//               inner pixels (default: 3 sigmabar, clean level 1)
//
//
Bool_t MImgCleanStd::HasCoreNeighbors(const MGeom &gpix) const
{
//    if (fKeepIsolatedPixels)
//        return kTRUE;

#ifdef DEBUG
    const TArrayD &data = fData->GetData();
#else
    const Double_t *data = fData->GetData().GetArray();
#endif

    //loop on the neighbors to check if they are used
    const Int_t n = gpix.GetNumNeighbors();
    for (Int_t i=0; i<n; i++)
    {
        const Int_t idx = gpix.GetNeighbor(i);

        // Check if a neighborpixel of our core pixel is
        // also a core pixel
        if (data[idx]<=fCleanLvl1)
            continue;

        // Ignore unmapped pixels
        MSignalPix &pix = (*fEvt)[idx];
        if (pix.IsPixelUnmapped())
            continue;

        return kTRUE;
    }

    return kFALSE;
}

Bool_t MImgCleanStd::HasUsedNeighbors(const MGeom &gpix) const
{
    //loop on the neighbors to check if they are used
    const Int_t n = gpix.GetNumNeighbors();
    for (Int_t i=0; i<n; i++)
    {
        const Int_t idx = gpix.GetNeighbor(i);

        MSignalPix &pix = (*fEvt)[idx];

        // Check if a neighborpixel of our core pixel is
        // also a core pixel
        if (pix.IsPixelUsed() && !pix.IsPixelUnmapped())
            return kTRUE;
    }

    return kFALSE;
}


void MImgCleanStd::SetUsedNeighbors(const MGeom &gpix, Int_t r) const
{
    if (r>fCleanRings)
        return;

#ifdef DEBUG
    const TArrayD &data = fData->GetData();
#else
    const Double_t *data = fData->GetData().GetArray();
#endif

    // At least one neighbor has been identified as core,
    // that means we will keep the pixel
    const Int_t n = gpix.GetNumNeighbors();
    for (Int_t i=0; i<n; i++)
    {
        const Int_t idx = gpix.GetNeighbor(i);

        MSignalPix &pix = (*fEvt)[idx];

        // If the pixel has been assigned to the same or a previous
        // ring we don't have to proceed. We have to try to set the
        // ring number of each pixel as low as possible. This means
        // we are only allowed to increase the ring number.
        if (pix.IsPixelUsed() && pix.GetRing()<=r)
            continue;

        // All pixels below the second cleaning level should be ignored
        if (data[idx] <= fCleanLvl2)
            continue;

        // Ignore unmapped pixels (remark: used (aka. ring>0)
        // and unmapped status is exclusive
        if (pix.IsPixelUnmapped())
            continue;

        // Set or reset the ring number
        pix.SetRing(r);

        // Step forward to the surrounding pixels
        SetUsedNeighbors((*fCam)[idx], r+1);
    }
}

// --------------------------------------------------------------------------
//
// Here we do the cleaning. We search for all the possible core candidates
// and from them on we recursively search for used pixels with
// SetUsedNeighbors. To check the validity of a core pixel
// either fTimeLvl2 and/or HasCoreNeighbors is used.
// The size of all removed
Int_t MImgCleanStd::DoCleaning(Float_t &size) const
{
    Int_t n = 0;
    size = 0;

#ifdef DEBUG
    const TArrayD &data = fData->GetData();
#else
    const Double_t *data = fData->GetData().GetArray();
#endif

    //
    // check the number of all pixels against the noise level and
    // set them to 'unused' state if necessary
    //
    const UInt_t npixevt = fEvt->GetNumPixels();
    for (UInt_t idx=0; idx<npixevt; idx++)
    {
        MSignalPix &pix = (*fEvt)[idx];

        // Check if this pixel is a possible candidate for a core pixel
        if (data[idx] <= fCleanLvl1)
            continue;

        // Ignore unmapped pixels
        if (pix.IsPixelUnmapped())
            continue;

        const MGeom &gpix = (*fCam)[idx];

        // Check if the pixel is an isolated core pixel
        if (!HasCoreNeighbors(gpix))
        {
            // Count size and number of isolated core pixels
            size += pix.GetNumPhotons();
            n++;

            // If isolated pixels should not be kept or the pixel
            // is lower than the cleaning level for isolated core
            // pixels. It is not treated as core pixel.
            if (!fKeepIsolatedPixels || data[idx]<=fCleanLvl0)
                continue;
        }

        // Mark pixel as used and core
        pix.SetPixelUsed();
        pix.SetPixelCore();

        // Check if neighbor pixels should be marked as used
        // This is done recursively depening on fCleanRings
        SetUsedNeighbors(gpix);
    }

    return n;
}

/*
Float_t MImgCleanStd::GetArrivalTimeNeighbor(const MGeom &gpix) const
{
    Float_t min = FLT_MAX;

    const Int_t n = gpix.GetNumNeighbors();
    for (int i=0; i<n; i++)
    {
        const Int_t idx = gpix.GetNeighbor(i);

        const MSignalPix &pix = (*fEvt)[idx];
        // FIXME: Check also used pixels?
        if (!pix.IsCorePixel())
            continue;

        const Float_t tm = pix.GetArrivalTime();
        if (tm<min)
            min = tm;
    }

    return tm;
}

void MImgCleanStd::CheckUsedPixelsForArrivalTime(Float_t timediff) const
{
    const MArrayD &data = fData->GetData();

    //
    // check the number of all pixels against the noise level and
    // set them to 'unused' state if necessary
    //
    const UInt_t npixevt = fEvt->GetNumPixels();
    for (UInt_t idx=0; idx<npixevt; idx++)
    {
        MSignalPix &pix = (*fEvt)[idx];

        // If pixel has previously been marked used, ignore
        if (!pix.IsPixelUsed() || pix.IsPixelCore())
            continue;

        const MGeom &gpix = (*fCam)[idx];

        // If isolated possible-corepixel doesn't have used
        // neighbors, ignore it
        const Float_t tm0 = pix.GetArrivalTime();
        const Float_t tm1 = GetArrivalTimeCoreNeighbor(gpix);

        if (TMath::Abs(tm0-tm1)<timediff)
            continue;

        // Mark pixel as used and core
        pix.SetPixelUnused();
    }
}
*/

Int_t MImgCleanStd::RecoverIsolatedPixels(Float_t &size) const
{
#ifdef DEBUG
    const TArrayD &data = fData->GetData();
#else
    const Double_t *data = fData->GetData().GetArray();
#endif

    Int_t n = 0;

    //
    // check the number of all pixels against the noise level and
    // set them to 'unused' state if necessary
    //
    const UInt_t npixevt = fEvt->GetNumPixels();
    for (UInt_t idx=0; idx<npixevt; idx++)
    {
        MSignalPix &pix = (*fEvt)[idx];

        // If pixel has previously been marked used, ignore
        if (pix.IsPixelUsed())
            continue;

        // If pixel is not a candidate for a core pixel, ignore
        if (data[idx] <= fCleanLvl1)
            continue;

        const MGeom &gpix = (*fCam)[idx];

        // If isolated possible-corepixel doesn't have used
        // neighbors, ignore it
        if (!HasUsedNeighbors(gpix))
            continue;

        // Mark pixel as used and core
        pix.SetPixelUsed();
        pix.SetPixelCore();

        // Check if neighbor pixels should be marked as used
        // This is done recursively depening on fCleanRings
        SetUsedNeighbors(gpix);

        size -= pix.GetNumPhotons();
        n++;
    }

    return n;
}

void MImgCleanStd::CleanTime(Int_t n, Double_t lvl) const
{
    MArrayI indices;

    // Add conversion factor for dx here
    const Double_t dtmax = lvl*fCam->GetConvMm2Deg();

    const UInt_t npixevt = fEvt->GetNumPixels();
    for (UInt_t idx=0; idx<npixevt; idx++)
    {
        // check if pixel is used or not
        const MSignalPix &pix = (*fEvt)[idx];
        if (!pix.IsPixelUsed())
            continue;

        // get arrival time
        const Double_t tm0 = pix.GetArrivalTime();

        // loop over its neighbpors
        const MGeom &gpix = (*fCam)[idx];

        Int_t cnt = 0;
        for (Int_t i=0; i<gpix.GetNumNeighbors(); i++)
        {
            // Get index of neighbor
            const Int_t idx2 = gpix.GetNeighbor(i);

            // check if neighbor is used or not
            const MSignalPix &npix = (*fEvt)[idx2];
            if (!npix.IsPixelUsed())
                continue;

            const Double_t dt = TMath::Abs(npix.GetArrivalTime()-tm0);
            const Double_t dx = gpix.GetDist((*fCam)[idx2]);

            // If this pixel is to far away (in arrival time) don't count
            if (dt>dtmax*dx)
                continue;

            // Now count the pixel. If we did not found n pixels yet
            // which fullfill the condition, go on searching
            if (++cnt>=n)
                break;
        }

        // If we found at least n neighbors which are
        // with a time difference of lvl keep the pixel
        if (cnt>=n)
            continue;

        indices.Set(indices.GetSize()+1);
        indices[indices.GetSize()-1] = idx;
    }

    // Now remove the pixels which didn't fullfill the requirement
    for (UInt_t i=0; i<indices.GetSize(); i++)
    {
        (*fEvt)[indices[i]].SetPixelUnused();
        (*fEvt)[indices[i]].SetPixelCore(kFALSE);
    }
}

void MImgCleanStd::CleanStepTime() const
{
    if (fPostCleanType<=0)
        return;

    if (fPostCleanType&2)
        CleanTime(2, fTimeLvl2);

    if (fPostCleanType&1)
        CleanTime(1, fTimeLvl1);
}

// --------------------------------------------------------------------------
//
//  Check if MEvtHeader exists in the Parameter list already.
//  if not create one and add them to the list
//
Int_t MImgCleanStd::PreProcess (MParList *pList)
{
    fCam = (MGeomCam*)pList->FindObject(AddSerialNumber(fNameGeomCam), "MGeomCam");
    if (!fCam)
    {
        *fLog << err << fNameGeomCam << " [MGeomCam] not found (no geometry information available)... aborting." << endl;
        return kFALSE;
    }
    fEvt = (MSignalCam*)pList->FindObject(AddSerialNumber(fNameSignalCam), "MSignalCam");
    if (!fEvt)
    {
        *fLog << err << fNameSignalCam << " [MSignalCam] not found... aborting." << endl;
        return kFALSE;
    }

    fPed=0;
    if (fCleaningMethod!=kAbsolute && fCleaningMethod!=kTime)
    {
        fPed = (MPedPhotCam*)pList->FindObject(AddSerialNumber(fNamePedPhotCam), "MPedPhotCam");
        if (!fPed)
        {
            *fLog << err << fNamePedPhotCam << " [MPedPhotCam] not found... aborting." << endl;
            return kFALSE;
        }
    }

    fData = (MCameraData*)pList->FindCreateObj(AddSerialNumber("MCameraData"));
    if (!fData)
        return kFALSE;

    if (fCleanLvl2>fCleanLvl1)
    {
        *fLog << warn << "WARNING - fCleanLvl2 (" << fCleanLvl2 << ") > fCleanLvl1 (" << fCleanLvl1 << ")... resetting fCleanLvl2." << endl;
        fCleanLvl2 = fCleanLvl1;
    }

    if (fCleanLvl2==fCleanLvl1 && fCleanRings>0)
    {
        *fLog << warn << "WARNING - fCleanLvl2 == fCleanLvl1 (" << fCleanLvl1 << ") but fCleanRings>0... resetting fCleanRings to 0." << endl;
        fCleanRings=0;
    }

    if (fKeepIsolatedPixels && fCleanLvl0<fCleanLvl1)
    {
        *fLog << warn << "WARNING - fKeepIsolatedPixels set but CleanLvl0 (" << fTimeLvl2 << ") < fCleanLvl1 (" << fCleanLvl1 << ")... resetting fTimeLvl2." << endl;
        fTimeLvl2 = fCleanLvl1;
    }
    if (!fKeepIsolatedPixels && fCleanLvl0>fCleanLvl1)
    {
        *fLog << warn << "WARNING - fKeepIsolatedPixels not set but CleanLvl0 (" << fTimeLvl2 << ") > fCleanLvl1 (" << fCleanLvl1 << ")... setting fKeepIsolatedCorePixels." << endl;
        fKeepIsolatedPixels=kTRUE;
    }

    if (fKeepIsolatedPixels && fTimeLvl2==fCleanLvl1 && fRecoverIsolatedPixels!=0)
    {
        *fLog << warn << "WARNING - fTimeLvl2 == fCleanLvl1 (" << fTimeLvl2 << ") and fKeepSinglePixels and fRecoverIsolatedPixels!=0... setting fRecoverIsolatedPixels=0." << endl;
        fRecoverIsolatedPixels = 0;
    }

    Print();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Cleans the image.
//
Int_t MImgCleanStd::Process()
{
    switch (fCleaningMethod)
    {
    case kStandard:
        fData->CalcCleaningLevel(*fEvt, *fPed, *fCam);
        break;
    case kScaled:
        fData->CalcCleaningLevel2(*fEvt, *fPed, *fCam);
        break;
    case kDemocratic:
        fData->CalcCleaningLevelDemocratic(*fEvt, *fPed, *fCam);
        break;
    case kProbability:
        fData->CalcCleaningProbability(*fEvt, *fPed, *fCam);
        break;
    case kAbsolute:
        fData->CalcCleaningAbsolute(*fEvt, *fCam);
        break;
    case kTime:
        fData->CalcCleaningArrivalTime(*fEvt, *fCam);
        break;
    default:
        break;
    }

#ifdef DEBUG
    *fLog << all << "ResetCleaning" << endl;
#endif
    ResetCleaning();

#ifdef DEBUG
    *fLog << all << "DoCleaning" << endl;
#endif
    Float_t size;
    Short_t n = DoCleaning(size);

#ifdef DEBUG
    *fLog << all << "RecoverIsolatedPixels" << endl;
#endif
    for (UInt_t i=0; i<(UInt_t)fRecoverIsolatedPixels; i++)
    {
        const Int_t rc=RecoverIsolatedPixels(size);
        if (rc==0)
            break;

        n -= rc;
    }

#ifdef DEBUG
    *fLog << all << "Time Cleaning" << endl;
#endif
    // FIXME: Remove removed core piselx?
    CleanStepTime();

    fEvt->SetSinglePixels(n, size);

#ifdef DEBUG
    *fLog << all << "CalcIslands" << endl;
#endif
    // Takes roughly 10% of the time
    fEvt->CalcIslands(*fCam);

#ifdef DEBUG
    *fLog << all << "Done." << endl;
#endif

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Print descriptor and cleaning levels.
//
void MImgCleanStd::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << " using ";
    switch (fCleaningMethod)
    {
    case kDemocratic:
        *fLog << "democratic";
        break;
    case kStandard:
        *fLog << "standard";
        break;
    case kScaled:
        *fLog << "scaled";
        break;
    case kProbability:
        *fLog << "probability";
        break;
    case kAbsolute:
        *fLog << "absolute";
        break;
    case kTime:
        *fLog << "time";
        break;

    }
    *fLog << " cleaning" << endl;
    *fLog << "initialized with level " << fCleanLvl1 << " and " << fCleanLvl2;
    *fLog << " (CleanRings=" << fCleanRings << ")" << endl;

    if (fPostCleanType)
    {
        *fLog << "Time post cleaning is switched on with:" << endl;
        if (fPostCleanType&2)
            *fLog << " - Two pixel coincidence window: " << fTimeLvl2 << "ns" << endl;
        if (fPostCleanType&1)
            *fLog << " - One pixel coincidence window: " << fTimeLvl1 << "ns" << endl;
    }

    if (fKeepIsolatedPixels)
        *fLog << "isolated core pixels will be kept above " << fCleanLvl0 << endl;

    if (fRecoverIsolatedPixels)
    {
        if (fRecoverIsolatedPixels<0)
            *fLog << "all ";
        *fLog << "isolated core pixels with used pixels as neighbors will be recovered";
        if (fRecoverIsolatedPixels>0)
            *fLog << " " << fRecoverIsolatedPixels << " times";
        *fLog << "." << endl;;
    }

    if (fCleaningMethod!=kAbsolute && fCleaningMethod!=kTime)
    {
        *fLog << "Name of MPedPhotCam container used: ";
        *fLog << (fPed?((MParContainer*)fPed)->GetName():(const char*)fNamePedPhotCam) << endl;
    }
}

// --------------------------------------------------------------------------
//
//  Create two text entry fields, one for each cleaning level and a
//  describing text line.
//
void MImgCleanStd::CreateGuiElements(MGGroupFrame *f)
{
    //
    // Create a frame for line 3 and 4 to be able
    // to align entry field and label in one line
    //
    TGHorizontalFrame *f1 = new TGHorizontalFrame(f, 0, 0);
    TGHorizontalFrame *f2 = new TGHorizontalFrame(f, 0, 0);
    
    /*
     * --> use with root >=3.02 <--
     *
     
     TGNumberEntry *fNumEntry1 = new TGNumberEntry(frame, 3.0, 2, M_NENT_LVL1, kNESRealOne, kNEANonNegative);
     TGNumberEntry *fNumEntry2 = new TGNumberEntry(frame, 2.5, 2, M_NENT_LVL1, kNESRealOne, kNEANonNegative);
  
    */
    TGTextEntry *entry1 = new TGTextEntry(f1, "****", kImgCleanLvl1);
    TGTextEntry *entry2 = new TGTextEntry(f2, "****", kImgCleanLvl2);
    
    // --- doesn't work like expected (until root 3.02?) --- fNumEntry1->SetAlignment(kTextRight);
    // --- doesn't work like expected (until root 3.02?) --- fNumEntry2->SetAlignment(kTextRight);
    
    entry1->SetText("3.0");
    entry2->SetText("2.5");
    
    entry1->Associate(f);
    entry2->Associate(f);
    
    TGLabel *l1 = new TGLabel(f1, "Cleaning Level 1");
    TGLabel *l2 = new TGLabel(f2, "Cleaning Level 2");
    
    l1->SetTextJustify(kTextLeft);
    l2->SetTextJustify(kTextLeft);
    
    //
    // Align the text of the label centered, left in the row
    // with a left padding of 10
    //
    TGLayoutHints *laylabel = new TGLayoutHints(kLHintsCenterY|kLHintsLeft, 10);
    TGLayoutHints *layframe = new TGLayoutHints(kLHintsCenterY|kLHintsLeft,  5, 0, 10);
    
    //
    // Add one entry field and the corresponding label to each line
    //
    f1->AddFrame(entry1);
    f2->AddFrame(entry2);
    
    f1->AddFrame(l1, laylabel);
    f2->AddFrame(l2, laylabel);
    
    f->AddFrame(f1, layframe);
    f->AddFrame(f2, layframe);
    
    f->AddToList(entry1);
    f->AddToList(entry2);
    f->AddToList(l1);
    f->AddToList(l2);
    f->AddToList(laylabel);
    f->AddToList(layframe);
}

// --------------------------------------------------------------------------
//
//  Process the GUI Events coming from the two text entry fields.
//
Bool_t MImgCleanStd::ProcessMessage(Int_t msg, Int_t submsg, Long_t param1, Long_t param2)
{
    if (msg!=kC_TEXTENTRY || submsg!=kTE_ENTER)
        return kTRUE;

    TGTextEntry *txt = (TGTextEntry*)FindWidget(param1);

    if (!txt)
        return kTRUE;

    Float_t lvl = atof(txt->GetText());

    switch (param1)
    {
    case kImgCleanLvl1:
        fCleanLvl1 = lvl;
        *fLog << "Cleaning level 1 set to " << lvl << endl;
        return kTRUE;

    case kImgCleanLvl2:
        fCleanLvl2 = lvl;
        *fLog << "Cleaning level 2 set to " << lvl << endl;
        return kTRUE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MImgCleanStd::StreamPrimitive(ostream &out) const
{
    out << "   MImgCleanStd " << GetUniqueName() << "(";
    out << fCleanLvl1 << ", " << fCleanLvl2;

    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << ", \"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
    }
    out << ");" << endl;

    if (fCleaningMethod!=kStandard)
    {
        out << "   " << GetUniqueName() << ".SetMethod(MImgCleanStd::k";
        switch (fCleaningMethod)
        {
        case kScaled:      out << "Scaled";      break;
        case kDemocratic:  out << "Democratic";  break;
        case kProbability: out << "Probability"; break;
        case kAbsolute:    out << "Absolute";    break;
        case kTime    :    out << "Time";        break;
        default:
            break;
        }
        out << ");" << endl;
    }
    if (fCleanRings!=1)
        out << "   " << GetUniqueName() << ".SetCleanRings(" << fCleanRings << ");" << endl;

    if (gsNamePedPhotCam!=fNamePedPhotCam)
        out << "   " << GetUniqueName() << ".SetNamePedPhotCam(\"" << fNamePedPhotCam << "\");" << endl;
    if (gsNameGeomCam!=fNameGeomCam)
        out << "   " << GetUniqueName() << ".SetNameGeomCam(\"" << fNameGeomCam << "\");" << endl;
    if (gsNameSignalCam!=fNameSignalCam)
        out << "   " << GetUniqueName() << ".SetNameSignalCam(\"" << fNameSignalCam << "\");" << endl;
    if (fKeepIsolatedPixels)
        out << "   " << GetUniqueName() << ".SetKeepIsolatedPixels();" << endl;
    if (fRecoverIsolatedPixels)
        out << "   " << GetUniqueName() << ".SetRecoverIsolatedPixels(" << fRecoverIsolatedPixels << ");" << endl;

}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MImgCleanStd.CleanLevel1: 3.0
//   MImgCleanStd.CleanLevel2: 2.5
//   MImgCleanStd.CleanMethod: Standard, Scaled, Democratic, Probability, Absolute
//   MImgCleanStd.CleanRings:  1
//   MImgCleanStd.KeepIsolatedPixels: yes, no
//
Int_t MImgCleanStd::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "CleanRings", print))
    {
        rc = kTRUE;
        SetCleanRings(GetEnvValue(env, prefix, "CleanRings", fCleanRings));
    }
    if (IsEnvDefined(env, prefix, "CleanLevel0", print))
    {
        rc = kTRUE;
        fCleanLvl0 = GetEnvValue(env, prefix, "CleanLevel0", fCleanLvl0);
    }
    if (IsEnvDefined(env, prefix, "CleanLevel1", print))
    {
        rc = kTRUE;
        fCleanLvl1 = GetEnvValue(env, prefix, "CleanLevel1", fCleanLvl1);
    }
    if (IsEnvDefined(env, prefix, "CleanLevel2", print))
    {
        rc = kTRUE;
        fCleanLvl2 = GetEnvValue(env, prefix, "CleanLevel2", fCleanLvl2);
    }
    if (IsEnvDefined(env, prefix, "TimeLevel1", print))
    {
        rc = kTRUE;
        fTimeLvl1 = GetEnvValue(env, prefix, "TimeLevel1", fTimeLvl1);
    }
    if (IsEnvDefined(env, prefix, "TimeLevel2", print))
    {
        rc = kTRUE;
        fTimeLvl2 = GetEnvValue(env, prefix, "TimeLevel2", fTimeLvl2);
    }
    if (IsEnvDefined(env, prefix, "KeepIsolatedPixels", print))
    {
        rc = kTRUE;
        fKeepIsolatedPixels = GetEnvValue(env, prefix, "KeepIsolatedPixels", fKeepIsolatedPixels);
    }
    if (IsEnvDefined(env, prefix, "RecoverIsolatedPixels", print))
    {
        rc = kTRUE;
        fRecoverIsolatedPixels = GetEnvValue(env, prefix, "RecoverIsolatedPixels", fKeepIsolatedPixels);
    }
    if (IsEnvDefined(env, prefix, "PostCleanType", print))
    {
        rc = kTRUE;
        fPostCleanType = GetEnvValue(env, prefix, "PostCleanType", fPostCleanType);
    }

    if (IsEnvDefined(env, prefix, "CleanMethod", print))
    {
        rc = kTRUE;
        TString s = GetEnvValue(env, prefix, "CleanMethod", "");
        s.ToLower();
        if (s.BeginsWith("standard"))
            SetMethod(kStandard);
        if (s.BeginsWith("scaled"))
            SetMethod(kScaled);
        if (s.BeginsWith("democratic"))
            SetMethod(kDemocratic);
        if (s.BeginsWith("probability"))
            SetMethod(kProbability);
        if (s.BeginsWith("absolute"))
            SetMethod(kAbsolute);
        if (s.BeginsWith("time"))
            SetMethod(kTime);
    }

    return rc;
}
