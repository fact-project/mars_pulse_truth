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
!   Author(s): Markus Gaug, 12/2004 <mailto:markus@ifae.es>
!         
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
//  MCalibCalcFromPast
//
//  Steers the occurrance of interlaced calibration events in one data run
// 
//  Input Containers:
//   MParList
//
//  Output Containers:
//
// Class version 2:
//  +  UInt_t  fNumPhesDump;         // Number of cams after which the number of phes gets averaged
//  +  Float_t fMeanPhes;
//  +  Float_t fMeanPhesRelVar;
//  +  Bool_t  fUpdateNumPhes;       // Update the number of photo-electrons only after fNumPhesDump number of Cams
//  +  TArrayF fPhes;
//  +  TArrayF fPhesVar;
//
//////////////////////////////////////////////////////////////////////////////
#include "MCalibCalcFromPast.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawRunHeader.h"

#include "MHCalibrationCam.h"

#include "MBadPixelsCam.h"

#include "MCalibrationQECam.h"
#include "MCalibrationBlindCam.h"
#include "MCalibrationChargeCam.h"
#include "MCalibrationChargePix.h"
#include "MCalibrationChargeCalc.h"
#include "MCalibrationRelTimeCalc.h"
#include "MCalibrateData.h"

ClassImp(MCalibCalcFromPast);

using namespace std;

const UInt_t MCalibCalcFromPast::fgNumEventsDump = 500;
const UInt_t MCalibCalcFromPast::fgNumPhesDump   = 10;

// --------------------------------------------------------------------------
//
//  Default constructor. 
//
// Sets:
// - fNumEventsDump to fgNumEventsDump
// - fNumPhesDump to fgNumPhesDump
//
MCalibCalcFromPast::MCalibCalcFromPast(const char *name, const char *title)
    : fGeom(NULL), fParList(NULL), fRunHeader(NULL),
    fCharge(NULL), fBlindCam(NULL), fQECam(NULL), fBadPixels(NULL),
    fChargeCalc(NULL), fRelTimeCalc(NULL), fCalibrate(NULL),
    fNumCam(0), fNumEvents(0), fUpdateWithFFactorMethod(kTRUE), fUpdateNumPhes(kTRUE),
    fNumFails(0)
{

  fName  = name  ? name  : "MCalibCalcFromPast";
  fTitle = title ? title : "Task to steer the processing of interlaced calibration events";

  SetNumEventsDump();
  SetNumPhesDump  ();
}

