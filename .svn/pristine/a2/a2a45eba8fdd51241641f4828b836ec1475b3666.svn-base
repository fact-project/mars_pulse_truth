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
!   Author(s): Thomas Bretz 7/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2006
!
!
\* ======================================================================== */

////////////////////////////////////////////////////////////////////////
//
//  MSQLMagic
//
// This is an enhancement of MSQLServer especially made the feature
// the interfaction with our database.
//
////////////////////////////////////////////////////////////////////////
#include "MSQLMagic.h"

#include <stdlib.h> // atoi (Ubuntu 8.10)

#include <iostream>

#include <TSQLRow.h>
#include <TSQLResult.h>

ClassImp(MSQLMagic);

using namespace std;

// --------------------------------------------------------------------------
//
// Return the name corresponding to a key. If col starts with f or
// end with KEY it is stripped.
//
//  If the query fails an empty string is returned.
//
//  On success the name of the key is returned.
//
TString MSQLMagic::QueryValOf(TString col, const char *ext, const char *key)
{
    if (col.EndsWith("KEY"))
        col.Remove(col.Length()-3);
    if (col.BeginsWith("f"))
        col.Remove(0, 1);

    const TString query=Form("SELECT f%s%s FROM %s WHERE f%sKEY=%s",
                             col.Data(), ext, col.Data(), col.Data(), key);

    TSQLResult *res = Query(query);
    if (!res)
        return "";

    TSQLRow *row=res->Next();

    const TString rc = row ? (*row)[0] : "";

    if (row)
        delete row;

    delete res;
    return rc;
}

// --------------------------------------------------------------------------
//
// Return the name corresponding to a key. If col starts with f or
// end with KEY it is stripped.
//
//  If the query fails an empty string is returned.
//
//  On success the name of the key is returned.
//
TString MSQLMagic::QueryNameOfKey(TString col, const char *key)
{
    return QueryValOf(col, "Name", key);
}

// --------------------------------------------------------------------------
//
// Return the value corresponding to a key. If col starts with f or
// end with KEY it is stripped.
//
//  If the query fails an empty string is returned.
//
//  On success the value of the key is returned.
//
TString MSQLMagic::QueryValOfKey(TString col, const char *key)
{
    return QueryValOf(col, "", key);
}

// --------------------------------------------------------------------------
//
//  return the key of f[col]KEY where f[col][ext]=[val]
//
//  return -1 if the query failed or the KEY was not found
//  return  0 if the KEY could not be determined after inserting
//  return the KEY in case of success
//
Int_t MSQLMagic::QueryKeyOf(const char *col, const char *ext, const char *val)
{
    const TString query1 = Form("SELECT f%sKEY FROM %s WHERE f%s%s='%s'",
                                col, col, col, ext, val);

    TSQLResult *res1 = Query(query1);
    if (!res1)
    {
        cout << "ERROR - Query has failed: " << query1 << endl;
        return -1;
    }

    TSQLRow *row=res1->Next();

    const Int_t rc1 = row && (*row)[0] ? atoi((*row)[0]) : -1;

    if (row)
        delete row;

    delete res1;

    return rc1;
}

// --------------------------------------------------------------------------
//
//  return the key of f[col]KEY where f[col]=[val]
//
//  return -1 if the query failed or the KEY was not found
//  return  0 if the KEY could not be determined after inserting
//  return the KEY in case of success
//
Int_t MSQLMagic::QueryKeyOfVal(const char *col, const char *val)
{
    return QueryKeyOf(col, "", val);
}

// --------------------------------------------------------------------------
//
//  return the key of f[col]KEY where f[col]Name=[name]
//
//  if value [name] is not existing, insert value (creates anew key)
//  and return the new key
//
//  return -1 if the query failed or the KEY was not found (insert=kFALSE)
//  return  0 if the KEY could not be determined after inserting
//  return the KEY in case of success
//
Int_t MSQLMagic::QueryKeyOfName(const char *col, const char *name, Bool_t insert)
{
    const Int_t rc1 = QueryKeyOf(col, "Name", name);

    if (rc1>=0)
        return rc1;

    if (!insert)
        return -1;

    //insert new value
    const Int_t rc2 = Insert(col, Form("f%sName=\"%s\"", col, name));
    if (rc2<0)       // Dummy mode
        return 0;
    if (rc2==kFALSE) // Query failed
        return -1;

    const Int_t key = QueryKeyOfName(col, name, kFALSE);
    if (key>0)
    {
        cout << " - New " << col << ": " << name << endl;
        return key;
    }

    return 0;
}

