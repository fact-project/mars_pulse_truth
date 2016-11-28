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
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

////////////////////////////////////////////////////////////////////////
//
//  MSQLServer
//
//  Using this instead of a TSQLServer gives the possibility to
//  browse a database server through the TBrowser and it will offer
//  many features usefull working with relational tables.
//
//  Use it like TSQlServer:
//    new MSQLServer("mysql://localhost:3306?compress", "hercules", "stdmagicpassword");
//    // now start your TBrowser
//    new TBrowser;
//
////////////////////////////////////////////////////////////////////////
#include "MSQLServer.h"

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include <TROOT.h>
#include <TMath.h>

#include <TH1.h>
#include <TEnv.h>
#include <TPRegexp.h>

#include <TSQLResult.h>
#include <TSQLServer.h>
#include <TSQLRow.h>

#include <TBrowser.h>

#include <TObjString.h>
#include <TObjArray.h>

#include "MTreeSQL.h"

ClassImp(MSQLServer);

using namespace std;

// --------------------------------------------------------------------------
//
// Used in virtual function TObject::Browse() to create the
//
void MSQLServer::BrowseColumn(TBrowser *b) /*FOLD00*/
{
    if (!fServ)
        return;

    const TString query0(Form("EXPLAIN %s.%s %s", (const char*)fDataBase, (const char*)fTable, (const char*)fColumn));
    const TString query1(Form("SELECT %s FROM %s.%s", (const char*)fColumn, (const char*)fDataBase, (const char*)fTable));

    //cout << query0 << endl;
    TSQLResult *res = fServ->Query(query0);
    if (!res)
    {
        cout << "query - failed: " << query0 << endl;
        return;
    }

    TSQLRow *row=res->Next();
    const TString desc(row ? (*row)[1] : "");

    if (row)
        delete row;

    delete res;

    const Bool_t isnum =
        !desc.Contains("char", TString::kIgnoreCase) &&
        !desc.Contains("text", TString::kIgnoreCase);

    cout << query1 << endl;
    res = fServ->Query(query1);
    if (!res)
    {
        cout << "query - failed: " << query1 << endl;
        return;
    }

    TArrayD arr(2);
    Int_t num=0;
    Double_t max=0;
    Double_t min=0;
    Double_t sum=0;
    Double_t sqr=0;

    while ((row=res->Next()))
    {
        const TString row0((*row)[0]);

        if (!isnum)
        {
            cout << row0 << endl;
            continue;
        }

        if (num==arr.GetSize())
            arr.Set(arr.GetSize()*2);

        arr[num] = atof(row0.Data());

        if (num==0)
            min=max=arr[0];

        if (arr[num]>max) max = arr[num];
        if (arr[num]<min) min = arr[num];

        sum += arr[num];
        sqr += arr[num]*arr[num];

        num++;
    }

    delete res;

    if (!isnum)
        return;

    if (max==min) max += 1;

    Int_t num0 = 1;

    if (num>0)
    {
        /*
         cout << "Num:   " << num << endl;
         cout << "Mean:  " << sum/num << endl;
         cout << "Range: " << max-min << endl;
         cout << "RMS:   " << TMath::Sqrt(sqr/num-sum*sum/num/num) << endl;
         */

        num0 = (Int_t)((max-min)*40/TMath::Sqrt(sqr/num-sum*sum/num/num));
    }

    const TString title(Form("#splitline{%s}{<%s>}", (const char*)query1, (const char*)desc));

    TH1F *hist=new TH1F(fColumn, title, num0, min, max);
    for (int i=0; i<num; i++)
        hist->Fill(arr[i]);

    //cout << "Done." << endl;

    hist->Draw();
    hist->SetBit(kCanDelete);
}

void MSQLServer::BrowseTable(TBrowser *b) /*FOLD00*/
{
    if (!fServ)
        return;

    TSQLResult *res = fServ->GetColumns(fDataBase, fTable);
    if (!res)
        return;

    TSQLRow *row;
    while ((row=res->Next()))
    {
        TString row0((*row)[0]);
        delete row;

        MSQLServer *sql = (MSQLServer*)fList.FindObject(Form("%s/%s/%s", (const char*)fDataBase, (const char*)fTable, (const char*)row0));
        if (!sql)
        {
            sql = new MSQLColumn(fServ, fDataBase, fTable, row0);
            fList.Add(sql);
        }
        b->Add(sql, row0);
    }
}

