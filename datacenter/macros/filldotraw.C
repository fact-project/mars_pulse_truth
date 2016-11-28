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
!   Author(s): Thomas Bretz, 08/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Dorner, 08/2004 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// filldotraw.C
// ============
//
// This macro is used to read a merpped raw data file or a raw data file
// directly. The descision is taken by the file-name extension (".root" or
// ".raw")
//
// Usage:
//   .x filldotraw.C("/data/MAGIC/Period014/filename.raw", kTRUE)
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// Filling the database is done with 'UPADTE' for _all_ columns
// matching the Run-Number!
//
// The macro can also be run without ACLiC but this is a lot slower...
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b filldotraw.C+\(\"filename\"\,kFALSE\) 2>&1 | tee filldotraw.log
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 2 in case of failure, 1 in case of success and 0 if the connection
// to the database is not working.
//
/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>

#include <TEnv.h>
#include <TFile.h>
#include <TTree.h>
#include <TRegexp.h>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MZlib.h"
#include "MSQLMagic.h"
#include "MRawRunHeader.h"
#include "MDirIter.h"

using namespace std;

Int_t QueryFromName(MSQLMagic &serv, const char *col, const char *val)
{
    const TString query1 = Form("SELECT f%sKEY FROM %s WHERE f%s='%s'",
                                col, col, col, val);

    TSQLResult *res1 = serv.Query(query1);
    if (!res1)
    {
        cout << "ERROR - Query has failed: " << query1 << endl;
        return -1;
    }

    TSQLRow *row=res1->Next();

    const Int_t rc1 = row && (*row)[0] ? atoi((*row)[0]) : -1;
    delete res1;
    return rc1;
}

//get key for a magic number
Int_t MagicNumber(MSQLMagic &serv, const MRawRunHeader &h)
{
    return QueryFromName(serv, "MagicNumber", Form("%d", h.GetMagicNumber()));
}

Bool_t ReadRaw(TString fname, MRawRunHeader &h)
{
    MZlib fin(fname);
    if (!fin)
    {
        cout << "ERROR - Couldn't open file " << fname << endl;
        return kFALSE;
    }

    if (!h.ReadEvt(fin))
    {
        cout << "ERROR - Reading header from file " << fname << endl;
        return kFALSE;
    }
    return kTRUE;
}

Bool_t ReadRoot(TString fname, MRawRunHeader *h)
{
    TFile file(fname, "READ");
    if (file.IsZombie())
    {
        cout << "ERROR - Cannot open file " << fname << endl;
        return kFALSE;
    }

    TTree *t = (TTree*)file.Get("RunHeaders");
    if (!t)
    {
        cout << "ERROR - Tree RunHeaders not found." << endl;
        return kFALSE;
    }

    t->SetBranchAddress("MRawRunHeader.", &h);
    t->GetEntry(0);

    return kTRUE;
}

Bool_t CheckRunNumber(MSQLMagic &serv, Int_t num)
{
    TString query(Form("SELECT fRunNumber from RunData where fRunNumber=%d", num));

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return kFALSE;
    }

    TSQLRow *row = res->Next();

    Bool_t rc = row && (*row)[0] ? atoi((*row)[0])==num : kFALSE;
    delete res;
    return rc;

}

