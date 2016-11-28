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
//  MReportRec
//
// This is the class interpreting and storing the REC-REPORT information.
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportRec.h"

#include "MLogManip.h"

#include "MParList.h"

#include "MCameraTH.h"
#include "MCameraTD.h"
#include "MCameraRecTemp.h"

ClassImp(MReportRec);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "CC-REPORT" Report
// is expected to have no 'subsystem' time.
//
MReportRec::MReportRec() : MReport("REC-REPORT", kFALSE)
{
    fName  = "MReportRec";
    fTitle = "Class for REC-REPORT information";
}

// --------------------------------------------------------------------------
//
// FindCreate the following objects:
//  - MCameraTH
//
Bool_t MReportRec::SetupReading(MParList &plist)
{
    fTH = (MCameraTH*)plist.FindCreateObj("MCameraTH");
    if (!fTH)
        return kFALSE;

    fTD = (MCameraTD*)plist.FindCreateObj("MCameraTD");
    if (!fTD)
        return kFALSE;

    fRecTemp = (MCameraRecTemp*)plist.FindCreateObj("MCameraRecTemp");
    if (!fRecTemp)
        return kFALSE;

    return MReport::SetupReading(plist);
}

Int_t MReportRec::InterpreteRec(TString &str, Int_t ver, MCameraTH &th, MCameraTD &td, MCameraRecTemp &temp)
{
    if (str.BeginsWith("RECEIVERS-COM-ERROR"))
    {
        *fLog << inf << "Receiver Com-error... threshold setting and receiver board temp. invalid." << endl;
        td.Invalidate();
        th.Invalidate();
        temp.Invalidate();
        str.Remove(0, 19);

        return kTRUE;
    }

    if (!CheckTag(str, "TH "))
        return kFALSE;

    if (!th.InterpreteTH(str, ver, td))
        return kCONTINUE;

    if (!CheckTag(str, "TD "))
        return kFALSE;

    if (!td.InterpreteTD(str, ver))
        return kCONTINUE;

    if (ver<200510250)
        return kTRUE;

    if (!CheckTag(str, "RECTEMP "))
        return kFALSE;

    if (!temp.InterpreteRecTemp(str))
        return kCONTINUE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the body of the CC-REPORT string
//
Int_t MReportRec::InterpreteBody(TString &str, Int_t ver)
{
    InterpreteRec(str, ver, *fTH, *fTD, *fRecTemp);

    if (str.Strip(TString::kBoth)!=(TString)"OVER")
    {
        *fLog << warn << "WARNING - 'OVER' tag not found." << endl;
        return kCONTINUE;
    }

    return kTRUE;
}
