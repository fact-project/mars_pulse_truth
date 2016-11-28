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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MRead
//
// Base class for all reading tasks
//
// You can set a selector. Depending on the impelementation in the derived
// class it can be used to skip events, if the filter return kFALSE.
// Make sure that the selector (filter) doesn't need information which
// doesn't exist before reading an event!
//
/////////////////////////////////////////////////////////////////////////////
#include "MRead.h"

#include <TSystem.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MDirIter.h"

ClassImp(MRead);

using namespace std;

Bool_t MRead::Rewind()
{
    *fLog << err << "ERROR - Rewind() not implemented for " << GetDescriptor() << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
//  Return the name of the file we are actually reading from.
//
TString MRead::GetFileName() const
{
    TString name(GetFullFileName());
    if (name.IsNull())
        return "<n/a>";
    return gSystem->BaseName(name);
}

Int_t MRead::AddFiles(MDirIter &files)
{
    files.Reset();

    Int_t rc = 0;

    TString str;
    while (!(str=files.Next()).IsNull())
    {
        const Int_t num = AddFile(str);
        if (num<0)
            *fLog << warn << "Warning: AddFile(\"" << str << "\") returned " << num << "... skipped." << endl;
        else
            rc += num;
    }

    return rc;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv:
//   File0, File1, File2, ..., File10, ..., File100, ...
//
// Searching stops if the first key isn't found in the TEnv
//
// Enclosing quotation marks (") are removed
//
// Number of entries at the moment not supported
//
Int_t MRead::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    //
    // Search (beginning with 0) all keys
    //
    int i=0;
    while (1)
    {
        TString idx = "File";
        idx += i;

        // Output if print set to kTRUE
        if (!IsEnvDefined(env, prefix, idx, print))
            break;

        // Try to get the file name
        TString name = GetEnvValue(env, prefix, idx, "");
        if (name.IsNull())
        {
            *fLog << warn << prefix+"."+idx << " empty." << endl;
            continue;
        }

        if (name.BeginsWith("\"") && name.EndsWith("\""))
        {
            name.Remove(name.Last('\"'), 1);
            name.Remove(name.First('\"'), 1);
        }

        *fLog << inf << "Add File: " << name << endl;

        AddFile(name);
        i++;
    }

    return i!=0;
}

// --------------------------------------------------------------------------
//
// Check if the file exists and has read permission. Derived classes
// should also check whether its file type is correct.
//
// Returning 0 means: file doesn't exist.
// A returned number corresponds to different file types (1 if only
// one exists)
//
Byte_t MRead::IsFileValid(const char *name)
{
    return !gSystem->AccessPathName(name, kFileExists) ? 1 : 0;
}
