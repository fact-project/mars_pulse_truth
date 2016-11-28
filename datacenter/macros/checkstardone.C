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
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// checkstardone.C
// ===============
//
// check the availability of all star files for one dataset:
// if star has been executed successfully, this information has been inserted
// into the database (SequenceProcessStatus) with this macro this information
// is checked 
//
// executing the macro:
//  .x checkstardone.C+(1)
//
// The macro returns 0, if there's no connection to the database, 2 if
// star is not done, and 1 if star done, 3 in case of error.
// the return value is checked by the script, that executes the macro
//
// This macro is very similar to the macro checkfileavail.C
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MSQLServer.h"

using namespace std;

int checkstardone(Int_t num)
{
    MSQLServer serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "checkstardone" << endl;
    cout << "-------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;
    cout << "Dataset: " << num << endl;
    cout << endl;

    // -------------------------------------------

    TString query = "SELECT MIN(NOT ISNULL(fStar)) FROM SequenceProcessStatus "
        "LEFT JOIN DataSetSequenceMapping USING (fTelescopeNumber,fSequenceFirst) ";
    query += Form("WHERE fDataSetNumber=%d", num);

    TSQLResult *res = serv.Query(query);
    if (!res)
        return 3;

    if (res->GetRowCount()!=1)
    {
        cout << "ERROR - Unexpected number of returned rows (" << res->GetRowCount() << ")" << endl;
        delete res;
        return 3;
    }

    TSQLRow *row = res->Next();

    if (!row || !(*row)[0])
    {
        cout << "ERROR - Unexpected result." << endl;
        delete res;
        return 3;
    }

    const Int_t rc = atoi((*row)[0]);

    delete res;

    return rc==1 ? 1 : 2;
}
