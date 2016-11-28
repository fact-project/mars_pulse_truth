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
!   Author(s): Thomas Bretz, 7/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MSequenceSQL
//
// This is an extension to retrieve a sequence from a database instead
// of readin it from a file
//
// Example 1:
//    MSQLMagic serv("sql.rc");
//    MSequenceSQL s(serv, 100002))
//    if (s.IsValid())
//        s.Print();
//
// Example 2:
//    MSequenceSQL s("sql.rc", 100002);
//    if (s.IsValid())
//        s.Print();
//
// Example 2:
//    MSequence s = MSequenceSQL("sql.rc", 100002);
//    if (s.IsValid())
//        s.Print();
//
// Instead of initializing the constructor directly you can
// also use the function GetFromDatabase.
//
// Instead of a resource file sql.rc you can also give the qualified
// path to the database:
//
//    mysql://user:password@url/database
//
//
// A telscope number of -1 means: Take fTelescope as telescope number
// to request. A telescope number of 0 tries to get the sequence from
// the db including the telescope number. This will fail if more than
// one sequence with the same number exist in the db for several
// telescopes.
//
//
// This tool will work from Period017 (2004_05_17) on...
//
/////////////////////////////////////////////////////////////////////////////
#include "MSequenceSQL.h"

#include <stdlib.h> // atof (Ubuntu 8.10)

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MLogManip.h"

#include "MSQLMagic.h"

ClassImp(MSequenceSQL);

using namespace std;

