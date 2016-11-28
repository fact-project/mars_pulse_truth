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
!   Author(s): Thomas Bretz, 06/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillcondor.C
// ============
//
// This macro is used to read self written condor log-files.
// These files are written to /magic/data/autologs/condor/
//
// Usage:
//   .x fillcondor.C("/magic/datacenter/autologs/condor", "2008-06-28", kTRUE)
//   .x fillcondor.C("/magic/datacenter/autologs/condor/2008/06/28", kTRUE)
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// The macro can also be run without ACLiC but this is a lot slower...
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b fillcondor.C+\(\"2008-06-28\"\,kFALSE\) 2>&1 | tee fillcondor.log
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 2 in case of failure, 1 in case of success and 0 if the connection
// to the database is not working.
//
/////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iomanip>
#include <iostream>

#include <TSystem.h>

#include "MTime.h"
#include "MDirIter.h"
#include "MSQLMagic.h"

Bool_t Process(MSQLMagic &serv, const TString &path)
{
    Int_t rc[3] = {0, 0, 0};

    MDirIter Next(path, "status.*", -1);
    while (1)
    {
        TString fname=Next();
        if (fname.IsNull())
            break;

        ifstream fin(fname);
        if (!fin)
            continue;

        // Remove path and "status."
        TString date = gSystem->BaseName(fname)-11;
        date.ReplaceAll("/status.", " ");
        date.ReplaceAll("/", "-");
        date.Insert(13, ":");
        date += ":00";

        Int_t    num   = 0;
        Double_t load  = 0;
        Double_t cload = 0;
        Int_t    claim = 0;

        MTime tm(date);
        while (1)
        {
            TString str;
            str.ReadLine(fin);
            if (!fin)
                break;

            if (str.IsNull())
                continue;

            if (str.Contains("coma"))
                continue;

            if (!str.BeginsWith("slot") && !str.BeginsWith("wast"))
                continue;

            TObjArray *obj = str.Tokenize(' ');

            const TString ld = ((*obj)[3])->GetName();
            const TString cm = ((*obj)[5])->GetName();

            load += ld.Atof();
            num++;

            if (cm=="Claimed")
            {
                cload += ld.Atof();
                claim++;
            }

            delete obj;
        }

        if (num==0)
            continue;

        const TString vars = Form("fNumTotal=%d, fNumClaimed=%d, fLoadTotal=%.2f, fLoadClaimed=%.2f",
                                  num, claim, load, cload);

        const Int_t res = serv.InsertUpdate("CondorStatus", "fTime", tm.GetSqlDateTime(), vars);
        rc[res+1]++;
        if (res==kFALSE)
            break;
    }

    if (rc[0]==0 && rc[1]==0 && rc[2]==0)
        return kTRUE;

    cout << "Dummies:  " << rc[0] << endl;
    cout << "Failed:   " << rc[1] << endl;
    cout << "Success:  " << rc[2] << endl;
    cout << endl;

    // Non failed -> success
    if (rc[1]==0)
        return kTRUE;

    // At least one failed -> error
    if (rc[1]>0)
        return 2;

    // Must have been dummy mode
    return -1;
}

int fillcondor(TString path="/magic/datacenter/autologs/condor", Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillcondor" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Path: " << path << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    return Process(serv, path);
}

int fillcondor(TString path, TString date, Bool_t dummy=kTRUE)
{
    if (!path.EndsWith("/"))
        path += '/';

    date.ReplaceAll("-", "/");
    path += date;

    return fillcondor(path, dummy);
}

