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
// resetallruns.C
// ==============
//
// This macro resets the values of a column in the table RunProcessStatus. It
// is used by the script filesondisk to reset the values for the columns,
// which indicate the availability of the files.
//
// Usage:
//   .x resetallrun.C+("filename", "column")
// The first argument is giving a file, in which a list of runs can be found
// for which the column will be set to Now() by this macro. The second
// argument is giving the column, which has to be reset.
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 0 in case of failure and 1 in case of success.
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TPRegexp.h>

#include "MSQLMagic.h"

using namespace std;

int resetallruns(TString filename, TString column, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "resetallruns" << endl;
    cout << "------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;
    cout << "File:   " << filename << endl;
    cout << "Column: " << column << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    //reset column to NULL, if the column is fCaCoFileAvail,
    //reset also the column fCaCoFileFound
    TString vars(column);
    vars += "=NULL";
    if (column.Contains("CaCo"))
        vars += ", fCaCoFileFound=NULL";

    if (serv.Update("RunProcessStatus", vars)==kFALSE)
        return 2;

    //read in file
    ifstream fin(filename);
    if (!fin)
    {
        cout << "ERROR - File " << filename << " missing!" << endl;
        return 2;
    }

    TPRegexp regtel("^_M[0-9]_");
    TPRegexp regrun("_[0-9]{5,8}");
    TPRegexp regfile("[.][0-9]{3,5}_$");
    while (1)
    {
        //get runnumber
        TString str;
        str.ReadLine(fin);
        if (!fin)
            break;

        const TString tel  = str(regtel);
        const TString run  = str(regrun);
        const TString file = str(regfile);

        const UInt_t ntel  = tel.IsNull()  ? 1 : atoi(tel.Data()+2);
        const UInt_t nfile = file.IsNull() ? 0 : atoi(file.Data()+1);
        const UInt_t nrun  = atoi(run.Data()+1);

        if (nrun>999999 && (tel.IsNull() || file.IsNull()))
        {
            cout << "ERROR - Run number " << nrun << ">999999 and telescope and/or run-number missing!" << endl;
            cout << str << endl;
            continue;
        }

        const UInt_t id = nrun*1000+nfile;

        //build query and set status for this run to Now()
        TString vars2(column);
        vars2 += "=Now()";
        if (column.Contains("CaCo"))
            vars2 += Form(", fCaCoFileFound=%d", id);

        TString where = Form("fTelescopeNumber=%d AND fRunNumber=%d AND fFileNumber=%d ",
                             ntel, nrun, nfile);

        if (serv.Update("RunProcessStatus", vars2, where)==kFALSE)
            return 2;
    }

    return 1;
}
