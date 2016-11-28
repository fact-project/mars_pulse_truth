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
!   Author(s): Thomas Bretz, 7/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2003-2008
!
!
\* ======================================================================== */


//////////////////////////////////////////////////////////////////////////////
//
// MArgs
//
// This is a helper class for executables to parse command line arguments
//
// Arguments beginning with a trailing '-' are called 'options'.
//
// Arguments without a trailing '-' are considered 'arguments'
//
// All arguments appearing after '--' on the commandline are
//  also cosidered as 'arguments'
//
//////////////////////////////////////////////////////////////////////////////
#include "MArgs.h"

#include <stdlib.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MArgsEntry);
ClassImp(MArgs);

using namespace std;

void MArgsEntry::Print(const Option_t *) const
{
    gLog << all << *this << endl;
}

// --------------------------------------------------------------------------
//
// Initializes:
//  fName: The name of the executable
//  fArgv: A TList containing all other command line arguments
//
MArgs::MArgs(int argc, char **argv) : fArgc(argc)
{
    TString cmdline;
    // FIXME: argv has no const-qualifier to be idetical with
    //        TApplication.
    fName  = argv[0];
    fTitle = argv[0];

    fArgv.SetOwner();

    for (int i=1; i<argc; i++)
    {
        fTitle += " ";
        fTitle += argv[i];
        fArgv.Add(new MArgsEntry(argv[i]));
    }
}

// --------------------------------------------------------------------------
//
//  Remove all root commandline options from the list,
//  namely: -b, -n, -q, -l, -?, -h, --help, -config
//
//  Returns the number of found root options (max 8)
//
Int_t MArgs::RemoveRootArgs()
{
    Int_t n = 0;

    n += HasOnlyAndRemove("-b");
    n += HasOnlyAndRemove("-n");
    n += HasOnlyAndRemove("-q");
    n += HasOnlyAndRemove("-l");
    n += HasOnlyAndRemove("-?");
    n += HasOnlyAndRemove("-h");
    n += HasOnlyAndRemove("--help");
    n += HasOnlyAndRemove("-config");

    return n;
}

// --------------------------------------------------------------------------
//
// Print everything parsed.
// Using 'options' as option only 'options' are printed.
// Using 'arguments' as option only 'arguments' are printed.
// Using 'cmdline' as option the command line is printed instead of
//       just the program name
//
void MArgs::Print(const Option_t *o) const
{
    TString str(o);

    gLog << all << underline;
    if (str.Contains("cmdline"))
        gLog << fTitle << endl;
    else
        gLog << fName << ":" << endl;

    str.ReplaceAll("cmdline", "");
    str.ReplaceAll(" ", "");

    if (!str.CompareTo("options", TString::kIgnoreCase))
    {
        TIter Next(&fArgv);
        TString *s = NULL;
        while ((s=dynamic_cast<TString*>(Next())))
            if (*s!="--" && s->BeginsWith("-"))
                gLog << *s << endl;
        return;
    }

    if (!str.CompareTo("arguments", TString::kIgnoreCase))
    {
        TIter Next(&fArgv);
        TString *s = NULL;
        while ((s=dynamic_cast<TString*>(Next())))
            if (*s!="--" && !s->BeginsWith("-"))
                gLog << *s << endl;
        return;
    }

    fArgv.Print();
}

// --------------------------------------------------------------------------
//
// Return the Integer corresponding to the command line argument 'name'
//  eg. executable -argument 5
//      GetInt("argument") will return 5
//
Int_t MArgs::GetInt(const TString name) const
{
    return atoi(GetString(name));
}

// --------------------------------------------------------------------------
//
// Return the floating point value corresponding to the command line argument
// 'name'
//  eg. executable -argument 5.7
//      GetFloat("argument") will return 5.7
//
Double_t MArgs::GetFloat(const TString name) const
{
    return atof(GetString(name));
}

// --------------------------------------------------------------------------
//
// Return the TString corresponding to the command line argument 'name'
//  eg. executable -argument=last
//      GetString("-argument=") will return "last"
//
TString MArgs::GetString(const TString name) const
{
    TIter Next(&fArgv);
    TString *s = NULL;
    while ((s=dynamic_cast<TString*>(Next())))
        if (s->BeginsWith(name))
            return s->Data()+s->Index(name)+name.Length();
    return "";
}

