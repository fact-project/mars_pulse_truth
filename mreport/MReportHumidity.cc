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
//  MReportHumidity
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportHumidity.h"

#include "fits.h"

#include "MLogManip.h"

ClassImp(MReportHumidity);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "DRIVE-REPORT"
//
MReportHumidity::MReportHumidity() : MReport("TEMPERATURES-REPORT"),
    fTimeStamp(0)
{
    fName  = "MReportHumidity";
    fTitle = "Class for DRIVE-REPORT information (raw telescope position)";

    memset(fHumidity, 0, sizeof(Float_t)*4);
}

Bool_t MReportHumidity::SetupReadingFits(fits &file)
{
    return
        file.SetRefAddress("t",  fTimeStamp)   &&
        file.SetPtrAddress("H",  fHumidity, 4);
}

Int_t MReportHumidity::InterpreteFits(const fits &fits)
{
    return kTRUE;
}

Float_t MReportHumidity::GetMean() const
{
    Double_t avg = 0;
    for (int i=0; i<4; i++)
        avg += fHumidity[i];

    return avg/4;
}

void MReportHumidity::Print(Option_t *o) const
{
    *fLog << GetDescriptor() << ": Time=" << fTimeStamp << " Hum=" << GetMean() << "%" << endl;
}
