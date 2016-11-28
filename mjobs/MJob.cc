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
!   Author(s): Thomas Bretz, 8/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJob
//
// A base class for jobs
//
// SetDebugEnv(0) // switch off debugging
// SetDebugEnv(1) // reserved
// SetDebugEnv(2) // print untouched resources after evtloop resources setup
// SetDebugEnv(3) // do 2) and debug setting env completely
//
// To allow overwriting the output files call SetOverwrite()
//
/////////////////////////////////////////////////////////////////////////////
#include "MJob.h"

#include "MEnv.h"

#include <TFile.h>
#include <TClass.h>
#include <TSystem.h>
#include <TRandom.h>
#include <TObjArray.h>

#include "MIter.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MEvtLoop.h"

ClassImp(MJob);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets fDataFlag to 0
//
MJob::MJob(const char *name, const char *title) : fEnv(0), fEnvDebug(0), fOverwrite(kFALSE), fMaxEvents(0)
{
    fName  = name  ? name  : "MJob";
    fTitle = title ? title : "Base class for jobs";
}

//------------------------------------------------------------------------
//
// If MJob is the owner of fEnv delete fEnv.
//
// Reset the owenership bit.
//
// Set fEnv to NULL.
//
void MJob::ClearEnv()
{
    if (fEnv && TestBit(kIsOwner))
        delete fEnv;
    ResetBit(kIsOwner);
    fEnv=0;
}

//------------------------------------------------------------------------
//
// ClearEnv()
//
MJob::~MJob()
{
    ClearEnv();
}

//------------------------------------------------------------------------
//
// If prefix==0 the prefix is taken from fName up to the first
// whitespace.
//
// A trailing dot is removed.void MJob::SetPrefix(const char *prefix)
void MJob::SetPrefix(const char *prefix)
{
    fEnvPrefix = prefix;

    if (!prefix)
        fEnvPrefix = fName.First(' ')>0 ? fName(0, fName.First(' ')) : fName;

    if (fEnvPrefix.EndsWith("."))
        fEnvPrefix.Remove(fEnvPrefix.Length()-1);
}

//------------------------------------------------------------------------
//
// Create a new MEnv from the file env. MJob takes of course the
// ownership of the newly created MEnv.
//
// SetPrefix(prefix)
//
// return kFALSE if MEnv is invalid
//
Bool_t MJob::SetEnv(const char *env, const char *prefix)
{
    SetEnv(new MEnv(env), prefix);

    // Take the owenership of the MEnv instance
    SetBit(kIsOwner);

    return fEnv->IsValid();
}

//------------------------------------------------------------------------
//
// Set a new fEnv and a new general prefix.
//
// Calls SetPrefix(prefix)
//
// MJob does not take the owenership of the MEnv instance.
//
void MJob::SetEnv(MEnv *env, const char *prefix)
{
    ClearEnv();

    fEnv = env;

    SetPrefix(prefix);
}

//------------------------------------------------------------------------
//
// Removes LF's from the path (necessary if the resource file was written
// with a different operating system than Linux.
//
// Removes a trailing slash if the path is not the root-path.
//
// Adds fname to the path if given.
//
void MJob::FixPath(TString &path)
{
    path.ReplaceAll("\015", "");

    if (path==(TString)"/")
        return;

    if (path.EndsWith("/"))
        path.Remove(path.Length()-1);
}

//------------------------------------------------------------------------
//
// Calls FixPath
//
// Adds fname to the path if given.
//
TString MJob::CombinePath(TString path, TString fname)
{
    FixPath(path);

    if (fname.IsNull())
        return path;

    if (path!=(TString)"/")
        path += "/";

    path += fname;

    return path;
}

//------------------------------------------------------------------------
//
// Sets the output path. The exact meaning (could also be a file) is
// deined by the derived class.
//
void MJob::SetPathOut(const char *path)
{
    fPathOut = path;
    FixPath(fPathOut);
}

//------------------------------------------------------------------------
//
// Sets the input path. The exact meaning (could also be a file) is
// deined by the derived class.
//
void MJob::SetPathIn(const char *path)
{
    fPathIn = path;
    FixPath(fPathIn);
}

//------------------------------------------------------------------------
//
// Returns the TEnv
//
const TEnv *MJob::GetEnv() const
{
    return static_cast<const TEnv *const>(fEnv);
}

