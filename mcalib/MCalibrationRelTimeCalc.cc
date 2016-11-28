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
!   Author(s): Markus Gaug  04/2004 <mailto:markus@ifae.es>
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MCalibrationRelTimeCalc
//
//   Task to finalize the relative time calibration obtained 
//   from the fit results to the summed FADC slice distributions delivered by 
//   MCalibrationRelTimeCam, calculated and filled by MHCalibrationRelTimeCam, 
//
//   PreProcess(): Initialize pointers to MCalibrationRelTimeCam, 
//               
//   ReInit():     Initializes pointer to MBadPixelsCam
//
//   Process():    Nothing to be done, histograms getting filled by MHCalibrationChargeCam
//
//   PostProcess(): - FinalizeRelTimes()
//                  - FinalizeBadPixels()
//
// Class Version 2:
//  + Byte_t fCheckFlags; // Bit-field to hold the possible check flags
//
// ClassVersionb 3:
//  -  TString fOutputPath;                           //  Path to the output file
//  -  TString fOutputFile;                           //  Name of the output file
//
//
//  Input Containers:
//   MCalibrationRelTimeCam
//   MBadPixelsCam
//   MGeomCam
//
//  Output Containers:
//   MCalibrationRelTimeCam
//   MBadPixelsCam
//
//  
//////////////////////////////////////////////////////////////////////////////
#include "MCalibrationRelTimeCalc.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MMath.h"

#include "MParList.h"

#include "MStatusDisplay.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MCalibrationRelTimeCam.h"
#include "MCalibrationRelTimePix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

ClassImp(MCalibrationRelTimeCalc);

using namespace std;

const Float_t MCalibrationRelTimeCalc::fgRelTimeResolutionLimit = 5.0;

// --------------------------------------------------------------------------
//
// Default constructor.
//
// Sets all pointers to NULL
// 
// Initializes:
// - fRelTimeResolutionLimit to fgRelTimeResolutionimit
//
// Calls:
// - Clear()
//
MCalibrationRelTimeCalc::MCalibrationRelTimeCalc(const char *name, const char *title)
    : fGeom(NULL), fFlags(0)
{

  fName  = name  ? name  : "MCalibrationRelTimeCalc";
  fTitle = title ? title : "Task to finalize the relative time calibration";

  SetCheckFitResults       ( kFALSE );
  SetCheckDeviatingBehavior( kTRUE  );
  SetCheckHistOverflow     ( kFALSE );
  SetCheckOscillations     ( kFALSE );

  SetRelTimeResolutionLimit();

  Clear();

}

// --------------------------------------------------------------------------
//
// Sets:
// - all flags to kFALSE
// - all pointers to NULL
//
void MCalibrationRelTimeCalc::Clear(const Option_t *o)
{
    fBadPixels  = NULL;
    fCam        = NULL;
}

// --------------------------------------------------------------------------
//
// Search for the following input containers and abort if not existing:
//  - MGeomCam
//  - MCalibrationRelTimeCam
//  - MBadPixelsCam
// 
// It defines the PixId of every pixel in:
//
// - MCalibrationRelTimeCam 
//
// It sets all pixels in excluded which have the flag fBadBixelsPix::IsBad() set in:
// 
// - MCalibrationRelTimePix
//
Bool_t MCalibrationRelTimeCalc::ReInit(MParList *pList )
{

  fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
  if (!fGeom)
    {
      *fLog << err << "No MGeomCam found... aborting." << endl;
      return kFALSE;
    }
  
  fBadPixels = (MBadPixelsCam*)pList->FindObject(AddSerialNumber("MBadPixelsCam"));
  if (!fBadPixels)
  {
      *fLog << err << "Cannot find MBadPixelsCam ... abort." << endl;
      return kFALSE;
  }

  fCam = (MCalibrationRelTimeCam*)pList->FindObject(AddSerialNumber("MCalibrationRelTimeCam"));
  if (!fCam)
  {
      *fLog << err << "Cannot find MCalibrationRelTimeCam ... abort." << endl;
      return kFALSE;
  }

  if (IsDebug())
  {
      const UInt_t npixels = fGeom->GetNumPixels();
      for (UInt_t i=0; i<npixels; i++)
          (*fCam)[i].SetDebug();
  }

  return kTRUE;
}

// -----------------------------------------------------------------------
//
// Return if number of executions is null.
//
Int_t MCalibrationRelTimeCalc::PostProcess()
{

  if (GetNumExecutions()==0)
    return kTRUE;

  return Finalize();
}

