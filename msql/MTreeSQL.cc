// @(#)root/tree:$Id: MTreeSQL.cxx 43270 2012-03-06 22:46:11Z pcanal $
// Author: Philippe Canal and al. 08/2004

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MTreeSQL                                                             //
//                                                                      //
// Implement TTree for a SQL backend                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <Riostream.h>
#include <map>
#include <stdlib.h>

#include "TString.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TError.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TBranch.h"
#include "TPRegexp.h"

#include "TSQLRow.h"
#include "TSQLResult.h"

#include "MSQLServer.h"

#include "MTreeSQL.h"
#include "MBasketSQL.h"

ClassImp(MTreeSQL);

using namespace std;

//______________________________________________________________________________
MTreeSQL::MTreeSQL(MSQLServer *server, const TString& table, const TString &addon) :
    TTree(table, addon, 0), fServer(server), fQuery(table+" "+addon), fResult(0), fRow(0)
{
    fEntries = 0;

    if (fServer==0)
    {
        Error("MTreeSQL","No TSQLServer specified");
        return;
    }

    fTables.push_back(table);

    // Constructor with an explicit TSQLServer
    TPRegexp reg(" +JOIN +([a-zA-Z0-9_.]+)");

    Int_t p = -1;
    while (p<addon.Length())
    {
        TArrayI pos;
        if (reg.Match(addon, "ig", ++p, 100, &pos)!=2)
            continue;

        if (pos[2] >= 0 && pos[3] >= 0)
        {
            const TString subStr = addon(pos[2], pos[3]-pos[2]);
            fTables.push_back(subStr);
        }

        p = pos[3];
    }

    for (auto it=fTables.begin(); it!=fTables.end(); it++)
        if (!CheckTable(*it))
            return;

    Init();
}

MTreeSQL::~MTreeSQL()
{
    for (auto it=fRows.begin(); it!=fRows.end(); it++)
        delete *it;

    delete fResult;
}

//______________________________________________________________________________
Bool_t MTreeSQL::CheckTable(const TString &table) const
{
    // Check the table exist in the database
    if (fServer==0)
        return kFALSE;

    TSQLResult *tables = fServer->GetTables(table);
    if (!tables)
        return kFALSE;

    TSQLRow * row = 0;
    while ((row = tables->Next()))
    {
        if (table.CompareTo(row->GetField(0),TString::kIgnoreCase)==0)
            return kTRUE;

        delete row;
    }
    delete tables;

    // The table is a not a permanent table, let's see if it is a 'temporary' table
    const Int_t before = gErrorIgnoreLevel;
    gErrorIgnoreLevel = kFatal;

    TSQLResult *res = fServer->GetColumns(table);
    if (res)
        delete res;

    gErrorIgnoreLevel = before;

    if (!res)
        Error("CheckTable", "%s", ("Table "+table+" not found.").Data());

    return res ? kTRUE : kFALSE;
}

//______________________________________________________________________________
void MTreeSQL::Init()
{
   // Initializeation routine
   // Number of entries needed to create the branches
   //GetEntries();

   vector<pair<TString,TString>> cols;

   for (auto it=fTables.begin(); it!=fTables.end(); it++)
   {
       TSQLResult *res = fServer->GetColumns(*it);

       TSQLRow *row = 0;
       while ((row=res->Next()))
       {
           TString f1 = row->GetField(1);

           Int_t p0 = f1.First('(');
           if (p0<0)
               p0 = f1.First(' ');
           if (p0<0)
               p0 = f1.Length();

           const TString type = f1(0, p0);

           if (type.CompareTo("int",       TString::kIgnoreCase) &&
               type.CompareTo("date",      TString::kIgnoreCase) &&
               type.CompareTo("time",      TString::kIgnoreCase) &&
               type.CompareTo("datetime",  TString::kIgnoreCase) &&
               type.CompareTo("year",      TString::kIgnoreCase) &&
               type.CompareTo("timestamp", TString::kIgnoreCase) &&
               type.CompareTo("bit",       TString::kIgnoreCase) &&
               type.CompareTo("tinyint",   TString::kIgnoreCase) &&
               type.CompareTo("smallint",  TString::kIgnoreCase) &&
               type.CompareTo("mediumint", TString::kIgnoreCase) &&
               type.CompareTo("integer",   TString::kIgnoreCase) &&
               type.CompareTo("bigint",    TString::kIgnoreCase) &&
               type.CompareTo("decimal",   TString::kIgnoreCase) &&
               type.CompareTo("int",       TString::kIgnoreCase) &&
               type.CompareTo("real",      TString::kIgnoreCase) &&
               type.CompareTo("float",     TString::kIgnoreCase) &&
               type.CompareTo("double",    TString::kIgnoreCase))
           {
               Info("Init", "%s", ("Skipping "+*it+"."+TString(row->GetField(0))+" ["+type+"]").Data());
               continue;
           }

           const TString name = *it+"."+row->GetField(0);
           cols.push_back(make_pair(name, type));

           delete row;
       }
       delete res;
   }

   TString list;
   for (auto it=cols.begin(); it!=cols.end(); it++)
       list += ","+it->first;

   list.Remove(0, 1);

   fResult = fServer->Query(("SELECT "+list+" FROM "+fQuery).Data());
   if (!fResult)
       return;

   TSQLRow *row = 0;
   while ((row=(TSQLRow*)fResult->Next()))
       fRows.push_back(row);

   fEntries = fRows.size();

   // ==============================================================

   // determine leaf description string
   Int_t idx = 0;
   for (auto it=cols.begin(); it!=cols.end(); it++, idx++)
   {
       const TString name = it->first;

       TBranch *br = TTree::Branch(name, 0, name+"/D");

       br->ResetAddress();
       br->SetEntries(fEntries);

       auto entry = br->GetBasketEntry();
       entry[0] = 0;
       entry[1] = fEntries;

       TBasket *bsk = new MBasketSQL(br, &fRow, idx, it->second);

       br->GetListOfBaskets()->AddAtAndExpand(bsk, 0);
   }
}