//------------------------------------------------------------------------
//
// Checks GetEnvValue(*fEnv, fEnvPrefix, name, dftl)
// For details see MParContainer
//
Int_t MJob::GetEnv(const char *name, Int_t dflt) const
{
    return GetEnvValue2(*fEnv, fEnvPrefix, name, dflt); //    return fEnv->GetValue(Form("%s%s", fEnvPrefix.Data(), name), dflt);
}

//------------------------------------------------------------------------
//
// Checks GetEnvValue(*fEnv, fEnvPrefix, name, dftl)
// For details see MParContainer
//
Double_t MJob::GetEnv(const char *name, Double_t dflt) const
{
    return GetEnvValue2(*fEnv, fEnvPrefix, name, dflt); //    return fEnv->GetValue(Form("%s%s", fEnvPrefix.Data(), name), dflt);
}

//------------------------------------------------------------------------
//
// Checks GetEnvValue(*fEnv, fEnvPrefix, name, dftl)
// For details see MParContainer
//
const char *MJob::GetEnv(const char *name, const char *dflt) const
{
    return GetEnvValue2(*fEnv, fEnvPrefix, name, dflt); //fEnv->GetValue(Form("%s%s", fEnvPrefix.Data(), name), dflt);
}

//------------------------------------------------------------------------
//
// Checks IsEnvDefined(*fEnv, fEnvPrefix, name, fEnvDebug>2)
// For details see MParContainer
//
Bool_t MJob::HasEnv(const char *name) const
{
    return IsEnvDefined(*fEnv, fEnvPrefix, name, fEnvDebug>2);//fEnv->Lookup(Form("%s%s", fEnvPrefix.Data(), name));
}

//------------------------------------------------------------------------
//
// Check fEnv for RadnomNumberGenerator. If it is empty or the
// corresponding class does either not exist or not inherit from TRandom,
// gRandom remains unchanged. Otherwise gRandom is set to a newly created
// instance of this class.
//
// The second resource which is checked is RandomNumberSeedValue. If it
// is empty (not given) the seed keeps unchanged. If a number is given
// the seed value of gRandom is set accordingly. (0 means that
// the seed value is set accoring to the time, see TRandom::SetSeed())
//
// If an error occured kFALSE is returned, kTRUE otherwise.
//
// For exmaple:
//    RandomNumberGenerator: TRandom3
//    RandomNumberSeedValue:  0
//
Bool_t MJob::InitRandomNumberGenerator() const
{
    const TString rng = GetEnv("RandomNumberGenerator", "");
    if (!rng.IsNull())
    {
        TClass *cls = MParContainer::GetClass(rng, &gLog);
        if (!cls)
            return kFALSE;

        if (!cls->InheritsFrom(TRandom::Class()))
        {
            *fLog << err << "ERROR - RandomNumberGenerator " << rng << " doesn't inherit from TRandom." <<  endl;
            return kFALSE;
        }

        delete gRandom;
        gRandom = static_cast<TRandom*>(cls->New());

        *fLog << inf << "Random number generator " << rng << " initialized." << endl;
    }

    // Nothing: Keep seed value, 0 set time as seed value, val set seed
    const TString seed = GetEnv("RandomNumberSeedValue", "");
    if (!seed.IsNull())
    {
        if (!seed.IsAlnum())
        {
            *fLog << err << "ERROR - RandomNumberSeedValue not an integer: " << seed <<  endl;
            return kFALSE;
        }

        gRandom->SetSeed(seed.Atoi());
        *fLog << inf << "Random number seed value set to " << seed.Atoi() << endl;
    }

    return kTRUE;
}

//------------------------------------------------------------------------
//
// Check the resource file for
//   PathOut
//   PathIn
//   MaxEvents
//   Overwrite
//   EnvDebug
//   RandomNumberGenerator
//   RandomNumberSeedValue
//
// and call the virtual function CheckEnvLocal
//
Bool_t MJob::CheckEnv()
{
    if (!fEnv)
        return kTRUE;

    if (!InitRandomNumberGenerator())
        return kFALSE;

    TString p;
    p = GetEnv("PathOut", "");
    if (!p.IsNull())
        SetPathOut(p);

    p = GetEnv("PathIn", "");
    if (!p.IsNull())
        SetPathIn(p);

    SetMaxEvents(GetEnv("MaxEvents", fMaxEvents));
    SetOverwrite(GetEnv("Overwrite", fOverwrite));
    SetEnvDebug( GetEnv("EnvDebug",  fEnvDebug));

    return CheckEnvLocal();
}

