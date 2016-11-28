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
!   Author(s): Markus Gaug, 09/2004 <mailto:markus@ifae.es>
!         
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MCalibColorSteer
//
//  Steers the occurrance of different calibration colours in one calibration 
//  run. 
// 
//  Input Containers:
//   MCalibrationPattern
//   MParList
//
//  Output Containers:
//
//////////////////////////////////////////////////////////////////////////////
#include "MCalibColorSteer.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MTaskList.h"

#include "MGeomCam.h"
#include "MRawRunHeader.h"

#include "MHCalibrationCam.h"
#include "MCalibrationChargeCam.h"
#include "MCalibrationBlindCam.h"
#include "MBadPixelsCam.h"

#include "MCalibrationPattern.h"
#include "MCalibrationQECam.h"
#include "MCalibrationBlindCam.h"

#include "MCalibrationChargeCam.h"
#include "MCalibrationChargeCalc.h"

#include "MCalibrationRelTimeCam.h"
#include "MCalibrationRelTimeCalc.h"

ClassImp(MCalibColorSteer);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default constructor. 
//
MCalibColorSteer::MCalibColorSteer(const char *name, const char *title)
    : fCalibPattern(NULL), fGeom(NULL), fParList(NULL), fCharge(NULL),
      fBad(NULL), fChargeCalc(NULL), fRelTimeCalc(NULL), fHistCopy(kFALSE)
{

  fName  = name  ? name  : "MCalibColorSteer";
  fTitle = title ? title : "Task to steer the processing of different colours in the calibration events";

}