// -----------------------------------------------------------------------
//
// First loop over pixels, average areas and sectors, call: 
//  - FinalizeRelTimes()
// for every entry. Count number of valid pixels in loop and return kFALSE
// if there are none (the "Michele check").
//
// Call FinalizeBadPixels()
//
// Call MParContainer::SetReadyToSave() for fCam
//
// Print out some statistics
//
Int_t MCalibrationRelTimeCalc::Finalize()
{
  
  //
  // First loop over pixels, call FinalizePedestals and FinalizeRelTimes
  //
  FinalizeRelTimes();

  //
  // Finalize Bad Pixels
  // 
  FinalizeBadPixels();

  //
  // Finalize calibration statistics
  //
  FinalizeUnsuitablePixels();

  fCam->SetReadyToSave();
  fBadPixels->SetReadyToSave();

  *fLog << inf << endl;
  *fLog << GetDescriptor() << ": Errors statistics:" << endl;  

  PrintUncalibrated(MBadPixelsPix::kDeviatingRelTimeResolution,
                    Form("%s%2.1f%s","Rel.time rms more than ", fRelTimeResolutionLimit, " dev from median:"));
  PrintUncalibrated(MBadPixelsPix::kRelTimeOscillating,   
                    "Pixels with changing Rel. Times over time:");
  PrintUncalibrated(MBadPixelsPix::kRelTimeNotFitted,     
                    "Pixels with unsuccesful Gauss fit to the times:");

  return kTRUE;
}