void MSQLServer::BrowseDataBase(TBrowser *b) /*FOLD00*/
{
    if (!fServ)
        return;

    TSQLResult *res = fServ->GetTables(fDataBase);
    if (!res)
        return;

    TSQLRow *row;
    while ((row=res->Next()))
    {
        TString row0((*row)[0]);
        delete row;

        MSQLServer *sql = (MSQLServer*)fList.FindObject(Form("%s/%s", (const char*)fDataBase, (const char*)row0));
        if (!sql)
        {
            sql = new MSQLServer(fServ, fDataBase, row0);
            fList.Add(sql);
        }
        b->Add(sql, row0);
    }
}

void MSQLServer::BrowseServer(TBrowser *b) /*FOLD00*/
{
    if (!fServ)
        return;

    TSQLResult *res = fServ->GetDataBases();
    if (!res)
        return;

    TSQLRow *row;
    while ((row=res->Next()))
    {
        const TString row0((*row)[0]);
        delete row;

        MSQLServer *sql = (MSQLServer*)fList.FindObject(row0);
        if (!sql)
        {
            sql = new MSQLServer(fServ, row0);
            fList.Add(sql);
        }
        b->Add(sql, row0);
    }
}

void MSQLServer::PrintLine(const TArrayI &max) /*FOLD00*/
{
    cout << "+" << setfill('-');
    for (int i=0; i<max.GetSize(); i++)
        cout << setw(max[i]+1) << "-" << "-+";
    cout << endl;
}

void MSQLServer::PrintTable(TSQLResult &res) /*FOLD00*/
{
    Int_t n = res.GetFieldCount();

    TArrayI max(n);

    for (int i=0; i<n; i++)
        max[i] = strlen(res.GetFieldName(i));

    TSQLRow *row;

    TList rows;
    rows.SetOwner();

    while ((row=res.Next()))
    {
        for (int i=0; i<n; i++)
            max[i] = TMath::Max((ULong_t)max[i], row->GetFieldLength(i));
        rows.Add(row);
    }

    cout << endl;

    PrintLine(max);

    cout << "|" << setfill(' ');
    for (int i=0; i<n; i++)
        cout << setw(max[i]+1) << res.GetFieldName(i) << " |";
    cout << endl;

    PrintLine(max);

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

    PrintLine(max);
}

TString MSQLServer::GetFields() const /*FOLD00*/
{
    if (!fServ)
        return "";

    TSQLResult *res = fServ->GetColumns(fDataBase, fTable);
    if (!res)
        return "";

    TString fields;

    TSQLRow *row;

    TList rows;
    rows.SetOwner();

    while ((row=res->Next()))
        rows.Add(row);

    TIter Next(&rows);
    while ((row=(TSQLRow*)Next()))
    {
        fields += (*row)[0];
        if (row!=rows.Last())
            fields += ", ";
    }

    return fields;
}

void MSQLServer::PrintQuery(const char *query) const /*FOLD00*/
{
    if (!fServ)
        return;

    TSQLResult *res = fServ->Query(query);
    if (res)
    {
        PrintTable(*res);
        delete res;
    }
    else
        cout << "Query failed: " << query << endl;
}

void MSQLServer::Print(Option_t *o) const /*FOLD00*/
{
    const TString opt(o);
    if (!opt.IsNull())
    {
        PrintQuery(o);
        return;
    }

    switch (fType)
    {
    case kIsServer:
        PrintQuery("SHOW DATABASES");
        break;

    case kIsDataBase:
        PrintQuery(Form("SHOW TABLES FROM %s", (const char*)fDataBase));
        break;

    case kIsTable:
        PrintQuery(Form("SELECT * FROM %s.%s", (const char*)fDataBase, (const char*)fTable));
        break;

    case kIsColumn:
        PrintQuery(Form("SELECT %s FROM %s.%s", (const char*)fColumn, (const char*)fDataBase, (const char*)fTable));
        break;

    default:
        break;
    }
}

void MSQLServer::ShowColumns() const /*FOLD00*/
{
    switch (fType)
    {
    case kIsTable:
        PrintQuery(Form("SHOW FULL COLUMNS FROM %s.%s", (const char*)fDataBase, (const char*)fTable));
        break;

    case kIsColumn:
        PrintQuery(Form("SHOW FULL COLUMNS FROM %s.%s LIKE %s", (const char*)fDataBase, (const char*)fTable, (const char*)fColumn));
        break;

    default:
        //Print();
        break;
    }
}