Bool_t InsertEntry(MSQLMagic &serv, MRawRunHeader &h)
{
    const Int_t magickey  = MagicNumber(serv, h);
    const Int_t runkey    = QueryFromName(serv, "RunType",         h.GetRunTypeStr());
    const Int_t projkey   = serv.QueryKeyOfName("Project",         h.GetProjectName());
    const Int_t sourcekey = serv.QueryKeyOfName("Source",          h.GetSourceName());
    const Int_t modekey   = serv.QueryKeyOfName("ObservationMode", h.GetObservationMode());

    if (magickey<0 || runkey<0 || projkey<0 || sourcekey<0 || modekey<0)
        return -1;

    TString query;

    query += Form("fRunNumber=%d, ",          h.GetRunNumber());
    query += Form("fMagicNumberKEY=%d, ",     magickey);
    query += Form("fFormatVersion=%d, ",      h.GetFormatVersion());
    query += Form("fRunTypeKEY=%d, ",         runkey);
    query += Form("fProjectKEY=%d, ",         projkey);
    query += Form("fSourceKEY=%d, ",          sourcekey);
    query += Form("fNumEvents=%d, ",          h.GetNumEvents());
    query += Form("fRunStart='%s', ",         h.GetRunStart().GetSqlDateTime().Data());
    query += Form("fRunStop='%s', ",          h.GetRunEnd().GetSqlDateTime().Data());
    query += Form("fObservationModeKEY=%d, ", modekey);

    query += "fExcludedFDAKEY=1, fTestFlagKEY=1, fLightConditionsKEY=1, ";
    query += "fCalibrationScriptKEY=1, fDiscriminatorThresholdTableKEY=1, ";
    query += "fTriggerDelayTableKEY=1, fL1TriggerTableKEY=1, fL2TriggerTableKEY=1, ";
    query += "fHvSettingsKEY=1, fZenithDistance=0, fAzimuth=0, ";
    query += "fDaqStoreRate=0, fDaqTriggerRate=0, fMeanTRiggerRate=0, ";
    query += "fL2RatePresc=0, fL2RateUnpresc=0 ";

    return serv.Insert("RunData", query);
}

Int_t UpdateEntry(MSQLMagic &serv, MRawRunHeader &h)
{
    //get key for the magic number
    const Int_t key = MagicNumber(serv, h);
    if (key<0)
        return -1;

    TString vars(Form("fMagicNumberKEY=%d, fFormatVersion=%d",
                       key, h.GetFormatVersion()));
    TString where(Form("fRunNumber=%d", h.GetRunNumber()));

    return serv.Update("RunData", vars, where);
}


int Process(MSQLMagic &serv, TString fname, Bool_t dummy)
{
    MRawRunHeader h;

    //read header either from root or from raw file
    if (fname.EndsWith(".root"))
        ReadRoot(fname, &h);
    if (fname.EndsWith(".raw"))
        ReadRaw(fname, h);
    if (fname.EndsWith(".raw.gz"))
        ReadRaw(fname, h);

    if (dummy)
        h.Print("header");

    Int_t rc = CheckRunNumber(serv, h.GetRunNumber()) ?
        UpdateEntry(serv, h) : InsertEntry(serv, h);

    return rc==0 ? 2 : 1;
}

int filldotraw(TString fname, Bool_t dummy=kTRUE)
{
    TEnv env("sql.rc");

    MSQLMagic serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    serv.SetIsDummy(dummy);

    cout << "filldotraw" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;

    return Process(serv, fname, dummy);
}

int filldotraw(Int_t runno, Bool_t dummy=kTRUE)
{
    TEnv env("sql.rc");

    MSQLMagic serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    serv.SetIsDummy(dummy);

    cout << "filldotraw" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Run: " << runno << endl;
    cout << endl;

    //get date for the run to build path of the file
    TString query(Form("SELECT DATE_FORMAT(ADDDATE(fRunStart, Interval 13 HOUR), '%%Y/%%m/%%d') FROM RunData WHERE fRunNumber=%d",
                       runno));

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return 2;
    }

    TSQLRow *row = 0;
    row = res->Next();
    TString date=(*row)[0];
    cout << "date: " << date << endl;
    TString path(Form("/magic/data/rawfiles/%s", date.Data()));
    TString file(Form("*%d_*_*_E.raw.?g?z?", runno));

    delete res;

    cout << "path: " << path << " - file : " << file << endl;
    TString fname;
    TString name;

    Int_t count=0;
    MDirIter Next(path, file, -1);
    while (1)
    {
        name = Next();
        if (name.IsNull())
            break;
        fname=name;
        cout << "filename: " << fname << endl;
        count++;
    }

    //check if there's only one file with this runno
    if (count!=1)
    {
        cout << "ERROR - there's are " << count << " files. " << endl;
        return 2;
    }

    return Process(serv, fname, dummy);
}
