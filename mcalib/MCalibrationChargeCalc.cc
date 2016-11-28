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
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
//   MCalibrationChargeCalc
//
//   Task to calculate the calibration conversion factors and quantum efficiencies 
//   from the fit results to the summed FADC slice distributions delivered by 
//   MCalibrationChargeCam, MCalibrationChargePix, MCalibrationChargeBlindPix and 
//   MCalibrationChargePINDiode, calculated and filled by MHCalibrationChargeCam, 
//   MHCalibrationChargePix, MHCalibrationChargeBlindPix and MHCalibrationChargePINDiode.
//
//   PreProcess(): Initialize pointers to MCalibrationChargeCam, MCalibrationChargeBlindPix
//                 MCalibrationChargePINDiode and  MCalibrationQECam
//
//                 Initialize pulser light wavelength
//               
//   ReInit():     MCalibrationCam::InitSize(NumPixels) is called from MGeomApply (which allocates
//                 memory in a TClonesArray of type MCalibrationChargePix)
//                 Initializes pointer to MBadPixelsCam
//
//   Process():    Nothing to be done, histograms getting filled by MHCalibrationChargeCam
//
//   PostProcess(): - FinalizePedestals()
//                  - FinalizeCharges()
//                  - FinalizeFFactorMethod()
//                  - FinalizeBadPixels()
//                  - FinalizeBlindCam()
//                  - FinalizePINDiode()
//                  - FinalizeFFactorQECam()
//                  - FinalizeBlindPixelQECam()
//                  - FinalizePINDiodeQECam()
//
//  Input Containers:
//   MCalibrationChargeCam
//   MCalibrationChargeBlindPix
//   MCalibrationChargePINDiode
//   MCalibrationQECam
//   MPedestalCam
//   MBadPixelsCam
//   MGeomCam
//   MTime
//
//  Output Containers:
//   MCalibrationChargeCam
//   MCalibrationChargeBlindPix
//   MCalibrationChargePINDiode
//   MCalibrationQECam
//   MBadPixelsCam
//
//
//  Preliminary description of the calibration in photons (email from 12/02/04)
//
//  Why calibrating in photons:
//  ===========================
//  
//  At the Barcelona meeting in 2002, we decided to calibrate the camera in
//  photons. This for the following reasons:
//  
//  * The physical quantity arriving at the camera are photons. This is
//    the direct physical information from the air shower. The photons
//    have a flux and a spectrum.
//  
//  * The photon fluxes depend mostly on the shower energy (with
//    corrections deriving from the observation conditions), while the photon
//    spectra depend mostly on the observation conditions: zenith angle,
//    quality of the air, also the impact parameter of the shower.
//  
//  * The photomultiplier, in turn, has different response properties
//    (quantum efficiencies) for photons of different colour. (Moreover,
//    different pixels have slightly different quantum efficiencies).
//    The resulting number of photo-electrons is then amplified (linearly)
//    with respect to the photo-electron flux.
//  
//  * In the ideal case, one would like to disentagle the effects
//    of the observation conditions from the primary particle energy (which
//    one likes to measure). To do so, one needs:
//  
//    1) A reliable calibration relating the FADC counts to the photo-electron
//       flux -> This is accomplished with the F-Factor method.
//  
//    2) A reliable calibration of the wavelength-dependent quantum efficiency
//       -> This is accomplished with the combination of the three methods,
//          together with QE-measurements performed by David in order to do
//          the interpolation.
//  
//    3) A reliable calibration of the observation conditions. This means:
//       - Tracing the atmospheric conditions   -> LIDAR
//       - Tracing the observation zenith angle -> Drive System
//
//    4) Some knowlegde about the impact parameter:
//       - This is the only part which cannot be accomplished well with a
//         single telescope. We would thus need to convolute the spectrum
//         over the distribution of impact parameters.
//  
//  
//  How an ideal calibration would look like:
//  =========================================
//  
//  We know from the combined PIN-Diode and Blind-Pixel Method the response of
//  each pixel to well-measured light fluxes in three representative
//  wavelengths (green, blue, UV). We also know the response to these light
//  fluxes in photo-electrons. Thus, we can derive:
//  
//  - conversion factors to photo-electrons
//  - conversion factors to photons in three wavelengths.
//  
//  Together with David's measurements and some MC-simulation, we should be
//  able to derive tables for typical Cherenkov-photon spectra - convoluted
//  with the impact parameters and depending on the athmospheric conditions
//  and the zenith angle (the "outer parameters").
//  
//  From these tables we can create "calibration tables" containing some
//  effective quantum efficiency depending on these outer parameters and which
//  are different for each pixel.
//  
//  In an ideal MCalibrate, one would thus have to convert first the FADC
//  slices to Photo-electrons and then, depending on the outer parameters,
//  look up the effective quantum efficiency and get the mean number of
//  photons which is then used for the further analysis.
//  
//  How the (first) MAGIC calibration should look like:
//  ===================================================
//  
//  For the moment, we have only one reliable calibration method, although
//  with very large systematic errors. This is the F-Factor method. Knowing
//  that the light is uniform over the whole camera (which I would not at all
//  guarantee in the case of the CT1 pulser), one could in principle already
//  perform a relative calibration of the quantum efficiencies in the UV.
//  However, the spread in QE at UV is about 10-15% (according to the plot
//  that Abelardo sent around last time. The spread in photo-electrons is 15%
//  for the inner pixels, but much larger (40%) for the outer ones.
//  
//  I'm not sure if we can already say that we have measured the relative
//  difference in quantum efficiency for the inner pixels and produce a first
//  QE-table for each pixel. To so, I would rather check in other wavelengths
//  (which we can do in about one-two weeks when the optical transmission of
//  the calibration trigger is installed).
//  
//  Thus, for the moment being, I would join Thomas proposal to calibrate in
//  photo-electrons and apply one stupid average quantum efficiency for all
//  pixels. This keeping in mind that we will have much preciser information
//  in about one to two weeks.
//  
//  
//  What MCalibrate should calculate and what should be stored:
//  ===========================================================
//  
//  It is clear that in the end, MCerPhotEvt will store photons.
//  MCalibrationCam stores the conversionfactors to photo-electrons and also
//  some tables of how to apply the conversion to photons, given the outer
//  parameters. This is not yet implemented and not even discussed.
//  
//  To start, I would suggest that we define the "average quantum efficiency"
//  (maybe something like 25+-3%) and apply them equally to all
//  photo-electrons. Later, this average factor can be easily replaced by a
//  pixel-dependent factor and later by a (pixel-dependent) table.
//  
//  
//  ClassVersion 2:
//   - Float_t fPheErrLimit;
//   + Float_t fPheErrLowerLimit; // Lower limit acceptance nr. phe's w.r.t. area idx mean (in sigmas)
//   + Float_t fPheErrUpperLimit; // Upper limit acceptance nr. phe's w.r.t. area idx mean (in sigmas)
//
//
//  ClassVersion 3:
//   + Bool_t  fUseExtractorRes;  // Include extractor resolution in F-Factor method
//
//  ClassVersion 4:
//   +  Float_t fUnsuitablesLimit;                   // Limit for relative number of unsuitable pixels
//   +  Float_t fUnreliablesLimit;                   // Limit for relative number of unreliable pixels
//   +  Float_t fExternalNumPhes;                    // External mean number of photo-electrons set from outside
//   +  Float_t fExternalNumPhesRelVar;              // External rel. var. number of photo-electrons set from outside  
//
//  ClassVersion 5:
//   -  TString fOutputPath
//   -  TString fOutputFile
//
//  ClassVersion 6:
//   -  Bool_t fContinousCalibration
//
//////////////////////////////////////////////////////////////////////////////
#include "MCalibrationChargeCalc.h"

#include <TSystem.h>
#include <TH1.h>
#include <TF1.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MMath.h"

#include "MParList.h"

#include "MCalibrationPattern.h"

#include "MGeomCam.h"
#include "MGeom.h"
#include "MHCamera.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MHCalibrationChargeCam.h"
#include "MHCalibrationChargeBlindCam.h"

#include "MCalibrationChargeCam.h"
#include "MCalibrationChargePix.h"
#include "MCalibrationChargePINDiode.h"
#include "MCalibrationBlindPix.h"
#include "MCalibrationBlindCam.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"
#include "MExtractedSignalBlindPixel.h"
#include "MExtractedSignalPINDiode.h"

#include "MBadPixelsCam.h"

#include "MCalibrationQECam.h"
#include "MCalibrationQEPix.h"

ClassImp(MCalibrationChargeCalc);

using namespace std;

const Float_t MCalibrationChargeCalc::fgChargeLimit            = 4.5;
const Float_t MCalibrationChargeCalc::fgChargeErrLimit         = 0.;
const Float_t MCalibrationChargeCalc::fgChargeRelErrLimit      = 1.;
const Float_t MCalibrationChargeCalc::fgLambdaErrLimit         = 0.2;
const Float_t MCalibrationChargeCalc::fgLambdaCheckLimit       = 0.5;
const Float_t MCalibrationChargeCalc::fgPheErrLowerLimit       = 6.0;
const Float_t MCalibrationChargeCalc::fgPheErrUpperLimit       = 5.5;
const Float_t MCalibrationChargeCalc::fgFFactorErrLimit        = 4.5;
const Float_t MCalibrationChargeCalc::fgArrTimeRmsLimit        = 5.0;
const Float_t MCalibrationChargeCalc::fgUnsuitablesLimit       = 0.1;
const Float_t MCalibrationChargeCalc::fgUnreliablesLimit       = 0.3;

const char *MCalibrationChargeCalc::fgNamePedestalCam = "MPedestalCam";


// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets the pointer to fQECam and fGeom to NULL
// 
// Calls AddToBranchList for: 
// - MRawEvtData.fHiGainPixId   
// - MRawEvtData.fLoGainPixId   
// - MRawEvtData.fHiGainFadcSamples
// - MRawEvtData.fLoGainFadcSamples
//
// Initializes:
// - fArrTimeRmsLimit     to fgArrTimeRmsLimit
// - fChargeLimit         to fgChargeLimit
// - fChargeErrLimit      to fgChargeErrLimit
// - fChargeRelErrLimit   to fgChargeRelErrLimit
// - fFFactorErrLimit     to fgFFactorErrLimit
// - fLambdaCheckLimit    to fgLambdaCheckLimit
// - fLambdaErrLimit      to fgLambdaErrLimit
// - fNamePedestalCam     to fgNamePedestalCam
// - fPheErrLowerLimit    to fgPheErrLowerLimit
// - fPheErrUpperLimit    to fgPheErrUpperLimit
// - fPulserColor         to MCalibrationCam::kCT1
// - fOutputPath          to "."
// - fOutputFile          to "ChargeCalibStat.txt"
// - flag debug           to kFALSE
// - IsUseExtractorRes    to kFALSE
// - IsUseExternalNumPhes to kFALSE
// - fExternalNumPhes     to 0.
// - fExternalNumPhesRelVar to 0.
//
// Sets all checks
//
// Calls:
// - Clear()
//
MCalibrationChargeCalc::MCalibrationChargeCalc(const char *name, const char *title)
    : fPulserColor(MCalibrationCam::kNONE), fContinousCalibration(kFALSE),
    fGeom(NULL), fSignal(NULL), fCalibPattern(NULL), fExtractor(NULL)
{
        
  fName  = name  ? name  : "MCalibrationChargeCalc";
  fTitle = title ? title : "Task to calculate the calibration constants and MCalibrationCam ";
  
  AddToBranchList("MRawEvtData.fHiGainPixId");
  AddToBranchList("MRawEvtData.fLoGainPixId");
  AddToBranchList("MRawEvtData.fHiGainFadcSamples");
  AddToBranchList("MRawEvtData.fLoGainFadcSamples");
  
  SetArrTimeRmsLimit   ();
  SetChargeLimit       ();
  SetChargeErrLimit    ();  
  SetChargeRelErrLimit ();
  SetDebug       ( kFALSE );
  SetFFactorErrLimit   ();
  SetLambdaCheckLimit  ();
  SetLambdaErrLimit    ();
  SetNamePedestalCam   ();
  SetPheErrLowerLimit  ();
  SetPheErrUpperLimit  ();
  SetUnsuitablesLimit  ();
  SetUnreliablesLimit  ();
  SetUseExtractorRes   ();
  SetUseUnreliables    ();
  SetUseExternalNumPhes(kFALSE);
 
  SetExternalNumPhes      ();
  SetExternalNumPhesRelVar();

  SetCheckArrivalTimes     ();
  SetCheckDeadPixels       ();
  SetCheckDeviatingBehavior();
  SetCheckExtractionWindow ();
  SetCheckHistOverflow     ();
  SetCheckOscillations     ();

  Clear();
  
}

// --------------------------------------------------------------------------
//
// Sets:
// - all variables to 0., 
// - all flags to kFALSE
// - all pointers to NULL
// - the pulser colour to kNONE
// - fBlindPixelFlags to 0
// - fPINDiodeFlags   to 0
//
void MCalibrationChargeCalc::Clear(const Option_t *o)
{
  
    fNumHiGainSamples          = 0.;
    fNumLoGainSamples          = 0.;
    fSqrtHiGainSamples         = 0.;
    fSqrtLoGainSamples         = 0.;
    fNumInnerFFactorMethodUsed = 0;

    fNumProcessed              = 0;

    fBadPixels   = NULL;
    fCam         = NULL;
    fHCam        = NULL;
    fQECam       = NULL;
    fBlindCam    = NULL;
    fHBlindCam   = NULL;
    fPINDiode    = NULL;
    fPedestals   = NULL;

    SetPulserColor ( MCalibrationCam::kNONE );

    fStrength    = 0.;
    fBlindPixelFlags.Set(0);
    fPINDiodeFlags  .Set(0);
    fResultFlags    .Set(0);
}


// -----------------------------------------------------------------------------------
//
// The following container are searched for and execution aborted if not in MParList:
//  - MPedestalCam
//  - MCalibrationPattern
//  - MExtractedSignalCam
//
Int_t MCalibrationChargeCalc::PreProcess(MParList *pList)
{
  
  /*
  if (IsInterlaced())
    {
      fTrigPattern = (MTriggerPattern*)pList->FindObject("MTriggerPattern");
      if (!fTrigPattern)
        {
          *fLog << err << "MTriggerPattern not found... abort." << endl;
          return kFALSE;
        }
    }
  */

  fCalibPattern = (MCalibrationPattern*)pList->FindObject("MCalibrationPattern");
  if (!fCalibPattern)
    {
      *fLog << err << "MCalibrationPattern not found... abort." << endl;
      return kFALSE;
    }

  // 
  // Containers that have to be there.
  //
  fSignal = (MExtractedSignalCam*)pList->FindObject("MExtractedSignalCam");
  if (!fSignal)
    {
      *fLog << err << "MExtractedSignalCam not found... aborting" << endl;
      return kFALSE;
    }

  if (fPedestals)
    return kTRUE;

  fPedestals = (MPedestalCam*)pList->FindObject(AddSerialNumber(fNamePedestalCam), "MPedestalCam");
  if (!fPedestals)
    {
      *fLog << err << fNamePedestalCam << " [MPedestalCam] not found... aborting" << endl;
      return kFALSE;
    }

  // Obsolete... it is already set in the constructor isn't it?
  fPulserColor = MCalibrationCam::kNONE;

  return kTRUE;
}


// --------------------------------------------------------------------------
//
// Search for the following input containers and abort if not existing:
//  - MGeomCam
// -  MCalibrationChargeCam
//  - MCalibrationQECam
//  - MBadPixelsCam
// 
// Search for the following input containers and give a warning if not existing:
//  - MCalibrationBlindPix
//  - MCalibrationChargePINDiode
//
// It retrieves the following variables from MCalibrationChargeCam:
//
//  - fNumHiGainSamples 
//  - fNumLoGainSamples 
//
// It defines the PixId of every pixel in:
//
// - MCalibrationChargeCam 
// - MCalibrationQECam
//
// It sets all pixels in excluded which have the flag fBadBixelsPix::IsBad() set in:
// 
// - MCalibrationChargePix
// - MCalibrationQEPix
//
// Sets the pulser colour and tests if it has not changed w.r.t. fPulserColor in:
//
// - MCalibrationChargeCam
// - MCalibrationBlindPix (if existing)
// - MCalibrationChargePINDiode (if existing)
//
Bool_t MCalibrationChargeCalc::ReInit(MParList *pList )
{

    //
    // Geometry
    //
  fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
  if (!fGeom)
    {
      *fLog << err << "No MGeomCam found... aborting." << endl;
      return kFALSE;
    }

    //
    // Charge histogram
    //
  fHCam = (MHCalibrationChargeCam*)pList->FindObject(AddSerialNumber("MHCalibrationChargeCam"));
  if (!fHCam)
    {
      *fLog << err << "Cannot find MHCalibrationChargeCam ... abort." << endl;
      *fLog << "Maybe you forget to call an MFillH for the MHCalibrationChargeCam before..." << endl;
      return kFALSE;
    }
  
    //
    // MCalibrationChargeCam
    //
  fCam = (MCalibrationChargeCam*)pList->FindObject(AddSerialNumber("MCalibrationChargeCam"));
  if (!fCam)
  {
      *fLog << err << "Cannot find MCalibrationChargeCam ... abort." << endl;
      *fLog << "Maybe you forget to call an MFillH for the MHCalibrationChargeCam before..." << endl;
      return kFALSE;
  }

  //
  // Blind pixel calibration
  //
  fBlindCam = (MCalibrationBlindCam*)pList->FindObject(AddSerialNumber("MCalibrationBlindCam"));
  if (!fBlindCam)
      *fLog << inf << "No MCalibrationBlindCam found... no Blind Pixel method!" << endl;

  //
  // Required containers
  //
  fQECam = (MCalibrationQECam*)pList->FindObject(AddSerialNumber("MCalibrationQECam"));
  if (!fQECam)
  {
      *fLog << err << "Cannot find MCalibrationQECam ... abort." << endl;
      return kFALSE;
  }

  fBadPixels = (MBadPixelsCam*)pList->FindObject(AddSerialNumber("MBadPixelsCam"));
  if (!fBadPixels)
  {
      *fLog << err << "Cannot find MBadPixelsCam ... abort." << endl;
      return kFALSE;
  }
  
  // 
  // Optional Containers 
  //
  fPINDiode = (MCalibrationChargePINDiode*)pList->FindObject("MCalibrationChargePINDiode");
  if (!fPINDiode)
    *fLog << inf << "No MCalibrationChargePINDiode found... no PIN Diode method!" << endl;

  fHBlindCam = (MHCalibrationChargeBlindCam*)pList->FindObject(AddSerialNumber("MHCalibrationChargeBlindCam"));
  if (!fHBlindCam)
    *fLog << inf << "No MHCalibrationChargeBlindCam found... no Blind Pixel method!" << endl;


  //
  // Further initialization
  //
  if (IsDebug())
  {
      const UInt_t npixels = fGeom->GetNumPixels();
      for (UInt_t i=0; i<npixels; i++)
          (*fCam)[i].SetDebug();
  }

  fResultFlags.Set(fGeom->GetNumAreas());

  return kTRUE;
}

// ----------------------------------------------------------------------------------
//  
// Set the correct colour to the charge containers
// 
Int_t MCalibrationChargeCalc::Process()
{

  const MCalibrationCam::PulserColor_t col = fCalibPattern->GetPulserColor();

  //
  // If pulser color didn't change there is nothing to do
  //
  const Float_t strength = fCalibPattern->GetPulserStrength();
  const Float_t strdiff  = TMath::Abs(strength-fStrength);
  if (col == fPulserColor && strdiff < 0.05 )
    {
      fNumProcessed++;
      return kTRUE;
    }

  //
  // If there is no color is nothing to do
  //
  if (col == MCalibrationCam::kNONE)
    return kTRUE;

  //
  // Now retrieve the colour and check if not various colours have been used
  //
  if (!fContinousCalibration && fPulserColor != MCalibrationCam::kNONE)
  {
      *fLog << warn << "Multiple colours used simultaneously!" << flush;
      fHCam->Finalize();
      if (fHBlindCam)
          fHBlindCam->Finalize();

      Finalize();

      fHCam->ResetHists();
      if (fHBlindCam)
          fHBlindCam->ResetHists();

      *fLog << inf << "Starting next calibration... " << flush;

      fHCam->SetColor(col);
      if (fHBlindCam)
          fHBlindCam->SetColor(col);

      fCam->SetPulserColor(col);
      if (fBlindCam)
          fBlindCam->SetPulserColor(col);
  }

  //
  // Keep pulser color and strength in mind and output some information
  //
  fPulserColor = col;
  fStrength    = strength;

  *fLog << inf << "Found new colour ... " << flush;

  switch (col)
    {
    case MCalibrationCam::kGREEN: *fLog << "Green";  break;
    case MCalibrationCam::kBLUE:  *fLog << "Blue";   break;
    case MCalibrationCam::kUV:    *fLog << "UV";     break;
    case MCalibrationCam::kCT1:   *fLog << "CT1";    break;
    default: break;
    }

  *fLog << inf << " with strength: " << strength << endl;
  
  //
  // Distribute new color to all containers
  //
  fCam->SetPulserColor(col);
  if (fBlindCam)
    fBlindCam->SetPulserColor(col);

  fHCam->SetColor(col);
  if (fHBlindCam)
    fHBlindCam->SetColor(col);
  if (fPINDiode)
    fPINDiode->SetColor(col);

  //
  // Reset number of processed events
  //
  fNumProcessed = 0;
  
  return kTRUE;
}