void MSQLServer::ShowStatus() const /*FOLD00*/
{
    switch (fType)
    {
    case kIsServer:
        PrintQuery("SHOW STATUS");
        break;

    case kIsDataBase:
        PrintQuery(Form("SHOW TABLE STATUS FROM %s", (const char*)fDataBase));
        break;

    case kIsTable:
        PrintQuery(Form("SHOW TABLE STATUS FROM %s LIKE %s", (const char*)fDataBase, (const char*)fTable));
        break;

    default:
        break;
    }
}

void MSQLServer::ShowTableIndex() const /*FOLD00*/
{
    switch (fType)
    {
    case kIsTable:
    case kIsColumn:
        PrintQuery(Form("SHOW INDEX FROM %s.%s", (const char*)fDataBase, (const char*)fTable));
        break;

    default:
        break;
    }
}

void MSQLServer::ShowTableCreate() const /*FOLD00*/
{
    switch (fType)
    {
    case kIsTable:
    case kIsColumn:
        PrintQuery(Form("SHOW CREATE TABLE %s.%s", (const char*)fDataBase, (const char*)fTable));
        break;

    default:
        break;
    }
}

void MSQLServer::Close(Option_t *option) /*FOLD00*/
{
    if (fType==kIsServer && fServ)
    {
        fServ->Close(option);
        if (TestBit(kIsOwner))
        {
            fTrees.Delete();

            delete fServ;
            fServ=0;
            ResetBit(kIsOwner);
            fType=kIsZombie;
        }
    }
}

// --------------------------------------------------------------------------
//
// Send a SQL query to the SQL server.
//
// If MSQLServer is no server (column, row, ...) NULL is returned and an
//  error message is send to stdout.
//
// If the query failed for some reason an error message is send to stdout
//  and NULL is returned.
//
// If everything works fine a TSQLResult is returned. Make sure that you
// delete it!
//
TSQLResult *MSQLServer::Query(const char *sql) /*FOLD00*/
{
    if (!fServ)
        return NULL;

    if (fType!=kIsServer)
    {
        cout << "ERROR: MSQLServer::Query - this is not a server!" << endl;
        return NULL;
    }

    TSQLResult *res = fServ->Query(sql);
    if (!res)
    {
        cout << /*"ERROR: MSQLServer::Query - Query failed: " <<*/ sql << endl;
        return NULL;
    }

    return res;
}

// --------------------------------------------------------------------------
//
// Send a SQL query to the SQL server.
//
// If MSQLServer is no server (column, row, ...) NULL is returned and an
//  error message is send to stdout.
//
// If the query failed kFALSE is returned.
//
// On success kTRUE is returned.
//
Bool_t MSQLServer::Exec(const char* sql)
{
    if (!fServ)
        return kFALSE;

#if ROOT_VERSION_CODE < ROOT_VERSION(5,12,00)
    TSQLResult *res = fServ->Query(sql);
    if (!res)
    {
        cout << "ERROR: MSQLServer::Exec - Query failed: " << sql << endl;
        return kFALSE;
    }
    delete res;
    return kTRUE;
#else
    if (fType!=kIsServer)
    {
        cout << "ERROR: MSQLServer::Exec - this is not a server!" << endl;
        return kFALSE;
    }

    return fServ->Exec(sql);
#endif
}

Int_t MSQLServer::SelectDataBase(const char *dbname) /*FOLD00*/
{
    fDataBase = dbname;
    return fType==kIsServer && fServ ? fServ->SelectDataBase(dbname) : 0;
}

TSQLResult *MSQLServer::GetDataBases(const char *wild) /*FOLD00*/
{
    return fType==kIsServer && fServ ? fServ->GetDataBases(wild) : NULL;
}

TSQLResult *MSQLServer::GetTables(const char *wild, const char *dbname) /*FOLD00*/
{
    return fType==kIsServer && fServ ? fServ->GetTables(dbname?dbname:fDataBase.Data(), wild) : NULL;
}

TSQLResult *MSQLServer::GetColumns(const char *table, const char *wild, const char *dbname) /*FOLD00*/
{
    return fType==kIsServer && fServ ? fServ->GetColumns(dbname?dbname:fDataBase.Data(), table, wild) : NULL;
}

Int_t MSQLServer::CreateDataBase(const char *dbname) /*FOLD00*/
{
    return fType==kIsServer && fServ ? fServ->CreateDataBase(dbname) : 0;
}

Int_t MSQLServer::DropDataBase(const char *dbname) /*FOLD00*/
{
    return fType==kIsServer && fServ ? fServ->DropDataBase(dbname) : 0;
}

Int_t MSQLServer::Reload() /*FOLD00*/
{
    return fType==kIsServer && fServ ? fServ->Reload() : 0;
}

