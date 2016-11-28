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
!   Author(s): Thomas Bretz, 01/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillcmt.C
// =========
//
// This macro is used to read an extinction file from the CMT and fill
// its contents into the db.
//
// Usage:
//   .x fillcmt.C("camext.06", kTRUE)
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// Filling the database is done with 'UPADTE' for _all_ columns
// matching the date!
//
// The macro can also be run without ACLiC but this is a lot slower...
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b fillcmt.C+\(\"filename\"\,kFALSE\) 2>&1 | tee fillcmt.log
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 2 in case of failure, 1 in case of success and 0 if the connection
// to the database is not working.
//
/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <errno.h>

#include <TEnv.h>
#include <TSystem.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MTime.h"
#include "MSQLMagic.h"

using namespace std;

int fillcmt(TString fname, Bool_t dummy=kTRUE)
{
    TEnv env("sql.rc");

    MSQLMagic serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    serv.SetIsDummy(dummy);

    gSystem->ExpandPathName(fname);

    cout << "fillcmt" << endl;
    cout << "-------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;


    ifstream fin(fname);
    if (!fin)
    {
        gLog << err << "Cannot open file " << fname << ": ";
        gLog << strerror(errno) << endl;
        return 2;
    }

    TString line;
    for (int i=0; i<23; i++)
        line.ReadLine(fin);

    if (!fin)
    {
        gLog << err << "File " << fname << ": Not enough lines." << endl;
        return 2;
    }

    while (1)
    {
        line.ReadLine(fin);
        if (!fin)
            break;

        const_cast<char&>(line.Data()[ 6]) = 0;
        const_cast<char&>(line.Data()[10]) = 0;
        const_cast<char&>(line.Data()[19]) = 0;
        const_cast<char&>(line.Data()[35]) = 0;
        const_cast<char&>(line.Data()[39]) = 0;
        const_cast<char&>(line.Data()[45]) = 0;
        const_cast<char&>(line.Data()[51]) = 0;

        const Int_t checked = atoi(line.Data()+7);
        if (checked==0)
            continue;

        const TString date    = line.Data();
        const Float_t extr    = atof(line.Data()+11);     //5.3
        const Int_t   phot    = line.Data()[18] == ':';
        const Float_t extrerr = atof(line.Data()+20);     //5.3
        const Float_t tmphot  = atof(line.Data()+40);     //5.2
        const Float_t tmnphot = atof(line.Data()+46);     //5.2

        MTime t;
        t.SetStringFmt(date, "%y%m%d");

        const TString vars = Form("fExtinctionR=%.3f, "
                                  "fExtinctionRerr=%.3f, "
                                  "fIsPhotometric=%d, "
                                  "fTimePhotometric=%.2f, "
                                  "fTimeNonPhotometric=%.2f",
                                  extr, extrerr, phot, tmphot, tmnphot);


        const TString where = Form("fData='%s'", t.GetSqlDateTime().Data());

        if (!serv.InsertUpdate("ExtinctionCMT", vars, where))
            return 2;
    }

    return 1;
}
