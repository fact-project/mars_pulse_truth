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
!   Author(s): Wolfgang Wittek, 04/2003 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Thomas Bretz, 04/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MFSoftwareTrigger
//
//  This is a class to evaluate a software trigger
//
//  The number of required pixels is setup by:
//   SetNumNeighbors(4)   // default
//
//  The Threshold is setup by:
//    SetThreshold(5)     // default
//
//  Time window for coincidence:
//    SetTimeWindow(3.3)
// To switch off time-coincidence use
//    SetTimeWindow(-1)
//   or
//    SetTimeWindow()
//
//  kSinglePixelsNeighbors <default>
//  ----------------------
//    Checks whether there is at least one pixel above threshold which
//    has fNumNeighbors direct neighbors which are also above threshold.
//    The outermost ring of pixels is ignord. Only 'used' pixels are
//    taken into account.
//
//  kAnyPattern 
//  -----------
//    Checks whether it find a cluster of pixels above fThreshold which
//    has a size bigger/equal than fNumNeighbors. The layout (pattern) of
//    the cluster is ignored. Unmapped pixels are ignored.
//
//  WARNING: Using trigger type kAllPattern resets the BIT(21) bit
//           of all pixels in MSignalCam
//
//
//  Input:
//    MSignalCam
//    MGeomCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MFSoftwareTrigger.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeom.h"
#include "MGeomCam.h"

#include "MSignalCam.h"
//#include "MArrivalTime.h"

ClassImp(MFSoftwareTrigger);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MFSoftwareTrigger::MFSoftwareTrigger(const char *name, const char *title)
    : fCam(NULL), fEvt(NULL), fThreshold(5),
    fTimeWindow(1.7), fNumNeighbors(4), fType(kSinglePixelNeighbors)
{
    fName  = name  ? name  : "MFSoftwareTrigger";
    fTitle = title ? title : "Filter for software trigger";
}

// --------------------------------------------------------------------------
//
// This function recursively finds all pixels of one island and sets
// BIT(14) for the pixel.
//
//  1) Check whether a pixel with the index idx exists, is unused
//     and has not yet BIT(14) set
//  2) Set BIT(14) to the pixel
//  3) Loop over all its neighbors taken from the geometry geom. For all
//     neighbors recursively call this function (CountPixels)
//  4) Sum the number of valid neighbors newly found
//
// Returns the size of the cluster
//
Int_t MFSoftwareTrigger::CountPixels(Int_t idx, Float_t tm0) const
{
    // get the pixel information of a pixel with this index
    MSignalPix &pix = (*fEvt)[idx];

    // If pixel already assigned to a cluster
    if (pix.TestBit(kWasChecked))
        return 0;

    if (pix.IsPixelUnmapped())
        return 0;

    // Assign the new island number num to this used pixel
    pix.SetBit(kWasChecked);

    // Get the size of this pixel and check threshold
    const Double_t size = pix.GetNumPhotons()*fCam->GetPixRatio(idx);
    if (size<fThreshold)
        return 0;

    const Float_t tm1 = pix.GetArrivalTime();
    if (TMath::Abs(tm1-tm0)>fTimeWindow)
        return 0;

    //pix.SetBit(kAboveThreshold);

    Int_t num = 1;

    // Get the geometry information (neighbors) of this pixel
    const MGeom &gpix = (*fCam)[idx];

    // Now do the same with all its neighbors and sum the
    // sizes which they correspond to
    const Int_t n = gpix.GetNumNeighbors();
    for (int i=0; i<n; i++)
        num += CountPixels(gpix.GetNeighbor(i), tm1);

    // return size of this (sub)cluster
    return num;
}
/*
Int_t MFSoftwareTrigger::CountCoincidencePixels(Int_t idx) const
{
    // Try to get the pixel information of a pixel with this index
    MSignalPix *pix = fEvt->GetPixById(idx);

    // If a pixel with this index is not existing... do nothing.
    if (!pix)
        return 0;

    // If pixel already assigned to a cluster
    if (pix->TestBit(kWasChecked))
        return 0;

    if (pix->IsPixelUnmapped())
        return 0;

    // Assign the new island number num to this used pixel
    pix->SetBit(kWasChecked);

    // Get the size of this pixel and check threshold
    const Double_t size = pix->GetNumPhotons();
    if (size<fThreshold)
        return 0;

    Int_t num = 1;

    // Get the geometry information (neighbors) of this pixel
    const MGeomPix &gpix = (*fCam)[idx];

    // Now do the same with all its neighbors and sum the
    // sizes which they correspond to
    const Int_t n = gpix.GetNumNeighbors();
    for (int i=0; i<n; i++)
        num += CountPixels(gpix.GetNeighbor(i));

    // return size of this (sub)cluster
    return num;
}
*/
void MFSoftwareTrigger::ResetBits(Int_t bits) const
{
    TObject *obj=0;

    TIter Next(*fEvt);
    while ((obj=Next()))
        obj->ResetBit(bits);
}

