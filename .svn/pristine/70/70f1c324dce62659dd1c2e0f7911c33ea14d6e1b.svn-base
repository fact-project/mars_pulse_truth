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
!   Author(s): Markus Gaug  02/2004 <mailto:markus@ifae.es>
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MFCosmics
//
//   Filter to reject cosmics by the criterion that at least 
//   fMaxEmptyPixels of the pixels have values of lower than 3 Pedestal RMS. 
//   fMaxEmptyPixels is set to 0.4 by default which is slightly higher 
//   than the number of outer pixels in MAGIC (for the case that 
//   the outer pixels have some defect).
//
//   In the PostProcess we...
//    a) check the ratio of events which were accepted against
//       fMinAcceptedFraction.
//    b) check the ratio of events which were rejected against
//       fMaxAcceptedFraction.
//
//  Input Containers:
//   MRawEvtData
//   MPedestalCam
//   MBadPixelsCam
//   MExtractedSignalCam
//
//  Output Containers:
//   -/-
//
//////////////////////////////////////////////////////////////////////////////
#include "MFCosmics.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

ClassImp(MFCosmics);

using namespace std;

const TString MFCosmics::fgNamePedestalCam = "MPedestalCam";

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MFCosmics::MFCosmics(const char *name, const char *title)
    : fPedestals(NULL), fSignals(NULL), fBadPixels(NULL), fRawEvt(NULL),
    fNamePedestalCam(fgNamePedestalCam), fMaxEmptyPixels(0.2),
    fMinAcceptedFraction(0), fMaxAcceptedFraction(1)
{
    fName  = name  ? name  : "MFCosmics";
    fTitle = title ? title : "Filter to reject cosmics";
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawEvtData
//  - MPedestalCam
//  - MExtractedSignalCam
//
Int_t MFCosmics::PreProcess(MParList *pList)
{
    fRawEvt = (MRawEvtData*)pList->FindObject("MRawEvtData");
    if (!fRawEvt)
    {
      *fLog << err << "MRawEvtData not found... aborting." << endl;
      return kFALSE;
    }

    fPedestals = (MPedestalCam*)pList->FindObject(fNamePedestalCam, "MPedestalCam");
    if (!fPedestals)
    {
        *fLog << err << fNamePedestalCam << " [MPedestalCam] not found... aborting." << endl;
        return kFALSE;
    }

    fBadPixels = (MBadPixelsCam*)pList->FindObject("MBadPixelsCam");
    if (!fBadPixels)
    {
        *fLog << err << "MBadPixelsCam not found... aborting." << endl;
        return kFALSE;
    }

    fSignals = (MExtractedSignalCam*)pList->FindObject("MExtractedSignalCam");
    if (!fSignals)
    {
        *fLog << err << "MExtractedSignalCam not found... aborting." << endl;
        return kFALSE;
    }

    memset(fCut, 0, sizeof(fCut));

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Initialize number of used FADC slices
//
Bool_t MFCosmics::ReInit(MParList *pList)
{
    // The number here is just an average number. The real number
    // might change from event to event (up to 50%!)
    fSqrtHiGainSamples = TMath::Sqrt((Float_t) fSignals->GetNumUsedHiGainFADCSlices());

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Retrieve the integral of the FADC time slices and compare them to the 
// pedestal values.
//
Int_t MFCosmics::Process()
{
    fResult = CosmicsRejection();
    fCut[fResult ? 0 : 1]++;

    return kTRUE;
}

// ---------------------------------------------------------
//
// Cosmics rejection: 
// 
// Requiring less than fMaxEmptyPixels pixels to have values 
// lower than 3 Pedestal RMS. 
// 
// fMaxEmptyPixels is set to 230 by default which is slightly higher 
// than the number of outer pixels in MAGIC (for the case that 
// the outer pixels have some defect).
//
Bool_t MFCosmics::CosmicsRejection() const
{
    MRawEvtPixelIter pixel(fRawEvt);

    Int_t cosmicpix = 0;
    Int_t allpix    = 0;

    //
    // Create a first loop to sort out the cosmics ...
    //
    while (pixel.Next())
    {
        const UInt_t idx = pixel.GetPixelId();

        if ((*fBadPixels)[idx].IsUnsuitable())
            continue;

        const MExtractedSignalPix &sig = (*fSignals)[idx];
        if (!sig.IsHiGainValid())
            continue;

        allpix++;

        //
        // Check whether the pixel has a saturating hi-gain. In
        // this case the extracted hi-gain might be empty.
        //
        if (sig.IsHiGainSaturated())
            continue;

        const MPedestalPix  &ped = (*fPedestals)[idx];

        const Float_t pedrms = ped.GetPedestalRms()*fSqrtHiGainSamples;
        const Float_t sumhi  = sig.GetExtractedSignalHiGain();

        //
        // We consider a pixel as presumably due to cosmics
        // if its sum of FADC slices is lower than 3 pedestal RMS
        //
        if (sumhi < 3.*pedrms )
            cosmicpix++;
    }

    //
    // If the camera contains more than fMaxEmptyPixels
    // presumed pixels due to cosmics, then the event is discarded.
    //
    return cosmicpix > fMaxEmptyPixels*allpix;
}

// ---------------------------------------------------------
//
//   In the PostProcess we...
//    a) check the ratio of events which were accepted against
//       fMinAcceptedFraction.
//    b) check the ratio of events which were rejected against
//       fMaxAcceptedFraction.
//
//   return failure (kFALSE) if condition is not fullfilled.
//
Int_t MFCosmics::PostProcess()
{
    const UInt_t n = GetNumExecutions();
    if (n==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');

    *fLog << " " << setw(7) << fCut[0] << " (" << setw(3) ;
    *fLog << (int)(fCut[0]*100/n);
    *fLog << "%) Detected cosmics " ;
    *fLog << " (with fMaxEmptyPixels = " << fMaxEmptyPixels*100 << "%)" << endl;

    *fLog << " " << setw(7) << fCut[1] << " (" << setw(3) ;
    *fLog << (int)(fCut[1]*100/n);
    *fLog << "%) No cosmics!" << endl;
    *fLog << endl;

    if (fCut[0]<fMinAcceptedFraction*n)
    {
        *fLog << err << "ERROR - Fraction of accepted events " << Form("%.1f", fCut[0]*100./n);
        *fLog << "% underrun " << fMinAcceptedFraction*100 << "%... abort." << endl;
        return kFALSE;
    }
    if (fCut[0]>fMaxAcceptedFraction*n)
    {
        *fLog << err << "ERROR - Fraction of accepted events " << Form("%.1f", fCut[0]*100./n);
        *fLog << "% exceeded " << fMaxAcceptedFraction*100 << "%... abort." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MFCosmics.MaxEmptyPixels:    0.2
//   MFCosmics.MaxAcceptedFraction: 1
//   MFCosmics.MinAcceptedFraction: 0
//
Int_t MFCosmics::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "MaxEmptyPixels", print))
    {
        rc = kTRUE;
        SetMaxEmptyPixels(GetEnvValue(env, prefix, "MaxEmptyPixels", fMaxEmptyPixels));
    }
    if (IsEnvDefined(env, prefix, "MaxAcceptedFraction", print))
    {
        rc = kTRUE;
        SetMaxAcceptedFraction(GetEnvValue(env, prefix, "MaxAcceptedFraction", fMaxAcceptedFraction));
    }
    if (IsEnvDefined(env, prefix, "MinAcceptedFraction", print))
    {
        rc = kTRUE;
        fMinAcceptedFraction = GetEnvValue(env, prefix, "MinAcceptedFraction", fMinAcceptedFraction);
    }
    return rc;
}
