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
!   Author(s): Thomas Bretz, 6/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MDirIter
//
// Iterator for files in several directories (with filters)
//
// Use this class if you want to get all filenames in a directory
// one-by-one.
//
// You can specify more than one directory (also recursivly) and you
// can use filters (eg. *.root)
//
// Here is an example which will print all *.root files in the current
// directory and all its subdirectories and all Gamma*.root files in
// the directory ../data.
//
// ------------------------------------------------------------------------
//
//  // Instatiate the iterator
//  MDirIter Next();
//
//  // Add the current directory (for *.root files) and recursive
//  // directories with the same filter
//  Next.AddDirectory(".", "*.root", kTRUE);
//  // Add the directory ../data, too (filter only Gamma*.root files)
//  Next.AddDirectory("../data", "Gamma*.root");
//
//  TString name;
//  while (!(name=Next()).IsNull())
//     cout << name << endl;
//
// ------------------------------------------------------------------------
//
// WARNING: If you specify relative directories (like in the example) the
//          result may depend on the current working directory! Better use
//          absolute paths.
//
/////////////////////////////////////////////////////////////////////////////
#include "MDirIter.h"

#include <fstream>
#include <iostream>

#include <TList.h>
#include <TNamed.h>
#include <TRegexp.h>
#include <TPRegexp.h>
#include <TSystem.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MDirIter);

using namespace std;