// -----------------------------------------------------------------------
//
// Return if number of executions is null.
//
Int_t MCalibrationChargeCalc::PostProcess()
{

  if (GetNumExecutions() < 1)
    return kTRUE;

  if (fPulserColor == MCalibrationCam::kNONE)
  {
      *fLog << err << "ERROR - No event was flagged with a pulser color... no calibration possible." << endl;
      return kFALSE;
  }

  if (fNumProcessed == 0)
    return kTRUE;

  *fLog << endl;

  return Finalize();
}

// ----------------------------------------------------------------------------------------------------
//
// Check for outliers. They are marked with
// MBadPixelsPix::kFluctuatingArrivalTimes
//
// see also MCalibrationRelTimeCalc::FinalizeRelTimes
//
void MCalibrationChargeCalc::FinalizeAbsTimes()
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
            const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[i];
            if (pix.IsExcluded())
                continue;

            // if TimePrecision is valid fill it into array
            if (pix.GetAbsTimeRms()>0)
                arr[n++] = pix.GetAbsTimeRms();
        }

        // Check the ratio of valid entries to the ratio of pixels
        const Float_t ratio = 100*n/fGeom->GetNumPixWithAidx(aidx);
        if (3*ratio<2)
            *fLog << warn << "Area   " << setw(4) << aidx << ": Only " << ratio << "% pixels with valid time resolution found." << endl;

        // Calculate median and median deviation
        Double_t med;
        const Double_t dev = MMath::MedianDev(n, arr.GetArray(), med);

        // Calculate upper and lower limit
        const Float_t lolim = TMath::Max(med-fArrTimeRmsLimit*dev, 0.);
        const Float_t hilim = TMath::Max(med+fArrTimeRmsLimit*dev, 0.);

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
            const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[i];

            // Check if time precision is valid (might be invalid
            // for example in cae of empty histograms)
            const Float_t res = pix.GetAbsTimeRms();
            if (res<0) //FIXME!!! How does this happen?
            {
                *fLog << warn << "Pixel  " << setw(4) << i << ": Abs-time rms could not be calculated." << endl;
                (*fBadPixels)[i].SetUncalibrated(MBadPixelsPix::kDeviatingAbsTimeResolution);
                continue;
            }

            if (res==0)
            {
                *fLog << warn << "Pixel  " << setw(4) << i << ": Abs-time rms==0." << endl;
                (*fBadPixels)[i].SetUncalibrated(MBadPixelsPix::kDeviatingAbsTimeResolution);
                continue;
            }

            // Now compare to a lower and upper limit
            if (res<=lolim || res>=hilim)
            {
                *fLog << warn << "Pixel  " << setw(4) << i << ": Deviation from abs-time rms: "
                    << Form("%5.2f", res) << " out of range "
                    << Form("[%4.2f,%4.2f]", lolim, hilim) << endl;

                (*fBadPixels)[i].SetUncalibrated(MBadPixelsPix::kDeviatingAbsTimeResolution);
            }
        }
    }
}

// -----------------------------------------------------------------------
//
// Return kTRUE if fPulserColor is kNONE
//
// First loop over pixels, average areas and sectors, call:
//  - FinalizePedestals() 
//  - FinalizeCharges()
// for every entry. Count number of valid pixels in loop and return kFALSE
// if there are none (the "Michele check").
//
// Call FinalizeBadPixels()
//
// Call FinalizeFFactorMethod() (second and third loop over pixels and areas)
//
// Call FinalizeBlindCam() 
// Call FinalizePINDiode()
//
// Call  FinalizeFFactorQECam() (fourth loop over pixels and areas)
// Call  FinalizeBlindPixelQECam() (fifth loop over pixels and areas)
// Call  FinalizePINDiodeQECam() (sixth loop over pixels and areas)
//
// Call  FinalizeUnsuitablePixels()
//  
// Call MParContainer::SetReadyToSave() for fCam, fQECam, fBadPixels and
//      fBlindCam and fPINDiode if they exist
//
// Print out some statistics
//
Int_t MCalibrationChargeCalc::Finalize()
{
    // The number of used slices are just a mean value
    // the real number might change from event to event.
    // (up to 50%!)

  fNumHiGainSamples  =  fSignal->GetNumUsedHiGainFADCSlices();
  fNumLoGainSamples  =  fSignal->GetNumUsedLoGainFADCSlices();

  fSqrtHiGainSamples =  TMath::Sqrt(fNumHiGainSamples);
  fSqrtLoGainSamples =  TMath::Sqrt(fNumLoGainSamples);
  
  if (fPINDiode)
    if (!fPINDiode->IsValid())
      {
        *fLog << warn << GetDescriptor() 
              << ": MCalibrationChargePINDiode is declared not valid... no PIN Diode method! " << endl;
        fPINDiode = NULL;
      }

  //
  // First loop over pixels, call FinalizePedestals and FinalizeCharges
  //
  Int_t nvalid = 0;
  for (Int_t pixid=0; pixid<fPedestals->GetSize(); pixid++)
  {
      //
      // Check if the pixel has been excluded from the fits
      //
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[pixid];
      if (pix.IsExcluded())
          continue;

      FinalizePedestals((*fPedestals)[pixid], pix, (*fGeom)[pixid].GetAidx());

      if (FinalizeCharges(pix, (*fBadPixels)[pixid], "Pixel  "))
          nvalid++;
  }

  FinalizeAbsTimes();

  *fLog << endl;  

  //
  // The Michele check ...
  //
  if (nvalid == 0)
  {
      if (!fContinousCalibration)
      {
        *fLog << warn << GetDescriptor() << ": All pixels have non-valid calibration. " 
              << "Did you forget to fill the histograms "
              << "(filling MHCalibrationChargeCam from MExtractedSignalCam using MFillH) ? " << endl;
        *fLog << warn << GetDescriptor() << ": Or, maybe, you have used a pedestal run " 
              << "instead of a calibration run " << endl;
        return kFALSE;
      }
  }

  for (UInt_t aidx=0; aidx<fGeom->GetNumAreas(); aidx++)
    {

      const MPedestalPix    &ped = fPedestals->GetAverageArea(aidx);
      MCalibrationChargePix &pix = (MCalibrationChargePix&)fCam->GetAverageArea(aidx);
      const MArrayI &arr         = fHCam->GetAverageAreaNum();

      FinalizePedestals(ped,pix,aidx);

      //
      // Correct for sqrt(number of valid pixels) in the pedestal RMS
      // (already done for calibration sigma in MHCalibrationCam::CalcAverageSigma()
      //
      const Double_t sqrtnum = TMath::Sqrt((Double_t)arr[aidx]);

      pix.SetPedRMS(pix.GetPedRms()*sqrtnum, pix.GetPedRmsErr()*sqrtnum);
      pix.SetSigma (pix.GetSigma()/pix.GetFFactorFADC2Phe());

      FinalizeCharges(pix, fCam->GetAverageBadArea(aidx),"area id");
    }
  
  *fLog << endl;

  for (UInt_t sector=0; sector<fGeom->GetNumSectors(); sector++)
    {

      const MPedestalPix     &ped = fPedestals->GetAverageSector(sector);

      MCalibrationChargePix &pix = (MCalibrationChargePix&)fCam->GetAverageSector(sector);
      FinalizePedestals(ped,pix, 0);
    }
  
  *fLog << endl;

  //
  // Finalize Bad Pixels
  // 
  FinalizeBadPixels();

  // 
  // Finalize F-Factor method
  //
  if (FinalizeFFactorMethod())
    fCam->SetFFactorMethodValid(kTRUE);
  else
    {
      *fLog << warn << "Could not calculate the photons flux from the F-Factor method " << endl;
      fCam->SetFFactorMethodValid(kFALSE);
      if (!fContinousCalibration)
          return kFALSE;
    }

  *fLog << endl;

  //
  // Finalize Blind Pixel
  //
  fQECam->SetBlindPixelMethodValid(FinalizeBlindCam());

  // 
  // Finalize PIN Diode
  //
  fQECam->SetBlindPixelMethodValid(FinalizePINDiode());

  //
  // Finalize QE Cam
  //
  FinalizeFFactorQECam();
  FinalizeBlindPixelQECam();
  FinalizePINDiodeQECam();
  FinalizeCombinedQECam();

  //
  // Re-direct the output to an ascii-file from now on:
  //
  *fLog << inf << endl;
  *fLog << GetDescriptor() << ": Fatal errors statistics:" << endl;  

  PrintUncalibrated(MBadPixelsPix::kChargeIsPedestal,    
                    Form("%s%2.1f%s","Signal less than ",fChargeLimit," Pedestal RMS:                "));
  PrintUncalibrated(MBadPixelsPix::kChargeRelErrNotValid,
                    Form("%s%2.1f%s","Signal Error bigger than ",fChargeRelErrLimit," times Mean Signal:   "));
  PrintUncalibrated(MBadPixelsPix::kLoGainSaturation,    
                    "Low Gain Saturation:                              ");
//  PrintUncalibrated(MBadPixelsPix::kMeanTimeInFirstBin,
//                    Form("%s%2.1f%s","Mean Abs. Arr. Time in First ",1.," Bin(s):          "));
//  PrintUncalibrated(MBadPixelsPix::kMeanTimeInLast2Bins,
//                    Form("%s%2.1f%s","Mean Abs. Arr. Time in Last ",2.," Bin(s):           "));
  PrintUncalibrated(MBadPixelsPix::kHiGainOverFlow,    
                    "Pixels with High Gain Overflow:                   ");
  PrintUncalibrated(MBadPixelsPix::kLoGainOverFlow,    
                    "Pixels with Low Gain Overflow :                   ");
//  PrintUncalibrated(MBadPixelsPix::kFluctuatingArrivalTimes,
//                    "Fluctuating Pulse Arrival Times:                  ");
  PrintUncalibrated(MBadPixelsPix::kDeviatingAbsTimeResolution,
                    Form("%s%2.1f%s ","Abs.time rms more than ", fArrTimeRmsLimit, " dev from median:      "));
  PrintUncalibrated(MBadPixelsPix::kDeadPedestalRms,
                    "Presumably dead from Pedestal Rms:                ");
  PrintUncalibrated(MBadPixelsPix::kDeviatingNumPhes,
                    "Deviating number of phes:                         ");
  PrintUncalibrated(MBadPixelsPix::kLoGainBlackout,
                    "Too many blackout events in low gain:             ");
  PrintUncalibrated(MBadPixelsPix::kPreviouslyExcluded,
                    "Previously excluded:                              ");

  *fLog << inf << endl;
  *fLog << GetDescriptor() << ": Unreliable errors statistics:" << endl;  

  PrintUncalibrated(MBadPixelsPix::kChargeSigmaNotValid, 
                    "Signal Sigma smaller than Pedestal RMS: ");
  PrintUncalibrated(MBadPixelsPix::kHiGainOscillating,   
                    "Changing Hi Gain signal over time:      ");
  PrintUncalibrated(MBadPixelsPix::kLoGainOscillating,   
                    "Changing Lo Gain signal over time:      ");
  PrintUncalibrated(MBadPixelsPix::kHiGainNotFitted,     
                    "Unsuccesful Gauss fit to the Hi Gain:   ");
  PrintUncalibrated(MBadPixelsPix::kLoGainNotFitted,     
                    "Unsuccesful Gauss fit to the Lo Gain:   ");
  PrintUncalibrated(MBadPixelsPix::kDeviatingFFactor,    
                    "Deviating F-Factor:                     ");

  fCam->SetReadyToSave();
  fQECam->SetReadyToSave();
  fBadPixels->SetReadyToSave();

  if (fBlindCam)
    fBlindCam->SetReadyToSave();
  if (fPINDiode)
    fPINDiode->SetReadyToSave();

  //
  // Finalize calibration statistics
  //
  return FinalizeUnsuitablePixels();
}

