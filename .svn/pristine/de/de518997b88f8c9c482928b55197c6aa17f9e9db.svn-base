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
!   Author(s): Thomas Bretz 2/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

////////////////////////////////////////////////////////////////////////
//
//  MSqlInsertRun
//
//  Input Containers:
//   MRawRunHeader
//
//  Output Containers:
//    -/-
//
////////////////////////////////////////////////////////////////////////
#include "MSqlInsertRun.h"

#include <TSystem.h>

#include <TSQLResult.h>
#include <TSQLRow.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MTaskList.h"

#include "MRawFileRead.h"
#include "MRawRunHeader.h"

#include "MSQLServer.h"

ClassImp(MSqlInsertRun);

using namespace std;

// --------------------------------------------------------------------------
//
MSqlInsertRun::MSqlInsertRun(const char *db, const char *user, const char *pw)
    : fIsUpdate(kFALSE)
{
    fName  = "MSqlInsertRun";
    fTitle = "Write run into database";

    *fLog << dbg << "Connecting to: " << db << " as " << user << " <" << pw << ">" << endl;

    fSqlServer = new MSQLServer(db, user, pw);
}

// --------------------------------------------------------------------------
//
MSqlInsertRun::MSqlInsertRun(const char *u)
    : fSqlServer(0), fIsUpdate(kFALSE)
{
    fName  = "MSqlInsertRun";
    fTitle = "Write run into database";

    fSqlServer = new MSQLServer(u); //::Connect(url, user, pasw);
}

MSqlInsertRun::~MSqlInsertRun()
{
    if (fSqlServer)
        delete fSqlServer;
}

Bool_t MSqlInsertRun::PrintError(const char *txt, const char *q) const
{
    *fLog << err;
    *fLog << "Fatal error acessing database: " << txt << endl;
    fLog->Underline();
    *fLog << "Query:" << flush << " " << q << endl;
    return kFALSE;
}

TString MSqlInsertRun::GetEntry(const char *table, const char *col, const char *where)
{
    return fSqlServer->GetEntry(where, col, table);
}

Bool_t MSqlInsertRun::IsRunExisting(MRawRunHeader *h, Bool_t &exist)
{
    exist = kFALSE;

    const TString str(GetEntry("RunData", "fRunNumber", Form("fRunNumber=%d", h->GetRunNumber())));
    if (str.IsNull())
        return kFALSE;

    exist = kTRUE;
    return kTRUE;
}

Int_t MSqlInsertRun::GetKey(const char *table, const char *where)
{
    const TString str(GetEntry(table, Form("f%sKEY", table), where));

    Int_t key;
    return sscanf(str.Data(), "%d", &key)==1 ? key : -1;
}

TString MSqlInsertRun::GetKeyStr(const char *table, const char *where, Bool_t &rc)
{
    const TString str(GetEntry(table, Form("f%sKEY", table), where));
    if (str.IsNull())
        rc = kFALSE;

    return str;
}

TString MSqlInsertRun::MagicNumber(MRawRunHeader *h, Bool_t &ok)
{
    Int_t key = GetKey("MagicNumber", Form("fMagicNumber=%d", h->GetMagicNumber()));
    if (key<0)
        key = GetKey("MagicNumber", "fMagicNumber=0xffff");
    if (key<0)
    {
        ok = kFALSE;
        return TString("");
    }
    return TString(Form("%d", key));
}

TString MSqlInsertRun::RunType(MRawRunHeader *h, Bool_t &ok)
{
    Int_t key = GetKey("RunType", Form("fRunType=%d", h->GetRunType()));
    if (key<0)
        key = GetKey("RunType", "fRunTye=0xffff");
    if (key<0)
    {
        ok = kFALSE;
        return TString("");
    }
    return TString(Form("%d", key));
}

TString MSqlInsertRun::Source(MRawRunHeader *h, Bool_t &ok)
{
    Int_t key = GetKey("Source", Form("fSourceName='%s'", h->GetSourceName()));
    if (key<0)
    {
        const char *q = Form("INSERT Source (fSourceName, fSourceTxt) VALUES ('%s', 'MSqlInsert: %s')",
                             h->GetSourceName(), h->GetSourceName());

        TSQLResult *res = fSqlServer->Query(q);
        if (!res)
        {
            ok = kFALSE;
            return TString("");
        }
        key = GetKey("Source", Form("fSourceName='%s'", h->GetSourceName()));
        if (key>0)
            *fLog << inf << "New Source '" << h->GetSourceName() << "' inserted into table Source of database." << endl;
    }
    else
    {
        // FIXME: check for consistency!
    }
    if (key<0)
    {
        ok = kFALSE;
        return TString("");
    }

    return TString(Form("%d", key));
}

