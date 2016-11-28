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
!   Author(s): Thomas Bretz, 12/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportRun
//
// This is the class interpreting and storing the RUN-REPORT information.
//
// PRELIMINARY!
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportRun.h"

#include "MLogManip.h"

ClassImp(MReportRun);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "RUN-REPORT" No subsystem
// is expected.
//
MReportRun::MReportRun() : MReport("RUN-REPORT", kFALSE), fRunNumber(-1)
{
    fName  = "MReportRun";
    fTitle = "Class for RUN-REPORT information";
}

// --------------------------------------------------------------------------
//
// Interprete the body of the RUN-REPORT string
//
Int_t MReportRun::InterpreteBody(TString &str, Int_t ver)
{
    // --------- Get Token (START/STOP) ---------

    const Int_t ws = str.First(' ');
    if (ws<0)
    {
        *fLog << warn << "WARNING - Token not found." << endl;
        return kCONTINUE;
    }
    const TString tok=str(0, ws);

    str.Remove(0, ws);
    str = str.Strip(TString::kBoth);

    // --------- Get Run Number ---------

    Int_t len, run;
    const Int_t n=sscanf(str.Data(), "%d %n", &run, &len);
    if (n!=1)
    {
        *fLog << warn << "WARNING - Wrong number of arguments." << endl;
        return kCONTINUE;
    }
    str.Remove(0, len);

    // --------- Get File Number ---------
    Int_t file = 0;
    if (ver>=200805190)
    {
        const Int_t nn=sscanf(str.Data(), "%d %n", &file, &len);
        if (nn!=1)
        {
            *fLog << warn << "WARNING - Wrong number of arguments." << endl;
            return kCONTINUE;
        }
        str.Remove(0, len);
    }

    // --------- Invalidate Run/File Number if run was stopped ---------
    if (tok=="START")
    {
        if (fRunNumber!=-1)
            *fLog << warn << "WARNING - RUN-REPORT STOP missing for run #" << dec << fRunNumber <<  endl;

        fRunNumber  = run;
        fFileNumber = file;
    }

    if (tok=="STOP")
    {
        if (fRunNumber==-1)
            *fLog << warn << "WARNING - RUN-REPORT START missing for run #" << dec << fRunNumber << endl;
        else
        {
            if (fRunNumber!=run)
                *fLog << warn << "WARNING - RUN-REPORT STOP run number #" << dec << run << " doesn't match started run #" << fRunNumber << "." << endl;

            if (fFileNumber!=file)
                *fLog << warn << "WARNING - RUN-REPORT STOP file number #" << dec << run << " doesn't match started file #" << fFileNumber << "." << endl;
        }

        fRunNumber  = -1;
        fFileNumber = -1;
    }

    // --------- Get source name ---------
    Ssiz_t pos = str.First(' ');
    if (pos<0)
        pos = str.Length();
    fSourceName = TString(str(0, pos+1)).Strip(TString::kBoth);

    return kTRUE;
}