// ----------------------------------------------------------------------------------
//  
// Retrieves pedestal and pedestal RMS from MPedestalPix 
// Retrieves total entries from MPedestalCam
// Sets pedestal*fNumHiGainSamples and pedestal*fNumLoGainSamples in MCalibrationChargePix
// Sets pedRMS *fSqrtHiGainSamples and pedRMS *fSqrtLoGainSamples in MCalibrationChargePix
//
// If the flag MCalibrationPix::IsHiGainSaturation() is set, call also: 
// - MCalibrationChargePix::CalcLoGainPedestal()
//
void MCalibrationChargeCalc::FinalizePedestals(const MPedestalPix &ped, MCalibrationChargePix &cal, const Int_t aidx)
{
  
  //
  // get the pedestals
  //
  const Float_t pedes  = ped.GetPedestal();
  const Float_t prms   = ped.GetPedestalRms();
  const Int_t   num    = fPedestals->GetNumSlices()*ped.GetNumEvents();
  
  //
  // RMS error set by PedCalcFromLoGain, 0 in case MPedCalcPedRun was used.
  //
  const Float_t prmserr = num>0 ? prms/TMath::Sqrt(2.*num) : ped.GetPedestalRmsError();

  //
  // set them in the calibration camera
  //
  if (cal.IsHiGainSaturation())
    {
      cal.SetPedestal(pedes   * fNumLoGainSamples,
                      prms    * fSqrtLoGainSamples,
                      prmserr * fSqrtLoGainSamples);
      cal.CalcLoGainPedestal(fNumLoGainSamples);
    }
  else
    {

      cal.SetPedestal(pedes   * fNumHiGainSamples,
                      prms    * fSqrtHiGainSamples,
                      prmserr * fSqrtHiGainSamples);
    }
  
}