// --------------------------------------------------------------------------
//
// Return the Integer corresponding to the command line argument 'name'
//  eg. executable -argument5
//      GetIntAndRemove("-argument") will return 5
// and removes the argument from the internal list.
//
Int_t MArgs::GetIntAndRemove(const TString name)
{
    return atoi(GetStringAndRemove(name));
}

// --------------------------------------------------------------------------
//
// Return the floating point value corresponding to the command line argument
// 'name'
//  eg. executable -argument5.7
//      GetFloatAndRemove("-argument") will return 5.7
// and removes the argument from the internal list.
//
Double_t MArgs::GetFloatAndRemove(const TString name)
{
    return atof(GetStringAndRemove(name));
}

// --------------------------------------------------------------------------
//
// Return the TString corresponding to the command line argument 'name'
//  eg. executable -argument=last
//      GetStringAndRemove("-argument=") will return "last"
// and removes the argument from the internal list.
//
TString MArgs::GetStringAndRemove(const TString n)
{
    const TString name = n.Strip(TString::kBoth);

    TIter Next(&fArgv);
    TString *s = NULL;
    while ((s=dynamic_cast<TString*>(Next())))
        if (s->BeginsWith(name))
        {
            TString str = s->Data()+s->Index(name)+name.Length();
            delete fArgv.Remove(dynamic_cast<TObject*>(s));
            return str;
        }
    return "";
}

// --------------------------------------------------------------------------
//
// Return the Integer corresponding to the i-th argument. This is ment
// for enumerations like
//  executable 1 7 2
//  GetArgumentInt(1) will return 7
//
Int_t MArgs::GetArgumentInt(Int_t i) const
{
    return atoi(GetArgumentStr(i));
}

// --------------------------------------------------------------------------
//
// Return the floating point value corresponding to the i-th argument.
// This is ment for enumerations like
//  executable 1.7 7.5 2.3
//  GetArgumentFloat(1) will return 7.5
//
Float_t MArgs::GetArgumentFloat(Int_t i) const
{
    return atof(GetArgumentStr(i));
}

// --------------------------------------------------------------------------
//
// Returns GetIntAndRemove. If HasOption returns kFALSE def is returned.
//
Int_t MArgs::GetIntAndRemove(const TString name, Int_t def)
{
    if (!HasOption(name))
        return def;
    return GetIntAndRemove(name);
}

// --------------------------------------------------------------------------
//
// Returns GetFloatAndRemove. If HasOption returns kFALSE def is returned.
//
Double_t MArgs::GetFloatAndRemove(const TString name, Double_t def)
{
    if (!HasOption(name))
        return def;
    return GetFloatAndRemove(name);
}

// --------------------------------------------------------------------------
//
// Returns GetStringAndRemove. If HasOption returns kFALSE def is returned.
//
TString MArgs::GetStringAndRemove(const TString name, const TString def)
{
    if (!HasOption(name))
        return def;
    return GetStringAndRemove(name);
}

// --------------------------------------------------------------------------
//
// Return a pointer to the MArgsEntry of the i-th argument
// This is ment for enumerations like
//  executable file1 file2 file3
//  GetArgumentStr(1) will return "file2"
// Only arguments without a trailing '-' are considered
//
MArgsEntry *MArgs::GetArgument(Int_t i) const
{
    Int_t num = 0;

    Bool_t allarg = kFALSE;

    TIter Next(&fArgv);
    MArgsEntry *e = NULL;
    while ((e=static_cast<MArgsEntry*>(Next())))
    {
        const TString &s=*dynamic_cast<TString*>(e);

        if (s=="--")
        {
            allarg = kTRUE;
            continue;
        }

        if (s.BeginsWith("-") && !allarg)
            continue;

        if (i==num++)
            return e;
    }

    return 0;
}

// --------------------------------------------------------------------------
//
// Return the TString corresponding to the i-th argument.
// This is ment for enumerations like
//  executable file1 file2 file3
//  GetArgumentStr(1) will return "file2"
// Only arguments without a trailing '-' are considered
//
TString MArgs::GetArgumentStr(Int_t i) const
{
    const MArgsEntry *e = GetArgument(i);
    return e==0 ? "" : dynamic_cast<const TString&>(*e);
}

