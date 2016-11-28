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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportCurrents
//
// This is the class interpreting and storing the DC-REPORT information.
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportCurrents.h"

#include "MLogManip.h"

#include "MParList.h"
#include "MCameraDC.h"

ClassImp(MReportCurrents);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "DC-REPORT" Reports
// are expected to have no 'subsystem' time.
//
MReportCurrents::MReportCurrents() : MReport("DC-REPORT", kFALSE)
{
    fName  = "MReportCurrents";
    fTitle = "Class for DC-REPORT information";
}

// --------------------------------------------------------------------------
//
// FindCreate the following objects:
//  - MCameraDC
//
Bool_t MReportCurrents::SetupReading(MParList &plist)
{
    fDC = (MCameraDC*)plist.FindCreateObj("MCameraDC");
    if (!fDC)
        return kFALSE;

    return MReport::SetupReading(plist);
}

// --------------------------------------------------------------------------
//
// Interprete the body of the DC-REPORT string
//
Int_t MReportCurrents::InterpreteBody(TString &str, Int_t ver)
{
    Int_t len;
    Short_t err1, err2;
    const Int_t n=sscanf(str.Data(), " %hd %hd %n", &err1, &err2, &len);
    if (n!=2)
    {
        *fLog << warn << "WARNING - Reading status information." << endl;
        return kCONTINUE;
    }

    fStatus1 = (Byte_t)err1;
    fStatus2 = (Byte_t)err2;

    // FIXME: Set fDC->fStatus ???

    return fDC->Interprete(str, len);
}
