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
!   Author(s): Daniela Dorner, 08/2004 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// doexclusions.C
// ==============
//
// this macro sets the ExcludedFDA flag for runs, that can be excluded
// automatically
// the information which runs have to be excluded is retrieved from the
// resource file automatic-exclusions.rc
//
// the macro can be executed either for a night or for a range of runnumbers
// or for all nights
// .x doexclusions.C+("night", telnum)
// .x doexclusions.C+(startrun,stoprun, telnum)
// .x doexclusions.C+
//
// resource file: automatic-exclustions.rc
// for each exclusion-reason there is one block of 6 lines in this file:
// example for one exclusion-reason:
//  #NumEventsSmaller10:                   (name of exclusion)
//  key15.Column: fNumEvents               (name of the affected column)
//  #key15.Join1:                          (name of column that has to 
//  #key15.Join2:                                             be joined)
//  key15.Cond: fNumEvents between 2 and 9 (condition that fulfils exclusion)
//  #key15.SpecialRunCond:                 (special condition, if exclusion
//                                          is needed only for certain runs)
// if a value is not needed for an exclusion (like the joins and the special
// condition in this example), the line is commented out
//
// Returns 2 in case of failure, 1 in case of success and 0 if the connection
// to the database is not working.
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>

#include <TEnv.h>
#include <TSystem.h>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MSQLMagic.h"

using namespace std;

//get minimum or maximum runnumber of the runs of a night
int GetRunNumber(MSQLServer &serv, Int_t tel, TString date, TString cmd)
{
    TString query;
    query  = Form("SELECT %s(fRunNumber) FROM RunData ", cmd.Data());
    query += Form("WHERE fTelescopeNumber=%d", tel);

    if (date!="NULL")
    {
        TString day=date+" 13:00:00";
        query += Form(" AND (fRunStart>ADDDATE(\"%s\", INTERVAL -1 DAY) AND fRunStart<\"%s\")",
                      day.Data(), day.Data());
    }

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Could not get " << cmd << " fRunNumber." << endl;
        return -1;
    }

    TSQLRow *row =res->Next();
    if (TString((*row)[0]).IsNull())
    {
        cout << "No run available for " << date << endl;
        delete res;
        return 0;
    }
    delete res;

    return atoi((*row)[0]);
}

int doexclusions(Int_t startrun, Int_t stoprun, Int_t tel=1, TString date="NULL", Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    // Open rc files with exclusions
    TEnv rc("resources/exclusions.rc");

    // Some information for the user
    cout << "doexclusions" << endl;
    cout << "------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Date:      " << date << endl;
    cout << "Telescope: " << tel << endl;

    serv.SetIsDummy(dummy);

    // if neither start- nor stoprun is given, the minimum and maximum
    // runnumber is queried from the database to do the exclusions for
    // all runs if a night is given for all runs of this night
    if (startrun==0 && stoprun==0)
    {
        startrun = GetRunNumber(serv, tel, date, "MIN");
        stoprun  = GetRunNumber(serv, tel, date, "MAX");
    }

    cout << "Start Run: " << startrun << endl;
    cout << "Stop  Run: " << stoprun  << endl;

    //check format of start- and stoprun
    if (startrun<0 || stoprun<0)
    {
        cout << "ERROR - Startrun<0 or stoprun<0." << endl;
        return 2;
    }

    //if no run for date is available, GetRunNumber() returns 0
    if (startrun==0 || stoprun==0)
        return 1;

    // Get exclusions-reasons (stored in the table ExcludedFDA) from the DB
    TSQLResult *res = serv.Query("SELECT fExcludedFDAKEY, fExcludedFDAName "
                                 "FROM ExcludedFDA "
                                 "ORDER BY fExcludedFDAImportance ASC");
    if (!res)
        return 2;

    //update the exclusion-reasons for all runs
    TSQLRow *row=0;
    while ((row = res->Next()))
    {
        // check the key (NULL means "No exclusion")
        const TString key = (*row)[0];
        if (key.IsNull())
            continue;

        // Get the corresponding condition from the file
        const TString cond = rc.GetValue("key"+key, "");
        if (cond.IsNull())
            continue;

        // Get all files to be excluded
        TString query2 = "SELECT fRunNumber, fFileNumber FROM RunData ";
        query2 += serv.GetJoins("RunData", query2+cond);

        query2 += Form("WHERE (%s) ", cond.Data());
        query2 += Form("AND fRunNumber BETWEEN %d AND %d ", startrun, stoprun);
        query2 += Form("AND fTelescopeNumber=%d", tel);

        TSQLResult *res2 = serv.Query(query2);
        if (!res2)
            return 2;

        // Update exlcusion for file
        TSQLRow *row2=0;
        while ((row2 = res2->Next()))
        {
            TString vars(Form("fExcludedFDAKEY=%s", key.Data()));
            TString where(Form("fRunNumber=%s AND fFileNumber=%s AND fTelescopeNumber=%d", (*row2)[0], (*row2)[1], tel));

            if (serv.Update("RunData", vars, where)==kFALSE)
                return 2;

            cout << "File M" << tel << ":" << (*row2)[0] << "/" << setw(3) << setfill('0') << (*row2)[1] << " excluded due to: " << (*row)[1] << "." << endl;
        }

        delete res2;
    }
    delete res;

    return 1;
}

//run doexclusions for one night
int doexclusions(TString date="NULL", Int_t tel=1, Bool_t dummy=kTRUE)
{
    return doexclusions(0, 0, tel, date, dummy);
}
