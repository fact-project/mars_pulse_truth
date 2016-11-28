// @(#)root/tree:$Id: MTreeSQL.h 30663 2009-10-11 22:11:51Z pcanal $
// Author: Rene Brun   12/01/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_MTreeSQL
#define ROOT_MTreeSQL

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MTreeSQL                                                             //
//                                                                      //
// A TTree object is a list of TBranch.                                 //
//   To Create a TTree object one must:                                 //
//    - Create the TTree header via the TTree constructor               //
//    - Call the TBranch constructor for every branch.                  //
//                                                                      //
//   To Fill this object, use member function Fill with no parameters.  //
//     The Fill function loops on all defined TBranch.                  //
//                                                                      //
// MTreeSQL is the TTree implementation interfacing with an SQL         //
// database                                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TTree
#include "TTree.h"
#endif

#include <vector>

class MSQLServer;
class TSQLRow;

class MTreeSQL : public TTree
{
protected:
   MSQLServer *fServer;

   TString     fQuery;

   TSQLResult *fResult;
   TSQLRow    *fRow;

   std::vector<TString>  fTables;
   std::vector<TSQLRow*> fRows;

   Bool_t CheckTable(const TString &table) const;
   void   Init();

   virtual TBranch *BranchImp(const char *branchname, const char *classname, TClass *ptrClass, void *addobj, Int_t bufsize, Int_t splitlevel);
   virtual TBranch *BranchImp(const char *branchname, TClass *ptrClass, void *addobj, Int_t bufsize, Int_t splitlevel);

public:
   MTreeSQL(MSQLServer *server, const TString& table, const TString &addon="");
   ~MTreeSQL();

   Int_t    Branch(TCollection *list, Int_t bufsize=32000, Int_t splitlevel=99, const char *name="");
   Int_t    Branch(TList *list, Int_t bufsize=32000, Int_t splitlevel=99);
   Int_t    Branch(const char *folder, Int_t bufsize=32000, Int_t splitlevel=99);
   TBranch *Bronch(const char *name, const char *classname, void *addobj, Int_t bufsize=32000, Int_t splitlevel=99);
   TBranch *BranchOld(const char *name, const char *classname, void *addobj, Int_t bufsize=32000, Int_t splitlevel=1);
   TBranch *Branch(const char *name, void *address, const char *leaflist, Int_t bufsize) { return TTree::Branch(name, address, leaflist, bufsize); }
#if !defined(__CINT__)
   TBranch *Branch(const char *name, const char *classname, void *addobj, Int_t bufsize=32000, Int_t splitlevel=99);
#endif

   Int_t    Fill();
   Long64_t GetEntries()    const;
   Long64_t GetEntries(const char *sel) { return TTree::GetEntries(sel); }
   Long64_t GetEntriesFast()const;
   Long64_t LoadTree(Long64_t entry);

   void     Refresh();

   ClassDef(MTreeSQL,1);  // TTree Implementation read and write to a SQL database.
};


#endif