// --------------------------------------------------------------------------
//
// get the list of runs according to the query. Add the runs to run-list
// according to the type. Stop adding runs if the given number of events
// is exceeded.
//
// return kFALSE in case of error or if no events are in the files.
//
Bool_t MSequenceSQL::GetRuns(MSQLMagic &serv, TString query, RunType_t type, UInt_t nevts)
{
    TSQLResult *res = serv.Query(query);
    if (!res)
        return kFALSE;

    UInt_t cnt=0;
    UInt_t evts=0;

    TSQLRow *row=0;
    while ((row = res->Next()) && evts<nevts)
    {
        AddFile(atoi((*row)[0]), atoi((*row)[1]), type);
        evts += atoi((*row)[2]);
        cnt++;
    }

    delete res;

    if (cnt==0)
    {
        *fLog << err << "ERROR - No " << type << " belonging to this sequence found." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Get start and stop time as requested and return stop time if start
// time is invalid. Return a null string in case of an error or no valid time.
//
TString MSequenceSQL::GetTimeFromDatabase(MSQLMagic &serv, const TString &query)
{
    TSQLResult *res = serv.Query(query);
    if (!res)
        return "";

    // FIXME: Check for the right number of results?

    TSQLRow *row=res->Next();

    const TString start = (*row)[0] ? (*row)[0] : "";
    const TString stop  = (*row)[1] ? (*row)[1] : "";

    delete res;

    if (!start.IsNull())
        return start;

    *fLog << warn << "WARNING - Requesting start time of first calibration run failed." << endl;

    if (!stop.IsNull())
        return stop;

    *fLog << err << "ERROR - Neither start nor stop time of first calibration could be requested." << endl;
    return "";
}

// --------------------------------------------------------------------------
//
// Convert the result into the MSequence data members
//
Bool_t MSequenceSQL::GetFromDatabase(MSQLMagic &serv, TSQLRow &data)
{
    // Convert the keys from the sequence table to strings
    TString str[6];
    str[0] = serv.QueryNameOfKey("Project",         data[1]);
    str[1] = serv.QueryNameOfKey("Source",          data[2]);
    str[2] = serv.QueryNameOfKey("L1TriggerTable",  data[3]);
    str[3] = serv.QueryNameOfKey("L2TriggerTable",  data[4]);
    str[4] = serv.QueryNameOfKey("HvSettings",      data[5]);
    str[5] = serv.QueryNameOfKey("LightConditions", data[6]);

    // check that all strings are valid
    if (str[0].IsNull() || str[1].IsNull() || str[2].IsNull() || str[3].IsNull() || str[4].IsNull() || str[5].IsNull())
        return kFALSE;

    // Setup the time and night
    MTime time;
    time.SetSqlDateTime(data[7]);
    MTime night = time.GetDateOfSunrise();

    SetNight(night.GetStringFmt("%Y-%m-%d"));

    // set the other values
    fStart         = time;
    fLastRun       = atoi(data[0]);
    fNumEvents     = atoi(data[8]);
    fTelescope     = atoi(data[11]);
    fProject       = str[0];
    fSource        = str[1];
    fTriggerTable  = str[2]+":"+str[3];
    fHvSettings    = str[4];
    fLightCondition = GetLightCondition(str[5]);

    // FIXME: ZdMin (str[9]) ZdMax(str[10]) --> Comment
    // FIXME: fMonteCarlo
    // FIXME: fComment?

    // Now prepare queries to request the runs from the database
    const TString where(Form(" FROM RunData WHERE"
                             " fTelescopeNumber=%d AND fSequenceFirst=%d AND"
                             " fExcludedFDAKEY=1 AND fRunTypeKEY%%s",
                             fTelescope, fSequence));

    const TString query1(Form("SELECT fRunNumber, fFileNumber, fNumEvents %s", where.Data()));
    const TString query2(Form("SELECT fRunStart, fRunStop %s", where.Data()));

    const TString queryA(Form(query1.Data(), " BETWEEN 2 AND 4 ORDER BY fRunNumber*1000+fFileNumber"));
    const TString queryC(Form(query1.Data(), "=4 ORDER BY fRunNumber*1000+fFileNumber"));
    const TString queryD(Form(query1.Data(), "=2 ORDER BY fRunNumber*1000+fFileNumber"));
    const TString queryT(Form(query2.Data(), "=4 ORDER BY fRunNumber*1000+fFileNumber LIMIT 1"));

    // Try to get a valid time for the first calibration run
    const TString timec = GetTimeFromDatabase(serv, queryT);
    if (timec.IsNull())
        return kFALSE;

    // Query to get pedestal runs sorted in the order by distance to the cal run
    const TString query4(Form("=3 ORDER BY ABS(TIME_TO_SEC(TIMEDIFF(fRunStop,'%s'))) ASC", timec.Data()));
    const TString queryP(Form(query1.Data(), query4.Data()));

    // get and setup runs
    if (!GetRuns(serv, queryA, kAll))
        return kFALSE;
    if (!GetRuns(serv, queryC, kCal))
        return kFALSE;
    if (!GetRuns(serv, queryP, kPed, 1000))
        return kFALSE;
    if (!GetRuns(serv, queryD, kDat))
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// This is a wrapper to get rid of the deletion of res in case of error.
// It also checks that exactly one sequence has been found and all
// requested column were returned.
//
Bool_t MSequenceSQL::GetFromDatabase(MSQLMagic &serv, TSQLResult &res)
{
    if (res.GetRowCount()==0)
    {
        *fLog << err << "ERROR - Requested Sequence not found in database." << endl;
        return kFALSE;
    }

    if (res.GetRowCount()>1)
    {
        *fLog << err << "ERROR - Database request returned morethan one sequence." << endl;
        return kFALSE;
    }

    if (res.GetFieldCount()!=12)
    {
        *fLog << err << "ERROR - Database request returned unexpected number of rows." << endl;
        return kFALSE;
    }

    TSQLRow *row = res.Next();

    return row ? GetFromDatabase(serv, *row) : kFALSE;
}

// --------------------------------------------------------------------------
//
// Set the contents of a sequence from the database.
// The sequence number is given as argument.
//
// Example 1:
//    // Define the database url
//    MSQLMagic serv("mysql://user:password@url/Database");
//    MSequenceSQL s;
//    s.GetFromDatabase(serv, 100002);
//    if (s.IsValid())
//        s.Print();
//
// Example 2:
//    // Define the database in the resource file
//    MSQLMagic serv("sql.rc");
//    MSequenceSQL s;
//    s.GetFromDatabase(serv, 100002);
//    if (s.IsValid())
//        s.Print();
//
//  In case of error the Sequence is set invalid and kFALSE is returned,
//  kTrue in case of success.
//
Bool_t MSequenceSQL::GetFromDatabase(MSQLMagic &serv, UInt_t sequno, Int_t tel)
{
    // Check if we are connected to the sql server
    if (!serv.IsConnected())
    {
        *fLog << err << "ERROR - No connection to database in GetSeqFromDatabase." << endl;
        return kFALSE;
    }

    /*
    // check if any telescope number is valid
    if (tel<0 && fTelescope==0)
    {
        *fLog << err << "ERROR - No telescope number given in GetSeqFromDatabase." << endl;
        return kFALSE;
    }
    */

    // check if any sequence number is valid
    if (sequno==(UInt_t)-1 && fSequence==(UInt_t)-1)
    {
        *fLog << err << "ERROR - No sequence number given in GetSeqFromDatabase." << endl;
        return kFALSE;
    }

    // set "filename" and sequence number
    fFileName  = serv.GetName();
    if (tel>=0)
        fTelescope = tel;
    if (sequno!=(UInt_t)-1)
        fSequence = sequno;

    // get sequence information from database
    TString query("SELECT fSequenceLast, fProjectKEY, fSourceKEY,"
                  " fL1TriggerTableKEY, fL2TriggerTableKEY, fHvSettingsKEY, "
                  " fLightConditionsKEY, fRunStart, fNumEvents, "
                  " fZenithDistanceMin, fZenithDistanceMax, fTelescopeNumber "
                  " FROM Sequences WHERE ");
    query += Form("fSequenceFirst=%d", fSequence);

    if (tel>0)
        query += Form(" AND fTelescopeNumber=%d", fTelescope);

    // Request information from database
    TSQLResult *res = serv.Query(query);
    if (!res)
        return kFALSE;

    const Bool_t rc = GetFromDatabase(serv, *res);
    delete res;

    // invalidate sequence if retrieval failed
    if (!rc)
    {
        fTelescope = 0;
        fSequence  = (UInt_t)-1;
    }

    // return result
    return rc;
}

// --------------------------------------------------------------------------
//
// Set the contents of a sequence from the database.
// The sequence number is given as argument.
//
// Example 1:
//    // Define the database url
//    MSequenceSQL s;
//    s.GetFromDatabase("mysql://user:password@url/Database", 100002);
//    if (s.IsValid())
//        s.Print();
//
// Example 2:
//    // Define the database in the resource file
//    MSequenceSQL s;
//    s.GetFromDatabase("sql.rc", 100002);
//    if (s.IsValid())
//        s.Print();
//
//  In case of error the Sequence is set invalid and kFALSE is returned,
//  kTrue in case of success.
//
Bool_t MSequenceSQL::GetFromDatabase(const char *rc, UInt_t sequno, Int_t tel)
{
    MSQLMagic serv(rc);
    return GetFromDatabase(serv, sequno, tel);
}
