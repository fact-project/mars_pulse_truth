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
// findcacofiles.C
// ===============
//
// Macro to find caco files for runs which don't have a dedicated caco file.
// Called by the script filesondisk
//
// Sometimes the DAQ aborts a run and starts itself a new one. In this cases
// the camera controll doesn't start a new file, as the command to
// start a new run was not sent by the central control. So the caco
// information is stored in the previous caco file, which has a different
// runnumber. To be able to merpp the information into the calibrated data
// file, the runnumber of the file containing the information has to be found.
// 
// findcacofiles.C searches in the database for runs which don't have a
// dedicated caco file, and searches for each of these runs, if one of the 10
// previous runs has a dedicated caco file. In case one is found, it is
// inserted into the database.
//
// Usage:
//   .x findcacofiles.C+
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

#include "MSQLMagic.h"

using namespace std;


int findcacofiles()
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }
    cout << "findcacofiles" << endl;
    cout << "-------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    //get runnumbers and dates from database
    TString query="SELECT RunProcessStatus.fRunNumber, ";
    query+=" DATE_FORMAT(ADDDATE(if(fRunStart='0000-00-00 00:00:00', fRunStop, fRunStart), INTERVAL +13 HOUR), '%Y/%m/%d') ";
    query+=" FROM RunProcessStatus ";
    query+=" LEFT JOIN RunData USING (fRunNumber) ";
    query+=" LEFT JOIN Source USING (fSourceKEY) ";
    query+=" WHERE IsNull(fCaCoFileFound) AND fExcludedFDAKEY=1 ";
    query+=" AND RunProcessStatus.fRunNumber > 10000 AND NOT IsNull(fCCFileAvail)";
    query+=" AND fTest='no'";

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "Error." << endl;
        return 0;
    }

    Int_t counter=0;
    Int_t counter2=0;
    TSQLRow *row=0;
    while ((row = res->Next()))
    {
        //search nearest previous available CaCoFile
        Int_t run=atoi((*row)[0]);
        if (TString((*row)[1]).IsNull())
        {
            cout << "For run " << (*row)[0] << " fRunStart and fRunStop are 0000-00-00 00:00:00. No CaCoFile can be determined. " << endl;
            continue;
        }
        cout << "CaCoFile missing for run " << (*row)[0] << " with date " << (*row)[1] << endl;

        query ="SELECT MAX(fCaCoFileFound) FROM RunProcessStatus ";
        query+=" LEFT JOIN RunData USING (fRunNumber) ";
        query+=Form("WHERE DATE_FORMAT(ADDDATE(fRunStart, INTERVAL +13 HOUR), '%%Y/%%m/%%d')='%s' ",
                    (*row)[1]);
        query+=Form("AND RunData.fRunNumber IN (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
                    run, run-1, run-2, run-3, run-4, run-5,
                    run-6, run-7, run-8, run-9, run-10);

        TSQLResult *res2 = serv.Query(query);
        if (!res2)
        {
            cout << "Error." << endl;
            return 0;
        }
        TSQLRow *row2=0;
        row2 = res2->Next();
        if ((*row2)[0])
        {
            cout << "Found CaCoFile at run " << (*row2)[0] << endl;
            TString vals=Form("fCaCoFileAvail=Now(), fCaCoFileFound=%s", (*row2)[0]);
            TString where=Form("fRunNumber=%d", run);

            //insert found runnumber
            if (!serv.Update("RunProcessStatus", vals,where))
                return 0;
            counter2+=1;
        }
        else
            cout << " No caco file found for run " << run << endl;

        delete res2;
        counter+=1;
    }
    cout << endl << counter << " missing caco files. " << endl;
    cout << counter2 << " caco files found and inserted. " << endl;

    delete res;
    return 1;
}


