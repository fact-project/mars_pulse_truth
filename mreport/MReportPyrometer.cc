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
!   Author(s): Thomas Bretz, 6/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportPyrometer
//
// This is the class interpreting and storing the PYRO-REPORT information.
//
// These reports exist since 2007/05/15
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportPyrometer.h"

#include "MLogManip.h"

ClassImp(MReportPyrometer);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "RUN-REPORT" No subsystem
// is expected.
//
MReportPyrometer::MReportPyrometer() : MReport("PYRO-REPORT"),
    fTempSky(-1), fTempAir(-1), fCloudiness(-1), fLidOpen(kFALSE)

{
    fName  = "MReportPyrometer";
    fTitle = "Class for PYRO-REPORT information";
}

// --------------------------------------------------------------------------
//
// Interprete the body of the PYRO-REPORT string
//
Int_t MReportPyrometer::InterpreteBody(TString &str, Int_t ver)
{
    str = str.Strip(TString::kBoth);

    Int_t status, len;

    const Int_t n=sscanf(str.Data(), "%f %f %d %f %n",
                         &fTempSky, &fCloudiness, &status, &fTempAir, &len);
    if (n!=4)
    {
        *fLog << warn << "WARNING - Wrong number of arguments." << endl;
        return kCONTINUE;
    }

    fLidOpen = status>0;

    str.Remove(0, len);
    str = str.Strip(TString::kBoth);

    return str=="OVER" ? kTRUE : kCONTINUE;
}