Int_t MSQLServer::Shutdown() /*FOLD00*/
{
    return fType==kIsServer && fServ ? fServ->Shutdown() : 0;
}

const char *MSQLServer::ServerInfo() /*FOLD00*/
{
    return fType==kIsServer && fServ ? fServ->ServerInfo() : "";
}

Bool_t MSQLServer::IsConnected() const
{
    return fType==kIsServer && fServ ? fServ->IsConnected() : kFALSE;
}

const char *MSQLServer::GetName() const
{
    if (!fServ)
        return "Unconnected!";

    switch (fType)
    {
    case kIsServer:   return Form("%s://%s:%d/%s", fServ->GetDBMS(), fServ->GetHost(), fServ->GetPort(), fDataBase.Data());
    case kIsDataBase: return GetNameDataBase();
    case kIsTable:    return GetNameTable();
    case kIsColumn:   return GetNameColumn();
    default:          return "n/a";
    }
}

Bool_t MSQLServer::Split(TString &url, TString &user, TString &pasw) const
{
    const Ssiz_t pos1 = url.First("://")+3;
    const Ssiz_t pos2 = url.Last(':')   +1;
    const Ssiz_t pos3 = url.First('@');

    if (pos1<0 || pos2<0 || pos3<0 || pos1>pos2 || pos2>pos3)
        return kFALSE;

    user = url(pos1, pos2-pos1-1);
    pasw = url(pos2, pos3-pos2);

    url.Remove(pos1, pos3+1-pos1);

    return kTRUE;
}

void MSQLServer::Init(const char *connection, const char *user, const char *password) /*FOLD00*/
{
    fType = kIsZombie;

    fServ = TSQLServer::Connect(connection, user, password);
    if (fServ)
    {
        gROOT->GetListOfBrowsables()->Add(this, connection);
        fType = kIsServer;
        SetBit(kIsOwner);
        SetBit(kMustCleanup);
    }
    else
        fType = kIsZombie;

    fList.SetOwner();
}

void MSQLServer::InitEnv(TEnv &env, const char *prefix)
{
    TString url  = env.GetValue("URL",      "");
    TString db   = env.GetValue("Database", "");
    TString user = env.GetValue("User",     "");
    TString pass = env.GetValue("Password", "");

    user = env.GetValue(Form("%s.User", db.Data()), user);

    pass = env.GetValue(Form("%s.Password", user.Data()), pass);
    pass = env.GetValue(Form("%s.%s.Password", db.Data(), user.Data()), pass);

    if (prefix)
    {
        url = env.GetValue(Form("%s.URL", prefix), url);
        db  = env.GetValue(Form("%s.Database", prefix), db);

        user = env.GetValue(Form("%s.User", prefix), user);
        user = env.GetValue(Form("%s.%s.User", prefix, db.Data()), user);

        pass = env.GetValue(Form("%s.Password", prefix), pass);
        pass = env.GetValue(Form("%s.%s.Password", prefix, user.Data()), pass);
        pass = env.GetValue(Form("%s.%s.%s.Password", prefix, db.Data(), user.Data()), pass);
    }

    user = user.Strip(TString::kBoth);
    pass = pass.Strip(TString::kBoth);
    url  = url.Strip(TString::kBoth);

    if (user.IsNull() && pass.IsNull())
    {
        if (!Split(url, user, pass))
        {
            fType = kIsZombie;
            return;
        }
    }

    Init(url, user, pass);

    if (IsConnected() && !db.IsNull())
        SelectDataBase(db);
}

MSQLServer::MSQLServer(const char *connection, const char *user, const char *password) /*FOLD00*/
{
    Init(connection, user, password);
}

// --------------------------------------------------------------------------
//
// Instantiate a dabase connection either by
//   mysql://user:password@url/database
// or by a resource file (in teh given string doesn't contain mysql://)
//
MSQLServer::MSQLServer(const char *u) : fType(kIsZombie) /*FOLD00*/
{
    if (TString(u).Contains("mysql://", TString::kIgnoreCase))
    {
        TString url(u);
        TString user, pasw;

        if (!Split(url, user, pasw))
        {
            fType = kIsZombie;
            return;
        }
        Init(url, user, pasw);
    }
    else
    {
        TEnv env(u);
        InitEnv(env);
    }
}

MSQLServer::MSQLServer(TEnv &env, const char *prefix)
{
    InitEnv(env, prefix);
}

MSQLServer::MSQLServer()
{
    if (gEnv)
        InitEnv(*gEnv);
}

