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
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MReportFileReadCC
//
// This is a report file reader which implements the CC header checking.
// Because diffrent subsystem are writing different headers it is not
// easily possible to have one Reader for all files. Because of this
// you must know to which subsystem the file belongs before you can
// instantiate your reader if you need the header or want to check the
// header.
//
// If you want to restrict reading to 'single run report files' you can
// call SetRunNumber(12345). In this case Checking the Header will fail
// if no run information is available or the runnumber in the header
// doesn't match. To request a 'all run' file use SetRunNumber(0).
// To allow both file types to be read use SetRunNumber(-1) <default>
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportFileReadCC.h"

#include <fstream>
#include <stdlib.h> // atoi on gcc 2.95.3

#include <TRegexp.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MReportFileReadCC);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. It tries to open the given file and creates a
// THashTable which allows faster access to the MReport* objects.
//
MReportFileReadCC::MReportFileReadCC(const char *fname, const char *name, const char *title)
    : MReportFileRead(fname, name, title), fTelescope(-1), fRunNumber(-1), fFileNumber(-1)
{
    fName  = name  ? name  : "MReportFileReadCC";
    fTitle = title ? title : "Read task to read Central Control report files";
}

Int_t MReportFileReadCC::GetRunNumber(const TString &str) const
{
    Int_t run = -1;

    // whole night report file
    if (str==TString("[CC Report File]"))
        run = 0;

    // report file matching a single run
    if (!str(TRegexp("^[CC Run [0-9]+ Control File]$")).IsNull())
        run = atoi(str(TRegexp(" [0-9]+")).Data());

    if (run<0)
    {
        *fLog << err << "ERROR - First line doesn't match '[CC Report File]' ";
        *fLog << "nor '^[CC Run [0-9]+ Control File]$'" << endl;
        return -1;
    }

    if (fRunNumber!=-1 && fRunNumber!=run)
    {
        *fLog << err << "ERROR - Requested run #" << fRunNumber << " doesn't ";
        *fLog << "match, found run #" << run << endl;
        return -1;
    }

    return run;
}

Int_t MReportFileReadCC::GetVersion(const TString &str) const
{
    if (str(TRegexp("^Arehucas Version Number [0-9]+-[0-9]$")).IsNull())
    {
        *fLog << err << "ERROR - Version '^Arehucas Version Number [0-9]+-[0-9]$' ";
        *fLog << "not found in second line." << endl;
        return -1;
    }

    TString num = str(TRegexp("[0-9]+-[0-9]"));
    num.Prepend("20");
    num.ReplaceAll("-", "");

    return atoi(num.Data());
}

Int_t MReportFileReadCC::GetTelescope(const TString &str) const
{
    if (str(TRegexp("^Telescope M[0-9]$")).IsNull())
    {
        *fLog << err << "ERROR - '^Telescope M[0-9]$' not found in third line." << endl;
        return -1;
    }

    const Int_t num = atoi(str.Data()+11);

    if (fTelescope != 1 && fTelescope !=2)
    {
        *fLog << err << "ERROR - Telsope number M" << num << " in third line unknown." << endl;
        return -1;
    }

    if (fTelescope!=-1 && fTelescope!=num)
    {
        *fLog << err << "ERROR - Requested telescope M" << fTelescope << " doesn't ";
        *fLog << "match, found M" << num << endl;
        return -1;
    }

    return num;
}

Int_t MReportFileReadCC::GetFileNumber(const TString &str) const
{
    if (str(TRegexp("^Subrun [0-9]+$")).IsNull())
    {
        *fLog << err << "ERROR - '^Subrun [0-9]+$' not found in fourth line." << endl;
        return -1;
    }

    const Int_t num = atoi(str.Data()+7);

    if (fFileNumber!=-1 && fFileNumber!=num)
    {
        *fLog << err << "ERROR - Requested file number (subrun) " << fFileNumber << " doesn't ";
        *fLog << "match, found " << num << endl;
        return -1;
    }

    return num;
}

// --------------------------------------------------------------------------
//
// Check whether the file header corresponds to a central control file
// header and check for the existance of a correct version number.
// The version number may later be used to be able to read different
// file versions
//
Int_t MReportFileReadCC::CheckFileHeader()
{
    TString str;
    str.ReadLine(*fIn);   // Read to EOF or newline
    if (!*fIn)
    {
        *fLog << "ERROR - Unexpected end of file (file empty)" << endl;
        return -1;
    }

    const Int_t run = GetRunNumber(str);
    if (run<0)
        return -1;

    fRunNumber = run;

    // -----------------------------------------------------------

    str.ReadLine(*fIn);   // Read to EOF or newline
    if (!*fIn)
    {
        *fLog << "ERROR - Unexpected end of file after line 1." << endl;
        return -1;
    }

    const Int_t ver = GetVersion(str);
    if (ver<0)
        return -1;

    *fLog << all << "Report File version: <" << ver << ">" << endl;
    SetVersion(ver);

    // -----------------------------------------------------------

    fTelescope = 1;

    if (ver<200805190)
        return 3;

    str.ReadLine(*fIn);   // Read to EOF or newline
    if (!*fIn)
    {
        *fLog << "ERROR - Unexpected end of file after line 2." << endl;
        return -1;
    }

    const Int_t tel = GetTelescope(str);
    if (tel<0)
        return -1;

    fTelescope = tel;

    // -----------------------------------------------------------

    if (fRunNumber==0)
        return kTRUE;

    str.ReadLine(*fIn);   // Read to EOF or newline
    if (!*fIn)
    {
        *fLog << "ERROR - Unexpected end of file after line 3." << endl;
        return -1;
    }

    const Int_t num = GetFileNumber(str);
    if (num<0)
        return -1;

    fFileNumber = num;

    // -----------------------------------------------------------

    return 4;
}
