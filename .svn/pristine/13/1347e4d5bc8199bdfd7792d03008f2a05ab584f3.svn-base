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
!   Author(s): Markus Gaug, 04/2004 <mailto:markus@ifae.es>
!   Author(s): Thomas Bretz, 04/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractTime
//
//   Base class for the signal extractors, used the functions 
//   FindTimeHiGain() and FindTimeLoGain() to extract the signal and 
//   substract the pedestal value    
//
//   The following figure gives and example of possible inheritance trees. 
//   An extractor class can inherit from each of the following base classes:
//    - MExtractor
//    - MExtractTime
//    - MExtractTimeAndCharge
//
//Begin_Html
/*
<img src="images/ExtractorClasses.gif">
*/
//End_Html
//
//   The following variables have to be set by the derived class and 
//   do not have defaults:
//   - fNumHiGainSamples
//   - fNumLoGainSamples
//   - fSqrtHiGainSamples
//   - fSqrtLoGainSamples
//
// Input Containers:
//   MRawEvtData
//   MRawRunHeader
//   MPedestalCam
//
// Output Containers:
//   MArrivalTimeCam
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractTime.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"
#include "MRawRunHeader.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

ClassImp(MExtractTime);

using namespace std;

const char *MExtractTime::fgNameTimeCam = "MArrivalTimeCam";

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Set: 
// - all pointers to NULL
// - all variables to 0
// - fSaturationLimit to fgSaturationLimit
// - fNameTimeCam to fgNameTimeCam
//
// Call:
// -  AddToBranchList("MRawEvtData.*")
//
MExtractTime::MExtractTime(const char *name, const char *title)
  : fArrTime(NULL)
{

  fName  = name  ? name  : "MExtractTime";
  fTitle = title ? title : "Base class for signal extractors";
  
  SetNameTimeCam();
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawEvtData
//  - MRawRunHeader
//  - MPedestalCam
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MArrivalTimeCam
//
Int_t MExtractTime::PreProcess(MParList *pList)
{
  fArrTime = (MArrivalTimeCam*)pList->FindCreateObj("MArrivalTimeCam",AddSerialNumber(fNameTimeCam));
  if (!fArrTime)
      return kFALSE;

  return PreProcessStd(pList);
}


void MExtractTime::Print(Option_t *o) const
{
    MExtractor::Print(o);
    if (HasLoGain())
        *fLog << " Offset Lo-Gain:     " << fOffsetLoGain << endl;
}
