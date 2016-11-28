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
!   Author(s): Nicola Galante  12/2004 <mailto:nicola.galante@pi.infn.it>
!   Author(s): Thomas Bretz 12/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MCalibrationPatternDecode
//
//  Decodes the trigger pattern from MRawEvtData into MCalibrationPattern.
//
// For files before file version 5 the trigger pattern is set to 00000000.
// This can be changed using the information about the file-type.
//
// Input:
//   MRawEvtData
//   MRawRunHeader
//
// Output:
//   MCalibrationPattern
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationPatternDecode.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MRawEvtHeader.h"
#include "MRawRunHeader.h"
#include "MCalibrationPattern.h"

ClassImp(MCalibrationPatternDecode);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor
//
MCalibrationPatternDecode::MCalibrationPatternDecode(const char *name, const char *title)
    : fRunHeader(0), fEvtHeader(0), fPattern(0)
{
    fName  = name  ? name  : "MCalibrationPatternDecode";
    fTitle = title ? title : "Task to decode Trigger Pattern";
}

// --------------------------------------------------------------------------
//
Int_t MCalibrationPatternDecode::PreProcess(MParList *pList)
{
    fRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRunHeader)
    {
	*fLog << err << "MRawRunHeader not found... abort." << endl;
	return kFALSE;
    }

    fEvtHeader = (MRawEvtHeader *)pList->FindObject("MRawEvtHeader");
    if (!fEvtHeader)
    {
	*fLog << err << "MRawEvtHeader not found... abort." << endl;
	return kFALSE;
    }

    fPattern = (MCalibrationPattern*)pList->FindCreateObj("MCalibrationPattern");
    if (!fPattern)
	return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// For files before file version 5 the trigger pattern is set to 00000000.
// This can be changed using the information about the file-type.
//
Int_t MCalibrationPatternDecode::Process()
{
    // No setting necessary because MCalibrationPattern::Reset() has
    // been called just before
    if (fRunHeader->GetFormatVersion()<5)
        return kTRUE;

    // Get calibration pattern from event header
    const UInt_t pattern = fEvtHeader->GetCalibrationPattern();

    const UInt_t str = (pattern    ) & 0x00ff;
    const UInt_t col = (pattern>> 8) & 0x000f;
    const UInt_t pat = (pattern>>16) & 0xffff;

    fPattern->fCLStrength = str;
    fPattern->fCLColor    = (MCalibrationPattern::CLColor_t)col;

    // Set a default pattern
    fPattern->fPulserColor =  MCalibrationCam::kNONE;

    // This is a MC workaround because the CT1 bit cannot be set
    // It would be bit 33 in the pattern which simple
    // doesn't exist in a 32 bit integer.
    if (fRunHeader->IsMonteCarloRun() && pat==0)
        fPattern->fPulserColor =  MCalibrationCam::kCT1;

    // Check the pattern
    if ((pat & kGreenAndBlue) || (pat & kBlueAndUV) || (pat & kGreenAndUV))
        fPattern->fPulserColor =  MCalibrationCam::kNONE;

    // FIXME: This condition can never be true!
    //if (pat & kCT1Pulser)
    //    fPattern->fPulserColor =  MCalibrationCam::kCT1;

    if (pat & kAnyUV)
        fPattern->fPulserColor =  MCalibrationCam::kUV;

    if (pat & kAnyGreen)
        fPattern->fPulserColor =  MCalibrationCam::kGREEN;

    if (pat & kAnyBlue)
        fPattern->fPulserColor =  MCalibrationCam::kBLUE;

    // Now decode the strength
    fPattern->fPulserStrength = 0.;

    switch (fPattern->fPulserColor)
    {
      case MCalibrationCam::kNONE:
          break;

      case MCalibrationCam::kGREEN:
          if (pat & kSlot1Green)
              fPattern->fPulserStrength += 5.0;
          if (pat & kSlot2Green)
              fPattern->fPulserStrength += 2.0;
          if (pat & kSlot15Green)
              fPattern->fPulserStrength += 1.0;
          if (pat & kSlot16AttGreen)
              fPattern->fPulserStrength += 0.2;
          break;

      case MCalibrationCam::kBLUE:
          if (pat & kSlot3Blue)
              fPattern->fPulserStrength += 5.1;
          if (pat & kSlot6Blue)
              fPattern->fPulserStrength += 5.2;
          if (pat & kSlot7Blue)
              fPattern->fPulserStrength += 5.4;
          if (pat & kSlot8Blue)
              fPattern->fPulserStrength += 2.0;
          if (pat & kSlot9AttBlue)
              fPattern->fPulserStrength += 0.25;
          if (pat & kSlot10Blue)
              fPattern->fPulserStrength += 0.0;
          if (pat & kSlot11Blue)
              fPattern->fPulserStrength += 1.0;
          if (pat & kSlot14Blue)
              fPattern->fPulserStrength += 5.8;
          break;

      case MCalibrationCam::kUV:
          if (pat & kSlot4UV)
              fPattern->fPulserStrength += 1.0;
          if (pat & kSlot5UV)
              fPattern->fPulserStrength += 2.0;
          if (pat & kSlot12UV)
              fPattern->fPulserStrength += 5.1;
          if (pat & kSlot13UV)
              fPattern->fPulserStrength += 5.2;
          break;

      case MCalibrationCam::kCT1:
          fPattern->fPulserStrength = 20.;
          break;
    }

    return kTRUE;
}