TString MSqlInsertRun::Project(MRawRunHeader *h, Bool_t &ok)
{
    Int_t key = GetKey("Project", Form("fProjectName='%s'", h->GetProjectName()));
    if (key<0)
    {
        const char *q = Form("INSERT Project (fProjectName, fProjectTxt) VALUES ('%s', 'MSqlInsert: %s')",
                             h->GetProjectName(), h->GetProjectName());

        TSQLResult *res = fSqlServer->Query(q);
        if (!res)
        {
            ok = kFALSE;
            return TString("");
        }
        key = GetKey("Project", Form("fProjectName='%s'", h->GetProjectName()));
        if (key>0)
            *fLog << inf << "New Project '" << h->GetProjectName() << "' inserted into table Project of database." << endl;
    }
    if (key<0)
    {
        ok = kFALSE;
        return TString("");
    }

    return TString(Form("%d", key));
}

TString MSqlInsertRun::RawFilePath(const char *path, Bool_t &ok)
{
    Int_t key = GetKey("FilePath", Form("fFilePathName='%s'", path));
    if (key<0)
    {
        const char *q = Form("INSERT FilePath (fFilePathName, fFilePath) VALUES ('%s', 'MSqlInsert: %s')",
                             path, path);

        TSQLResult *res = fSqlServer->Query(q);
        if (!res)
        {
            ok = kFALSE;
            return TString("");
        }
        key = GetKey("FilePath", Form("fFilePathName='%s'", path));
        if (key>0)
            *fLog << inf << "New FilePath '" << path << "' inserted into table FilePath of database." << endl;

    }
    if (key<0)
    {
        ok = kFALSE;
        return TString("");
    }

    return TString(Form("%d", key));
}
/*
#include <TArrayI.h>

void Line(const TArrayI &max)
{
    cout << "+" << setfill('-');
    for (int i=0; i<max.GetSize(); i++)
        cout << setw(max[i]+1) << "-" << "-+";
    cout << endl;
}

void PrintResult(TSQLResult *res)
{
    Int_t n = res->GetFieldCount();

    TArrayI max(n);

    for (int i=0; i<n; i++)
        max[i] = strlen(res->GetFieldName(i));

    TSQLRow *row;

    TList rows;
    while ((row=res->Next()))
    {
        for (int i=0; i<n; i++)
            max[i] = TMath::Max((ULong_t)max[i], row->GetFieldLength(i));
        rows.Add(row);
    }

    Line(max);

    cout << "|" << setfill(' ');
    for (int i=0; i<n; i++)
        cout << setw(max[i]+1) << res->GetFieldName(i) << " |";
    cout << endl;

    Line(max);

    cout << setfill(' ');
    TIter Next(&rows);
    while ((row=(TSQLRow*)Next()))
    {
        cout << "|";
        for (int i=0; i<n; i++)
        {
            const char *c = (*row)[i];
            cout << setw(max[i]+1) << (c?c:"") << " |";
        }
        cout << endl;
    }

    Line(max);
}
*/
Bool_t MSqlInsertRun::InsertRun(MRawRunHeader *h, Bool_t update)
{
    // FIXME: Insert input file (PreProcess), output file (PostProcess)

    TString query(update ? "UPDATE" : "INSERT");

    Bool_t ok=kTRUE;

    query += " RunData SET fMagicNumberKEY='";
    query += MagicNumber(h, ok);
    query += "', fFormatVersion='";
    query += h->GetFormatVersion();
    query += "', fRunTypeKEY='";
    query += RunType(h, ok);
    query += "', fRunNumber='";
    query += h->GetRunNumber();
    query += "', fProjectKEY='";
    query += Project(h, ok);
    query += "', fSourceKEY='";
    query += Source(h, ok);
    query += "', fNumEvents='";
    query += h->GetNumEvents();
    query += "', fRunStart='";
    query += h->GetRunStart().GetSqlDateTime();
    query += "', fRunStop='";
    query += h->GetRunEnd().GetSqlDateTime();
    query += "'";

    if (update)
    {
        query += " WHERE fRunNumber=";
        query += h->GetRunNumber();
    }

    if (!ok)
    {
        *fLog << err << "ERROR - while concatenating query..." << endl;
        *fLog << query << endl;
        return kFALSE;
    }

    TSQLResult *res = fSqlServer->Query(query);
    if (!res)
        return PrintError("TSQLResult==NULL", query);

    *fLog << inf << dec;
    *fLog << "Run #" << h->GetRunNumber() << " ";
    *fLog << (update ? "updated" : "inserted");
    *fLog << " in database successfully." << endl;

    return kTRUE;
}