// -----------------------------------------------------------------------------------
//
// The following container are searched for and execution aborted if not in MParList:
//  - MCalibrationPattern
//  - MTaskList
//
Int_t MCalibColorSteer::PreProcess(MParList *pList)
{

  fCalibPattern = (MCalibrationPattern*)pList->FindObject("MCalibrationPattern");
  if (!fCalibPattern)
    {
      *fLog << err << "MCalibrationPattern not found... abort." << endl;
      return kFALSE;
    }

  fRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
  if (!fRunHeader)
    {
      *fLog << err << "MRawRunHeader not found... abort." << endl;
      return kFALSE;
    }

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

  MTaskList *tlist = (MTaskList*)pList->FindObject("MTaskList");
  if (!tlist)
    {
      *fLog << err << "MTaskList not found... abort." << endl;
      return kFALSE;
    }

  // 
  // Look for the MBadPixelsCam
  //
  fBad = (MBadPixelsCam*)pList->FindObject("MBadPixelsCam");
  if (!fBad)
  {
      *fLog << err << "MBadPixelsCam not found... abort." << endl;
      return kFALSE;
  }

  // 
  // Look for the MCalibrationBlindCam
  //
  fBlindCam = (MCalibrationBlindCam*)pList->FindCreateObj("MCalibrationBlindCam");
  if (!fBlindCam)
      return kFALSE;

  // 
  // Look for the MFillH name "FillChargeCam".
  //
  if (pList->FindObject(AddSerialNumber("MHCalibrationChargeCam")))
  {
      *fLog << inf << "Found MHCalibrationChargeCam ... " << flush;

      fCharge = (MCalibrationChargeCam*)pList->FindCreateObj("MCalibrationChargeCam");
      if (!fCharge)
          return kFALSE;

      fQECam = (MCalibrationQECam*)pList->FindCreateObj("MCalibrationQECam");
      if (!fQECam)
          return kFALSE;

      fChargeCalc = (MCalibrationChargeCalc*)tlist->FindObject("MCalibrationChargeCalc");
      if (!fChargeCalc)
      {
          *fLog << err << "Could not find MCalibrationChargeCalc abort... " << endl;
          return kFALSE;
      }
  }

  // 
  // Look for the MFillH name "FillRelTimeCam".
  //
  if (pList->FindObject(AddSerialNumber("MHCalibrationRelTimeCam")))
  {

    *fLog << inf << "Found MHCalibrationRelTimeCam ... " << flush;

    fRelTimeCam = (MCalibrationRelTimeCam*)pList->FindCreateObj("MCalibrationRelTimeCam");
    if (!fRelTimeCam)
        return kFALSE;

    fRelTimeCalc = (MCalibrationRelTimeCalc*)tlist->FindObject(AddSerialNumber("MCalibrationRelTimeCalc"));
    if (!fRelTimeCalc)
        return kFALSE;

    // CALL ITS PrePorcess ???
  }
  
  fColor = MCalibrationCam::kNONE;
  fStrength = 0.;
      
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Reads the pattern from MCalibrationPattern and clear
// Cams, if the pattern has changed. Executes Finalize of the
// MCalibration*Calc classes in that case.
//
Int_t MCalibColorSteer::Process()
{

  const MCalibrationCam::PulserColor_t col = fCalibPattern->GetPulserColor();
  const Float_t strength                   = fCalibPattern->GetPulserStrength();

  if (fColor ==MCalibrationCam::kNONE)
    {
      fColor = col;
      fStrength = strength;
      return kTRUE;
    }

  const Float_t strdiff = TMath::Abs(fStrength-strength);

  if (col  == MCalibrationCam::kNONE || (col == fColor && strdiff < 0.05))
    return kTRUE;

  *fLog << inf << GetDescriptor() << " : Color - old=" << fColor << flush;
  fColor = col;
  *fLog << " / new=" << fColor << endl;

  *fLog << inf << GetDescriptor() << " : Strength - old=" << fStrength << flush;
  fStrength = strength;
  *fLog << " / new=" << fStrength << endl;

  //
  // Finalize Possible calibration histogram classes...
  //
  *fLog << inf << GetDescriptor() << " : Finalize calibration histograms..." << flush;
  if (Finalize("MHCalibrationChargeCam"))      *fLog << "MHCalibrationChargeCam...";
  if (Finalize("MHCalibrationChargeBlindCam")) *fLog << "MHCalibrationChargeBlindCam...";
  if (Finalize("MHCalibrationRelTimeCam"))     *fLog << "MHCalibrationRelTimeCam...";
  if (Finalize("MHCalibrationTestCam"))        *fLog << "MHCalibrationChargeCam...";
  if (Finalize("MHCalibrationTestTimeCam"))    *fLog << "MHCalibrationChargeCam...";

  //
  // Finalize possible calibration calculation tasks
  //
  *fLog << endl;
  *fLog << inf << GetDescriptor() << " : Finalize calibration calculations..." << flush;
  if (fChargeCalc)
    fChargeCalc->Finalize();
  if (fRelTimeCalc)
    fRelTimeCalc->Finalize();

  ReInitialize();

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Reads the pattern from MCalibrationPattern and clear
// Cams, if the pattern has changed. Executes Finalize of the
// MCalibration*Calc classes in that case.
//
Int_t MCalibColorSteer::PostProcess()
{
    //
    // Finalize Possible calibration histogram classes...
    //
    *fLog << inf << "Finalize calibration histograms..." << flush;
    if (Finalize("MHCalibrationChargeCam"))      *fLog << inf << "MHCalibrationChargeCam..." << flush;
    if (Finalize("MHCalibrationChargeBlindCam")) *fLog << inf << "MHCalibrationChargeBlindCam..." << flush;
    if (Finalize("MHCalibrationRelTimeCam"))     *fLog << inf << "MHCalibrationRelTimeCam..." << flush;
    if (Finalize("MHCalibrationTestCam"))        *fLog << inf << "MHCalibrationChargeCam..." << flush;
    if (Finalize("MHCalibrationTestTimeCam"))    *fLog << inf << "MHCalibrationChargeCam..." << flush;
    *fLog << inf << "done." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Searches for name in the MParList and calls, if existing: 
// - MHCalibrationCam::Finalize()
// - MHCalibrationCam::ResetHists()
//
Bool_t MCalibColorSteer::Finalize(const char* name)
{

  MHCalibrationCam *hist = (MHCalibrationCam*)fParList->FindObject(name);
  if (!hist)
      return kFALSE;

  hist->Finalize();
  //CopyHist(name);
  hist->ResetHists();
  hist->SetColor( fCalibPattern->GetPulserColor());
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Clear cams
//
Bool_t MCalibColorSteer::ReInitialize()
{

  *fLog << endl;

  TString namep = GetNamePattern();

  fBad->Clear(); // FIXME:::::: MERGE PreExcl!!!!

  if (fQECam)
      fQECam->Clear();

  fBlindCam->Clear();
  fBlindCam->SetPulserColor(fCalibPattern->GetPulserColor());

  if (fRelTimeCam)
  {
      fRelTimeCam->Clear();
      fRelTimeCam->SetPulserColor(fCalibPattern->GetPulserColor());
  }

  if (fCharge)
  {
      fCharge->Clear();
      fCharge->SetPulserColor(fCalibPattern->GetPulserColor());
  }

  return kTRUE;

}

TString MCalibColorSteer::GetNamePattern()
{

  const Float_t strength = fCalibPattern->GetPulserStrength();
  const MCalibrationCam::PulserColor_t col = fCalibPattern->GetPulserColor();

  TString result = Form("%2.1f",strength);
  
  switch (col)
    {
    case MCalibrationCam::kCT1:
      result += "CT1";
      break;
    case MCalibrationCam::kGREEN:
      result += "GREEN";
      break;
    case MCalibrationCam::kBLUE:
      result += "BLUE";
      break;
    case MCalibrationCam::kUV:
      result += "UV";
      break;
    default:
      break;
    }
  return result;
}
