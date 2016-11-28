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
!   Author(s): Markus Gaug, 2/2005 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJCalib
//
// A base class for the calibration jobs
//
/////////////////////////////////////////////////////////////////////////////
#include "MJCalib.h"

ClassImp(MJCalib);

using namespace std;

const Int_t MJCalib::fgCheckedPixId = 100;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets:
// - fDataFlag      to kIsUseRootData
// - fCheckedPixId  to fgCheckedPixId
// - fPixelCheck    to kFALSE
// - fPulsePosCheck to kFALSE
//
MJCalib::MJCalib() : fStorage(0), fIsPixelCheck(kFALSE), fIsPulsePosCheck(kFALSE)
{
  SetUseBlindPixel(kFALSE);
  SetUsePINDiode(kFALSE);

  SetCheckedPixId();
}

Bool_t MJCalib::CheckEnvLocal()
{
    SetPixelCheck(GetEnv("PixelCheck", fIsPixelCheck));
    SetPulsePosCheck(GetEnv("PulsePosCheck", fIsPulsePosCheck));
    SetCheckedPixId(GetEnv("CheckedPixId",fCheckedPixId));

    if (HasEnv("StorageType"))
      {
        TString type = GetEnv("StorageType", "");
        type = type.Strip(TString::kBoth);
        type.ToLower();

        if (type == (TString)"hists")
          SetHistsStorage();
        
        if (type == (TString)"no" || type == (TString)"nostorage")
          SetNoStorage();
      }

    if (!HasEnv("DataType"))
        return kTRUE;

    return kTRUE;
}
