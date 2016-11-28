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
!   Author(s): Daniela Dorner, 05/2005 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// resetcolumn.C
// =============
//
// This macro is used to reset values in the DB.
//
// If a step in the automatic analysis chain has to be repeated, the value
// for this step in the DB has to be reset. This can be done with this macro.
//
// As arguments you have to give the column, the table and from which
// to which Date/Sequence/Run/Dataset you want to reset the value.
//
// The last argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests (i.e. if you want to see if the query is correct).
//
// Usage:
//   .x resetcolumn.C("fStar","SequenceProcessStatus","45114","49329",kTRUE)
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 2 in case of failure, 1 in case of success and 0 if the connection
// to the database is not working.
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <fstream>

#include <TEnv.h>
#include <TObjString.h>
#include <TList.h>

#include <MSQLServer.h>
#include <TSQLRow.h>
#include <TSQLResult.h>

using namespace std;


int resetcolumn(TString column, TString table, TString from, TString to, Bool_t dummy=kTRUE)
{
    cout << "This macro has not yet been adapted to the new data structure (Magic II)." << endl;
    return 0;

    TEnv env("sql.rc");

    MSQLServer serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }
    cout << "resetcolumn" << endl;
    cout << "-----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    TEnv rc("steps.rc");
    //get steps which are influenced by the step, which is resetted
    TString influences  = rc.GetValue(table+"."+column+".Influences", "");

    cout << "resetting column " << column << flush;
    cout << " and the columns it influences(" << influences << ") from " << flush;
    cout << from << " to " << to << "..." << endl;

    TList l;
    while (!influences.IsNull())
    {
        influences = influences.Strip(TString::kBoth);

        Int_t idx = influences.First(' ');
        if (idx<0)
            idx = influences.Length();

        TString influence = influences(0, idx);
        influences.Remove(0, idx);
        l.Add(new TObjString(influence));
    }

    //build query and reset columns
    TString query(Form("Update %s SET %s=NULL ", table.Data(), column.Data()));
    query+=", fStartTime=NULL, fFailedTime=NULL, fReturnCode=NULL, fProgramId=NULL ";

    TIter Next(&l);
    TObject *o=0;
    while ((o=Next()))
        query+=Form(", %s=NULL ", o->GetName());

    query+=Form(" WHERE %s between '%s' and '%s'", rc.GetValue(table+".Primary", ""), from.Data(), to.Data());

    if (dummy)
    {
        cout << "query: " << query << endl;
        return 1;
    }

    TSQLResult *res = serv.Query(query);
    if (!res)
        return 2;

    delete res;

    return 1;
}