//------------------------------------------------------------------------
//
// Returns the result of c.ReadEnv(*fEnv, fEnvPrefix, fEnvDebug>2)
// By adding the container first to a MParList it is ensured that
// all levels are checked.
//
Bool_t MJob::CheckEnv(MParContainer &c) const
{
    if (!fEnv)
        return kTRUE;

    // Make sure that all levels are checked
    MParList l;
    l.AddToList(&c);
    return l.ReadEnv(*fEnv, fEnvPrefix+".", fEnvDebug>2);
}

//------------------------------------------------------------------------
//
// Call the eventsloops ReadEnv and print untouched resources afterwards
// if fEnvDebug>1
//
Bool_t MJob::SetupEnv(MEvtLoop &loop) const
{
    if (!fEnv)
        return kTRUE;

    *fLog << all << "Evaluating resources from: " << fEnv->GetRcName() << endl;

    if (!loop.ReadEnv(*fEnv, fEnvPrefix, fEnvDebug>2))
        return kFALSE;

    if (fEnvDebug>1)
        fEnv->PrintUntouched();

    return kTRUE;
}

//------------------------------------------------------------------------
//
// Checks whether write permissions to fname exists including
// the fOverwrite data member. Empty file names return kTRUE
//
Bool_t MJob::HasWritePermission(TString fname) const
{
    gSystem->ExpandPathName(fname);

    const Bool_t exists = !gSystem->AccessPathName(fname, kFileExists);
    if (!exists)
    {
        const TString dirname = gSystem->DirName(fname);

        Long_t flags;
        const Bool_t exists2 = !gSystem->GetPathInfo(dirname, 0, (Long_t*)0, &flags, 0);
        if (!exists2)
        {
            *fLog << err << "ERROR - Directory " << dirname << " doesn't exist." << endl;
            return kFALSE;
        }

        if ((flags&2)==0)
        {
            *fLog << err << "ERROR - " << dirname << " is not a directory." << endl;
            return kFALSE;
        }

        const Bool_t write = !gSystem->AccessPathName(dirname, kWritePermission);
        if (!write)
        {
            *fLog << err << "ERROR - No permission to write to directory " << dirname << endl;
            return kFALSE;
        }

        return kTRUE;
    }

    Long_t flags;
    gSystem->GetPathInfo(fname, 0, (Long_t*)0, &flags, 0);
    if (flags&4)
    {
        *fLog << err << "ERROR - " << fname << " is not a regular file." << endl;
        return kFALSE;
    }

    const Bool_t write = !gSystem->AccessPathName(fname, kWritePermission);
    if (!write)
    {
        *fLog << err << "ERROR - No permission to write to " << fname << endl;
        return kFALSE;
    }

    if (fOverwrite)
        return kTRUE;

    *fLog << err;
    *fLog << "ERROR - File " << fname << " already exists and overwrite not allowed." << endl;

    return kFALSE;
}

//------------------------------------------------------------------------
//
// Write containers in list to gFile. Returns kFALSE if no gFile or any
// container couldn't be written. kTRUE otherwise.
//
Bool_t MJob::WriteContainer(TCollection &list) const
{
    if (!gFile)
    {
        *fLog << err << dbginf << "ERROR - No file open (gFile==0)" << endl;
        return kFALSE;
    }

    TIter Next(&list);
    TObject *o=0;
    while ((o=Next()))
    {
        *fLog << inf << " - Writing " << MParContainer::GetDescriptor(*o) << "..." << flush;
        if (o->Write(o->GetName())<=0)
        {
            *fLog << err << dbginf << "ERROR - Writing " << MParContainer::GetDescriptor(*o) << " to file " << gFile->GetName() << endl;
            return kFALSE;
        }
        *fLog << "ok." << endl;
    }
    return kTRUE;
}