// ----------------------------------------------------------------------------------------------------
//
// Check for outliers. They are marked with
// MBadPixelsPix::kDeviatingTimeResolution
//
// see also MCalibrationChargeCalc::FinalizeAbsTimes
//
void MCalibrationRelTimeCalc::FinalizeRelTimes()
{
    const Int_t npixels = fGeom->GetNumPixels();
    const Int_t nareas  = fGeom->GetNumAreas();

    // Create an array capable of holding all pixels
    TArrayF arr(npixels);

    for (Int_t aidx=0; aidx<nareas; aidx++)
    {
        Int_t n = 0;
        for (Int_t i=0; i<npixels; i++)
        {
            // Check for this aidx only
            if ((*fGeom)[i].GetAidx()!=aidx)
                continue;

            // check if pixel may not contain a valid value
            if ((*fBadPixels)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
                continue;

            // check if it was excluded for some reason
            const MCalibrationRelTimePix &pix = (MCalibrationRelTimePix&)(*fCam)[i];
            if (pix.IsExcluded())
                continue;

            // if TimePrecision is valid fill it into array
            if (pix.GetTimePrecision()>0)
                arr[n++] = pix.GetTimePrecision();
        }

        // Check the ratio of valid entries to the ratio of pixels
        const Float_t ratio = 100*n/fGeom->GetNumPixWithAidx(aidx);
        if (3*ratio<2)
            *fLog << warn << "Area   " << setw(4) << aidx << ": Only " << ratio << "% pixels with valid time resolution found." << endl;

        // Calculate median and median deviation
        Double_t med;
        const Double_t dev = MMath::MedianDev(n, arr.GetArray(), med);

        // Calculate upper and lower limit
        const Float_t lolim = TMath::Max(med-fRelTimeResolutionLimit*dev, 0.);
        const Float_t hilim = TMath::Max(med+fRelTimeResolutionLimit*dev, 0.);

        // Now find the outliers
        for (Int_t i=0; i<npixels; i++)
        {
            // Search only within this aidx
            if ((*fGeom)[i].GetAidx()!=aidx)
                continue;

            // skip pixels already known to be unsuitable
            if ((*fBadPixels)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
                continue;

            // check if a pixel has been excluded. This
            const MCalibrationRelTimePix &pix = (MCalibrationRelTimePix&)(*fCam)[i];

            // Check if time precision is valid (might be invalid
            // for example in cae of empty histograms)
            const Float_t res = pix.GetTimePrecision();
            if (res<0) //FIXME!!! How does this happen?
            {
                *fLog << warn << "Pixel  " << setw(4) << i << ": Rel-time rms could not be calculated." << endl;
                (*fBadPixels)[i].SetUncalibrated(MBadPixelsPix::kDeviatingRelTimeResolution);
                continue;
            }

            // Now compare to a lower and upper limit
            if (res<=lolim || res>=hilim)
            {
                *fLog << warn << "Pixel  " << setw(4) << i << ": Deviation from rel-time rms: "
                    << Form("%5.2f", res) << " out of range "
                    << Form("[%4.2f,%4.2f]", lolim, hilim) << endl;

                (*fBadPixels)[i].SetUncalibrated(MBadPixelsPix::kDeviatingRelTimeResolution);
            }
        }
    }
}


// -----------------------------------------------------------------------------------
//
// Sets pixel to MBadPixelsPix::kUnsuitableRun, if one of the following flags is set:
// - MBadPixelsPix::kRelTimeIsPedestal
// - MBadPixelsPix::kRelTimeErrNotValid 
// - MBadPixelsPix::kRelTimeRelErrNotValid 
//
void MCalibrationRelTimeCalc::FinalizeBadPixels()
{

  for (Int_t i=0; i<fBadPixels->GetSize(); i++)
    {
      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (IsCheckDeviatingBehavior())
          if (bad.IsUncalibrated(MBadPixelsPix::kDeviatingRelTimeResolution))
              bad.SetUnsuitable(MBadPixelsPix::kUnsuitableRun);

      if (IsCheckFitResults())
          if (bad.IsUncalibrated(MBadPixelsPix::kRelTimeNotFitted))
              bad.SetUnsuitable(MBadPixelsPix::kUnreliableRun);

      if (IsCheckOscillations())
          if (bad.IsUncalibrated(MBadPixelsPix::kRelTimeOscillating))
              bad.SetUnsuitable(MBadPixelsPix::kUnreliableRun);
    }

}


// -----------------------------------------------------------------------------------------------
//
// - Print out statistics about BadPixels of type UnsuitableType_t 
// - store numbers of bad pixels of each type in fIntensCam or fCam
//
void MCalibrationRelTimeCalc::FinalizeUnsuitablePixels()
{
    *fLog << inf << endl;
    *fLog << GetDescriptor() << ": Rel. Times Calibration status:" << endl;
    *fLog << dec;

    const Int_t nareas = fGeom->GetNumAreas();

    TArrayI unsuit(nareas);
    TArrayI unrel(nareas);

    for (int aidx=0; aidx<nareas; aidx++)
    {
        unsuit[aidx] += fBadPixels->GetNumUnsuitable(MBadPixelsPix::kUnsuitableRun, fGeom, aidx);
        unrel[aidx]  += fBadPixels->GetNumUnsuitable(MBadPixelsPix::kUnreliableRun, fGeom, aidx);
        fCam->SetNumUnsuitable(unsuit[aidx], aidx);
        fCam->SetNumUnreliable(unrel[aidx],  aidx);
    }

    if (fGeom->InheritsFrom("MGeomCamMagic"))
    {
        PrintUncalibrated("Uncalibrated Pixels:", unsuit[0], unsuit[1]);
        PrintUncalibrated("Unreliable Pixels:",   unrel[0],  unrel[1]);
    }
}

// -----------------------------------------------------------------------------------------------
//
// Print out statistics about BadPixels of type UncalibratedType_t 
// 
void MCalibrationRelTimeCalc::PrintUncalibrated(MBadPixelsPix::UncalibratedType_t typ, const char *text) const 
{
    UInt_t countinner = 0;
    UInt_t countouter = 0;
    for (Int_t i=0; i<fBadPixels->GetSize(); i++)
    {
        if ((*fBadPixels)[i].IsUncalibrated(typ))
        {
            if (fGeom->GetPixRatio(i) == 1.)
                countinner++;
            else
                countouter++;
        }
    }

    PrintUncalibrated(text, countinner, countouter);
}

void MCalibrationRelTimeCalc::PrintUncalibrated(const char *text, Int_t in, Int_t out) const
{
    *fLog << " " << setfill(' ') << setw(48) << setiosflags(ios::left) << text;
    *fLog << " Inner: " << Form("%3i", in);
    *fLog << " Outer: " << Form("%3i", out);
    *fLog << resetiosflags(ios::left) << endl;

}

// --------------------------------------------------------------------------
//
// MCalibrationRelTimeCam.CheckFitResults: Yes
// MCalibrationRelTimeCam.CheckDeviatingBehavior: Yes
// MCalibrationRelTimeCam.CheckHistOverflow: Yes
// MCalibrationRelTimeCam.CheckOscillations: Yes
// 
Int_t MCalibrationRelTimeCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "CheckFitResults", print))
    {
        SetCheckFitResults(GetEnvValue(env, prefix, "CheckFitResults", IsCheckFitResults()));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "CheckDeviatingBehavior", print))
    {
        SetCheckDeviatingBehavior(GetEnvValue(env, prefix, "CheckDeviatingBehavior", IsCheckDeviatingBehavior()));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "CheckHistOverflow", print))
    {
        SetCheckHistOverflow(GetEnvValue(env, prefix, "CheckHistOverflow", IsCheckHistOverflow()));
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "CheckOscillations", print))
    {
        SetCheckOscillations(GetEnvValue(env, prefix, "CheckOscillations", IsCheckOscillations()));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "RelTimeResolutionLimit", print))
    {
        SetRelTimeResolutionLimit(GetEnvValue(env, prefix, "RelTimeResolutionLimit", fRelTimeResolutionLimit));
        rc = kTRUE;
    }

    return rc;
}
