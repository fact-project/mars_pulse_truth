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
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// checkfileavail.C
// ================
//
// check the availability of the files of one sequence:
// the script filesondisk has inserted the information (which files are on
// disk into the database) and with this macro this information is checked
// for the runs of one sequence
//
// executing the macro:
//  .x checkfileavail.C+(100002)
// the sequencefile (including path) has to be given, as the macro retrieves
// from there the runnumbers
//
// the macro returns 0, if there's no connection to the database, 2 if a
// file is missing, and 1 if all files are there, 3 in case of error.
// the return value is checked by the script, that executes the macro
//
// this macro is very similar to the macro checkstardone.C
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MSQLServer.h"

using namespace std;

int checkfileavail(Int_t num, Int_t tel)
{
    MSQLServer serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "checkfileavail" << endl;
    cout << "--------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;
    cout << "Sequence: " << num << endl;
    cout << "Telecope: " << tel << endl;
    cout << endl;

    // -------------------------------------------

    TString query = "SELECT MIN( NOT ("
        "ISNULL(fRawFileAvail) OR "
        "ISNULL(fCCFileAvail) OR "
       // "ISNULL(fCaCoFileAvail) OR "
       // "ISNULL(fCaCoFileFound) OR "
        "ISNULL(fTimingCorrection) OR "
        "ISNULL(fCompmux) "
        ")) "
        "FROM RunProcessStatus "
        "LEFT JOIN RunData USING (fTelescopeNumber,fRunNumber,fFileNumber) ";
    query += Form("WHERE fSequenceFirst=%d AND fTelescopeNumber=%d", num, tel);

    TSQLResult *res = serv.Query(query);
    if (!res)
        return 3;

    if (res->GetRowCount()!=1)
    {
        cout << "ERROR - Unexpected number of returned rows (" << res->GetRowCount() << ")" << endl;
        cout << query << endl;
        delete res;
        return 3;
    }

    TSQLRow *row = res->Next();

    if (!row || !(*row)[0])
    {
        cout << "ERROR - Unexpected result." << endl;
        cout << query << endl;
        delete res;
        return 3;
    }

    const Int_t rc = atoi((*row)[0]);

    delete res;

    return rc==1 ? 1 : 2;
}