// --------------------------------------------------------------------------
//
// return the number of arguments without a trainling '-'
//
Int_t MArgs::GetNumArguments() const
{
    Int_t num = 0;

    Bool_t allarg = kFALSE;

    TIter Next(&fArgv);
    TString *s = NULL;
    while ((s=dynamic_cast<TString*>(Next())))
    {
        if (*s=="--")
        {
            allarg = kTRUE;
            continue;
        }

        if (s->BeginsWith("-") && !allarg)
            continue;

        num++;
    }

    return num;
}

// --------------------------------------------------------------------------
//
// Remove the i-th argument from the list. Return kTRUE in case of sucess
// kFALSE otherwise
//
Bool_t MArgs::RemoveArgument(Int_t i)
{
    MArgsEntry *e = GetArgument(i);
    if (!e)
        return kFALSE;

    delete fArgv.Remove(e);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// return the number of arguments with a trainling '-'
//
Int_t MArgs::GetNumOptions() const
{
    Int_t num = 0;

    TIter Next(&fArgv);
    TString *s = NULL;
    while ((s=dynamic_cast<TString*>(Next())))
    {
        if (*s=="--")
            return num;

        if (s->BeginsWith("-"))
            num++;
    }

    return num;
}

// --------------------------------------------------------------------------
//
// return the total number of entries
//
Int_t MArgs::GetNumEntries() const
{
    return fArgv.FindObject("--") ? fArgv.GetSize()-1 : fArgv.GetSize();
}

// --------------------------------------------------------------------------
//
// Checks whether an argument beginning with 'n' is existing, eg:
//  executable -value5
//  executable -value
//  HasOption("-value") will return true in both cases
//
Bool_t MArgs::Has(const TString n) const
{
    const TString name = n.Strip(TString::kBoth);

    TIter Next(&fArgv);
    TString *s = NULL;
    while ((s=dynamic_cast<TString*>(Next())))
        if (s->BeginsWith(name))
            return kTRUE;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Checks whether an argument beginning with 'n' is existing, eg:
//  executable -value5
//   HasOption("-value") will return false
//  executable -value
//   HasOption("-value") will return true
//
Bool_t MArgs::HasOnly(const TString n) const
{
    const TString name = n.Strip(TString::kBoth);

    TIter Next(&fArgv);
    TString *s = NULL;
    while ((s=dynamic_cast<TString*>(Next())))
        if (*s==name)
            return kTRUE;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Checks whether an argument beginning with 'n' is exists and a
// corresponding option is available, eg.
//  executable -value5
//  HasOption("-value") will return true
// but:
//  executable -value
//  HasOption("-value") will return false
//
Bool_t MArgs::HasOption(const TString n) const
{
    const TString name = n.Strip(TString::kBoth);

    TIter Next(&fArgv);
    TString *s = NULL;
    while ((s=dynamic_cast<TString*>(Next())))
        if (s->BeginsWith(name) && s->Length()>name.Length())
            return kTRUE;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Checks whether an argument beginning with 'n' is exists and a
// corresponding option is available, eg.
//  executable -value5
//  HasOption("-value") will return false
// but:
//  executable -value
//  HasOption("-value") will return true
//
// The argument is removed from the internal list.
//
Bool_t MArgs::HasOnlyAndRemove(const TString n)
{
    const TString name = n.Strip(TString::kBoth);

    Bool_t rc = kFALSE;

    TIter Next(&fArgv);
    TString *s = NULL;
    while ((s=dynamic_cast<TString*>(Next())))
        if (*s==name)
        {
            delete fArgv.Remove(dynamic_cast<TObject*>(s));
            rc = kTRUE;
        }

    return rc;
}

// --------------------------------------------------------------------------
//
// Return all arguments and options in the order as they are stored
// in memory.
//
TString MArgs::GetCommandLine() const
{
    TString rc;

    TIter Next(&fArgv);
    TString *s = NULL;
    while ((s=dynamic_cast<TString*>(Next())))
    {
        rc += *s;
        rc += " ";
    }

    return rc.Strip(TString::kBoth);
}