// --------------------------------------------------------------------------
//
// Check if the column of an entry in a table is existing and not null.
// The entry is defined by column=test (column name) and a optional
// WHERE statement.
//
Bool_t MSQLMagic::ExistStr(const char *column, const char *table, const char *test, const char *where)
{
    TString query = test ?
        Form("SELECT %s FROM %s WHERE %s='%s' %s %s", column, table, column, test, where?"AND":"", where?where:"") :
        Form("SELECT %s FROM %s WHERE %s", column, table, where);

    TSQLResult *res = Query(query);
    if (!res)
        return kFALSE;

    Bool_t rc = kFALSE;

    TSQLRow *row=res->Next();
    if (row && (*row)[0])
        rc=kTRUE;

    if (row)
        delete row;

    delete res;
    return rc;
}

// --------------------------------------------------------------------------
//
// Check if at least one row with one field exists in table
// defined by where
//
Bool_t MSQLMagic::ExistRow(const char *table, const char *where)
{
    return ExistStr("*", table, 0, where);
}

// --------------------------------------------------------------------------
//
// An abbreviation for an Insert-Query.
//
// It builds "INSERT table SET vars"
// The whitespaces are already conatined.
//
// On success kTRUE is returned, kFALSE otherwise.
// In Dummy mode no query is send an -1 is returned.
//
Int_t MSQLMagic::Insert(const char *table, const char *vars, const char *where)
{
    // Build query
    TString query("INSERT ");
    query += table;
    query += " SET ";
    query += vars;
    if (!TString(where).IsNull())
    {
        query += ", ";
        query += where;
    }

    // Check for dummy mode
    if (fIsDummy)
    {
        cout << "MSQLMagic - DUMMY: " << query << endl;
        return -1;
    }

    // Execute query
    if (Exec(query))
        return kTRUE;

    // Return error on failure
    cout << "Error - Insert failed: " << query << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// An abbreviation for an Update-Query.
//
// It builds "UPDATE table SET vars WHERE where"
// The whitespaces are already conatined.
//
// On success kTRUE is returned, kFALSE otherwise.
// In Dummy mode no query is send an -1 is returned.
//
Int_t MSQLMagic::Update(const char *table, const char *vars, const char *where)
{
    // Build query
    TString query("UPDATE ");
    query += table;
    query += " SET ";
    query += vars;

    if (!TString(where).IsNull())
    {
        query += " WHERE ";
        query += where;
    }

    // Check for dummy mode
    if (fIsDummy)
    {
        cout << "MSQLMagic - DUMMY: " << query << endl;
        return -1;
    }

    // Execute Query
    if (Exec(query))
        return kTRUE;

    // return kFALSE on failure
    cout << "Error - Update failed: " << query << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// An abbreviation for checking the existance with ExistStr and
// calling insert or update respectively.
//
Int_t MSQLMagic::InsertUpdate(const char *table, const char *col, const char *val, const char *vars)
{
    return ExistStr(col, table, val) ?
        Update(table, vars, Form("%s='%s'", col, val)) :
        Insert(table, vars, Form("%s='%s'", col, val));
}

// --------------------------------------------------------------------------
//
// An abbreviation for checking whether a row with the condition where
// exists. If no such row exist Insert vars into table, otherwise update
// vars in table at row(s) defined by where.
//
Int_t MSQLMagic::InsertUpdate(const char *table, const char *vars, const char *where)
{
    return ExistRow(table, where) ?
        Update(table, vars, where) :
        Insert(table, vars);
}

// --------------------------------------------------------------------------
//
// An abbreviation for a Dalete-Query.
//
// It builds "DELETE FROM table WHERE where"
// The whitespaces are already conatined.
//
// On success kTRUE is returned, kFALSE otherwise.
// In Dummy mode no query is send an -1 is returned.
//
Int_t MSQLMagic::Delete(const char *table, const char *where)
{
    // Build query
    TString query("DELETE FROM ");
    query += table;
    query += " WHERE ";
    query += where;

    // Check for dummy mode
    if (fIsDummy)
    {
        cout << "MSQLMagic - DUMMY: " << query << endl;
        return -1;
    }

    // Execute query
    if (Exec(query))
        return kTRUE;

    // return kFALSE on failure
    cout << "Error - Delete failed: " << query << endl;
    return kFALSE;
}

