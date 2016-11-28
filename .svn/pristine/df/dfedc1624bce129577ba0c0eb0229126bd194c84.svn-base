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
!   Author(s): Thomas Bretz, 08/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// filldotrbk.C
// ============
//
// This macro is used to read the central control runbook files from
// the data center and store their contents in the runbook-database.
//
// Usage:
//   .x filldotrbk.C("/magic/subsystemdata/cc/", kTRUE)
//
// The first argument is the directory in which all subdirectories are
// searched for CC_*.rbk files. All these files are analysed and the runbook
// entries will be put into the DB, eg:
//  "/magic/subsystemdata/cc"                  would do it for all data
//  "/magic/subsystemdata/cc/2005"                  for one year
//  "/magic/subsystemdata/cc/2005/11"               for one month
//  "/magic/subsystemdata/cc/2005/11/11"            for a single day
//  "/magic/subsystemdata/cc/2005/11/11/file.rbk"   for a single file
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// Before an entry is added, its existance is checked... if it is already
// in the database, it is ignored.
//
// The macro can also be run without ACLiC but this is a lot slower...
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b filldotrbk.C+\(\"path\"\,kFALSE\) 2>&1 | tee filldotrbk.log
//
// Make sure, that database and password are corretly set in the macro.
//
// Returns 0 in case of failure and 1 in case of success.
//
///////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TRegexp.h>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MDirIter.h"
#include "MSQLServer.h"

using namespace std;

// --------------------------------------------------------------------------
//
// Checks whether an entry for this date is already existing
//
Bool_t ExistStr(MSQLServer &serv, const char *column, const char *table, const char *test)
{
    TString query(Form("SELECT %s FROM %s WHERE %s='%s'", column, table, column, test));
    TSQLResult *res = serv.Query(query);
    if (!res)
        return kFALSE;

    TSQLRow *row;

    Bool_t rc = kFALSE;
    while ((row=res->Next()))
    {
        if ((*row)[0])
        {
            rc = kTRUE;
            break;
        }
    }
    delete res;
    return rc;
}

int insert(MSQLServer &serv, Bool_t dummy, TString entry, TString date)
{

    //check if entry is already in database
    if (ExistStr(serv, "fRunBookDate", "RunBook", date))
        return 0;

    entry.ReplaceAll("'", "\\'");
    entry.ReplaceAll("\"", "\\\"");

    // This is a sanity check for \0-bytes in .rbk-files
    for (int i=0; i<entry.Length(); i++)
        if ((int)entry[i]==0)
            entry.Remove(i--);

    //insert entry into the database
    TString query("INSERT RunBook (fRunBookDate, fRunBookText) VALUES (\"");
    query += date;
    query += "\", \"";
    query += entry;
    query += "\");";

    if (dummy)
        return 1;

    TSQLResult *res = serv.Query(query);
    if (!res)
        return 0;

    delete res;

    return 1;
}

// --------------------------------------------------------------------------
//
// insert the entries from this runbook file into the database
//
int process(MSQLServer &serv, Bool_t dummy, TString fname)
{
    ifstream fin(fname);
    if (!fin)
    {
        cout << "Could not open file " << fname << endl;
        return 0;
    }

    TRegexp regexp("^.20[0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9].$", kFALSE);

    Int_t num=0;

    TString entry="";
    TString date="";
    while (1)
    {
        TString line;
        line.ReadLine(fin);
        if (!fin)
        {
            num += insert(serv, dummy, entry, date);
            break;
        }

        TString l0 = line(regexp);

        if (l0.IsNull() || entry.IsNull())
        {
            entry += line;
            entry += "\n";
            continue;
        }
/*
        if (entry.Contains("Operator names: "))
        {
            cout << "OPERATORS: " << entry << flush;
            entry="";
        }
*/

        //skip run statistics from old runbooks
        if (entry.Contains("MAGIC ELECTRONIC RUNBOOK")   ||
            entry.Contains("DATA RUN STATISTICS")        ||
            entry.Contains("CALIBRATION RUN STATISTICS") ||
            entry.Contains("PEDESTAL RUN STATISTICS"))
            entry ="";

        if (!entry.IsNull() && !date.IsNull())
            num += insert(serv, dummy, entry, date);

        date=l0(1, l0.Length()-2);
        entry="";
    }

    cout << fname(TRegexp("CC_.*.rbk", kFALSE)) << " <" << num << ">";
    cout << (dummy?" DUMMY":"") << endl;

    return 1;
}

// --------------------------------------------------------------------------
//
// loop over all files in this path
//
int filldotrbk(TString path="/data/MAGIC/Period017/ccdata", Bool_t dummy=kTRUE)
{
    MSQLServer serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << endl;
    cout << "filldotrbk" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Search Path: " << path << endl;
    cout << endl;

    //get all runbook files in path
    if (path.EndsWith(".rbk"))
        return process(serv, dummy, path);

    //fill entries for each runbook file
    MDirIter Next(path, "CC_*.rbk", -1);
    while (1)
    {
        TString name = Next();
        if (name.IsNull())
            break;

        if (!process(serv, dummy, name))
            return 0;
    }

    return 1;
}