MSQLServer::MSQLServer(MSQLServer &serv)
{
    fServ = serv.fServ;

    fDataBase = serv.fDataBase;
    fTable = serv.fTable;
    fColumn = serv.fColumn;

    fType = serv.fType;
}

MSQLServer::~MSQLServer() /*FOLD00*/
{
    if (gDebug>0)
        cout << "Delete: " << GetName() << endl;

    Close();
}

Bool_t MSQLServer::PrintError(const char *txt, const char *q) const /*FOLD00*/
{
    cout << "Fatal error acessing database: " << txt << endl;
    cout << "Query: " << q << endl;
    return kFALSE;
}

TString MSQLServer::GetEntry(const char *where, const char *col, const char *table) const /*FOLD00*/
{
    if (!fServ)
        return "";

    if (table==0)
        table = Form("%s.%s", (const char *)fDataBase, (const char*)fTable);
    if (col==0)
        col = (const char *)fColumn;

    const TString query(Form("SELECT %s FROM %s WHERE %s", col, table, where));

    TSQLResult *res = fServ->Query(query);
    if (!res)
        return (PrintError("GetEntry - TSQLResult==NULL", query), "");

    if (res->GetFieldCount()!=1)
    {
        delete res;
        return (PrintError("GetEntry - Number of columns != 1", query), "");
    }

    if (res->GetRowCount()>1)
    {
        delete res;
        return (PrintError("GetEntry - Number of rows > 1", query), "");
    }

    if (res->GetRowCount()==0)
    {
        delete res;
        return "";
    }

    const char *fld = res->Next()->GetField(0);
    if (!fld)
    {
        delete res;
        return (PrintError("GetEntry - Entry is empty", query), "");
    }

    const TString rc(fld);
    delete res;
    return rc;
}

// --------------------------------------------------------------------------
//
// Return the name of the (first) column with a primary key
//
TString MSQLServer::GetPrimaryKeys(const char *table)
{
    TSQLResult *res = GetColumns(table);
    if (!res)
        return "";

    TObjArray arr;
    arr.SetOwner();

    TSQLRow *row = 0;
    while ((row=res->Next()))
    {
        const TString key = (*row)[3];
        if (key=="PRI")
            arr.Add(new TObjString((*row)[0]));
        delete row;
    }
    delete res;

    arr.Sort();

    TString rc;
    for (int i=0; i<arr.GetEntries(); i++)
    {
        if (i>0)
            rc += ", ";
        rc += arr[i]->GetName();
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// Searches in the text for patterns like "Table.Column". If such a pettern
// is found the primary key of the table is requested a "LEFT JOIN"
// with this Table is added ON the identity of the primary key of Table
// with the given table.
//
TString MSQLServer::GetJoins(const char *table, const TString text)
{
    Int_t p=0;

    TString mods;
    TArrayI pos;

    // Find all Table.Column expression. Because also floating point
    // numbers can contain a dot the result has to be checked carefully
    TString joins;
    TPRegexp reg = TPRegexp("\\w+[.]\\w+");
    while (1)
    {
        // Check whether expression is found
        if (reg.Match(text, mods, p, 130, &pos)==0)
            break;

        // Get expression from text
        const TString expr = text(pos[0], pos[1]-pos[0]);
        p = pos[1];

        if (expr.IsFloat())
            continue;

        const TString tab = expr(0, expr.First('.'));
        //const TString var = expr(expr.First('.')+1, expr.Length());

        // If the table found is the primary table itself skip it.
        if (tab==table)
            continue;

        // If this join has already be set, skip it.
        if (joins.Contains(Form(" %s ", tab.Data())))
            continue;

        // Now get the primary key of the table to be joined
        const TString prim = GetPrimaryKeys(tab);
        if (prim.IsNull())
            continue;

        joins += Form("LEFT JOIN %s USING (%s) ", tab.Data(), prim.Data());
    }

    if (!joins.IsNull())
        joins += " ";

    return joins;
}

MTreeSQL *MSQLServer::GetTree(TString table, TString addon)
{
    if (fType!=kIsServer || !fServ || !TestBit(kIsOwner))
        return 0;

    TIter Next(&fTrees);
    TObject *o = 0;
    while ((o=Next()))
    {
        if (o->GetName()==table && o->GetName()==addon)
            return (MTreeSQL*)o;
    }

    MTreeSQL *t = new MTreeSQL(this, table, addon);
    fTrees.Add(t);

    return t;
}

void MSQLServer::RecursiveRemove(TObject *obj)
{
    if (fServ==obj)
    {
        fServ=NULL;
        fType = kIsZombie;
        ResetBit(kIsOwner);
    }
}