// ----------------------------------------------------------------------------------------------------
//
// Check fit results validity. Bad Pixels flags are set if:
//
// 1) Pixel has a mean smaller than fChargeLimit*PedRMS    ( Flag: MBadPixelsPix::kChargeIsPedestal)
// 2) Pixel has a mean error smaller than fChargeErrLimit  ( Flag: MBadPixelsPix::kChargeErrNotValid)
// 3) Pixel has mean smaller than fChargeRelVarLimit times its mean error 
//                                                         ( Flag: MBadPixelsPix::kChargeRelErrNotValid)
// 4) Pixel has a sigma bigger than its Pedestal RMS       ( Flag: MBadPixelsPix::kChargeSigmaNotValid )
// 
// Further returns if flags: MBadPixelsPix::kUnsuitableRun is set
// 
// Calls MCalibrationChargePix::CalcReducedSigma() and sets flag:  MBadPixelsPix::kChargeIsPedestal 
//       and returns kFALSE if not succesful. 
// 
// Calls MCalibrationChargePix::CalcFFactor() and sets flag: MBadPixelsPix::kDeviatingNumPhes) 
//       and returns kFALSE if not succesful.
//
// Calls MCalibrationChargePix::CalcConvFFactor()and sets flag: MBadPixelsPix::kDeviatingNumPhes) 
//       and returns kFALSE if not succesful.
//
Bool_t MCalibrationChargeCalc::FinalizeCharges(MCalibrationChargePix &cal, MBadPixelsPix &bad, const char* what)
{

  if (bad.IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
    return kFALSE;

  const TString desc = Form("%7s%4d: ", what, cal.GetPixId());

  if (cal.GetMean()<0)
  {
      *fLog << warn << desc << "Charge not fitted." << endl;
      bad.SetUncalibrated( MBadPixelsPix::kChargeIsPedestal);
      return kFALSE;
  }

  if (cal.GetSigma()<0)
  {
      *fLog << warn << desc << "Charge Sigma invalid." << endl;
      bad.SetUncalibrated( MBadPixelsPix::kChargeIsPedestal);
      return kFALSE;
  }

  if (cal.GetMean() < fChargeLimit*cal.GetPedRms())
    {
      *fLog << warn << desc
            << Form("Fitted Charge: %5.2f < %2.1f",cal.GetMean(),fChargeLimit)
            << Form(" * Pedestal RMS %5.2f",cal.GetPedRms()) << endl;
      bad.SetUncalibrated( MBadPixelsPix::kChargeIsPedestal);
    }
  
   if (cal.GetMean() < fChargeRelErrLimit*cal.GetMeanErr()) 
    {
      *fLog << warn << desc
            << Form("Fitted Charge: %4.2f < %2.1f",cal.GetMean(),fChargeRelErrLimit)
            << Form(" * its error %4.2f",cal.GetMeanErr()) << endl;
      bad.SetUncalibrated( MBadPixelsPix::kChargeRelErrNotValid );
    }

  if (cal.GetSigma() < cal.GetPedRms())
    {
      *fLog << warn << desc << "Sigma of Fitted Charge: "
            << Form("%6.2f <",cal.GetSigma()) << " Ped. RMS="
            << Form("%5.2f", cal.GetPedRms()) << endl;
      bad.SetUncalibrated( MBadPixelsPix::kChargeSigmaNotValid );
      return kFALSE;
    }

  if (!cal.CalcReducedSigma())
    {
      *fLog << warn << desc << "Could not calculate the reduced sigma" << endl;
      bad.SetUncalibrated( MBadPixelsPix::kChargeSigmaNotValid );
      return kFALSE;
    }

  if (!cal.CalcFFactor())
    {
      *fLog << warn << desc << "Could not calculate the F-Factor"<< endl;
      bad.SetUncalibrated(MBadPixelsPix::kDeviatingNumPhes);
      bad.SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
      return kFALSE;
    }

  if (cal.GetPheFFactorMethod() < 0.)
    {
      bad.SetUncalibrated(MBadPixelsPix::kDeviatingNumPhes);
      bad.SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
      cal.SetFFactorMethodValid(kFALSE);
      return kFALSE;
    }

  if (!cal.CalcConvFFactor())
    {
      *fLog << warn << desc << "Could not calculate the Conv. FADC counts to Phes"<< endl;
      bad.SetUncalibrated(MBadPixelsPix::kDeviatingNumPhes);
      return kFALSE;
    }

  if (!IsUseExtractorRes())
    return kTRUE;

  if (!fExtractor)
    {
      *fLog << err << "Extractor resolution has been chosen, but no extractor is set. Cannot calibrate." << endl;
      return kFALSE;
    }

  const Float_t resinphes = cal.IsHiGainSaturation()
    ? cal.GetPheFFactorMethod()*fExtractor->GetResolutionPerPheLoGain()
    : cal.GetPheFFactorMethod()*fExtractor->GetResolutionPerPheHiGain();

  Float_t resinfadc = cal.IsHiGainSaturation()
    ? resinphes/cal.GetMeanConvFADC2Phe()/cal.GetConversionHiLo()
    : resinphes/cal.GetMeanConvFADC2Phe();

  if (resinfadc > 3.0*cal.GetPedRms() )
    {
      *fLog << warn << desc << "Extractor Resolution " << Form("%5.2f", resinfadc) << " bigger than 3 Pedestal RMS "
            << Form("%4.2f", cal.GetPedRms()) << endl;
      resinfadc = 3.0*cal.GetPedRms();
    }

  if (!cal.CalcReducedSigma(resinfadc))
    {
        *fLog << warn << desc << "Could not calculate the reduced sigma" << endl;
        bad.SetUncalibrated( MBadPixelsPix::kChargeSigmaNotValid );
        return kFALSE;
    }

  if (!cal.CalcFFactor())
    {
        *fLog << warn << desc << "Could not calculate the F-Factor" << endl;
      bad.SetUncalibrated(MBadPixelsPix::kDeviatingNumPhes);
      bad.SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
      return kFALSE;
    }

  if (cal.GetPheFFactorMethod() < 0.)
    {
      bad.SetUncalibrated(MBadPixelsPix::kDeviatingNumPhes);
      bad.SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
      cal.SetFFactorMethodValid(kFALSE);
      return kFALSE;
    }

  if (!cal.CalcConvFFactor())
    {
      *fLog << warn << desc << "Could not calculate the conv. FADC cts to phes" << endl;
      bad.SetUncalibrated(MBadPixelsPix::kDeviatingNumPhes);
      return kFALSE;
    }

  return kTRUE;
}

// -----------------------------------------------------------------------------------
//
// Sets pixel to MBadPixelsPix::kUnsuitableRun, if one of the following flags is set:
// - MBadPixelsPix::kChargeIsPedestal
// - MBadPixelsPix::kChargeRelErrNotValid 
// - MBadPixelsPix::kMeanTimeInFirstBin 
// - MBadPixelsPix::kMeanTimeInLast2Bins 
// - MBadPixelsPix::kDeviatingNumPhes
// - MBadPixelsPix::kHiGainOverFlow
// - MBadPixelsPix::kLoGainOverFlow
//
// Sets pixel to MBadPixelsPix::kUnreliableRun, if one of the following flags is set:
// - MBadPixelsPix::kChargeSigmaNotValid 
//
void MCalibrationChargeCalc::FinalizeBadPixels()
{
  
  for (Int_t i=0; i<fBadPixels->GetSize(); i++)
    {
      
      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (IsCheckDeadPixels())
        {
          if (bad.IsUncalibrated( MBadPixelsPix::kChargeIsPedestal))
            bad.SetUnsuitable(   MBadPixelsPix::kUnsuitableRun   );
        }
/*
      if (IsCheckExtractionWindow())
        {
          if (bad.IsUncalibrated( MBadPixelsPix::kMeanTimeInFirstBin ))
            bad.SetUnsuitable(   MBadPixelsPix::kUnsuitableRun    );
          
          if (bad.IsUncalibrated( MBadPixelsPix::kMeanTimeInLast2Bins ))
            bad.SetUnsuitable(   MBadPixelsPix::kUnsuitableRun    );
        }
 */
      if (IsCheckDeviatingBehavior())
        {
          if (bad.IsUncalibrated( MBadPixelsPix::kDeviatingNumPhes ))
            bad.SetUnsuitable(   MBadPixelsPix::kUnsuitableRun    );
        }

      if (IsCheckHistOverflow())
        {
          if (bad.IsUncalibrated( MBadPixelsPix::kHiGainOverFlow   ))
            bad.SetUnsuitable(   MBadPixelsPix::kUnsuitableRun    );

          if (bad.IsUncalibrated( MBadPixelsPix::kLoGainOverFlow   ))
            bad.SetUnsuitable(   MBadPixelsPix::kUnsuitableRun    );
        }

      if (IsCheckArrivalTimes())
        {
          if (bad.IsUncalibrated( MBadPixelsPix::kDeviatingAbsTimeResolution ))
            bad.SetUnsuitable(   MBadPixelsPix::kUnsuitableRun    );
          //if (bad.IsUncalibrated( MBadPixelsPix::kFluctuatingArrivalTimes ))
          //  bad.SetUnsuitable(   MBadPixelsPix::kUnsuitableRun    );
        }

      if (bad.IsUncalibrated( MBadPixelsPix::kChargeSigmaNotValid ))
        bad.SetUnsuitable(   MBadPixelsPix::kUnreliableRun    );
    }
}

// ------------------------------------------------------------------------
//
//
// First loop: Calculate a mean and mean RMS of photo-electrons per area index 
//             Include only pixels which are not MBadPixelsPix::kUnsuitableRun nor 
//             MBadPixelsPix::kChargeSigmaNotValid (see FinalizeBadPixels()) and set 
//             MCalibrationChargePix::SetFFactorMethodValid(kFALSE) in that case.
//              
// Second loop: Get mean number of photo-electrons and its RMS including 
//              only pixels with flag MCalibrationChargePix::IsFFactorMethodValid() 
//              and further exclude those deviating by more than fPheErrLimit mean 
//              sigmas from the mean (obtained in first loop). Set 
//              MBadPixelsPix::kDeviatingNumPhes if excluded.
//
//              For the suitable pixels with flag MBadPixelsPix::kChargeSigmaNotValid 
//              set the number of photo-electrons as the mean number of photo-electrons
//              calculated in that area index.
// 
//              Set weighted mean and variance of photo-electrons per area index in: 
//              average area pixels of MCalibrationChargeCam (obtained from: 
//              MCalibrationChargeCam::GetAverageArea() )
// 
//              Set weighted mean  and variance of photo-electrons per sector in:
//              average sector pixels of MCalibrationChargeCam (obtained from: 
//              MCalibrationChargeCam::GetAverageSector() )
// 
//
// Third loop: Set mean number of photo-electrons and its RMS in the pixels
//             only excluded as: MBadPixelsPix::kChargeSigmaNotValid 
// 
Bool_t MCalibrationChargeCalc::FinalizeFFactorMethod()
{
  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nareas   = fGeom->GetNumAreas();
  const Int_t nsectors = fGeom->GetNumSectors();

  TArrayF lowlim        (nareas);
  TArrayF upplim        (nareas);
  TArrayD areavars      (nareas);
  TArrayD areaweights   (nareas);
  TArrayD sectorweights (nsectors);
  TArrayD areaphes      (nareas);
  TArrayD sectorphes    (nsectors);
  TArrayI numareavalid  (nareas);
  TArrayI numsectorvalid(nsectors);

  //
  // First loop: Get mean number of photo-electrons and the RMS
  //             The loop is only to recognize later pixels with very deviating numbers
  //
  MHCamera camphes(*fGeom,"Camphes","Phes in Camera");

  for (Int_t i=0; i<npixels; i++)
    {
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[i];

      MBadPixelsPix &bad = (*fBadPixels)[i];
      
      if (!pix.IsFFactorMethodValid())
        continue;

      if (bad.IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        {
          pix.SetFFactorMethodValid(kFALSE);
          continue;
        }

      if (bad.IsUncalibrated(MBadPixelsPix::kChargeSigmaNotValid))
        continue;

      if (!IsUseUnreliables())
        if (bad.IsUnsuitable(MBadPixelsPix::kUnreliableRun))
          continue;

      const Float_t nphe  = pix.GetPheFFactorMethod();
      const Int_t   aidx  = (*fGeom)[i].GetAidx();
      camphes.Fill(i,nphe);
      camphes.SetUsed(i);
      areaphes    [aidx] += nphe;
      areavars    [aidx] += nphe*nphe;
      numareavalid[aidx] ++;
    } 

  for (Int_t i=0; i<nareas; i++)
    {
      if (numareavalid[i] == 0)
        {
          *fLog << warn << GetDescriptor() << ": No pixels with valid number of photo-electrons found "
                << "in area index: " << i << endl;
          continue;
        }

      if (numareavalid[i] == 1)
        areavars[i] = 0.;
      else
        {
          areavars[i] = (areavars[i] - areaphes[i]*areaphes[i]/numareavalid[i]) / (numareavalid[i]-1);
          areaphes[i] /= numareavalid[i];
        }
      
      if (areavars[i] < 0.)
        {
          *fLog << warn << "Area   " << setw(4) << i << ": No pixels with valid variance of photo-electrons found." << endl;
          continue;
        }

      // FIXME: WHAT IS THIS FOR? It is overwritten!
      lowlim[i] = areaphes[i] - fPheErrLowerLimit*TMath::Sqrt(areavars[i]);
      upplim[i] = areaphes[i] + fPheErrUpperLimit*TMath::Sqrt(areavars[i]);
 

      TH1D *hist = camphes.ProjectionS(TArrayI(),TArrayI(1,&i),"_py",100);
      hist->Fit("gaus","Q0");
      const Float_t mean  = hist->GetFunction("gaus")->GetParameter(1);
      const Float_t sigma = hist->GetFunction("gaus")->GetParameter(2);
      const Int_t   ndf   = hist->GetFunction("gaus")->GetNDF();

      if (IsDebug())
        hist->DrawClone();

      if (ndf < 5)
        {
            *fLog << warn << GetDescriptor() << ": Cannot use a Gauss fit to the number of photo-electrons " << endl;
            *fLog << " in the camera with area index: " << i << endl;
            *fLog << " Number of dof.: " << ndf << " is smaller than 5 " << endl;
            *fLog << " Will use the simple mean and rms " << endl;
            delete hist;
            SetPheFitOK(i,kFALSE);
            continue;
        }
      
      const Double_t prob = hist->GetFunction("gaus")->GetProb();

      if (prob < 0.001)
        {
            *fLog << warn << GetDescriptor() << ": Cannot use a Gauss fit to the number of photo-electrons " << endl;
            *fLog << " in the camera with area index: " << i << endl;
            *fLog << " Fit probability " << prob << " is smaller than 0.001 " << endl;
            *fLog << " Will use the simple mean and rms " << endl;
            delete hist;
            SetPheFitOK(i,kFALSE);
            continue;
        }
      
      if (mean < 0.)
        {
            *fLog << inf  << "Area   " << setw(4) << i << ": Fitted mean number of phe smaller 0." << endl;
            *fLog << warn << "Area   " << setw(4) << i << ": Will use the simple mean and rms " << endl;
            SetPheFitOK(i,kFALSE);
            delete hist;
            continue;
        }
      
      *fLog << inf << "Area   " << setw(4) << i << ": Mean number of phes: "
            << Form("%7.2f+-%6.2f",mean,sigma) << endl;

      lowlim[i] = mean - fPheErrLowerLimit*sigma;
      upplim[i] = mean + fPheErrUpperLimit*sigma;

      if (lowlim[i]<1)
          lowlim[i] = 1;

      delete hist;

      SetPheFitOK(i,kTRUE);
    }

  *fLog << endl;

  numareavalid.Reset();
  areaphes    .Reset();
  areavars    .Reset();
  //
  // Second loop: Get mean number of photo-electrons and its RMS excluding 
  //              pixels deviating by more than fPheErrLimit sigma. 
  //              Set the conversion factor FADC counts to photo-electrons
  // 
  for (Int_t i=0; i<npixels; i++)
    {
      
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[i];

      if (!pix.IsFFactorMethodValid())
        continue;

      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (bad.IsUncalibrated(MBadPixelsPix::kChargeSigmaNotValid))
        continue;

      if (!IsUseUnreliables())
        if (bad.IsUnsuitable(MBadPixelsPix::kUnreliableRun))
          continue;

      const Float_t nvar  = pix.GetPheFFactorMethodVar();
      if (nvar <= 0.)
        {
          pix.SetFFactorMethodValid(kFALSE);
          continue;
        }
      
      const Int_t   aidx   = (*fGeom)[i].GetAidx();
      const Int_t   sector = (*fGeom)[i].GetSector();
      const Float_t area   = (*fGeom)[i].GetA();
      const Float_t nphe   = pix.GetPheFFactorMethod();

      if ( nphe < lowlim[aidx] || nphe > upplim[aidx] )
        {
            *fLog << warn << "Pixel  " << setw(4) << i << ": Num of phe "
                << Form("%7.2f out of +%3.1f-%3.1f sigma limit ",nphe,fPheErrUpperLimit,fPheErrLowerLimit)
                << Form("[%7.2f,%7.2f]",lowlim[aidx],upplim[aidx]) << endl;
          bad.SetUncalibrated( MBadPixelsPix::kDeviatingNumPhes );

          if (IsCheckDeviatingBehavior())
                bad.SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
          continue;
        }
      
      areaweights   [aidx]   += nphe*nphe;
      areaphes      [aidx]   += nphe;
      numareavalid  [aidx]   ++;

      if (aidx == 0)
        fNumInnerFFactorMethodUsed++;

      sectorweights [sector] += nphe*nphe/area/area;
      sectorphes    [sector] += nphe/area;
      numsectorvalid[sector] ++;
    }

  *fLog << endl;

  for (Int_t aidx=0; aidx<nareas; aidx++)
    {

      MCalibrationChargePix &apix = (MCalibrationChargePix&)fCam->GetAverageArea(aidx);

      if (numareavalid[aidx] == 1)
        areaweights[aidx] = 0.;
      else if (numareavalid[aidx] == 0)
        {
          areaphes[aidx]    = -1.;
          areaweights[aidx] = -1.;
        }
      else
        {
          areaweights[aidx] = (areaweights[aidx] - areaphes[aidx]*areaphes[aidx]/numareavalid[aidx])
                            / (numareavalid[aidx]-1);
          areaphes[aidx]    /=  numareavalid[aidx];
        }

      if (areaweights[aidx] < 0. || areaphes[aidx] <= 0.)
        {
          *fLog << warn << "Area   " << setw(4) << aidx << ": Mean number phes could not be calculated: "
                << " Mean: "  << areaphes[aidx] 
                << " Var: " << areaweights[aidx]    << endl;
          apix.SetFFactorMethodValid(kFALSE);
          continue;
        }

      *fLog << inf << "Area   " << setw(4) << aidx << ": Average total phes: "
            << Form("%7.2f +- %6.2f",areaphes[aidx],TMath::Sqrt(areaweights[aidx])) << endl;

      apix.SetPheFFactorMethod   ( areaphes[aidx] );
      apix.SetPheFFactorMethodVar( areaweights[aidx] / numareavalid[aidx] );      
      apix.SetFFactorMethodValid ( kTRUE );

    }

  *fLog << endl;

  for (Int_t sector=0; sector<nsectors; sector++)
    {

      if (numsectorvalid[sector] == 1)
        sectorweights[sector] = 0.;
      else if (numsectorvalid[sector] == 0)
        {
          sectorphes[sector]    = -1.;
          sectorweights[sector] = -1.;
        }
      else
        {
          sectorweights[sector] = (sectorweights[sector] 
                                   - sectorphes[sector]*sectorphes[sector]/numsectorvalid[sector]
                                  ) 
                                / (numsectorvalid[sector]-1.);
          sectorphes[sector]     /=  numsectorvalid[sector];
        }
      
      MCalibrationChargePix &spix = (MCalibrationChargePix&)fCam->GetAverageSector(sector);

      if (sectorweights[sector] < 0. || sectorphes[sector] <= 0.)
        {
          *fLog << warn << "Sector " << setw(4) << sector
                <<": Mean number phes/area could not be calculated:"
                << " Mean: "  << sectorphes[sector] << " Var: " << sectorweights[sector]    << endl;
          spix.SetFFactorMethodValid(kFALSE);
          continue;
        }

      *fLog << inf << "Sector " << setw(4) << sector 
            << ": Avg number phes/mm^2: "
            << Form("%5.3f+-%4.3f",sectorphes[sector],TMath::Sqrt(sectorweights[sector]))
            << endl;

      spix.SetPheFFactorMethod   ( sectorphes[sector] );
      spix.SetPheFFactorMethodVar( sectorweights[sector] / numsectorvalid[sector]);      
      spix.SetFFactorMethodValid ( kTRUE );

    }

  //
  // Third loop: Set mean number of photo-electrons and its RMS in the pixels
  //             only excluded as: MBadPixelsPix::kChargeSigmaNotValid 
  // 
  for (Int_t i=0; i<npixels; i++)
    {
      
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[i];

      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (bad.IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue;
      
      if (bad.IsUncalibrated(MBadPixelsPix::kChargeSigmaNotValid))
        {
          const Int_t            aidx = (*fGeom)[i].GetAidx();
          MCalibrationChargePix &apix = (MCalibrationChargePix&)fCam->GetAverageArea(aidx);

          pix.SetPheFFactorMethod   ( apix.GetPheFFactorMethod()    );
          pix.SetPheFFactorMethodVar( apix.GetPheFFactorMethodVar() );

          if (!pix.CalcConvFFactor())
            {
              *fLog << warn << GetDescriptor() 
                    << "Pixel  " << setw(4) << pix.GetPixId()
                    <<": Could not calculate the Conv. FADC counts to Phes"
                    << endl;
              bad.SetUncalibrated( MBadPixelsPix::kDeviatingNumPhes );
              if (IsCheckDeviatingBehavior())
                bad.SetUnsuitable  ( MBadPixelsPix::kUnsuitableRun    );
            }

        }
    }
  
  return kTRUE;
}



// ------------------------------------------------------------------------
//
// Returns kFALSE if pointer to MCalibrationBlindCam is NULL
//
// The check returns kFALSE if:
//
// 1) fLambda and fLambdaCheck are separated relatively to each other by more than fLambdaCheckLimit
// 2) BlindPixel has an fLambdaErr greater than fLambdaErrLimit
// 
// Calls:
// - MCalibrationBlindPix::CalcFluxInsidePlexiglass()
//
Bool_t MCalibrationChargeCalc::FinalizeBlindCam()
{
  if (!fBlindCam)
    return kFALSE;  

  Int_t nvalid  = 0;

  for (Int_t i=0; i<fBlindCam->GetSize(); i++)
    {
      
      MCalibrationBlindPix &blindpix = (MCalibrationBlindPix&)(*fBlindCam)[i];

      if (!blindpix.IsValid())
        continue;
      
      const Float_t lambda      = blindpix.GetLambda();
      const Float_t lambdaerr   = blindpix.GetLambdaErr();
      const Float_t lambdacheck = blindpix.GetLambdaCheck();

      if (2.*(lambdacheck-lambda)/(lambdacheck+lambda) > fLambdaCheckLimit)
        {
          *fLog << warn << "BlindPix " << i << ": Lambda="
                << Form("%4.2f", lambda) << " and Lambda-Check="
                << Form("%4.2f", lambdacheck) << " differ by more than "
                << Form("%4.2f", fLambdaCheckLimit) << endl;
          blindpix.SetValid(kFALSE);
          continue;
        }
      
      if (lambdaerr > fLambdaErrLimit) 
        {
            *fLog << warn << "BlindPix " << i << ": Error of Fitted Lambda="
                << Form("%4.2f", lambdaerr) << " is greater than "
                << Form("%4.2f", fLambdaErrLimit) << endl;
          blindpix.SetValid(kFALSE);
          continue;
        }
      
      if (!blindpix.CalcFluxInsidePlexiglass())
        {
          *fLog << warn << "Could not calculate the flux of photons from Blind Pixel Nr." << i << endl;
          blindpix.SetValid(kFALSE);
          continue;
        }
      
      nvalid++;
    }

  if (!nvalid)
    return kFALSE;
  
  return kTRUE;
}

// ------------------------------------------------------------------------
//
// Returns kFALSE if pointer to MCalibrationChargePINDiode is NULL
//
// The check returns kFALSE if:
//
// 1) PINDiode has a fitted charge smaller than fChargeLimit*PedRMS
// 2) PINDiode has a fit error smaller than fChargeErrLimit
// 3) PINDiode has a fitted charge smaller its fChargeRelErrLimit times its charge error
// 4) PINDiode has a charge sigma smaller than its Pedestal RMS
// 
// Calls:
// - MCalibrationChargePINDiode::CalcFluxOutsidePlexiglass()
//
Bool_t MCalibrationChargeCalc::FinalizePINDiode()
{

  if (!fPINDiode)
    return kFALSE;  

  if (fPINDiode->GetMean() < fChargeLimit*fPINDiode->GetPedRms())
    {
      *fLog << warn << "PINDiode   : Fitted Charge is smaller than "
            << fChargeLimit << " Pedestal RMS." << endl;
      return kFALSE;
    }
  
  if (fPINDiode->GetMeanErr() < fChargeErrLimit) 
    {
        *fLog << warn << "PINDiode   : Error of Fitted Charge is smaller than "
            << fChargeErrLimit << endl;
      return kFALSE;
    }
      
  if (fPINDiode->GetMean() < fChargeRelErrLimit*fPINDiode->GetMeanErr()) 
    {
        *fLog << warn << "PINDiode   : Fitted Charge is smaller than "
            << fChargeRelErrLimit << "* its error" << endl;
      return kFALSE;
    }
      
  if (fPINDiode->GetSigma() < fPINDiode->GetPedRms())
    {
        *fLog << warn << "PINDiode   : Sigma of Fitted Charge smaller than Pedestal RMS" << endl;
      return kFALSE;
    }


  if (!fPINDiode->CalcFluxOutsidePlexiglass())
    {
        *fLog << warn << "PINDiode   : Could not calculate the flux of photons, "
            << "will skip PIN Diode Calibration " << endl;
      return kFALSE;
    }
  
  return kTRUE;
}

// ------------------------------------------------------------------------
//
// Calculate the average number of photons outside the plexiglass with the 
// formula: 
// 
// av.Num.photons(area index) = av.Num.Phes(area index) 
//                            / MCalibrationQEPix::GetDefaultQE(fPulserColor) 
//                            / MCalibrationQEPix::GetPMTCollectionEff()
//                            / MCalibrationQEPix::GetLightGuidesEff(fPulserColor)
//                            / MCalibrationQECam::GetPlexiglassQE()
//
// Calculate the variance on the average number of photons assuming that the error on the 
// Quantum efficiency is reduced by the number of used inner pixels, but the rest of the 
// values keeps it ordinary error since it is systematic.
//
// Loop over pixels: 
//
// - Continue, if not MCalibrationChargePix::IsFFactorMethodValid() and set:
//                    MCalibrationQEPix::SetFFactorMethodValid(kFALSE,fPulserColor)
//
// - Call MCalibrationChargePix::CalcMeanFFactor(av.Num.photons) and set: 
//        MCalibrationQEPix::SetFFactorMethodValid(kFALSE,fPulserColor) if not succesful
//
// - Calculate the quantum efficiency with the formula:
// 
//   QE = ( Num.Phes / av.Num.photons ) * MGeomCam::GetPixRatio()
//
// - Set QE in MCalibrationQEPix::SetQEFFactor ( QE, fPulserColor );
//
// - Set Variance of QE in  MCalibrationQEPix::SetQEFFactorVar ( Variance, fPulserColor );
// - Set bit MCalibrationQEPix::SetFFactorMethodValid(kTRUE,fPulserColor) 
//
// - Call MCalibrationQEPix::UpdateFFactorMethod()
//
void MCalibrationChargeCalc::FinalizeFFactorQECam()
{

  if (fNumInnerFFactorMethodUsed < 2)
    {
      *fLog << warn << GetDescriptor() 
            << ": Could not calculate F-Factor Method: Less than 2 inner pixels valid! " << endl;
      return;
    }
  
  MCalibrationChargePix &avpix = (MCalibrationChargePix&)fCam->GetAverageArea(0);
  MCalibrationQEPix     &qepix = (MCalibrationQEPix&)    fQECam->GetAverageArea(0);

  if (IsDebug())
    *fLog << dbginf << "External Phes: " << fExternalNumPhes
          << " Internal Phes: " << avpix.GetPheFFactorMethod() << endl;

  const Float_t avphotons   = ( IsUseExternalNumPhes()
                                ? fExternalNumPhes
                                : avpix.GetPheFFactorMethod() )
                           / qepix.GetDefaultQE(fPulserColor) 
                           / qepix.GetPMTCollectionEff()
                           / qepix.GetLightGuidesEff(fPulserColor)
                           / fQECam->GetPlexiglassQE();

  const Float_t avphotrelvar = ( IsUseExternalNumPhes()
                                 ? fExternalNumPhesRelVar
                                 : avpix.GetPheFFactorMethodRelVar() )
                            + qepix.GetDefaultQERelVar(fPulserColor) / fNumInnerFFactorMethodUsed
                            + qepix.GetPMTCollectionEffRelVar()
                            + qepix.GetLightGuidesEffRelVar(fPulserColor)
                            + fQECam->GetPlexiglassQERelVar();

  const UInt_t nareas   = fGeom->GetNumAreas();

  //
  // Set the results in the MCalibrationChargeCam
  //
  fCam->SetNumPhotonsFFactorMethod (avphotons);

  if (avphotrelvar > 0.)
    fCam->SetNumPhotonsFFactorMethodErr(TMath::Sqrt( avphotrelvar * avphotons * avphotons));

  TArrayF lowlim           (nareas);
  TArrayF upplim           (nareas);
  TArrayD avffactorphotons (nareas);
  TArrayD avffactorphotvar (nareas);
  TArrayI numffactor       (nareas);

  const UInt_t npixels  = fGeom->GetNumPixels();

  MHCamera camffactor(*fGeom,"Camffactor","F-Factor in Camera");

  for (UInt_t i=0; i<npixels; i++)
    {
      
      MCalibrationChargePix &pix   = (MCalibrationChargePix&)(*fCam)[i];
      MCalibrationQEPix     &qpix  = (MCalibrationQEPix&)    (*fQECam)[i];

      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (bad.IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue;

      const Float_t photons = avphotons / fGeom->GetPixRatio(i);
      const Float_t qe      = pix.GetPheFFactorMethod() / photons ;

      const Float_t qerelvar = avphotrelvar +  pix.GetPheFFactorMethodRelVar();

      qpix.SetQEFFactor    ( qe            , fPulserColor );
      qpix.SetQEFFactorVar ( qerelvar*qe*qe, fPulserColor );
      qpix.SetFFactorMethodValid(  kTRUE   , fPulserColor );

      if (!qpix.UpdateFFactorMethod( fQECam->GetPlexiglassQE() ))
        *fLog << warn << GetDescriptor() 
              << ": Cannot update Quantum efficiencies with the F-Factor Method" << endl;

      //
      // The following pixels are those with deviating sigma, but otherwise OK, 
      // probably those with stars during the pedestal run, but not the cal. run.
      //
      if (!pix.CalcMeanFFactor( photons , avphotrelvar ))
        {
          bad.SetUncalibrated( MBadPixelsPix::kDeviatingFFactor );
          if (IsCheckDeviatingBehavior())
            bad.SetUnsuitable  ( MBadPixelsPix::kUnreliableRun    );
          continue;
        }

      const Int_t aidx = (*fGeom)[i].GetAidx();
      const Float_t ffactor = pix.GetMeanFFactorFADC2Phot();

      camffactor.Fill(i,ffactor);
      camffactor.SetUsed(i);

      avffactorphotons[aidx] += ffactor;
      avffactorphotvar[aidx] += ffactor*ffactor;
      numffactor[aidx]++;
    }

  for (UInt_t i=0; i<nareas; i++)
    {

      if (numffactor[i] == 0)
        {
          *fLog << warn << GetDescriptor() << ": No pixels with valid total F-Factor found "
                << "in area index: " << i << endl;
          continue;
        }

      avffactorphotvar[i] = (avffactorphotvar[i] - avffactorphotons[i]*avffactorphotons[i]/numffactor[i]) 
                          / (numffactor[i]-1.);
      avffactorphotons[i] = avffactorphotons[i] / numffactor[i];

      if (avffactorphotvar[i] < 0.)
        {
          *fLog << warn << GetDescriptor() << ": No pixels with valid variance of total F-Factor found "
                << "in area index: " << i << endl;
          continue;
        }

      lowlim  [i] = 1.;   // Lowest known F-Factor of a PMT
      upplim  [i] = avffactorphotons[i] + fFFactorErrLimit*TMath::Sqrt(avffactorphotvar[i]);

      TArrayI area(1);
      area[0] = i;

      TH1D *hist = camffactor.ProjectionS(TArrayI(),area,"_py",100);
      hist->Fit("gaus","Q0");
      const Float_t mean  = hist->GetFunction("gaus")->GetParameter(1);
      const Float_t sigma = hist->GetFunction("gaus")->GetParameter(2);
      const Int_t   ndf   = hist->GetFunction("gaus")->GetNDF();

      if (IsDebug())
        camffactor.DrawClone();

      if (ndf < 2)
        {
          *fLog << warn << GetDescriptor() << ": Cannot use a Gauss fit to the F-Factor " << endl;
          *fLog << " in the camera with area index: " << i << endl;
          *fLog << " Number of dof.: " << ndf << " is smaller than 2 " << endl;
          *fLog << " Will use the simple mean and rms." << endl;
          delete hist;
          SetFFactorFitOK(i,kFALSE);
          continue;
        }
      
      const Double_t prob = hist->GetFunction("gaus")->GetProb();

      if (prob < 0.001)
        {
          *fLog << warn << GetDescriptor() << ": Cannot use a Gauss fit to the F-Factor " << endl;
          *fLog << " in the camera with area index: " << i << endl;
          *fLog << " Fit probability " << prob << " is smaller than 0.001 " << endl;
          *fLog << " Will use the simple mean and rms." << endl;
          delete hist;
          SetFFactorFitOK(i,kFALSE);
         continue;
        }

      *fLog << inf << "Area   " << setw(4) << i <<": Mean F-Factor :"
          << Form("%4.2f+-%4.2f",mean,sigma) << endl;

      lowlim  [i] = 1.;
      upplim  [i] = mean  + fFFactorErrLimit*sigma;

      delete hist;

      SetFFactorFitOK(i,kTRUE);
    }
  
  *fLog << endl;
  
  for (UInt_t i=0; i<npixels; i++)
    {
      
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[i];
      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (bad.IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        continue;
      
      const Float_t ffactor = pix.GetMeanFFactorFADC2Phot();
      const Int_t   aidx   = (*fGeom)[i].GetAidx();

      if ( ffactor < lowlim[aidx] || ffactor > upplim[aidx] )
        {
            *fLog << warn << "Pixel  " << setw(4) << i<< ": Overall F-Factor "
                << Form("%5.2f",ffactor) << " out of range ["
                << Form("%5.2f,%5.2f",lowlim[aidx],upplim[aidx]) << "]" << endl;

          bad.SetUncalibrated( MBadPixelsPix::kDeviatingFFactor );
          if (IsCheckDeviatingBehavior())
            bad.SetUnsuitable  ( MBadPixelsPix::kUnreliableRun    );
        }
    }

  for (UInt_t i=0; i<npixels; i++)
    {
      
      MCalibrationChargePix &pix  = (MCalibrationChargePix&)(*fCam)[i];
      MCalibrationQEPix     &qpix = (MCalibrationQEPix&)(*fQECam)[i];

      if ((*fBadPixels)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        {
          qpix.SetFFactorMethodValid(kFALSE,fPulserColor);
          pix.SetFFactorMethodValid(kFALSE);
          continue;
        }
    }
}


// ------------------------------------------------------------------------
//
// Loop over pixels: 
//
// - Continue, if not MCalibrationBlindPix::IsFluxInsidePlexiglassAvailable() and set:
//                    MCalibrationQEPix::SetBlindPixelMethodValid(kFALSE,fPulserColor)
//
// - Calculate the quantum efficiency with the formula:
// 
//   QE =  Num.Phes / MCalibrationBlindPix::GetFluxInsidePlexiglass() 
//        / MGeomPix::GetA() * MCalibrationQECam::GetPlexiglassQE()
//
// - Set QE in MCalibrationQEPix::SetQEBlindPixel ( QE, fPulserColor );
// - Set Variance of QE in  MCalibrationQEPix::SetQEBlindPixelVar ( Variance, fPulserColor );
// - Set bit MCalibrationQEPix::SetBlindPixelMethodValid(kTRUE,fPulserColor) 
//
// - Call MCalibrationQEPix::UpdateBlindPixelMethod()
//
void MCalibrationChargeCalc::FinalizeBlindPixelQECam()
{

  if (!fBlindCam)
    return;

  //
  // Set the results in the MCalibrationChargeCam
  //
  if (!fBlindCam->IsFluxInsidePlexiglassAvailable())
    {
  
      const Float_t photons = fBlindCam->GetFluxInsidePlexiglass() * (*fGeom)[0].GetA()
                           / fQECam->GetPlexiglassQE();
      fCam->SetNumPhotonsBlindPixelMethod(photons);
          
      const Float_t photrelvar = fBlindCam->GetFluxInsidePlexiglassRelVar()
                              + fQECam->GetPlexiglassQERelVar();
 
      if (photrelvar > 0.)
        fCam->SetNumPhotonsBlindPixelMethodErr(TMath::Sqrt( photrelvar * photons * photons));
    }
  
  //
  //  With the knowledge of the overall photon flux, calculate the 
  //  quantum efficiencies after the Blind Pixel and PIN Diode method
  //
  const UInt_t npixels  = fGeom->GetNumPixels();
  for (UInt_t i=0; i<npixels; i++)
    {
      
      MCalibrationQEPix &qepix = (MCalibrationQEPix&)(*fQECam)[i];
      
      if (!fBlindCam->IsFluxInsidePlexiglassAvailable())
        {
          qepix.SetBlindPixelMethodValid(kFALSE, fPulserColor);
          continue;
        }

      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (bad.IsUnsuitable (MBadPixelsPix::kUnsuitableRun))
        {
          qepix.SetBlindPixelMethodValid(kFALSE, fPulserColor);
          continue;
        }
      
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[i];
      MGeom &geo = (*fGeom)[i];

      const Float_t qe        = pix.GetPheFFactorMethod() 
                             / fBlindCam->GetFluxInsidePlexiglass()
                             / geo.GetA() 
                             * fQECam->GetPlexiglassQE();

      const Float_t qerelvar = fBlindCam->GetFluxInsidePlexiglassRelVar()
                               + fQECam->GetPlexiglassQERelVar()
                               + pix.GetPheFFactorMethodRelVar();

      qepix.SetQEBlindPixel    ( qe            , fPulserColor );
      qepix.SetQEBlindPixelVar ( qerelvar*qe*qe, fPulserColor );      
      qepix.SetBlindPixelMethodValid(  kTRUE   , fPulserColor );

      if (!qepix.UpdateBlindPixelMethod( fQECam->GetPlexiglassQE()))
        *fLog << warn << GetDescriptor() 
              << ": Cannot update Quantum efficiencies with the Blind Pixel Method" << endl;
    }
}

// ------------------------------------------------------------------------
//
// Loop over pixels: 
//
// - Continue, if not MCalibrationChargePINDiode::IsFluxOutsidePlexiglassAvailable() and set:
//                    MCalibrationQEPix::SetPINDiodeMethodValid(kFALSE,fPulserColor)
//
// - Calculate the quantum efficiency with the formula:
// 
//   QE =  Num.Phes / MCalibrationChargePINDiode::GetFluxOutsidePlexiglass() / MGeomPix::GetA()
//
// - Set QE in MCalibrationQEPix::SetQEPINDiode ( QE, fPulserColor );
// - Set Variance of QE in  MCalibrationQEPix::SetQEPINDiodeVar ( Variance, fPulserColor );
// - Set bit MCalibrationQEPix::SetPINDiodeMethodValid(kTRUE,fPulserColor) 
//
// - Call MCalibrationQEPix::UpdatePINDiodeMethod()
//
void MCalibrationChargeCalc::FinalizePINDiodeQECam()
{
  
  const UInt_t npixels  = fGeom->GetNumPixels();

  if (!fPINDiode)
    return;

  //
  //  With the knowledge of the overall photon flux, calculate the 
  //  quantum efficiencies after the PIN Diode method
  //
  for (UInt_t i=0; i<npixels; i++)
    {
      
      MCalibrationQEPix &qepix = (MCalibrationQEPix&)(*fQECam)[i];

      if (!fPINDiode)
        {
          qepix.SetPINDiodeMethodValid(kFALSE, fPulserColor);
          continue;
        }
      
      if (!fPINDiode->IsFluxOutsidePlexiglassAvailable())
        {
          qepix.SetPINDiodeMethodValid(kFALSE, fPulserColor);
          continue;
        }

      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (!bad.IsUnsuitable (MBadPixelsPix::kUnsuitableRun))
        {
          qepix.SetPINDiodeMethodValid(kFALSE, fPulserColor);
          continue;
        }

      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[i];
      MGeom &geo = (*fGeom)[i];

      const Float_t qe       =  pix.GetPheFFactorMethod()
                               / fPINDiode->GetFluxOutsidePlexiglass() 
                               / geo.GetA();

      const Float_t qerelvar = fPINDiode->GetFluxOutsidePlexiglassRelVar() + pix.GetPheFFactorMethodRelVar();

      qepix.SetQEPINDiode    ( qe            , fPulserColor );
      qepix.SetQEPINDiodeVar ( qerelvar*qe*qe, fPulserColor );      
      qepix.SetPINDiodeMethodValid(  kTRUE   , fPulserColor );

      if (!qepix.UpdatePINDiodeMethod())
        *fLog << warn << GetDescriptor() 
              << ": Cannot update Quantum efficiencies with the PIN Diode Method" << endl;
    }
}

// ------------------------------------------------------------------------
//
// Loop over pixels: 
//
// - Call MCalibrationQEPix::UpdateCombinedMethod()
//
void MCalibrationChargeCalc::FinalizeCombinedQECam()
{
  
  const UInt_t npixels  = fGeom->GetNumPixels();

  for (UInt_t i=0; i<npixels; i++)
    {
      
      MCalibrationQEPix &qepix = (MCalibrationQEPix&)(*fQECam)[i];

      if (!(*fBadPixels)[i].IsUnsuitable (MBadPixelsPix::kUnsuitableRun))
        {
          qepix.SetPINDiodeMethodValid(kFALSE, fPulserColor);
          continue;
        }
      
      qepix.UpdateCombinedMethod();
    }
}

// -----------------------------------------------------------------------------------------------
//
// - Print out statistics about BadPixels of type UnsuitableType_t 
// - store numbers of bad pixels of each type in fCam
//
Bool_t MCalibrationChargeCalc::FinalizeUnsuitablePixels()
{
  
  *fLog << inf << endl;
  *fLog << GetDescriptor() << ": Charge Calibration status:" << endl;
  *fLog << dec << setfill(' ');

  const Int_t nareas = fGeom->GetNumAreas();

  TArrayI suit(nareas);
  TArrayI unsuit(nareas);
  TArrayI unrel(nareas);

  Int_t unsuitcnt=0;
  Int_t unrelcnt =0;

  // Count number of succesfully calibrated pixels
  for (Int_t aidx=0; aidx<nareas; aidx++)
  {
      suit[aidx]   = fBadPixels->GetNumSuitable(MBadPixelsPix::kUnsuitableRun,   fGeom, aidx);
      unsuit[aidx] = fBadPixels->GetNumUnsuitable(MBadPixelsPix::kUnsuitableRun, fGeom, aidx);
      unrel[aidx]  = fBadPixels->GetNumUnsuitable(MBadPixelsPix::kUnreliableRun, fGeom, aidx);

      unsuitcnt   += unsuit[aidx];
      unrelcnt    += unrel[aidx];

      fCam->SetNumUnsuitable(unsuit[aidx], aidx);
      fCam->SetNumUnreliable(unrel[aidx],  aidx);
  }

  TArrayI counts(nareas);
  for (Int_t i=0; i<fCam->GetSize(); i++)
  {
      const MCalibrationPix &pix = (*fCam)[i];
      if (pix.IsHiGainSaturation())
      {
          const Int_t aidx = (*fGeom)[i].GetAidx();
          counts[aidx]++;
      }
  }

  if (fGeom->InheritsFrom("MGeomCamMagic"))
  {
      *fLog << " Successfully calibrated Pixels: Inner: "
          << Form("%3i",suit[0])   << " Outer: " << Form("%3i",suit[1])   << endl;
      *fLog << " Uncalibrated Pixels:            Inner: "
          << Form("%3i",unsuit[0]) << " Outer: " << Form("%3i",unsuit[1]) << endl;
      *fLog << " Unreliable Pixels:              Inner: "
          << Form("%3i",unrel[0])  << " Outer: " << Form("%3i",unrel[1])  << endl;
      *fLog << " High-gain saturated Pixels:     Inner: "
          << Form("%3i",counts[0]) << " Outer: " << Form("%3i",counts[1]) << endl;
      *fLog << endl;
  }

  if (unsuitcnt > fUnsuitablesLimit*fGeom->GetNumPixels())
  {
      *fLog << err << "Number of unsuitable pixels: " << 100.*unsuitcnt/fGeom->GetNumPixels()
          << "% exceeds limit of " << fUnsuitablesLimit*100 << "%" <<  endl;
      return kFALSE;
  }

  if (unrelcnt > fUnreliablesLimit*fGeom->GetNumPixels())
  {
      *fLog << err << "Relative number of unreliable pixels: " << 100.*unrelcnt/fGeom->GetNumPixels()
          << "% exceeds limit of " << fUnreliablesLimit*100 << "%" << endl;
      return kFALSE;
  }
  return kTRUE;
}

// -----------------------------------------------------------------------------------------------
//
// Print out statistics about BadPixels of type UncalibratedType_t 
// 
void MCalibrationChargeCalc::PrintUncalibrated(MBadPixelsPix::UncalibratedType_t typ, const char *text) const 
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

  *fLog << " " << setw(7) << text << "Inner: " << Form("%3i",countinner)
      << " Outer: " << Form("%3i", countouter) << endl;
}

// --------------------------------------------------------------------------
//
// Read the environment for the following data members:
// - fChargeLimit
// - fChargeErrLimit
// - fChargeRelErrLimit
// - fDebug
// - fFFactorErrLimit
// - fLambdaErrLimit
// - fLambdaCheckErrLimit
// - fPheErrLimit
// 
Int_t MCalibrationChargeCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

  Bool_t rc = kFALSE;
  if (IsEnvDefined(env, prefix, "ChargeLimit", print))
    {
      SetChargeLimit(GetEnvValue(env, prefix, "ChargeLimit", fChargeLimit));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "ChargeErrLimit", print))
    {
      SetChargeErrLimit(GetEnvValue(env, prefix, "ChargeErrLimit", fChargeErrLimit));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "ChargeRelErrLimit", print))
    {
      SetChargeRelErrLimit(GetEnvValue(env, prefix, "ChargeRelErrLimit", fChargeRelErrLimit));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "Debug", print))
    {
      SetDebug(GetEnvValue(env, prefix, "Debug", IsDebug()));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "ArrTimeRmsLimit", print))
    {
      SetArrTimeRmsLimit(GetEnvValue(env, prefix, "ArrTimeRmsLimit", fArrTimeRmsLimit));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "FFactorErrLimit", print))
    {
      SetFFactorErrLimit(GetEnvValue(env, prefix, "FFactorErrLimit", fFFactorErrLimit));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "LambdaErrLimit", print))
    {
      SetLambdaErrLimit(GetEnvValue(env, prefix, "LambdaErrLimit", fLambdaErrLimit));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "LambdaCheckLimit", print))
    {
      SetLambdaCheckLimit(GetEnvValue(env, prefix, "LambdaCheckLimit", fLambdaCheckLimit));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "CheckDeadPixels", print))
    {
      SetCheckDeadPixels(GetEnvValue(env, prefix, "CheckDeadPixels", IsCheckDeadPixels()));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "CheckDeviatingBehavior", print))
    {
      SetCheckDeviatingBehavior(GetEnvValue(env, prefix, "CheckDeviatingBehavior", IsCheckDeviatingBehavior()));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "CheckExtractionWindow", print))
    {
      SetCheckExtractionWindow(GetEnvValue(env, prefix, "CheckExtractionWindow", IsCheckExtractionWindow()));
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
  if (IsEnvDefined(env, prefix, "CheckArrivalTimes", print))
    {
      SetCheckArrivalTimes(GetEnvValue(env, prefix, "CheckArrivalTimes", IsCheckArrivalTimes()));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "PheErrLowerLimit", print))
    {
      SetPheErrLowerLimit(GetEnvValue(env, prefix, "PheErrLowerLimit", fPheErrLowerLimit));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "PheErrUpperLimit", print))
    {
      SetPheErrUpperLimit(GetEnvValue(env, prefix, "PheErrUpperLimit", fPheErrUpperLimit));
      rc = kTRUE;
    }
   if (IsEnvDefined(env, prefix, "UseExtractorRes", print))
     {
       SetUseExtractorRes(GetEnvValue(env, prefix, "UseExtractorRes", IsUseExtractorRes()));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "UseUnreliables", print))
    {
      SetUseUnreliables(GetEnvValue(env, prefix, "UseUnreliables", IsUseUnreliables()));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "UseExternalNumPhes", print))
    {
      SetUseExternalNumPhes(GetEnvValue(env, prefix, "UseExternalNumPhes", IsUseExternalNumPhes()));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "UnsuitablesLimit", print))
    {
      SetUnsuitablesLimit(GetEnvValue(env, prefix, "UnsuitablesLimit", fUnsuitablesLimit));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "UnreliablesLimit", print))
    {
       SetUnreliablesLimit(GetEnvValue(env, prefix, "UnreliablesLimit", fUnreliablesLimit));
       rc = kTRUE;
     }
 

  return rc;
}