// --------------------------------------------------------------------------
//
// Check if there is at least one pixel which fullfills the condition
//
Bool_t MFSoftwareTrigger::ClusterTrigger() const
{
    ResetBits(kWasChecked);

    const UInt_t npixevt = fEvt->GetNumPixels();
    for (UInt_t idx=0; idx<npixevt; idx++)
    {
        const MSignalPix &pix = (*fEvt)[idx];
        if (!pix.IsPixelUsed())
            continue;

        // Check if trigger condition is fullfilled for this pixel
        if (CountPixels(idx, pix.GetArrivalTime()) >= fNumNeighbors)
            return kTRUE;
    }

/*
    // Reset bit
    MSignalPix *pix=0;

    // We could loop over all indices which looks more straight
    // forward but should be a lot slower (assuming zero supression)
    TIter Next(*fEvt);
    while ((pix=static_cast<MSignalPix*>(Next())))
    {
        // Check if trigger condition is fullfilled for this pixel
        const Int_t idx = pix->GetPixId();
        if (CountPixels(idx, (*fTme)[idx]) >= fNumNeighbors)
            return kTRUE;
    }
  */
    return kFALSE;
}
/*
Int_t MFSoftwareTrigger::CheckCoincidence(Int_t idx, Float_t tm0) const
{
    // Try to get the pixel information of a pixel with this index
    MSignalPix *pix = fEvt->GetPixById(idx);

    // If a pixel with this index is not existing... do nothing.
    if (!pix)
        return 0;

    // If pixel already assigned to a cluster
    if (pix->TestBit(kWasChecked))
        return 0;

    if (pix->IsPixelUnmapped())
        return 0;

    // Assign the new island number num to this used pixel
    pix->SetBit(kWasChecked);

    const Double_t size = pix->GetNumPhotons();
    if (size<fThreshold)
        return 0;

    const Float_t tm1 = (*fTme)[idx];
    if (TMath::Abs(tm1-tm0)>fTimeWindow)
        return 0;

    pix->SetBit(kIsCoincident);


    Int_t num = 1;

    // Get the geometry information (neighbors) of this pixel
    const MGeomPix &gpix = (*fCam)[idx];

    Int_t cnt = 0;

    // Now do the same with all its neighbors and sum the
    // sizes which they correspond to
    const Int_t n = gpix.GetNumNeighbors();
    for (int i=0; i<n; i++)
    {
        const Int_t rc = CheckCoincidence(gpix.GetNeighbor(i), tm0);
        if (fEvt->GetPixById(gpix.GetNeighbor(i))->TestBit(kIsCoincident))
            cnt++;
        num += rc;
    }

    // return size of this (sub)cluster
    return cnt<2 ? 0 : num;

}

Bool_t MFSoftwareTrigger::MagicLvl1Trigger() const
{
    // Reset bit
    MSignalPix *pix=0;

    // We could loop over all indices which looks more straight
    // forward but should be a lot slower (assuming zero supression)
    TIter Next(*fEvt);
    while ((pix=static_cast<MSignalPix*>(Next())))
    {
        ResetBits(kWasChecked|kIsCoincident);

        const Int_t idx = pix->GetPixId();
        if (CheckCoincidence(idx, (*fTme)[idx])<fNumNeighbors)
            continue;

        return kTRUE;
    }
    return kFALSE;
}
*/

const MSignalPix *MFSoftwareTrigger::CheckPixel(Int_t i) const
{
    const MSignalPix &pix = (*fEvt)[i];

    if (!pix.IsPixelUsed())
        return NULL;

    if (pix.GetNumPhotons()*fCam->GetPixRatio(i)<fThreshold)
        return NULL;

    if ((*fCam)[i].IsInOutermostRing())
        return NULL;

    return &pix;
}

