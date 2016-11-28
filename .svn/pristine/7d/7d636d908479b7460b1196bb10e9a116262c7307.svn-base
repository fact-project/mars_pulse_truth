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
!   Author(s): Thomas Bretz, 02/2012 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2012
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportRates
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportRates.h"

#include "fits.h"

#include "MLogManip.h"

ClassImp(MReportRates);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "DRIVE-REPORT"
//
MReportRates::MReportRates() : MReport("RATES-REPORT"),
    fTimeStamp(0), fOnTimeCounter(0), fTriggerCounter(0), fTriggerRate(0),
    fElapsedTime(0), fElapsedOnTime(0)
{
    fName  = "MReportRates";
    fTitle = "Class for DRIVE-REPORT information (raw telescope position)";

    memset(fBoardRate, 0, sizeof(Float_t)*40);
    memset(fPatchRate, 0, sizeof(Float_t)*160);
}

Bool_t MReportRates::SetupReadingFits(fits &file)
{
    return
        file.SetRefAddress("FTMtimeStamp",   fTimeStamp)       &&
        file.SetRefAddress("OnTimeCounter",  fOnTimeCounter)   &&
        file.SetRefAddress("TriggerCounter", fTriggerCounter)  &&
        file.SetRefAddress("TriggerRate",    fTriggerRate)     &&
        file.SetPtrAddress("BoardRate",      fBoardRate,  40)  &&
        file.SetPtrAddress("PatchRate",      fPatchRate, 160)  &&
        file.SetRefAddress("ElapsedTime",    fElapsedTime)     &&
        file.SetRefAddress("OnTime",         fElapsedOnTime);
}

Int_t MReportRates::InterpreteFits(const fits &fits)
{
    if (fElapsedTime>0 || fElapsedOnTime>0)
        return kTRUE;

    fElapsedTime   = fTimeStamp*1e-6;
    fElapsedOnTime = fOnTimeCounter*1e-6;
    fTriggerRate   = fTriggerCounter*1e6/fTimeStamp;

    return kTRUE;
}

void MReportRates::Print(Option_t *o) const
{
    *fLog << GetDescriptor() << ": Time=" << fTimeStamp << "/" << fElapsedTime << " OnTime=" << fOnTimeCounter << "/" << fElapsedOnTime << endl;
}
