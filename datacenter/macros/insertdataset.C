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
// insertdataset.C
// ===============
//
// This macro inserts datasets into the database.
// If a new dataset file has been stored in the dataset directory, the
// information file is read by a script and the information is inserted with
// this macro into the database in the tables DataSets and
// DataSetProcessStatus, where in the information about datasets is stored.
//
// Usage:
//   .x insertdataset.C+("number","source,"wobble","comment",kTRUE)
// The first argument is the dataset number, the second is giving the source
// name, the third the observation mode (wobble/on-off), the fourth a comment
// about the dataset. This arguments are extracted by a script from the
// dataset file.
// The last argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 0 in case of failure and 1 in case of success.
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "MSQLMagic.h"

using namespace std;

int insertdataset(TString number, TString source, TString wobble, TString comment, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    serv.SetIsDummy(dummy);

    cout << "insertdataset" << endl;
    cout << "-------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    //get source key
    Int_t sourcekey = serv.QueryKeyOfName("Source", source.Data(), kFALSE);
    if (sourcekey<0)
    {
        cout << "Error - could not get sourcename from DB -> " << flush;
        cout << "maybe you have the wrong sourcename in your datasetfile" << endl;
        return 2;
    }

    cout << "no:" << number << endl;

    //if dataset is not yet in database, insert the information
    if (serv.ExistStr("fDataSetNumber", "DataSets", number.Data())) // Form("%d", number)
    {
        cout << number << " already exists... " << endl;
        return 3;
    }

    TString vals = Form("fDataSetNumber='%s', fSourceKEY=%d, "
                        "fWobble='%s', fComment='%s' ", number.Data(),
                        sourcekey, wobble.Data(), comment.Data());

    Int_t rc = serv.Insert("DataSets", vals);

    if (rc<=0) // dummy mode or failed
        return 0;

    vals = Form("fDataSetNumber='%s', fPriority='%s', fDataSetInserted=Now()",
                number.Data(), number.Data());
    rc = serv.Insert("DataSetProcessStatus", vals);

    if (rc<=0) // dummy mode or failed
        return 0;

    return 1;
}


