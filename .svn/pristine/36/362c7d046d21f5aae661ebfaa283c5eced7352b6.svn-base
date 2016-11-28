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
!   Author(s): Thomas Bretz, 04/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Dorner, 04/2005 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillsinope.C
// ============
//
// This macro is used to read the sinope output files sinope*.txt
// For each run sinope is run twice: once for the data events and once for the
// calibration events. The pulse position and height is checked. The output is
// stored in a sinope*.root and a sinope*.txt file. The files for data events
// are marked with -dat and the ones for calibration events with -cal.
// From the txt files the result is extracted and inserted into the database
// in the table DataCheck, which stores the datacheck results.
// As the column names in the database differ only by the addition 'Interlaced'
// which is given to columns corresponding to the calibration events, the
// files can be processed with the same function.
//
// Usage:
//   .x fillsinope.C(runnumber, "datapath", kTRUE)
//
// The first argument is the runnumber (given as int), the second argument is
// the datapath, where the rawfiles can be found. The last argument is the
// 'dummy-mode'. If it is kTRUE dummy-mode is switched on and nothing will be
// written into the database. This is usefull for tests.
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b fillsinope.C+\(runno\,kFALSE\) 2>&1 | tee fillsinope.log
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 2 in case of failure, 1 in case of success and 0 if the connection
// to the database is not working.
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include <TEnv.h>
#include <TRegexp.h>

#include <TFile.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

#include "MSQLServer.h"

#include "MStatusArray.h"
#include "MHCamera.h"

using namespace std;

Bool_t ExistStr(MSQLServer &serv, const char *column, const char *table, const char *test)
{
    TString query(Form("SELECT %s FROM %s WHERE %s='%s'", column, table, column, test));
    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return kFALSE;
    }

    Bool_t rc = kFALSE;

    TSQLRow *row=res->Next();
    if (row && (*row)[0])
        rc=kTRUE;

    delete res;
    return rc;
}

int Process(MSQLServer &serv, TString fname, Int_t runno, Bool_t cal, Bool_t dummy)
{
    TEnv env(fname);

    //build query
    TString query="UPDATE DataCheck SET ";

    //array with part of column names
    TString values[9] = { "Events" , "HasSignal" , "HasPedestal" , "PositionSignal" , "PositionFWHM" , "PositionAsym" , "HeightSignal" , "HeightFWHM" , "HeightAsym" };

    //get values from the file add them to query
    TString str;
    for (Int_t i=0 ; i<9 ; i++)
    {
        str = env.GetValue(values[i], "");
        if (str.IsNull())
            continue;

        if (cal)
            values[i]+="InterLaced";
        values[i]+="='";
        values[i]+=str;

        if (i!=0)
            query+=", ";
        query+=" f";
        query+=values[i];
        query+="' ";

        cout << "value: " << values[i] << endl;

    }

    query+=Form(" WHERE fRunNumber=%d", runno);

    cout << "Q: " << query << endl;
    //insert information into db
    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return 2;
    }
    delete res;
    return 1;
}

int fillsinope(Int_t runno, TString datapath, Bool_t dummy=kTRUE)
{
    TEnv env("sql.rc");

    MSQLServer serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillsignal" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Run: " << runno << endl;
    cout << endl;

    //get date of run from database
    TString query(Form("SELECT DATE_FORMAT(ADDDATE(fRunStart, Interval 13 HOUR), '%%Y/%%m/%%d') FROM RunData WHERE fRunNumber=%d",
                       runno));

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return 2;
    }

    TSQLRow *row = 0;
    row = res->Next();
    TString date=(*row)[0];
    cout << "date: " << date << endl;
    delete res;

    //insert entry for the run into the database in the table DataCheck, if it is not yet existing
    if (!ExistStr(serv, "fRunNumber", "DataCheck", Form("%d", runno)))
    {
        query=Form("INSERT DataCheck SET fRunNumber=%d", runno);

        res = serv.Query(query);
        if (!res)
        {
            cout << "ERROR - Query failed: " << query << endl;
            return 2;
        }
    }

    //get filenames of sinope output files
    TString fname(Form("%s/sinope/%s/sinope-dat%08d.txt",
                       datapath.Data(), date.Data(), runno));
    cout << "file: " << fname << endl;
    TString fnamecal(Form("%s/sinope/%s/sinope-cal%08d.txt",
                        datapath.Data(), date.Data(), runno));
    cout << "file-cal: " << fnamecal << endl;

    Int_t rc=0;
    //process dat-file
    rc=Process(serv, fname, runno, kFALSE, dummy);
    if (rc==2)
        return rc;

    //process cal-file
    rc=Process(serv, fnamecal, runno, kTRUE, dummy);
    return rc;
}
