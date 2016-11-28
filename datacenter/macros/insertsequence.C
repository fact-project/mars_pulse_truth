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
// insertsequence.C
// ================
//
// This macro inserts a sequence into the database. It extracts the
// information from a sequence file.
// This macro is not used by the automatic analysis. It is needed to insert
// manually built sequences into the database.
//
// Usage:
//  .x insertsequence.C+("filename", kTRUE)
// The first argument is the filename of the manual written sequencefile.
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
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

#include <TEnv.h>

#include <MSQLMagic.h>
#include <TSQLRow.h>
#include <TSQLResult.h>

using namespace std;

int insertsequence(TString filename, Bool_t dummy=kTRUE)
{
    cout << "ERROR - TelescopeNumber and FileNumber not implemeted." << endl;
    return 2;
/*
    TEnv env("sql.rc");
    TEnv sequ(filename);

    MSQLMagic serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    serv.SetIsDummy(dummy);

    cout << "insertsequence" << endl;
    cout << "--------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    //get sequence number from file
    TString sequnum;
    sequnum=sequ.GetValue("Sequence", "");
    Int_t seq=atoi(sequnum.Data());

    //get runs from sequence file
    TString runs;
    runs = sequ.GetValue("Runs", "");
    runs.ReplaceAll(" ", ",");
    if (runs.IsNull())
    {
        cout << "ERROR - No runs in file " << filename << " found." << endl;
        return 0;
    }

    //get source key
    TString sourcename = sequ.GetValue("Source", "");
    Int_t sourcekey = serv.QueryKeyOfName("Source", sourcename.Data(), kFALSE);
    if (sourcekey<0)
    {
        cout << "Error - could not get sourcename from DB -> " << flush;
        cout << "maybe you have the wrong sourcename in your datasetfile" << endl;
        return 2;
    }

    //get values for the sequence
    TString query="SELECT max(fRunNumber), min(fRunStart), ";
    query +="sum(time_to_sec(fRunStop)-time_to_sec(fRunStart)), ";
    query +="min(fZenithDistance), max(fZenithDistance), ";
    query +="min(fAzimuth), max(fAzimuth), sum(fNumEvents) from RunData ";
    query +=Form("WHERE fRunNumber IN (%s)",runs.Data());

    cout << "runs: " << runs << endl;
    cout << "q1: " << query << endl;

    TSQLResult *res = serv.Query(query);
    if (!res)
        return 0;

    TSQLRow *row=res->Next();
    TString lastrun=(*row)[0];
    TString starttime=(*row)[1];
    TString uptime=(*row)[2];
    TString zdmin=(*row)[3];
    TString zdmax=(*row)[4];
    TString azmin=(*row)[5];
    TString azmax=(*row)[6];
    TString numevts=(*row)[7];

    delete res;

    query  ="SELECT fProjectKEY, fSourceKEY, fHvSettingsKEY, ";
    query +="fTriggerDelayTableKEY, fDiscriminatorThresholdTableKEY, ";
    query +="fTestFlagKEY, fLightConditionsKEY, fL1TriggerTableKEY, ";
    query +=Form("fL2TriggerTableKEY FROM RunData WHERE fRunNumber=%d", seq);

    cout << "q2: " << query << endl;

    res = serv.Query(query);
    if (!res)
        return 0;

    row=res->Next();
    TString project=(*row)[0];
    TString source=(*row)[1];
    TString hv=(*row)[2];
    TString delay=(*row)[3];
    TString dt=(*row)[4];
    TString testflag=(*row)[5];
    TString lightcond=(*row)[6];
    TString l1tt=(*row)[7];
    TString l2tt=(*row)[8];

    delete res;

    cout << "seq: "       << seq        << endl;
    cout << "  lastrun   " << lastrun   << endl;
    cout << "  startime  " << starttime << endl;
    cout << "  uptime    " << uptime    << endl;
    cout << "  zdmin     " << zdmin     << endl;
    cout << "  zdmax     " << zdmax     << endl;
    cout << "  azmin     " << azmin     << endl;
    cout << "  azmax     " << azmax     << endl;
    cout << "  numevts   " << numevts   << endl;
    cout << " keys:"                    <<  endl;
    cout << "  project   " << project   << endl;
    cout << "  source1   " << source    << " (from db -> run "  << seq << ") " << endl;
    cout << "  source2   " << sourcekey << " (from sequ file) " << endl;
    if (!(atoi(source.Data())==sourcekey))
    {
        cout << "new source name: " << sourcename << " -> inserting..." << endl;
        sourcekey = serv.QueryKeyOfName("Source", sourcename.Data(), kFALSE);
        if (sourcekey<0)
        {
            cout << "Error - could not get sourcename from DB -> " << flush;
            cout << "maybe you have the wrong sourcename in your datasetfile" << endl;
            return 2;
        }
        source=Form("%d",sourcekey);
    }
    cout << "  source    " << source    << endl;
    cout << "  hv        " << hv        << endl;
    cout << "  delay     " << delay     << endl;
    cout << "  dt        " << dt        << endl;
    cout << "  testflag  " << testflag  << endl;
    cout << "  lightcond " << lightcond << endl;
    cout << "  l1tt      " << l1tt      << endl;
    cout << "  l2tt      " << l2tt      << endl;

    //build queries
    TString vals=
        Form("fManuallyChangedKEY=2, "
             "fSequenceFirst=%d, fSequenceLast=%s, "
             "fProjectKEY=%s, fSourceKEY=%s, fNumEvents=%s, "
             "fRunStart='%s', fHvSettingsKEY=%s, fRunTime=%s, "
             "fTriggerDelayTableKEY=%s, fDiscriminatorThresholdTableKEY=%s, "
             "fTestFlagKEY=%s, fLightConditionsKEY=%s, fAzimuthMin=%s, "
             "fAzimuthMax=%s, fZenithDistanceMin=%s, fZenithDistanceMax=%s, "
             "fL1TriggerTableKEY=%s, fL2TriggerTableKEY=%s ",
             seq, lastrun.Data(), project.Data(), source.Data(),
             numevts.Data(), starttime.Data(), hv.Data(), uptime.Data(),
             delay.Data(), dt.Data(), testflag.Data(), lightcond.Data(),
             azmin.Data(), azmax.Data(), zdmin.Data(), zdmax.Data(),
             l1tt.Data(), l2tt.Data());

    const Int_t rc1 = serv.Insert("Sequences", vals);
    if (rc1<0)       // dummy
        return 1;
    if (rc1==kFALSE) // insert failed
        return 0;

    //the time of the column fSequenceFileWritten is set to 'not to be done'
    vals=Form("fSequenceFirst=%d, fSequenceFileWritten='1970-01-01 00:00:00'", seq);

    const Int_t rc2 = serv.Insert("SequencesProcessStatus", vals);

    if (rc2<0)       // dummy
        return 1;
    if (rc2==kFALSE) // insert failed
        return 0;

    return 1;
    */
}