Int_t MSqlInsertRun::GetIndex(MRawRunHeader *h)
{
    Bool_t ok=kTRUE;

    TString query("SELECT fRunDataKEY from RunData WHERE fMagicNumberKEY='");

    query += MagicNumber(h, ok);
    query += "' AND fFormatVersion='";
    query += h->GetFormatVersion();
    query += "' AND fRunTypeKEY='";
    query += RunType(h, ok);
    query += "' AND fRunNumber='";
    query += h->GetRunNumber();
    query += "' AND fProjectKEY='";
    query += Project(h, ok);
    query += "' AND fSourceKEY='";
    query += Source(h, ok);
    query += "' AND fNumEvents='";
    query += h->GetNumEvents();
    query += "' AND fRunStart='";
    query += h->GetRunStart().GetSqlDateTime();
    query += "' AND fRunStop='";
    query += h->GetRunEnd().GetSqlDateTime();
    query += "'";

    if (!ok)
    {
        *fLog << err << "ERROR - while concatenating query..." << endl;
        *fLog << query << endl;
        return -1;
    }

    TSQLResult *res = fSqlServer->Query(query);
    if (!res)
        return (PrintError("TSQLResult==NULL", query), -1);

    if (res->GetFieldCount()!=1)
        return (PrintError("Number of columns != 1", query), -1);

    if (res->GetRowCount()>1)
        return (PrintError("Number of rows > 1", query), -1);

    if (res->GetRowCount()==0)
        return 0;

    const char *fld = res->Next()->GetField(0);
    if (!fld)
        return (PrintError("Entry is empty", query), -1);

    return atoi(fld);
}

Bool_t MSqlInsertRun::InsertFile(MRawRunHeader *h, MParList *pList, Bool_t update)
{
    const Int_t key = GetIndex(h);
    if (key<0)
        return kFALSE;

    if (key==0)
        return kTRUE;

    MTaskList *tasks = (MTaskList*)pList->FindObject("MTaskList");
    if (!tasks)
    {
        *fLog << err << "MTaskList not found in parameter list... aborting." << endl;
        return kFALSE;
    }
    MRawFileRead *read = (MRawFileRead*)tasks->FindObject("MRawFileRead");
    if (!read)
    {
        *fLog << err << "MRawFileRead not found in task list... aborting." << endl;
        return kFALSE;
    }

    const char *base = gSystem->BaseName(read->GetFileName());
    const char *path = gSystem->DirName(read->GetFileName());

    const TString str(GetEntry("RawFile", "fRawFileName", Form("fRawFileName='%s'", base)));
    if (!update && !str.IsNull())
    {
        *fLog << err << "ERROR - fRawFileName=" << base << " already existing in RawFile... aborting." << endl;
        return kFALSE;
    }

    Bool_t ok = kTRUE;

    TString query(update ? "UPDATE" : "INSERT");
    query += " RawFile SET fRawFileName='";
    query += base;
    query += "', fFilePathKEY=";
    query += RawFilePath(path, ok);
    query += ", fRawFileKEY=";
    query += key;

    if (!ok)
    {
        *fLog << err << "ERROR - while concatenating query..." << endl;
        *fLog << query << endl;
        return kFALSE;
    }

    TSQLResult *res = fSqlServer->Query(query);
    if (!res)
        return PrintError("TSQLResult==NULL", query);

    *fLog << inf << dec;
    *fLog << "File '" << base << "' ";
    *fLog << (update ? "updated" : "inserted");
    *fLog << " in database successfully." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MSqlInsertRun::PreProcess(MParList *pList)
{
    if (!fSqlServer)
    {
        *fLog << err << "Connection to SQL server failed... aborting." << endl;
        return kFALSE;
    }

    MRawRunHeader *header = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!header)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }
/*
    if (header->GetFormatVersion()<3)
    {
        *fLog << err << "MSqlInsertRun cannot be used for files with format <3... abort." << endl;
        return kTRUE;
    }
 */
    Bool_t update = kFALSE;

    // Is run already in databae?
    const TString str(GetEntry("MRawRunHeader", "fRunNumber", Form("fRunNumber=%d", header->GetRunNumber())));
    if (!str.IsNull())
    {
        if (!fIsUpdate)
        {
            *fLog << err << "Run #" << dec << header->GetRunNumber() << " already in database... abort." << endl;
            return kFALSE;
        }
        update=kTRUE;
    }

    if (!InsertRun(header, update))
        return kFALSE;

    if (!InsertFile(header, pList, update))
        return kFALSE;

    return kTRUE;
}

Int_t MSqlInsertRun::PostProcess()
{
    return kTRUE;
}
