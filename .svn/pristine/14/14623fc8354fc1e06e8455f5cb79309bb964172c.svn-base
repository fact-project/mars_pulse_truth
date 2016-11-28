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
!   Author(s): Thomas Bretz, 06/2012 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2012
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MImgCleanSimple
//
/////////////////////////////////////////////////////////////////////////////
#include "MImgCleanSimple.h"

#include <TEnv.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MSignalPix.h"
#include "MSignalCam.h"

ClassImp(MImgCleanSimple);

using namespace std;

static const TString gsDefName  = "MImgCleanSimple";
static const TString gsDefTitle = "Task to perform image cleaning";

const TString MImgCleanSimple::gsNameSignalCam ="MSignalCam";  // default name of the 'MSignalCam' container
const TString MImgCleanSimple::gsNameGeomCam   ="MGeomCam";    // default name of the 'MGeomCam' container

// --------------------------------------------------------------------------
//
// Default constructor. Here you can specify the cleaning method and levels. 
// If you don't specify them the 'common standard' values 3.0 and 2.5 (sigma
// above mean) are used.
// Here you can also specify how many rings around the core pixels you want 
// to analyze (with the fixed lvl2). The default value for "rings" is 1.
//
MImgCleanSimple::MImgCleanSimple(const Float_t lvl1, const Float_t lvl2,
                                 const char *name, const char *title) : fCleaningMethod(kStandard),
    fCleanLvl1(lvl1), fCleanLvl2(-1), fTimeLvl1(lvl2), fTimeLvl2(-1),
    fNameGeomCam(gsNameGeomCam), fNameSignalCam(gsNameSignalCam)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

void MImgCleanSimple::ResetCleaning() const
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

void MImgCleanSimple::DoCleanStd() const
{
    const UInt_t npixevt = fEvt->GetNumPixels();
    for (UInt_t idx=0; idx<npixevt; idx++)
    {
        MSignalPix &pix = (*fEvt)[idx];

        // Check if this is above the cleaning level
        if (pix.GetNumPhotons() <= fCleanLvl1)
            continue;

        // Ignore unmapped pixels
        if (pix.IsPixelUnmapped())
            continue;

        const MGeom &gpix = (*fCam)[idx];

        const Int_t n = gpix.GetNumNeighbors();
        for (Int_t i=0; i<n; i++)
        {
            const MSignalPix &npix = (*fEvt)[gpix.GetNeighbor(i)];

            // Check if one neighbor pixel is also above the cleaning level
            if (npix.GetNumPhotons() <= fCleanLvl1)
                continue;

            // Check if both pixels are within the given time window
            if (TMath::Abs(npix.GetArrivalTime()-pix.GetArrivalTime())>=fTimeLvl1)
                continue;

            // Ignore unmapped pixels
            if (npix.IsPixelUnmapped())
                continue;

            // The pixel is now marked to survive the cleaning. It's neighbor will
            // be marked later, because the condition is symmetric and the neighbor
            // will fullfil the condition, too.
            pix.SetPixelUsed();
            pix.SetPixelCore();

            break;
        }
    }
}

void MImgCleanSimple::DoCleanSum2() const
{
    const UInt_t npixevt = fEvt->GetNumPixels();
    for (UInt_t idx=0; idx<npixevt; idx++)
    {
        MSignalPix &pix = (*fEvt)[idx];

        // Ignore unmapped pixels
        if (pix.IsPixelUnmapped())
            continue;

        const MGeom &gpix = (*fCam)[idx];

        const Int_t n = gpix.GetNumNeighbors();
        for (Int_t i=0; i<n; i++)
        {
            const MSignalPix &npix = (*fEvt)[gpix.GetNeighbor(i)];

            // Check if the sum (or average) charge of the two neighbors is below threshold
            if (pix.GetNumPhotons()+npix.GetNumPhotons()<=fCleanLvl1*2)
                continue;

            // Check if both pixels are within the given time window
            if (TMath::Abs(npix.GetArrivalTime()-pix.GetArrivalTime())>=fTimeLvl1)
                continue;

            // Ignore unmapped pixels
            if (npix.IsPixelUnmapped())
                continue;

            // The pixel is now marked to survive the cleaning. It's neighbor will
            // be marked later, because the condition is symmetric and the neighbor
            // will fullfil the condition, too.
            pix.SetPixelUsed();
            pix.SetPixelCore();
            break;
        }
    }
}

