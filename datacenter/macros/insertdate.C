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
!   Author(s): Daniela Dorner, 01/2005 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// insertdate.C
// ============
//
// This macro is inserting a date into the table SequenceBuildStatus.
// It is executed by the script copyscript, which copies the slow control
// data to the appropriate directory and inserts the information into the
// database.
//
// Usage:
//  .x insertdate.C+("date")
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 0 in case of failure and 1 in case of success.
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include <TEnv.h>

#include <MSQLMagic.h>

using namespace std;

int insertdate(TString date)
{
    TEnv env("sql.rc");

    MSQLMagic serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }
    cout << "insertdate" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    //insert entry for date into the table SequenceBuildStatus,
    // if entry is not yet existing
    if (!serv.ExistStr("fDate", "SequenceBuildStatus", date))
    {
        TString vals(Form("fDate='%s', fCCFilled=Now()", date.Data()));

        const Int_t rc = serv.Insert("SequenceBuildStatus", vals);
        if (rc==kFALSE) // Query failed
            return -1;
        //if (rc<0)       // Dummy mode
        //    return 0;
    }
    else
    {
        cout << date << " already exists... do update. " << endl;

        const Int_t rc = serv.Update("SequenceBuildStatus",
                                     "fCCFilled=Now(), fExclusionsDone=NULL, fSequenceEntriesBuilt=NULL",
                                     Form("fDate='%s'", date.Data()));

        if (rc==kFALSE) // Query failed
            return -1;
        //if (rc<0)       // Dummy mode
        //    return 0;

    }

    return 1;
}