//------------------------------------------------------------------------
//
// Read containers in list into list from gFile
// Returns kFALSE if no gFile or any container couldn't be read.
//
Bool_t MJob::ReadContainer(TCollection &list) const
{
    if (!gFile)
    {
        *fLog << err << dbginf << "ERROR - No file open (gFile==0)" << endl;
        return kFALSE;
    }

    TIter Next(&list);
    TObject *o=0;
    while ((o=Next()))
    {
        *fLog << inf << " - Reading " << MParContainer::GetDescriptor(*o) << "..." << flush;
        if (o->Read(o->GetName())<=0)
        {
            *fLog << err << dbginf << "ERROR - Reading " << MParContainer::GetDescriptor(*o) << " from file " << gFile->GetName() << endl;
            return kFALSE;
        }
        *fLog << "ok." << endl;
    }
    return kTRUE;
}

//------------------------------------------------------------------------
//
// Write containers in cont to fPathOut+"/"+name, or fPathOut only
// if name is empty.
//
Bool_t MJob::WriteContainer(TCollection &cont, const char *name, const char *option, const int compr) const
{
    if (fPathOut.IsNull())
    {
        *fLog << inf << "No output path specified via SetPathOut - no output written." << endl;
        return kTRUE;
    }

    TString oname = CombinePath(fPathOut, name);

    // In case the update-option is selected check whether
    // the file is already open
    if (TString(option).Contains("update", TString::kIgnoreCase))
    {
        TFile *file = dynamic_cast<TFile*>(gROOT->GetListOfFiles()->FindObject(oname));
        if (!file || !file->IsOpen() || file->IsZombie())
        {
            gSystem->ExpandPathName(oname);
            file = dynamic_cast<TFile*>(gROOT->GetListOfFiles()->FindObject(oname));
        }

        if (file && file->IsOpen() && !file->IsZombie())
        {
            *fLog << inf << "Updating open file " << oname << "." << endl;
            file->cd();
            return WriteContainer(cont);
        }
    }

    *fLog << inf << "Writing to file " << oname << "." << endl;

    TString title("File written by ");
    title += fName;

    // Open a new file with the defined option for writing
    TFile file(oname, option, title, compr);
    if (!file.IsOpen() || file.IsZombie())
    {
        *fLog << err << "ERROR - Couldn't open file " << oname << " for writing..." << endl;
        return kFALSE;
    }

    return WriteContainer(cont);
}

//------------------------------------------------------------------------
//
// return kTRUE if no display is set.
//
// Write the display to the TFile with name name, options option and
// compression level comp.
//
// If name IsNull fPathOut is assumed to contain the name, otherwise
// name is appended to fPathOut. fPathOut might be null.
//
Bool_t MJob::WriteDisplay(const char *name, const char *option, const int compr) const
{
    if (!fDisplay)
        return kTRUE;

    TObjArray arr;
    arr.Add((TObject*)(fDisplay));
    return WriteContainer(arr, name, option, compr);
}

TString MJob::ExpandPath(TString fname)
{
    // empty
    if (fname.IsNull())
        return "";

    // Expand path using environment
    gSystem->ExpandPathName(fname);

    // Absolute path
    if (fname[0]=='/')
    {
        gLog << dbg << "MJob::ExpandPath - Path " << fname << " is absolute." << endl;
        return fname;
    }

    // relative path to file and file could be found
    if (!gSystem->AccessPathName(fname, kFileExists))
    {
        gLog << dbg << "MJob::ExpandPath - Relative path " << fname << " found..." << endl;
        return fname;
    }

    // Now check gEnv and MARSSYS. gEnv can overwrite MARSSYS
    TString path(gEnv ? gEnv->GetValue("Mars.Path", "$MARSSYS") : "$MARSSYS");

    // Expand path using environment
    gSystem->ExpandPathName(path);

    // check if path ends with a slash
    path = CombinePath(path, fname);

    gLog << dbg << "MJob::ExpandPath - Filename expanded to " << path << endl;

    // return new path
    return path;
}

//------------------------------------------------------------------------
//
// Sorts the array.
//
void MJob::SortArray(TArrayI &arr)
{
    TArrayI idx(arr.GetSize());
    TArrayI srt(arr);

    TMath::Sort(arr.GetSize(), srt.GetArray(), idx.GetArray(), kFALSE);

    for (int i=0; i<arr.GetSize(); i++)
        arr[i] = srt[idx[i]];
}