//______________________________________________________________________________
Long64_t  MTreeSQL::GetEntries() const
{
   // Get the number of rows in the database
    return GetEntriesFast();
}

//______________________________________________________________________________
Long64_t  MTreeSQL::GetEntriesFast()    const
{
   // Return the number of entries as of the last check.
   // Use GetEntries for a more accurate count.

   return fEntries;
}

//______________________________________________________________________________
Long64_t MTreeSQL::LoadTree(Long64_t entry)
{
    // Setup the tree to the load the specified entry.
    // Make sure the server and result set are setup for the requested entry
    if (entry<0 || entry>=fEntries)
        return -1;

    fReadEntry = entry;
    fRow = fRows[entry];

    return entry;
}

//______________________________________________________________________________
void MTreeSQL::Refresh()
{
    if (!fServer)
        return;

    //  Refresh contents of this Tree and his branches from the current
    //  Tree status in the database
    //  One can call this function in case the Tree on its file is being
    //  updated by another process
    for (auto it=fRows.begin(); it!=fRows.end(); it++)
        delete *it;

    fRows.clear();

    delete fResult;

    Init();
}

//______________________________________________________________________________
TBranch* MTreeSQL::BranchImp(const char *, const char *,
                             TClass *, void *, Int_t ,
                             Int_t )
{
   // Not implemented yet

   Fatal("BranchImp","Not implemented yet");
   return 0;
}

//______________________________________________________________________________
TBranch* MTreeSQL::BranchImp(const char *, TClass *,
                             void *, Int_t , Int_t )
{
   // Not implemented yet

   Fatal("BranchImp","Not implemented yet");
   return 0;
}

//______________________________________________________________________________
Int_t MTreeSQL::Branch(TCollection *, Int_t,
                       Int_t, const char *)
{
   // Not implemented yet

   Fatal("Branch","Not implemented yet");
   return 0;
}

//______________________________________________________________________________
Int_t MTreeSQL::Branch(TList *, Int_t, Int_t)
{
   // Not implemented yet

   Fatal("Branch","Not implemented yet");
   return 0;
}

//______________________________________________________________________________
Int_t MTreeSQL::Branch(const char *, Int_t ,
                       Int_t)
{
   // Not implemented yet

   Fatal("Branch","Not implemented yet");
   return 0;
}

//______________________________________________________________________________
TBranch* MTreeSQL::Bronch(const char *, const char *, void *,
                          Int_t, Int_t)
{
   // Not implemented yet

   Fatal("Bronc","Not implemented yet");
   return 0;
}

//______________________________________________________________________________
TBranch* MTreeSQL::BranchOld(const char *, const char *,
                             void *, Int_t, Int_t)
{
   // Not implemented yet

   Fatal("BranchOld","Not implemented yet");
   return 0;
}

//______________________________________________________________________________
TBranch *MTreeSQL::Branch(const char *, const char *, void *,
                          Int_t, Int_t)
{
   // Not implemented yet

   Fatal("Branch","Not implemented yet");
   return 0;
}

//______________________________________________________________________________
Int_t MTreeSQL::Fill()
{
   Fatal("Fill","Not implemented yet");
   return -1;
}
