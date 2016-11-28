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
!   Author(s): Thomas Bretz, 08/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!         
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
//  MCalibColorSet
//
//  Sets the color for events depending on the color which was used for
//  the run. This is a workaround for runs which were taken before the
//  digital module could deliver the color 'per event'
// 
//  Input Containers:
//   MRawRunHeader
//
//  Output Containers:
//   MCalibrationPattern
//
//////////////////////////////////////////////////////////////////////////////
#include "MCalibColorSet.h"

#include <stdlib.h>   // needed for atoi on some platforms

#include <TRegexp.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MCalibrationCam.h"
#include "MCalibrationPattern.h"
#include "MRawRunHeader.h"

ClassImp(MCalibColorSet);

using namespace std;

const Int_t  MCalibColorSet::gkIFAEBoxInaugurationRun = 20113;
const UInt_t MCalibColorSet::gkFirstRunWithFinalBits  = 45626;

// --------------------------------------------------------------------------
//
//  Default constructor. MGeomCamMagic is the default geometry.
//
MCalibColorSet::MCalibColorSet(const char *name, const char *title)
    : fPattern(0), fHeader(0), fIsExplicitColor(kFALSE)
{
  fName  = name  ? name  : "MCalibColorSet";
  fTitle = title ? title : "Task to set workaround missing colors calibration events";

  Clear();
}

void MCalibColorSet::Clear(const Option_t *o)
{
  
  fIsValid         = kFALSE;

  if (fIsExplicitColor)
    return;

  fColor           = MCalibrationCam::kNONE;
  fStrength        = -1.;
}