void MImgCleanSimple::DoCleanSum3() const
{
    const Double_t clvl = fCleanLvl2>0 ? fCleanLvl2*3 : fCleanLvl1*3;
    const Double_t tlvl = fTimeLvl2>0  ? fTimeLvl2    : fTimeLvl1;

    const UInt_t npixevt = fEvt->GetNumPixels();
    for (UInt_t idx=0; idx<npixevt; idx++)
    {
        MSignalPix &pix = (*fEvt)[idx];

        // Ignore unmapped pixels
        if (pix.IsPixelUnmapped())
            continue;

        const MGeom &gpix = (*fCam)[idx];

        const Int_t n = gpix.GetNumNeighbors();
        for (Int_t i=0; i<n; i++)
        {
            const MSignalPix &npix1 = (*fEvt)[gpix.GetNeighbor(i)];
            const MSignalPix &npix2 = (*fEvt)[gpix.GetNeighbor((i+1)%n)];

            // Check if the sum (or average) charge of the three neighbors (closed package)
            // is below threshold
            if (pix.GetNumPhotons()+npix1.GetNumPhotons()+npix2.GetNumPhotons()<=clvl)
                continue;

            // Check if all thre pixels fullfil the time window condition
            if (TMath::Abs(pix.GetArrivalTime()-npix1.GetArrivalTime())>=tlvl)
                continue;
            if (TMath::Abs(npix1.GetArrivalTime()-npix2.GetArrivalTime())>=tlvl)
                continue;
            if (TMath::Abs(npix2.GetArrivalTime()-pix.GetArrivalTime())>=tlvl)
                continue;

            // Ignore unmapped pixels
            if (npix1.IsPixelUnmapped() || npix2.IsPixelUnmapped())
                continue;

            // The pixel is now marked to survive the cleaning. It's neighbors will
            // be marked later, because the condition is symmetric and the neighbors
            // will fullfil the condition, too.
            pix.SetPixelUsed();
            pix.SetPixelCore();
            break;
        }
    }
}

// --------------------------------------------------------------------------
//
//  Check if MEvtHeader exists in the Parameter list already.
//  if not create one and add them to the list
//
Int_t MImgCleanSimple::PreProcess(MParList *pList)
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

    Print();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Cleans the image.
//
Int_t MImgCleanSimple::Process()
{
    ResetCleaning();

    switch (fCleaningMethod)
    {
    case kStandard:
        DoCleanStd();   // Check each of the neighbors charge individually
        break;
    case kSum2:
        DoCleanSum2();  // Check the sum of two neighbors
    case kSum3:
        DoCleanSum2();  // Check the sum of two   neighbors
        DoCleanSum3();  // Check the sum of three neighbors
        break;
    }

    // Takes roughly 10% of the time
    fEvt->CalcIslands(*fCam);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Print descriptor and cleaning levels.
//
void MImgCleanSimple::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << " using ";
    switch (fCleaningMethod)
    {
    case kStandard:
        *fLog << "standard";
        break;
    case kSum2:
        *fLog << "sum2";
        break;
    case kSum3:
        *fLog << "sum3";
        break;

    }
    /*
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
    */
}

/*
// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MImgCleanSimple::StreamPrimitive(ostream &out) const
{
    out << "   MImgCleanSimple " << GetUniqueName() << "(";
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
        out << "   " << GetUniqueName() << ".SetMethod(MImgCleanSimple::k";
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
*/

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MImgCleanSimple.CleanLevel1: 3.0
//   MImgCleanSimple.CleanLevel2: 2.5
//   MImgCleanSimple.CleanMethod: Standard, Scaled, Democratic, Probability, Absolute
//   MImgCleanSimple.CleanRings:  1
//   MImgCleanSimple.KeepIsolatedPixels: yes, no
//
Int_t MImgCleanSimple::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
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

    if (IsEnvDefined(env, prefix, "CleanMethod", print))
    {
        rc = kTRUE;
        TString s = GetEnvValue(env, prefix, "CleanMethod", "");
        s.ToLower();
        if (s.BeginsWith("standard"))
            SetMethod(kStandard);
        if (s.BeginsWith("sum2"))
            SetMethod(kSum2);
        if (s.BeginsWith("sum3"))
            SetMethod(kSum3);
    }

    return rc;
}