// --------------------------------------------------------------------------
//
//  Add a directory, eg dir="../data"
//  Using a filter (wildcards) will only return files matching this filter.
//  recursive is the number of recursive directories (use 0 for none and -1
//  for all)
//  Returns the number of directories added.
//  If a directory is added using a filter and the directory is already
//  existing without a filter the filter is replaced.
//  If any directory to be added is already existing with a different
//  filter a new entry is created, eg:
//   already existing:  ../data <*.root>
//   new entry:         ../data <G*>
//  The filters are or'ed.
//
Int_t MDirIter::AddDirectory(const char *d, const char *filter, Int_t recursive)
{
    TString dir(d);

    // Sanity check
    if (dir.IsNull())
        return 0;

#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,05)
    if (dir[dir.Length()-1]!='/')
        dir += '/';
#else
    if (!dir.EndsWith("/"))
        dir += '/';
#endif
    gSystem->ExpandPathName(dir);

    // Try to find dir in the list of existing dirs
    TObject *o = fList.FindObject(dir);
    if (o)
    {
        const TString t(o->GetTitle());

        // Check whether the existing dir has an associated filter
        if (t.IsNull())
        {
            // Replace old filter by new one
            ((TNamed*)o)->SetTitle(filter);
            return 0;
        }

        // If the filters are the same no action is taken
        if (t==filter)
            return 0;
    }

    fList.Add(new TNamed((const char*)dir, filter ? filter : ""));

    // No recuresive directories, return
    if (recursive==0)
        return 1;

    Int_t rc = 1;

    // Create an iterator to iterate over all entries in the directory
    MDirIter NextD(dir);

    TString c;
    while (!(c=NextD(kTRUE)).IsNull())
    {
        // Do not process . and .. entries
        if (c.EndsWith("/.") || c.EndsWith("/.."))
            continue;

        // If entry is a directory add it with a lower recursivity
        if (IsDir(c)==0)
            rc += AddDirectory(c, filter, recursive-1);
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// Add a single file to the iterator
//
Int_t MDirIter::AddFile(const char *name)
{
    return AddDirectory(gSystem->DirName(name), gSystem->BaseName(name));
}

// --------------------------------------------------------------------------
//
// Adds all entries from iter to this object
//
void MDirIter::Add(const MDirIter &iter)
{
    TIter NextD(&iter.fList);
    TObject *o=0;
    while ((o=NextD()))
        fList.Add(o->Clone());
}

Int_t MDirIter::ReadList(const char *name, const TString &path)
{
    const char *con = gSystem->ConcatFileName(path.Data(), name);
    TString fname(path.IsNull() ? name : con);
    delete [] con;

    gSystem->ExpandPathName(fname);

    ifstream fin(fname);
    if (!fin)
    {
        gLog << err << "Cannot open file " << fname << ": ";
        gLog << strerror(errno) << " [errno=" << errno << "]" <<endl;
        return -1;
    }

    Int_t rc = 0;

    while (1)
    {
        TString line;
        line.ReadLine(fin);
        if (!fin)
            break;

        line = line.Strip(TString::kBoth);
        if (line[0]=='#' || line.IsNull())
            continue;

        TObjArray &arr = *line.Tokenize(' ');

        for (int i=1; i<arr.GetEntries(); i++)
            rc += AddDirectory(arr[0]->GetName(), arr[i]->GetName(), -1);

        delete &arr;
    }

    return true;
}

// --------------------------------------------------------------------------
//
//  Return the pointer to the current directory. If the pointer is NULL
//  a new directory is opened. If no new directory can be opened NULL is
//  returned.
//
void *MDirIter::Open()
{
    // Check whether a directory is already open
    if (fDirPtr)
        return fDirPtr;

    // Get Next entry of list
    fCurrentPath=fNext();

    // Open directory if new entry was found
    return fCurrentPath ? gSystem->OpenDirectory(fCurrentPath->GetName()) : NULL;
}

// --------------------------------------------------------------------------
//
//  Close directory is opened. Set fDirPtr=NULL
//
void MDirIter::Close()
{
    if (fDirPtr)
        gSystem->FreeDirectory(fDirPtr);
    fDirPtr = NULL;
}

// --------------------------------------------------------------------------
//
//  Returns the concatenation of 'dir' and 'name'
//
TString MDirIter::ConcatFileName(const char *dir, const char *name) const
{
    return TString(dir)+name;
}

// --------------------------------------------------------------------------
//
// Check whether the given name n matches the filter f.
//
// If the filter is encapsulated in ^ and $ it is assumed to be
// a valid regular expression and TPRegexp is used for filtering.
//
// In other cases TRegex is used:
//
// As the filter string may contain a + character, we have to replace
// this filter by a new filter contaning a \+ at all locations where a +
// was in the original filter.
//
// We replace:
//   .  by  \\.
//   +  by  \\+
//   *  by  [^\\/:]*
//
// And surround the filter by ^ and $.
//
// For more details you can have a look at the template:
//  TRegexp::MakeWildcard
//
Bool_t MDirIter::MatchFilter(const TString &n, TString f) const
{
    if (f.IsNull())
        return kTRUE;

    if (f[0]=='^' && f[f.Length()-1]=='$')
    {
        TPRegexp regex(f);
        return !n(regex).IsNull();
    }

    f.Prepend("^");
    f.ReplaceAll(".", "\\.");
    f.ReplaceAll("+", "\\+");
    f.ReplaceAll("*", "[^\\/:]*");
    //f.ReplaceAll("?", "[^\\/:]?");
    f.Append("$");

    return !n(TRegexp(f)).IsNull();
}

// --------------------------------------------------------------------------
//
// Check whether fqp is a directory.
// Returns -1 if fqp couldn't be accesed, 0 if it is a directory,
// 1 otherwise
//
Int_t MDirIter::IsDir(const char *fqp) const
{
    Long_t t[4];
    if (gSystem->GetPathInfo(fqp, t, t+1, t+2, t+3))
        return -1;

    if (t[2]==3)
        return 0;

    return 1;
}

// --------------------------------------------------------------------------
//
// Check whether the current entry in the directory n is valid or not.
// Entries must:
//  - not be . or ..
//  - match the associated filter
//  - match the global filter
//  - not be a directory
//  - have read permission
//
Bool_t MDirIter::CheckEntry(const TString n) const
{
    // Check . and ..
    if (n=="." || n=="..")
        return kFALSE;

    // Check associated filter
    if (!MatchFilter(n, fCurrentPath->GetTitle()))
        return kFALSE;

    // Check global filter
    if (!MatchFilter(n, fFilter))
         return kFALSE;

    // Check for file or directory
    const TString fqp = ConcatFileName(fCurrentPath->GetName(), n);
    if (IsDir(fqp)<=0)
        return kFALSE;

    // Check for rread perissions
    return !gSystem->AccessPathName(fqp, kReadPermission);
}

// --------------------------------------------------------------------------
//
// Reset the iteration and strat from scratch. To do this correctly we have
// to reset the list of directories to iterate _and_ to close the current
// directory. When you call Next() the next time the first directory will
// be reopened again and you'll get the first entry.
//
// Do not try to only close the current directory or to reset the directory
// list only. This might not give the expected result!
//
void  MDirIter::Reset()
{
    Close();
    fNext.Reset();
}

// --------------------------------------------------------------------------
//
// Return the Next file in the directory which is valid (see Check())
// nocheck==1 returns the next entry unchecked
//
TString MDirIter::Next(Bool_t nocheck)
{
    while (1)
    {
        fDirPtr = Open();
        if (!fDirPtr)
            break;

        // Get next entry in dir, if existing check validity
        const char *n = gSystem->GetDirEntry(fDirPtr);
        if (!n)
        {
            // Otherwise close directory and try to get next entry
            Close();
            continue;
        }

        if (nocheck || CheckEntry(n))
            return ConcatFileName(fCurrentPath->GetName(), n);
    }

    return "";
}

// --------------------------------------------------------------------------
//
// Print a single entry in the list
//
void MDirIter::PrintEntry(const TObject &o) const
{
    TString p = o.GetName();
    TString f = o.GetTitle();
    cout << p;
    if (!f.IsNull())
        cout << " <" << f << ">";
    cout << endl;
}

// --------------------------------------------------------------------------
//
// Print all scheduled directories. If "all" is specified also all
// matching entries are printed.
//
void MDirIter::Print(const Option_t *opt) const
{
    TString s(opt);
    if (s.Contains("dbg", TString::kIgnoreCase))
        fList.Print();

    if (!s.Contains("all", TString::kIgnoreCase))
    {
        TIter NextD(&fList);
        TObject *o=NULL;
        while ((o=NextD()))
            PrintEntry(*o);
        return;
    }

    MDirIter NextD(*this);
    TString name;
    TString d;
    while (!(name=NextD()).IsNull())
    {
        const TString p = NextD.fCurrentPath->GetName();
        if (p!=d)
        {
            d=p;
            PrintEntry(*NextD.fCurrentPath);
        }
        cout << " " << name << endl;
    }
}

// --------------------------------------------------------------------------
//
// Loop over all contents (files). Sort the files alphabetically.
// Delete the contents of this DirIter and add all sorted files
// to this DirIter.
//
void MDirIter::Sort()
{
    MDirIter NextD(*this);

    TList l;
    l.SetOwner();

    TString name;
    while (!(name=NextD()).IsNull())
        l.Add(new TNamed(name.Data(), ""));

    l.Sort();

    fList.Delete();
    Close();
    fFilter = "";

    TIter NextN(&l);
    TObject *o=0;
    while ((o=NextN()))
    {
        TString dir   = o->GetName();
        TString fname = o->GetName();

        const Int_t last = dir.Last('/');
        if (last<0)
            continue;

        dir.Remove(last);
        fname.Remove(0, last+1);

        AddDirectory(dir, fname);
    }
}