// --------------------------------------------------------------------------
//
// Software single pixel coincidence trigger
//
Bool_t MFSoftwareTrigger::SwTrigger() const
{
    const Int_t entries = fEvt->GetNumPixels();
 
    for (Int_t i=0; i<entries; i++)
    {
        const MSignalPix *pix0 = CheckPixel(i);
        if (!pix0)
            continue;

        Int_t num = 1;

        const MGeom &gpix = (*fCam)[i];

        const Int_t nneighbors = gpix.GetNumNeighbors();
        for (Int_t n=0; n<nneighbors; n++)
        {
            const Int_t idx1 = gpix.GetNeighbor(n);

            const MSignalPix *pix1 = CheckPixel(idx1);
            if (!pix1)
                continue;

            const Float_t t0 = pix0->GetArrivalTime();
            const Float_t t1 = pix1->GetArrivalTime();

            if (TMath::Abs(t0-t1)>fTimeWindow)
                continue;

            if (++num==fNumNeighbors)
                return kTRUE;
        }
    }
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Request pointer to MSignalCam and MGeomCam from paremeter list
//
Int_t MFSoftwareTrigger::PreProcess(MParList *pList)
{
    fEvt = (MSignalCam*)pList->FindObject("MSignalCam");
    if (!fEvt)
    {
        *fLog << err << "MSignalCam not found... aborting." << endl;
        return kFALSE;
    }

    fCam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fCam)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    memset(fCut, 0, sizeof(fCut));

    switch (fType)
    {
    case kSinglePixelNeighbors:
        if (fNumNeighbors<2)
            *fLog << inf << "Software trigger switched off (fNumNeighbors<2)." << endl;
        else
            *fLog << inf << fNumNeighbors << " required above " << fThreshold << " within " << fTimeWindow << "ns." << endl;
        break;
    case kAnyPattern:
        *fLog << inf << "Cluster trigger switched on." << endl;
        break;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Evaluate software trigger
//
Int_t MFSoftwareTrigger::Process()
{
    switch (fType)
    {
    case kSinglePixelNeighbors:
        fResult = fNumNeighbors>1 ? SwTrigger() : kTRUE;
        break;
    case kAnyPattern:
        fResult = ClusterTrigger();
        break;
    }

    fCut[fResult ? 0 : 1]++;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Prints some statistics about the Basic selections.
//
Int_t MFSoftwareTrigger::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    TString type;
    switch (fType)
    {
    case kSinglePixelNeighbors:
        type = " single pixel trigger";
        break;
    case kAnyPattern:
        type = " any pattern trigger";
        break;
    }

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << " Threshold=" << fThreshold << ", Number=" << (int)fNumNeighbors;
    if (fTimeWindow>0)
        *fLog << ", Time Window=" << fTimeWindow << "ns";
    *fLog << endl;
    *fLog << dec << setfill(' ');

    *fLog << " " << setw(7) << fCut[0] << " (" << setw(3) ;
    *fLog << (int)(fCut[0]*100/GetNumExecutions());
    *fLog << "%) Evts fullfilled" << type << endl;
    *fLog << " " << setw(7) << fCut[1] << " (" << setw(3) ;
    *fLog << (int)(fCut[1]*100/GetNumExecutions());
    *fLog << "%) Evts didn't fullfill" << type << "."  << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MFSoftwareTrigger.Threshold:    5
//   MFSoftwareTrigger.NumNeighbors: 4
//   MFSoftwareTrigger.TimeWindow: 3.3
//   MFSoftwareTrigger.TriggerType: SinglePixel, AnyPattern
//
// To switch off time-coincidence use
//   MFSoftwareTrigger.TimeWindow: -1
//
Int_t MFSoftwareTrigger::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "Threshold", print))
    {
        rc = kTRUE;
        SetThreshold(GetEnvValue(env, prefix, "Threshold", fThreshold));
    }
    if (IsEnvDefined(env, prefix, "NumNeighbors", print))
    {
        rc = kTRUE;
        SetNumNeighbors(GetEnvValue(env, prefix, "NumNeighbors", fNumNeighbors));
    }
    if (IsEnvDefined(env, prefix, "TimeWindow", print))
    {
        rc = kTRUE;
        SetTimeWindow(GetEnvValue(env, prefix, "TimeWindow", fTimeWindow));
    }

    if (IsEnvDefined(env, prefix, "TriggerType", print))
    {
        TString dat = GetEnvValue(env, prefix, "TriggerType", "");
        dat = dat.Strip(TString::kBoth);
        dat.ToLower();

        if (dat == (TString)"singlepixel")
            SetTriggerType(kSinglePixelNeighbors);
        if (dat == (TString)"anypattern")
            SetTriggerType(kAnyPattern);

        rc = kTRUE;
    }

    return rc;
}
