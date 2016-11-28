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
// insertcacofiles.C
// =================
//
// Macro to insert runnumber of the caco files, which contain the information
// for a run, for which no caco file is available.
//
// Sometimes the DAQ aborts a run and starts itself a new one. In this cases
// the camera controll doesn't start a new file, as the command to
// start a new run was not sent by the central control. So the caco
// information is stored in the previous caco file, which has a different
// runnumber. To be able to merpp the information into the calibrated data
// file, the runnumber of the file containing the information has to be found.
// This is done by a script.
// insertcacofiles.C inserts the runnumber of the file, that has been found,
// into the column CaCoFileFound. 
//
// Usage:
//   .x insertcacofile.C+("runnumber", "newrunnumber")
// The first is the runnumber of which the cacofile is missing, the second is
// the runnumber of the file in which the information probably can be found.
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

#include <TSystem.h>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MSQLServer.h"

using namespace std;

int insertcacofile(TString runnumber, TString newrunnumber)
{
    cout << "ERROR - TelescopeNumber and FileNumber not implemeted." << endl;
    return 2;
/*
    MSQLServer serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }
    cout << "insertcacofile" << endl;
    cout << "--------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    //get new runnumber
    Int_t newrunno=atoi(newrunnumber.Data());
    //build query and insert information
    TString query(Form("UPDATE RunProcessStatus SET fCaCoFileAvail=Now(), fCaCoFileFound=%d WHERE fRunNumber=%s ",
                       newrunno, runnumber.Data()));
    cout << "qu: " << query << endl;

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "Error - update didn't work." << endl;
        return 0;
    }

    delete res;
    return 1;
    */
}