// -----------------------------------------------------------------------------------
//
// The following container are searched for and execution aborted if not in MParList:
//  - MCalibrationPattern
//
Int_t MCalibColorSet::PreProcess(MParList *pList)
{

  fPattern = (MCalibrationPattern*)pList->FindObject("MCalibrationPattern");
  if (!fPattern)
    {
      *fLog << err << "MCalibrationPattern not found... abort." << endl;
      return kFALSE;
    }

  fHeader = (MRawEvtHeader*)pList->FindObject("MRawEvtHeader");
  if (!fHeader)
    {
      *fLog << err << "MRawEvtHeader not found... abort." << endl;
      return kFALSE;
    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Check if str contains regexp.
// If so or pat to pattern and set color to col.
// Otherwise do nothing.
//
// Normally this function is much to simple (more arguments than lines!)
// but in this particular case it is worth to have it to avois chaotic
// repitions of the same piece of code for many many times.
//
void MCalibColorSet::CheckAndSet(const TString &str, const char *regexp, MCalibrationCam::PulserColor_t col, Float_t strength)
{
    if (!str.Contains(TRegexp(regexp)))
        return;

    fStrength = strength;
    fColor    = col;
}

// --------------------------------------------------------------------------
//
// Search for the following input containers and abort if not existing:
// - MRawRunHeader
//
// If Runnumber < gkIFAEBoxInaugurationRun, set colour pattern: 0
//
// If Runnumber > gkIFAEBoxInaugurationRun, search for colour in 
// the project name: Set colour pattern according to the following 
// convention: 
//    Green: assume slot 1  ( 5 Leds Green)
//    Blue:  assume slot 14 ( 5 Leds Blue )
//    UV:    assume slot 12 ( 5 Leds UV   )
//    CT1:   take  'slot 17'
//
Bool_t MCalibColorSet::ReInit(MParList *pList)
{

  Clear();

  MRawRunHeader *header = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
  if (!header)
    {
      *fLog << err << "MRawRunHeader not found... abort." << endl;
      return kFALSE;
    }
  
  //
  // Consider the case that a pedestal run is interleaved in the calibration run sequence ... prepare
  // to skip this run.
  //
  if (header->GetRunType() == MRawRunHeader::kRTPedestal)
    {
      *fLog << warn << "New run is a pedestal run... need intensity calibration to treat this case!" << endl;
      fColor = MCalibrationCam::kNONE;
      fPattern->SetPulserColor(fColor);
      fIsValid = kTRUE;      
      return kTRUE;
    }
 
  //
  // Check if the color was set explicitly from outside
  //
  if (fIsExplicitColor)
    {
      fPattern->SetPulserColor(fColor);
      fIsValid = kTRUE;
      return kTRUE;
    }

  //
  // 1) Do nothing if this is a MC run. Correct settings of all flags is assumed
  // 2) Skip the rest if the run already contains the correct color information
  //
  if (header->IsMonteCarloRun() || header->GetRunNumber()>gkFirstRunWithFinalBits)
      return kTRUE;

  const Int_t num = header->GetRunNumber();

  if (num<gkIFAEBoxInaugurationRun)
    {
      *fLog << inf << "Run taken before inauguration of IFAE-box... using CT1 pulser." << endl;
      fColor    = MCalibrationCam::kCT1;
      fStrength = 10.;
      fPattern->SetPulserColor(fColor);
      fPattern->SetPulserStrength(fStrength);
      fIsValid  = kTRUE;
      return kTRUE;
    }
  
  fColor    = MCalibrationCam::kNONE;
  fStrength = 0.;

  switch (num)
    {
    case 22246:
    case 22253:
    case 25792:
    case 26402:
    case 34814:
    case 35415:
    case 39942:
    case 39944:
    case 44768:
    case 44976:
    case 45082:
    case 45083:
    case 45089:
    case 45090:
    case 45091:
    case 45094:
    case 45119:
    case 45249:
    case 45253:
    case 45262:
    case 45274:
    case 45275:
    case 45276:
    case 45365:
    case 45366:
    case 45367:
    case 45368:
    case 45369:
    case 45370:
    case 45371:
    case 45382:
    case 45401:
    case 45419:
    case 45432:
    case 45471:
    case 45485:
    case 45489:
    case 45490:
    case 45494:
    case 45503:
    case 45526:
    case 45538:
    case 45549:
    case 45557:
    case 45562:
    case 45571:
    case 45579:
    case 45607:
      //    case 31756:
      fColor = MCalibrationCam::kBLUE;
      fStrength = 1.;
      break;
      
    case 30090:
    case 31745:
    case 31746:
    case 31747:
    case 31748:
    case 34815:
    case 20660:
    case 20661:
    case 26408:
    case 26409:
    case 26412:
    case 26568:
    case 26924:
    case 44834:
    case 45051:
    case 45084:
    case 45085:
    case 45092:
    case 45227:
    case 45241:
    case 45250:
    case 45254:
    case 45263:
    case 45372:
    case 45373:
    case 45608:
      fColor = MCalibrationCam::kGREEN;
      fStrength = 5.;
      break;
      
    case 39941:
    case 39943:
    case 44833:
    case 45086:
    case 45088:
    case 45111:
    case 45115:
    case 45216:
    case 45218:
    case 45226:
    case 45240:
    case 45251:
    case 45278:
    case 45336:
    case 45341:
    case 45358:
    case 45374:
    case 45375:
    case 45376:
    case 45377:
    case 45381:
    case 45400:
    case 45418:
    case 45431:
    case 45470:
    case 45484:
    case 45493:
    case 45502:
    case 45525:
    case 45537:
    case 45548:
    case 45556:
    case 45561:
    case 45570:
    case 45578:
    case 45614:
    case 45618:
      fColor = MCalibrationCam::kUV;
      fStrength = 10.;
      break;
      
    case 43914:
    case 43916:
    case 43918:
    case 43920:
    case 43922:
      fColor = MCalibrationCam::kCT1;
      fStrength = 20.;
      break;

    case 27474:
      *fLog << err << "Sorry, run 27474 was taken with CLOSED LIDS. It should not be used! " << endl;
      return kFALSE;

    case 40493:
    case 45116:
    case 45609:
    case 45219:
      *fLog << err << dec << "Sorry, run " << num 
            << " was taken with a combination of colours used to flat-field ";
      *fLog << err << "the camera. It cannot be used for the standard calibration " << endl;
      return kFALSE;
      
    case 45605:
      *fLog << err << "Sorry, run 45605 was taken with the continuous light source." << endl;
      *fLog << err << "It cannot be used for calibration. Try to run a pedestal extraction on it." << endl;
      return kFALSE;

    case 45606:
      *fLog << err << "Sorry, run 45606 was taken with mal-functionning pulser." << endl;
      *fLog << err << "It cannot be used for calibration. Try to run a pedestal extraction on it." << endl;
      return kFALSE;

    }
  
  if (fColor != MCalibrationCam::kNONE)
  {
      *fLog << inf << "Color determined from the run-number... ";
      switch (fColor)
      {
      case MCalibrationCam::kGREEN: *fLog << "Green."; break;
      case MCalibrationCam::kBLUE:  *fLog << "Blue.";  break;
      case MCalibrationCam::kUV:    *fLog << "UV.";    break;
      case MCalibrationCam::kCT1:   *fLog << "CT1.";   break;
      default: break;
      }
      *fLog << endl;
      fPattern->SetPulserColor(fColor);
      fIsValid  = kTRUE;
      return kTRUE;
  }

  TString proj = header->GetProjectName();
  proj.ToLower();

  // Possible green combinations
  CheckAndSet(proj, "0.1led[s]?gree", MCalibrationCam::kGREEN, 0.1);
  CheckAndSet(proj, "1led[s]?gree",   MCalibrationCam::kGREEN, 1. );
  CheckAndSet(proj, "2led[s]?gree",   MCalibrationCam::kGREEN, 2. );
  CheckAndSet(proj, "3led[s]?gree",   MCalibrationCam::kGREEN, 3. );
  CheckAndSet(proj, "5led[s]?gree",   MCalibrationCam::kGREEN, 5. );
  CheckAndSet(proj, "6led[s]?gree",   MCalibrationCam::kGREEN, 6. );
  CheckAndSet(proj, "7led[s]?gree",   MCalibrationCam::kGREEN, 7. );
  CheckAndSet(proj, "8led[s]?gree",   MCalibrationCam::kGREEN, 8. );

  // Possible blue combinations
  CheckAndSet(proj, "0.1led[s]?blue", MCalibrationCam::kBLUE, 0.1);
  CheckAndSet(proj, "1led[s]?blue",   MCalibrationCam::kBLUE,  1.);
  CheckAndSet(proj, "2led[s]?blue",   MCalibrationCam::kBLUE,  2.);
  CheckAndSet(proj, "3led[s]?blue",   MCalibrationCam::kBLUE,  3.);
  CheckAndSet(proj, "5led[s]?blue",   MCalibrationCam::kBLUE,  5.);
  CheckAndSet(proj, "6led[s]?blue",   MCalibrationCam::kBLUE,  6.);
  CheckAndSet(proj, "7led[s]?blue",   MCalibrationCam::kBLUE,  7.);
  CheckAndSet(proj, "8led[s]?blue",   MCalibrationCam::kBLUE,  8.);
  CheckAndSet(proj, "10led[s]?blue",  MCalibrationCam::kBLUE, 10.);
  CheckAndSet(proj, "15led[s]?blue",  MCalibrationCam::kBLUE, 15.);
  CheckAndSet(proj, "20led[s]?blue",  MCalibrationCam::kBLUE, 20.);
  CheckAndSet(proj, "21led[s]?blue",  MCalibrationCam::kBLUE, 21.);
  CheckAndSet(proj, "22led[s]?blue",  MCalibrationCam::kBLUE, 22.);
  CheckAndSet(proj, "23led[s]?blue",  MCalibrationCam::kBLUE, 23.);

  // Possible UV combinations
  CheckAndSet(proj, "1led[s]?uv",     MCalibrationCam::kUV,  1.);
  CheckAndSet(proj, "2led[s]?uv",     MCalibrationCam::kUV,  2.);
  CheckAndSet(proj, "3led[s]?uv",     MCalibrationCam::kUV,  3.);
  CheckAndSet(proj, "5led[s]?uv",     MCalibrationCam::kUV,  5.);
  CheckAndSet(proj, "6led[s]?uv",     MCalibrationCam::kUV,  6.);
  CheckAndSet(proj, "7led[s]?uv",     MCalibrationCam::kUV,  7.);
  CheckAndSet(proj, "8led[s]?uv",     MCalibrationCam::kUV,  8.);
  CheckAndSet(proj, "10led[s]?uv",    MCalibrationCam::kUV, 10.);
  CheckAndSet(proj, "11led[s]?uv",    MCalibrationCam::kUV, 11.);
  CheckAndSet(proj, "12led[s]?uv",    MCalibrationCam::kUV, 12.);
  CheckAndSet(proj, "13led[s]?uv",    MCalibrationCam::kUV, 13.);

  // Possible slot combinations
  TRegexp slot("slot");
  if (proj.Contains(slot))
  {
      proj.ReplaceAll("slot","");
      UInt_t nr = 0;
      TRegexp slotnr("^[0-9]");

      if (proj.Contains(slotnr))
      {
          proj.Replace(2,99,"");
          proj.ReplaceAll("u","");
          proj.ReplaceAll("v","");
          proj.ReplaceAll("g","");
          nr = atoi(proj.Data())-1;

          fColor = nr < 2 ? MCalibrationCam::kGREEN :
              ( nr < 3 )  ? MCalibrationCam::kBLUE :
              ( nr < 5 )  ? MCalibrationCam::kUV :
              ( nr < 11 ) ? MCalibrationCam::kBLUE :
              ( nr < 13 ) ? MCalibrationCam::kUV :
              ( nr < 14 ) ? MCalibrationCam::kBLUE :
              ( nr < 16 ) ? MCalibrationCam::kGREEN :
              MCalibrationCam::kCT1;

	  switch (nr)
	    {
	    case 0: 
	      fStrength = 5;
	      break;
	    case 1: 
	      fStrength = 2.;
	      break;
	    case 2: 
	      fStrength = 5.1;
	      break;
	    case 3: 
	      fStrength = 1.;
	      break;
	    case 4: 
	      fStrength = 2.;
	      break;
	    case 5: 
	      fStrength = 5.2;
	      break;
	    case 6: 
	      fStrength = 5.4;
	      break;
	    case 7: 
	      fStrength = 2.;
	      break;
	    case 8: 
	      fStrength = 0.2;
	      break;
	    case 9: 
	      fStrength = 0.;
	      break;
	    case 10: 
	      fStrength = 1.;
	      break;
	    case 11: 
	      fStrength = 5.8;
	      break;
	    case 12: 
	      fStrength = 5.1;
	      break;
	    case 13: 
	      fStrength = 5.2;
	      break;
	    case 14: 
	      fStrength = 1;
	      break;
	    case 15: 
	      fStrength = 0.2;
	      break;
	    }
      }
  }

  if (fColor == MCalibrationCam::kNONE)
  {
      CheckAndSet(proj, "gree", MCalibrationCam::kGREEN, 1.);
      CheckAndSet(proj, "blue", MCalibrationCam::kBLUE,  1.);
      CheckAndSet(proj, "uv",   MCalibrationCam::kUV,    1.);
      CheckAndSet(proj, "ct1",  MCalibrationCam::kCT1,   1.);

      if (fColor != MCalibrationCam::kNONE)
          *fLog << inf << "Color determined from project-name (" << proj << ")... ";
      else
          if (proj.Contains("cl",TString::kIgnoreCase))
          {
              *fLog << err;
              *fLog << "This run has been taken with the continuous light source." << endl;
              *fLog << "It cannot be used for calibration. Try to run a pedestal extraction on it." << endl;
              return kFALSE;
          }
  }
  else
      *fLog << inf << "Color and Intensity determined from project-name (" << proj << ")... ";

  if (fColor == MCalibrationCam::kNONE)
  {
      *fLog << err;
      *fLog << "Sorry, calibration run " << num << " was taken before the events could be" << endl;
      *fLog << "flagged with a color by the digital modul and no color" << endl;
      *fLog << "could be determined... abort." << endl;
      return kFALSE;
  }

  switch (fColor)
  {
  case MCalibrationCam::kGREEN: *fLog << "Green.";  break;
  case MCalibrationCam::kBLUE:  *fLog << "Blue.";   break;
  case MCalibrationCam::kUV:    *fLog << "UV.";     break;
  case MCalibrationCam::kCT1:   *fLog << "CT1.";    break;
  default: break;
  }
  *fLog << endl;

  fPattern->SetPulserColor(fColor);
  fPattern->SetPulserStrength(fStrength);

  fIsValid = kTRUE;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Sets the pattern to MCalibrationPattern from outside, if fIsValid is set.
//
Int_t MCalibColorSet::Process()
{

  if (fIsValid)
    {
      if (fColor == MCalibrationCam::kNONE)
        return kCONTINUE;

      fPattern->SetPulserColor(fColor);
      fPattern->SetPulserStrength(fStrength);
    }
  
  return kTRUE;
}

Int_t MCalibColorSet::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

  Bool_t rc = kFALSE;

  if (IsEnvDefined(env, prefix, "ExplicitColor", print))
    {
      TString dat = GetEnvValue(env, prefix, "ExplicitColor", "");
      if (dat.BeginsWith("green", TString::kIgnoreCase))
        SetExplicitColor(MCalibrationCam::kGREEN);
      if (dat.BeginsWith("blue", TString::kIgnoreCase))
        SetExplicitColor(MCalibrationCam::kBLUE);
      if (dat.BeginsWith("uv", TString::kIgnoreCase))
        SetExplicitColor(MCalibrationCam::kUV);
      if (dat.BeginsWith("ct1", TString::kIgnoreCase))
        SetExplicitColor(MCalibrationCam::kCT1);
      rc = kTRUE;
    }

  return rc;
}