// -----------------------------------------------------------------------------------
//
Int_t MCalibCalcFromPast::PreProcess(MParList *pList)
{

  fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
  if (!fGeom)
    {
      *fLog << err << "MGeomCam not found... abort." << endl;
      return kFALSE;
    }

  fParList = pList;
  if (!fParList)
    {
      *fLog << err << "MParList not found... abort." << endl;
      return kFALSE;
    }

  fRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
  if (!fRunHeader)
  {
      *fLog << err << "MRawRunHeader not found... abort." << endl;
      return kFALSE;
  }

  // 
  // Look for the MBadPixels Cam
  //
  fBadPixels = (MBadPixelsCam*)pList->FindObject("MBadPixelsCam");
  if (!fBadPixels)
  {
      *fLog << err << "MBadPixelsCam not found... abort." << endl;
      return kFALSE;
  }

  //
  // Look for the MCalibrationQECam
  //
  fQECam = (MCalibrationQECam*)pList->FindObject("MCalibrationQECam");
  if (!fQECam)
  {
      *fLog << err << "MCalibrationQECam not found... abort." << endl;
      return kFALSE;
  }

  // 
  // Look for the MFillH-class "MHCalibrationBlindCam".
  //
  if (pList->FindObject(AddSerialNumber("MHCalibrationBlindCam")))
  {
      *fLog << inf << "Found MHCalibrationBlindCam ... " << flush;

      fBlindCam = (MCalibrationBlindCam*)pList->FindCreateObj("MCalibrationBlindCam");
      if (!fBlindCam)
          return kFALSE;
  }

  //
  // Look for the MFillH-class "MHCalibrationChargeCam". 
  //
  if (pList->FindObject(AddSerialNumber("MHCalibrationChargeCam")))
  {

    fCharge = (MCalibrationChargeCam*)pList->FindCreateObj("MCalibrationChargeCam");
    if (!fCharge)
        return kFALSE;

    if (!fChargeCalc)
      fChargeCalc   = (MCalibrationChargeCalc*)pList->FindObject("MCalibrationChargeCalc");
    if (!fChargeCalc)
      {
        *fLog << err << "Could not find MCalibrationChargeCalc abort... " << endl;
        return kFALSE;
      }

    if (!fCalibrate)
      fCalibrate   = (MCalibrateData*)pList->FindObject("MCalibrateData");
    if (!fCalibrate)
      {
        *fLog << err << "Could not find MCalibrateData abort... " << endl;
        return kFALSE;
      }
    
    *fLog << inf << "Found MHCalibrationChargeCam ... " << flush;

  }

  // 
  // Look for the MFillH name "FillRelTimeCam".
  //
  if (pList->FindObject(AddSerialNumber("MHCalibrationRelTimeCam")))
  {
    if (!fRelTimeCalc)
      fRelTimeCalc   = (MCalibrationRelTimeCalc*)pList->FindObject(AddSerialNumber("MCalibrationRelTimeCalc"));
    if (!fRelTimeCalc)
      {
        *fLog << err << "Could not find MCalibrationRelTimeCalc abort... " << endl;
        return kFALSE;
      }

    *fLog << inf << "Found MHCalibrationRelTimeCam ... " << flush;
  }

  fNumCam    = 0;
  fNumEvents = 0;
  fNumPhes   = 0;

  fChargeCalc->SetUseExternalNumPhes(kFALSE);

  if (fUpdateNumPhes)
    {
      fPhes.Set(fNumPhesDump);
      fPhesVar.Set(fNumPhesDump);
    }

  fNumFails = 0;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set fNumEvents=0
//
// This is necessary because the calibration histograms do reset themself
// if ReInit is called, so they are empty. MCalibCalcFromPast errornously
// ignores how many events are in the histograms but count the number
// itself.
//
Bool_t MCalibCalcFromPast::ReInit(MParList *pList)
{
    if (fNumEvents>0)
        *fLog << inf << fNumEvents << " calibration events at the end of the last file have been skipped." << endl;

    fNumEvents = 0;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// - Initializes new containers in the 
// - if the number of calibration events has reach fNumEventsDump.
// - Executes Finalize() of the MCalibration*Calc classes in that case.
// - Sets the latest MCalibrationChargeCam as update class into MCalibrateData
// - clean MCalibration*Cams
//
Int_t MCalibCalcFromPast::Process()
{
  if (fNumEvents++ < fNumEventsDump)
    return kTRUE;

  // Replace the old cams by (empty) new ones
  // MCalibrationChargeCam: fCharge
  // MCalibrationQECam:     fIntensQE
  // MCalibrationBlindCam:  fIntensBlind
  fNumEvents = 0;
  ReInitialize();

  *fLog << all << "MCalibCalcFromPast: Calibration Update..." << flush;

  //
  // Finalize Possible calibration histogram classes...
  //
  *fLog << inf << "Finalize calibration histograms:" << endl;

  // This fills the Cams which are cleared by
  // ReInitialize with the result of the last calib cycle
  Finalize("MHCalibrationChargeCam");
  Finalize("MHCalibrationChargeBlindCam");
  Finalize("MHCalibrationRelTimeCam");

  //
  // Finalize possible calibration calculation tasks
  //
  *fLog << endl;
  *fLog << inf << "Finalize calibration calculations..." << endl;
  if (fChargeCalc)
    {
        // Finalized Pedestals, Charges, Bad Pixels and all QE cams
        if (!fChargeCalc->Finalize())
        {
            fNumFails++;
            *fLog << warn << "WARNING - Finalization of charges failed the " << fNumFails << ". time..." << endl;
            return kTRUE;
        }

        if (fUpdateNumPhes)
        {
          MCalibrationChargePix &avpix =(MCalibrationChargePix&)fCharge->GetAverageArea(0);
          fPhes   [fNumPhes] = avpix.GetPheFFactorMethod();
          fPhesVar[fNumPhes] = avpix.GetPheFFactorMethodVar();

          fNumPhes++;

          if (fNumPhes == fNumPhesDump)
            {
              fNumPhes = 0;
              if (!UpdateMeanPhes())
                {
                  *fLog << warn << "Could not update mean number of photo-electrons. "
                        << "Skip it until next update" << endl;
                  fChargeCalc->SetUseExternalNumPhes(kFALSE);
                }
              else
                {
                  *fLog << inf << "New averaged number photo-electrons: " << fMeanPhes << endl;
                  fChargeCalc->SetExternalNumPhes      ( fMeanPhes );
                  fChargeCalc->SetExternalNumPhesRelVar( fMeanPhesRelVar );
                  fChargeCalc->SetUseExternalNumPhes();
                }
            }
        }
    }

  if (fRelTimeCalc)
    fRelTimeCalc->Finalize();

  if (fCalibrate)
    return fCalibrate->UpdateConversionFactors(fUpdateWithFFactorMethod ? NULL : fCharge);

  return kTRUE;
}


// --------------------------------------------------------------------------
//
// Searches for name in the MParList and calls, if existing: 
// - MHCalibrationCam::Finalize()
// - MHCalibrationCam::ResetHists()
//
void MCalibCalcFromPast::Finalize(const char* name, Bool_t finalize)
{
    MHCalibrationCam *hist = (MHCalibrationCam*)fParList->FindObject(name, "MHCalibrationCam");
    if (!hist)
        return;

    *fLog << inf << "Finalize " << name << ":" << endl;

    if (finalize)
        hist->Finalize();

    hist->ResetHists();
}

// --------------------------------------------------------------------------
//
// Clear contents of cams
//
Bool_t MCalibCalcFromPast::ReInitialize()
{
  fNumCam++;

  *fLog << inf << "MCalibCalcFromPast::ReInitialize #" << fNumCam << ": ";

  const Int_t runnumber = fRunHeader->GetRunNumber();

  // The "DeleteOldCam" function must not delete the first entry in
  // the array because it is a special cam from the MParList. (see above)
  *fLog << "MBadPixelsCam...";
  fBadPixels->Clear();  // FIXME:::::: MERGE PreExcl!!!!
  // IS IT REALLY NECESSARY?

  *fLog << "MCalibrationQECam...";
  fQECam->Clear();
  fQECam->SetRunNumber(runnumber);
  // IS IT REALLY NECESSARY?

  if (fBlindCam)
  {
      *fLog << "MCalibrationBlindCam...";
      fBlindCam->Clear();
      fBlindCam->SetRunNumber(runnumber);
  }
  // IS IT REALLY NECESSARY?

  // IF SIMPLE ENOUGH, REMOVE THE FUNCTION!

  if (fCharge)
  {
      *fLog << "MCalibrationChargeCam...";
      fCharge->Clear();
      fCharge->SetRunNumber(runnumber);
  }

  *fLog << endl;

  return kTRUE;

}

Int_t MCalibCalcFromPast::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    // Now we reset all histograms to make sure that the PostProcess
    // of the following tasks doesn't try to finalize a partly empty
    // histogram!
    Finalize("MHCalibrationChargeCam",      kFALSE);
    Finalize("MHCalibrationChargeBlindCam", kFALSE);
    Finalize("MHCalibrationRelTimeCam",     kFALSE);

    if (fChargeCalc)
        fChargeCalc->ResetNumProcessed();

    if (fNumCam==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << " " << setfill(' ') << setw(7) << fNumFails << " (" << Form("%5.1f", 100.*fNumFails/fNumCam) << "%) updates failed." << endl;
    *fLog << endl;

    return kTRUE;
}


Bool_t MCalibCalcFromPast::UpdateMeanPhes()
{
    Double_t sumw = 0.;
    Double_t sum  = 0.;

    for (Int_t i=0; i<fPhes.GetSize(); i++)
    {
        const Float_t weight = 1./fPhesVar[i];
        sum  += fPhes[i]*weight;
        sumw += weight;
    }

    if (sumw < 0.000001)
        return kFALSE;

    if (sum < 0.000001)
        return kFALSE;

    fMeanPhes       = sum/sumw;
    fMeanPhesRelVar = sumw/sum/sum;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MCalibCalcFromPast.UpdateWithFFactorMethod: Off, On
//   MCalibCalcFromPast.NumEventsDump: 500
//   MCalibCalcFromPast.UpdateNumPhes: yes/no
//   MCalibCalcFromPast.NumPhesDump: 10
//
Int_t MCalibCalcFromPast::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "UpdateWithFFactorMethod", print))
    {
        rc = kTRUE;
        SetUpdateWithFFactorMethod(GetEnvValue(env, prefix, "UpdateWithFFactorMethod", fUpdateWithFFactorMethod));
    }

    if (IsEnvDefined(env, prefix, "NumEventsDump", print))
    {
        SetNumEventsDump(GetEnvValue(env, prefix, "NumEventsDump", (Int_t)fNumEventsDump));
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "UpdateNumPhes", print))
    {
        TString s = GetEnvValue(env, prefix, "UpdateNumPhes", "");
        s.ToLower();
        if (s.BeginsWith("no"))
            SetUpdateNumPhes(kFALSE);
        if (s.BeginsWith("yes"))
            SetUpdateNumPhes(kTRUE);
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "NumPhesDump", print))
    {
        SetNumPhesDump(GetEnvValue(env, prefix, "NumPhesDump", (Int_t)fNumPhesDump));
        rc = kTRUE;
    }

    return rc;
}
