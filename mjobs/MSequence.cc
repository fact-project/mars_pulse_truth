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
!   Copyright: MAGIC Software Development, 2004-2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MSequence
//
//  This class describes a sequence. For sequences see:
//    http://db.astro.uni-wuerzburg.de
//
//  A sequence is a collection of runs which should be used together.
//  Any run can be contained only once.
//
//  Here is an example how a file describing a sequence could look like.
//
//  Reading the file is based on TEnv. For more details see also
//  the class reference of TEnv.
//
// ===========================================================================
//
//  sequence.txt
//  ------------
//
//   # Sequence number (identifier) - necessary if the path
//   # contains the sequence number, e.g. image files
//   Sequence:     31015
//
//   # Telescope number the sequence belongs to (if the resource is omitted
//   #  the default is 1)
//   Telescope:    1
//
//   # Observation Period (not needed, calculated from Night)
//   # Period:     18
//
//   # Date of sunrise of the observation night - necessary if the path
//   # contains the date, e.g. raw data
//   Night:        2004-06-24
//
//   # Start time of the sequence (first data run, not needed)
//   Start:        2004-06-24 03:12:42
//
//   # Run number of last data run in sequence (not needed)
//   LastRun:      31032
//
//   # Project name of data-runs of sequence (not needed)
//   Project:      3EG2033+41
//
//   # Source name of all runs of sequence (not needed)
//   Source:       3EG2033+41
//
//   # Trigger table of data-runs of sequence (not needed)
//   TriggerTable: L1_4NN:L2_DEFAULT
//
//   # HV Setting table of data-runs of sequence (not needed)
//   HvSettings:   HVSettings_FF36q
//
//   # Total number of data-events in sequence (not needed)
//   NumEvents:    250914
//
//   # Brightest light condition of the contained runs (not needed)
//   LightCondition: Twilight
//
//   # Whether this is MC data or not (necessary in case of MCs if
//   # default paths should be used)
//   MonteCarlo: Yes
//
//   # List of all runs of this sequence (not needed)
//   Runs: 31015:31017 31018 31019.0 31019.3 31019.5:7 31020+ 31021::3
//
//   # List of all calibration runs of this sequence (necessary if accessed)
//   CalRuns: 31015 31016 31017
//   # List of pedestal runs belonging to the calibration runs of this sequence (necessary if accessed)
//   PedRuns: 31018
//   # List of all data runs belonging to this sequence (necessary)
//   DatRuns: 31019.0 31019.3 31019:5:7 31020+ 31021::3
//
//   Run00031020: :3 7:9 15
//
//   # Just for fun ;-) (not needed, but helpful)
//   Comment: This is a template for a sequence file
//
// ===========================================================================
//
//   Runs are devided into file since run 1000000. These Run-/Filenumbers
//   are given by a dot. X.Y means Run X, File Y.
//
//   In the Runs, CalRuns, PedRuns and DatRuns tag you can use
//   several abbreviationa:
//
//    31015:31017     means      31015.0 31016.0 31017.0
//    31018           means      31018.0
//    31019.3         means      31019.3
//    31019.5:7       means      31019.5 31019.6 31019.7
//    31020+          means      file list for run 21020 given below
//    31021::3        means      31021.0 31021.1 31021.2 31021.3
//
//   For the run-list defined above (note the number must have 8 digits,
//   for example 'Run00031020') you can also use abbreviations:
//
//    :3              means      0 1 2 3
//    7:9             means      7 8 9
//
// ===========================================================================
//
//  For special cases you can also setup a sequence directly from a macro,
//  for example:
//
//    MDirIter pediter, datiter, caliter;
//
//    MSequence seq;
//    seq.SetNight("2004-07-06");
//    seq.AddRun(31751, 'P');
//    seq.AddRun(31752, 'C');
//    seq.AddRuns(31753, 31764, 'D');
//    seq.SetupPedRuns(pediter);
//    seq.SetupCalRuns(caliter);
//    seq.SetupDatRuns(datiter);
//
//  or
//
//    MDirIter iter;
//
//    MSequence seq;
//    seq.SetNight("2004-07-06");
//    seq.AddFiles(31753, 0, 120);
//    seq.GetRuns(iter);
//    seq.SetupPedRuns(iter, "/mypath", "[DPC]");
//
//
// ===========================================================================
//
// ToDO:
//   * Default paths could be moved into the global .rootrc
//
// ===========================================================================
//
//
//  Class Version 2:
//  ----------------
//   + fMonteCarlo
//
//  Class Version 3:
//  ----------------
//   + fComment
//
//  Class Version 4:
//  ----------------
//   + fExclRuns
//
//  Class Version 5:
//  ----------------
//   + fRunsSub
//   + fDatRunsSub
//   + fPedRunsSub
//   + fCalRunsSub
//   + fExclRunsSub
//
//  Class Version 6:
//  ----------------
//   + fTelescope
//
//  Class Version 7:
//  ----------------
//   + fDrsRuns
//   + fDrsRunsSub
//   + fDrsFiles
//   + fDrsFilesSub
//   + fDrsSequence
//
/////////////////////////////////////////////////////////////////////////////
#include "MSequenceSQL.h"

#include <stdlib.h>
#include <errno.h>

#include <fstream>

#include <TEnv.h>
#include <TPRegexp.h>
#include <TSystem.h> // TSystem::ExpandPath

#include "MLog.h"
#include "MLogManip.h"

#include "MEnv.h"
#include "MJob.h"
#include "MAstro.h"
#include "MString.h"
#include "MDirIter.h"

ClassImp(MSequence);

using namespace std;

// --------------------------------------------------------------------------
//
// This adds an run/file entry to the data/sub arrays. If it is already
// contained a waring is printed.
//
void MSequence::AddEntry(Int_t run, Int_t file, TArrayI &data, TArrayI &sub) const
{
    const Int_t n = data.GetSize();

    // skip already existing entries
    if (IsContained(data, sub, run, file))
    {
        *fLog << warn << "WARNING - File " << run << "." << file << " already in list... skipped." << endl;
        return;
    }

    if (!fMonteCarlo && run<1000000 && file>0)
        *fLog << warn << "WARNING - Run number " << run << "<" << "1,000,000 but file " << file << ">0..." << endl;

    // set new entry
    data.Set(n+1);
    sub.Set(n+1);

    data[n] = run;
    sub[n]  = file;
}

// --------------------------------------------------------------------------
//
// Evaluate a token in thr Run string. Add the coresponding run/files
// with AddEntry
//
void MSequence::EvalEntry(const TEnv *env, const TString &prefix, const TString &num, TArrayI &data, TArrayI &sub) const
{
    // Split entry into run and file
    const Int_t run = num.Atoi();

    // Syntax error forbidden by construction
    const Ssiz_t p1 = num.First('.');
    const Ssiz_t p2 = num.Last(':');

    // ---------------------------------------------------------------

    const Int_t n1 = atoi(num.Data()+p1+1);
    const Int_t n2 = atoi(num.Data()+p2+1);

    // ---------------------------------------------------------------
    //  p1>=0 && p2<0  (. but no :)    run.n1     run.file1
    if (p1>=0 && p2<0)
    {
        AddEntry(run, n1, data, sub);
        return;
    }

    // ---------------------------------------------------------------
    //  p1>=0 && p2>=0  (. and :)      run:n1:n2  run.file1-run.file2
    if (p1>=0 && p2>=0)
    {
        if (n2<n1)
        {
            *fLog << warn << "WARNING - Invalid range '" << num << "'... ignored." << endl;
            return;
        }

        for (int i=n1; i<=n2; i++)
            AddEntry(run, i, data, sub);
        return;
    }

    // ---------------------------------------------------------------
    //  p1<0 && p2>=0  (no . but :)    n1:n2      run1-run2
    if (p1<0 && p2>=0)
    {
        if (n2<run)
        {
            *fLog << warn << "WARNING - Invalid range in '" << num << "'... ignored." << endl;
            return;
        }

        for (int i=run; i<=n2; i++)
            AddEntry(i, 0, data, sub);
        return;
    }

    // ---------------------------------------------------------------
    //  p0<0 and p1<0  (no . and no :)   run     run

    if (!num.EndsWith("+"))
    {
        AddEntry(run, 0, data, sub);
        return;
    }

    if (!env)
        return;

    TPRegexp regexp("([0-9]*:[0-9]+|[0-9]+(:[0-9]*)?)( +|$)");

    TString files = GetEnvValue3(*env, prefix, "Run%d", run);
    if (files.IsNull())
    {
        AddEntry(run, 0, data, sub);
        return;
    }

    while (!files.IsNull())
    {
        const TString num = files(regexp);
        if (num.IsNull())
        {
            *fLog << warn << "WARNING - File in run " << run << " is NaN (not a number): '" << files << "'" << endl;
            break;
        }

        const Ssiz_t p1 = num.First(':');
        if (p1>=0)
        {
            const Int_t n1 = atoi(num.Data());
            const Int_t n2 = atoi(num.Data()+p1+1);

            if (n2<n1)
            {
                *fLog << warn << "WARNING - Invalid range in '" << num << "'... ignored." << endl;
                return;
            }

            // FIXME: n2==0 || n2<n1
            for (int i=n1; i<=n2; i++)
                AddEntry(run, i, data, sub);
        }
        else
        {
            const Int_t file = atoi(num.Data());
            AddEntry(run, file, data, sub);
        }

        files.Remove(0, files.First(num)+num.Length());
    }
}

// --------------------------------------------------------------------------
//
// Interprete the TString and add the run/files to the arrays.
// Run/files which are twice in the list are only added once. In this case
// a warning is emitted.
//
void MSequence::Split(TString &runs, TArrayI &data, TArrayI &sub, const TEnv *env, const TString prefix) const
{
    TPRegexp regexp("^[0-9]+([+]|(:|[.]([0-9]*:)?)[0-9]+)?( +|$)");

    data.Set(0);
    sub.Set(0);

    runs.ReplaceAll("\t", " ");
    runs = runs.Strip(TString::kBoth);

    while (!runs.IsNull())
    {
        const TString num = runs(regexp);
        if (num.IsNull())
        {
            *fLog << warn << "WARNING - Run syntax error: '" << runs << "'" << endl;
            break;
        }

        EvalEntry(env, prefix, num.Strip(TString::kTrailing), data, sub);

        runs.Remove(0, runs.First(num)+num.Length());
    }

    SortArrays(data, sub);
}

// --------------------------------------------------------------------------
//
// Get the String from the TEnv file prefixed by prefix-name.
// Intepret the string using Split which adds the run/files to the arrays.
//
void MSequence::Split(const TEnv &env, const TString &prefix, const char *name, TArrayI &data, TArrayI &sub) const
{
    TString str = GetEnvValue2(env, prefix, name, "");
    Split(str, data, sub, &env, prefix);
}

TString MSequence::InflateFilePath(const MTime &night, Bool_t mc)
{
    TString rc = GetStandardPath(mc);
    rc += "rawfiles/";
    rc += night.GetStringFmt("%Y/%m/%d/");
    return rc;
}

TString MSequence::InflateFilePath(const MTime &night, UShort_t tel, Int_t run, Int_t file, Int_t type)
{
    return InflateFilePath(night)+InflateFileName(night, tel, run, file, type);
}

// --------------------------------------------------------------------------
//
// Compile path from the given path name and expand it. If it IsNull()
// the path is compiled as standard path from tha datacenter). The
// returned path will end with a "/".
//
TString MSequence::GetPathName(TString d, FileType_t type) const
{
    // Setup path
    if (d.IsNull())
    {
        d = GetStandardPath();
        switch (type&kFilePaths)
        {
        case kDrs:
        case kFits:
            d += "raw/";
            d += fNight.GetStringFmt("%Y/%m/%d");
            break;
        case kRaw:  // rawdata
        case kRoot: // mcdata
            d += "rawfiles/";
            if (!fNight)
                *fLog << "WARNING - No Night-resource... path not supported." << endl;
            d += fNight.GetStringFmt("%Y/%m/%d");
            break;
        case kReport:
            d += "../subsystemdata/cc/";
            d += fNight.GetStringFmt("%Y/%m/%d");
            break;
        case kCalibrated:
            d += MString::Format("callisto/%04d/%08d", fSequence/10000, fSequence);
            break;
        case kImages:
            d += MString::Format("star/%04d/%08d", fSequence/10000, fSequence);
            break;
        }
    }
    else
        gSystem->ExpandPathName(d);

    if (!d.EndsWith("/"))
        d += '/';

    return d;
}

// --------------------------------------------------------------------------
//
// Compile FileName according to night, tel, run, file and type.
//
TString MSequence::InflateFileName(const MTime &night, UShort_t tel, Int_t run, Int_t file, Int_t type)
{
    if (!night && type==kCorsika)
    {
        // /magic/simulated/ceres/001/000/0001/cer123456
        return MString::Format("cer%06d", -run);
    }

    TString n;

    if (type&kDrsFile)
    {
        n += night.GetStringFmt("%Y%m%d_");
        n += MString::Format("%03d", run);
        n += ".drs.fits.?[fg]?z?";
        return n;
    }
    if (type&(kFits|kFact))
    {
        n += night.GetStringFmt("%Y%m%d_");
        n += MString::Format("%03d", run);

        if (type==kFactCal)
            n += "_C.root";
        else if (type==kFactImg)
            n += "_I.root";
        else
            n += ".fits.?[fg]?z?";
        return n;
    }

    const char *id="_";
    switch ((RunType_t)(type&kRunType))
    {
    case kDat:
        id = "D";
        break;
    case kPed:
        id = "P";
        break;
    case kCal:
        id = "C";
        break;
    case kAll:
        id = "[PCD]";
        break;
    case kCalib:
        id = "Y";
        break;
    case kImage:
        id = "I";
        break;
    case kExcl:
        break;
    }

    // Is report file?
    const Bool_t isrep = type&kReport;

    // Calculate limit for transition from 5-digit to 8-digit numbers
    const Int_t limit = isrep ? 39373 : 35487;

    // ------------- Create file name --------------
    if (night)
        n += night.GetStringFmt("%Y%m%d_");

    // This is for MCs (FIXME!!!!)
    if (run<0)
    {
        n += MString::Format("%08d", -run);
        if (file>=0)
            n += MString::Format(".%03d", file);
    }
    else
    {
        if (tel>0)
            n += MString::Format("M%d_", tel);

        // R. DeLosReyes and T. Bretz
        // Changes to read the DAQ numbering format. Changes takes place
        // between runs 35487 and 00035488 (2004_08_30)
        n += MString::Format(run>limit ? "%08d" : "%05d", run);

        if (tel>0)
            n += MString::Format(".%03d", file);
    }


    n += "_";
    n += id;
    n += "_*";

    if (tel==0 && run>=0)
        n += isrep ? "_S" : "_E";

    switch (type&kFileType)
    {
    case kRaw:       // MC     // DATA
        n += run<0 ? ".root" : ".raw.?[fg]?z?";  // TPRegexp: (\.gz)?
        break;
    case kReport:
        n += ".rep";  // TPRegexp: (\.gz)?
        break;
    default:
        n += ".root";
        break;
    }

    return n;
}

// --------------------------------------------------------------------------
//
// Return the expression describing the file-name for the file defined
// by i-th entry of the the given arrays. The file type identifier is
// defined by type. The source name is replaced by a wildcard and
// the extension is defined by the type as well.
//
TString MSequence::GetFileRegexp(UInt_t i, const TArrayI &arr, const TArrayI &sub, FileType_t type) const
{
    // Entry doesn't exist
    if (i>=(UInt_t)arr.GetSize())
        return "";

    // This is (so far) well defined for MAGIC data
    if (!fMonteCarlo)
        return InflateFileName(fNight, arr[i]>999999?fTelescope:0, arr[i], sub.GetSize()>0?sub[i]:0, type);

    // Monte Carlos
    return InflateFileName(fNight, fTelescope, -arr[i], sub.GetSize()>0?sub[i]:0, type);
}

// --------------------------------------------------------------------------
//
// Get the filename from the dirctory and the file-regexp.
// return "" if file is not accessible or the regular expression is not
// unambiguous.
//
TString MSequence::FindFile(const TString &d, const TString &f) const
{
    // Check existance and accessibility of file
    MDirIter file(d, f, 0);

    TString name = file();
    gSystem->ExpandPathName(name);
    if (gSystem->AccessPathName(name, kFileExists))
    {
        *fLog << err;
        *fLog << "ERROR - File " << d << f << " not accessible!" << endl;
        return "";
    }
    if (!file().IsNull())
    {
        *fLog << err;
        *fLog << "ERROR - Searching for file " << d << f << " gave more than one result!" << endl;
        return "";
    }

    return name;
}

// --------------------------------------------------------------------------
//
// Set the path accoring to type (if fDataPath=path=0), fDataPath
// (if path=0) or path.
// Returns accessability of the path.
//
Bool_t MSequence::FindPath(TString &path, FileType_t type) const
{
    if (path.IsNull())
        path = fDataPath;

    const Bool_t def = path.IsNull();

    path = GetPathName(path, type);

    return def;
}

// --------------------------------------------------------------------------
//
// Add the entries from the arrays to the MDirIter
//
UInt_t MSequence::GetRuns(MDirIter &iter, const TArrayI &arr, const TArrayI &sub, FileType_t type, TString path) const
{
    const Bool_t def = FindPath(path, type);

    // For this particular case we assume that the files are added one by
    // one without wildcards.
    const Int_t n0 = iter.GetNumEntries();

    Int_t excluded = 0;
    for (int i=0; i<arr.GetSize(); i++)
    {
        if (IsExcluded(arr[i], sub[i]))
        {
            excluded++;
            continue;
        }

        const TString n = GetFileRegexp(i, arr, sub, type);

        if (FindFile(path, n).IsNull())
            continue;

        // Add Path/File to TIter
        iter.AddDirectory(path, n, 0);
    }

    // n0: The previous contents of the iter
    // n1: The number of files which have been added to the iter
    // n2: The number of files which should have been added from the array
    const Int_t n1 = iter.GetNumEntries()-n0;
    const Int_t n2 = arr.GetSize()-excluded;
    if (n1==0)
    {
        *fLog << err;
        *fLog << "ERROR - No input files for sequence #" << GetSequence() << endl;
        *fLog << "        read from " << GetBaseName() << endl;
        *fLog << "        found in" << (def?" default-path ":" ") << path << endl;
        return 0;
    }

    if (n1==n2)
        return n1;

    *fLog << err;
    *fLog << "ERROR - " << n1 << " input files for sequence #" << GetSequence() << " found in" << endl;
    *fLog << "        " << (def?" default-path ":" ") << path << endl;
    *fLog << "        but " << n2 << " files were defined in sequence file" << endl;
    *fLog << "        " << GetBaseName() << endl;
    if (fLog->GetDebugLevel()<=4)
        return 0;

    *fLog << inf << "Files which are searched for this sequence:" << endl;
    iter.Print();
    return 0;
}

UInt_t MSequence::GetDrsFile() const
{
    if (fDrsRuns.GetSize()==0)
        return fDrsFiles.GetSize()==0 ? 0 : fDrsFiles[0];

    UInt_t rc = 0;
    for (int i=0; i<fDrsFiles.GetSize(); i++)
    {
        for (int j=0; j<fPedRuns.GetSize(); j++)
        {
            if (fDrsFiles[i]==fPedRuns[j])
                rc = fDrsFiles[i];
        }
    }

    return rc;
}

// --------------------------------------------------------------------------
//
// Returns "0" if file at i-th index is excluded. Get the access-path
// defined by the default or data-path (if path=0) or path.
// Get the full file-name by accessing the file.
//
TString MSequence::GetFileName(UInt_t i, const TArrayI &arr, const TArrayI &sub, FileType_t type, const char *path) const
{
    if (i>=(UInt_t)arr.GetSize())
        return "";

    if (IsExcluded(arr[i], sub[i]))
        return "0";

    const TString d = FindPath(type, path);
    const TString n = GetFileRegexp(i, arr, sub, type);

    return FindFile(d, n);
}

// --------------------------------------------------------------------------
//
// Return the pointers to the arrays with the runs/files depending on the
// given type.
//
void MSequence::GetArrays(const TArrayI* &r, const TArrayI* &f, Int_t type) const
{
    switch ((RunType_t)(type&(kRunType|kExcl)))
    {
    case kDrsRun:
        r = &fDrsRuns;
        f = &fDrsRunsSub;
        break;
    case kDrsFile:
        r = &fDrsFiles;
        f = &fDrsFilesSub;
        break;
    case kPed:
        r = &fPedRuns;
        f = &fPedRunsSub;
        break;
    case kDat:
    case kImage:
    case kCalib:
        r = &fDatRuns;
        f = &fDatRunsSub;
        break;
    case kCal:
        r = &fCalRuns;
        f = &fCalRunsSub;
        break;
    case kExcl:
        r = &fExclRuns;
        f = &fExclRunsSub;
        break;
    case kAll:
        r = &fRuns;
        f = &fRunsSub;
        break;
    }
}

// --------------------------------------------------------------------------
//
// Add runs from the sequence to MDirIter. If path==0 fDataPath is
// used instead. If it is also empty the standard path of the
// data-center is assumed. If you have the runs locally use path="."
//
// Returns the number of files added.
//
// Runs which are in fExlRuns are ignored.
//
UInt_t MSequence::GetRuns(MDirIter &iter, FileType_t type, const char *path) const
{
    const TArrayI *r=0, *f=0;
    GetArrays(r, f, type);
    return r==0 ? 0 : GetRuns(iter, *r, *f, type, path);
}

// --------------------------------------------------------------------------
//
// Returns the num-th entry of FileType_t as regular expression.
// If the file is excluded return "".
// To get the number of valid entries use GetNumEntries(FileType_t)
//
//  e.g.
//        if (GetNumEntries(MSequence::kDat))<5)
//            GetFileRegexp(3, MSequence::kRawDat);
//
TString MSequence::GetFileRegexp(UInt_t num, FileType_t type) const
{
    const TArrayI *r=0, *f=0;
    GetArrays(r, f, type);

    TString rc = GetFileRegexp(num, *r, *f, type);
    rc.ReplaceAll(".", "\\.");
    rc.ReplaceAll("*", ".*");
    return rc;
}

// --------------------------------------------------------------------------
//
// Returns the num-th entry of FileType_t as filename. To access the
// path the default is used if path IsNull, path otherwise.
// If the file could not be accessed "" is returned, if it is
// excluded "0".
// To get the number of valid entries use GetNumEntries(FileType_t)
//
//  e.g.
//        if (GetNumEntries(MSequence::kDat))<5)
//            GetFileName(3, MSequence::kRawDat);
//
TString MSequence::GetFileName(UInt_t num, FileType_t type, const char *path) const
{
    const TArrayI *r=0, *f=0;
    GetArrays(r, f, type);
    return GetFileName(num, *r, *f, type, path);
}

// --------------------------------------------------------------------------
//
// Returns the num-th entry of FileType_t as filename.
// If the file could not be accessed or excluded return "".
// To get the number of valid entries use GetNumEntries(RunType_t)
//
UInt_t MSequence::GetNumEntries(RunType_t type) const
{
    const TArrayI *r=0, *f=0;
    GetArrays(r, f, type);
    return r->GetSize();
}

// --------------------------------------------------------------------------
//
// Returns the num-th entry of RunType_t as run- and file-number.
// Returns -1 if index is out of range, 0 if file is excluded and
// 1 in case of success.
//
//  e.g.
//        UInt_t run, file;
//        GetFile(3, MSequence::kDat, run, file);
//
Int_t MSequence::GetFile(UInt_t i, RunType_t type, UInt_t &run, UInt_t &file) const
{
    const TArrayI *r=0, *f=0;
    GetArrays(r, f, type);

    if (i>=(UInt_t)r->GetSize())
        return -1;

    run  = (*r)[i];
    file = i<(UInt_t)f->GetSize() ? (*f)[i] : 0;

    if (IsExcluded(run, file))
        return 0;

    return 1;
}

// --------------------------------------------------------------------------
//
// Add a file (run/file) to the arrays defined by RunType_t
//
void MSequence::AddFile(UInt_t run, UInt_t file, RunType_t type)
{
    const TArrayI *rconst=0, *fconst=0;

    GetArrays(rconst, fconst, type);

    if (!rconst || !fconst)
    {
        *fLog << err << "ERROR - Invalid values returned by MSequence::GetArrays... " << run << "." << file << " ignored." << endl;
        return;
    }

    TArrayI &r=*const_cast<TArrayI*>(rconst);
    TArrayI &f=*const_cast<TArrayI*>(fconst);

    AddEntry(run, file, r, f);

    SortArrays(r, f);
}

// --------------------------------------------------------------------------
//
// Get LightCondition from str and convert it to LightCondition_t
//
MSequence::LightCondition_t MSequence::GetLightCondition(const TString &str) const
{
    if (!str.CompareTo("n/a", TString::kIgnoreCase))
        return kNA;
    if (!str.CompareTo("No_Moon", TString::kIgnoreCase))
        return kNoMoon;
    if (!str.CompareTo("NoMoon", TString::kIgnoreCase))
        return kNoMoon;
    if (!str.CompareTo("Twilight", TString::kIgnoreCase))
        return kTwilight;
    if (!str.CompareTo("Moon", TString::kIgnoreCase))
        return kMoon;
    if (!str.CompareTo("Day", TString::kIgnoreCase))
        return kDay;

    gLog << warn;
    gLog << "WARNING - in " << GetBaseName() << ":" << endl;
    gLog << "          LightCondition-tag is '" << str << "' but must be n/a, no_moon, twilight, moon or day." << endl;
    return kNA;
}

// --------------------------------------------------------------------------
//
// Set the path to the data. If no path is given the data-center default
// path is used to access the files.
//
// kTRUE is returned if the path is acessible, kFALSE otherwise.
// (In case of the default path kTRUE is returned in any case)
//
Bool_t MSequence::SetDataPath(const char *path)
{
    fDataPath = path;

    gSystem->ExpandPathName(fDataPath);

    const Bool_t rc = !fDataPath.IsNull() && gSystem->AccessPathName(fDataPath, kFileExists);
    if (rc)
        gLog << err << "ERROR - Directory '" << fDataPath << "' doesn't exist." << endl;

    return !rc;
}

// --------------------------------------------------------------------------
//
// Read a sequence from a file. To set the location of the data (datapath)
// use SetDataPath. The sequence number given is used if no sequence
// number is found in the sequence file.
//
// If the file could not be found or accesed kFALSE is returned, kTRUE
// otherwise.
//
Bool_t MSequence::ReadFile(const char *filename, UInt_t seq)
{
    fFileName = filename;

    gSystem->ExpandPathName(fFileName);

    const Bool_t rc = gSystem->AccessPathName(fFileName, kFileExists);
    if (rc)
        gLog << err << "ERROR - Sequence file '" << filename << "' doesn't exist." << endl;

    MEnv env(fFileName);

    fSequence = (UInt_t)env.GetValue("Sequence", (Int_t)seq);

    const TString prefix = MString::Format("Sequence%08d", fSequence);

    fTelescope = GetEnvValue2(env, prefix, "Telescope",  1);
    fLastRun   = GetEnvValue2(env, prefix, "LastRun",   -1);
    fNumEvents = GetEnvValue2(env, prefix, "NumEvents", -1);
    fDrsSequence = GetEnvValue2(env, prefix, "DrsSequence", 0);

    TString str;
    str = GetEnvValue2(env, prefix, "LightCondition", "n/a");
    fLightCondition = GetLightCondition(str);

    str = GetEnvValue2(env, prefix, "Start", "");
    fStart.SetSqlDateTime(str);

    str = GetEnvValue2(env, prefix, "Night", "");
    SetNight(str);

    fProject      = GetEnvValue2(env, prefix, "Project", "");
    fSource       = GetEnvValue2(env, prefix, "Source", "");
    fTriggerTable = GetEnvValue2(env, prefix, "TriggerTable", "");
    fHvSettings   = GetEnvValue2(env, prefix, "HvSettings", "");
    fMonteCarlo   = GetEnvValue2(env, prefix, "MonteCarlo", kFALSE);
    fComment      = GetEnvValue2(env, prefix, "Comment",    "");

    Split(env, prefix, "Runs",    fRuns,     fRunsSub);
    Split(env, prefix, "CalRuns", fCalRuns,  fCalRunsSub);
    Split(env, prefix, "PedRuns", fPedRuns,  fPedRunsSub);
    Split(env, prefix, "DatRuns", fDatRuns,  fDatRunsSub);
    Split(env, prefix, "DrsRuns", fDrsRuns,  fDrsRunsSub);
    Split(env, prefix, "DrsFiles",fDrsFiles, fDrsFilesSub);
    Split(env, prefix, "Exclude", fExclRuns, fExclRunsSub);

    if (fDrsRuns.GetSize()==0 && fDrsFiles.GetSize()>1)
        gLog << warn << "WARNING - In a data sequence only one DrsFiles entry makes sense." << endl;

    // Dummies:
    env.Touch("Period");
    env.Touch("ZdMin");
    env.Touch("ZdMax");
    env.Touch("L1TriggerTable");
    env.Touch("L2TriggerTable");

    if (seq==(UInt_t)-1 && env.GetNumUntouched()>0)
    {
        gLog << warn << "WARNING - At least one resource in the sequence-file has not been touched!" << endl;
        env.PrintUntouched();
    }

    return !rc;
}

// --------------------------------------------------------------------------
//
// Instantiate a file from either a file or the database.
//
// If not valid sequence could be created the sequence is invalidated,
// which can be queried with IsValid.
//
//  see ReadFile, ReadDatabase and SetDataPath for more details.
//
MSequence::MSequence(const char *fname, const char *path, UInt_t seq)
   : fTelescope(1), fSequence((UInt_t)-1), fLastRun((UInt_t)-1),
     fNumEvents((UInt_t)-1), fPeriod((UInt_t)-1), fLightCondition(kNA),
     fMonteCarlo(kFALSE)
{
    fName  = "MSequence";
    fTitle = "Sequence file";

    if (TString(fname).IsNull() && TString(path).IsNull() && seq==(UInt_t)-1)
        return;

    // regular expression to distinguish beween a mysql-path and
    // a filename from where the sequence should be retrieved
    TPRegexp regexp("mysql://[a-zA-Z0-9]+:.+@[a-zA-Z0-9]+/[a-zA-Z0-9]+/([0-9]+:)?[0-9]+");

    const TString db = TString(fname)(regexp);

    const Bool_t rc1 = db.IsNull() ? ReadFile(fname, seq) : ReadDatabase(fname, seq);
    const Bool_t rc2 = SetDataPath(path);

    if (!rc1 || !rc2)
        fSequence = (UInt_t)-1;
}

// --------------------------------------------------------------------------
//
// Read a sequence from the database. Give the path to the database as:
//    mysql://user:password@name/database
// In this case you MUST give a sequence number.
//
// You can code the telescope and sequence number into the path and
// omit the arguments:
//    mysql://user:password@name/database/[tel:]sequence
//
// The third option is to give just the sql.rc as string in the argument.
// In this case you MUST give a sequence number.
//
// If the telecope number is omitted as argument, i.e. ==0, or in the
// database path, a sequence is read from the database if the sequence
// number is unambiguous.
//
// kFALSE is returned if the final sequence is invalid, kTRUE in case of
// success.
//
Bool_t MSequence::ReadDatabase(TString name, UInt_t seq, UShort_t tel)
{
    TPRegexp regexp("([0-9]+:)?[0-9]+$");

    const TString telseq = name(regexp);

    if (!telseq.IsNull())
    {
        const Ssiz_t p = telseq.First(':');

        const TString strtel = p<0 ? TString("") : telseq(0, p);
        const TString strseq = telseq(p+1, telseq.Length());

        tel = strtel.Atoi();
        seq = strseq.Atoi();

        name = gSystem->DirName(name);
    }

    *this = MSequenceSQL(name, seq, tel);

    return IsValid();
}

// --------------------------------------------------------------------------
//
// Create a sequence from the database, e.g.
//
//    TString mysql     = "mysql://MAGIC:password@vela/MyMagic";
//    Int_t   sequence  = 100002;
//    Int_t   telescope = 1;
//
//    MSequence seq(mysql, sequence, telescope);
//
// Is the telescope number is omitted (==0) the database is checked for
// an unambiguous sequence.
//
// For more details see ReadDatabase and MSequenceSQL.
//
MSequence::MSequence(const char *fname, UInt_t seq, UShort_t tel)
{
    const Bool_t rc = ReadDatabase(fname, seq, tel);

    fName  = "MSequence";
    fTitle = "Sequence file";

    if (!rc)
        fSequence = (UInt_t)-1;
}

// --------------------------------------------------------------------------
//
// Some helper functions for the comparison functions
//
static int operator==(const TArrayI &a, const TArrayI &b)
{
    return a.GetSize()==b.GetSize() &&
        memcmp(a.GetArray(), b.GetArray(), a.GetSize()*sizeof(Int_t));
}

static int IsNull(const TArrayI &a, const TArrayI &b)
{
    return a==TArrayI(a.GetSize()) && b==TArrayI(b.GetSize());
}

static int Check(const TArrayI &a, const TArrayI &b)
{
    return a==b || IsNull(a, b);
}

// --------------------------------------------------------------------------
//
// Here we test all things mandatory for a sequence:
//   fTelescope
//   fSequence
//   fNight
//   fMonteCarlo
//   fCalRuns
//   fCalRunsSub
//   fPedRuns
//   fPedRunsSub
//   fDatRuns
//   fDatRunsSub
//   fDrsRuns
//   fDrsRunsSub
//   fExclRuns
//   fExclRunsSub
//
Bool_t MSequence::IsCompatible(const MSequence &s) const
{
    return // Mandatory
        fTelescope   == s.fTelescope   &&
        fSequence    == s.fSequence    &&
        fNight       == s.fNight       &&

        fMonteCarlo  == s.fMonteCarlo  &&

        fCalRuns     == s.fCalRuns     &&
        fPedRuns     == s.fPedRuns     &&
        fDatRuns     == s.fDatRuns     &&
        fDrsRuns     == s.fDrsRuns     &&
        fExclRuns    == s.fExclRuns    &&

        Check(fCalRunsSub,  s.fCalRunsSub) &&
        Check(fPedRunsSub,  s.fPedRunsSub) &&
        Check(fDatRunsSub,  s.fDatRunsSub) &&
        Check(fDrsRunsSub,  s.fDrsRunsSub) &&
        Check(fExclRunsSub, s.fExclRunsSub);
}

// --------------------------------------------------------------------------
//
// Here we test whether all values of a sequence are identical
//   IsCompaticle(s)
//   fStart
//   fLastRun
//   fNumEvents
//   fPeriod
//   fProject
//   fSource
//   fRuns
//   fHvSettings
//   fTriggerTable
//   fLightCondition
//   fRuns
//   fRunsSub
//
Bool_t MSequence::operator==(const MSequence &s) const
{
    return IsCompatible(s) &&
        // Unnecessary
        fStart          == s.fStart          &&
        fLastRun        == s.fLastRun        &&
        fNumEvents      == s.fNumEvents      &&
        fPeriod         == s.fPeriod         &&
        fProject        == s.fProject        &&
        fSource         == s.fSource         &&
        fRuns           == s.fRuns           &&
        fHvSettings     == s.fHvSettings     &&
        fTriggerTable   == s.fTriggerTable   &&
        fLightCondition == s.fLightCondition &&
        Check(fRunsSub, s.fRunsSub);
}

// --------------------------------------------------------------------------
//
// Check whether the sequence has also identical source, i.e.
// the sequence filepath and the datapath in addition to operator==
//
Bool_t MSequence::IsIdentical(const MSequence &s) const
{
    return *this==s &&
        // Obsolete
        fDataPath==s.fDataPath && fFileName==s.fFileName;
}

//---------------------------------------------------------------------------
//
// Make sure that the name used for writing doesn't contain a full path
//
const char *MSequence::GetBaseName() const
{
    return gSystem->BaseName(fFileName);
}

//---------------------------------------------------------------------------
//
// Make sure that the name used for writing doesn't contain a full path
//
const char *MSequence::GetFilePath() const
{
    return gSystem->DirName(fFileName);
}

// --------------------------------------------------------------------------
//
// Find a sequence of continous numbers in f starting at pos (looking
// only at n entries). The output is converted into sequences
// of X (single r) and X:Y (a sequence between x and r). The function
// returnes when position pos+n is reached
//
TString MSequence::GetNumSequence(Int_t pos, Int_t n, const TArrayI &f) const
{
    TString str;

    Int_t p=pos;
    while (p<pos+n)
    {
        str += MString::Format(" %d", f[p]);

        if (p==pos+n-1)
            break;

        int i=0;
        while (++i<n)
            if (f[p+i]-f[p]!=i)
                break;

        if (i>1)
            str += MString::Format(":%d", f[p+i-1]);

        p += i;
    }

    return str;
}

// --------------------------------------------------------------------------
//
// Search for a sequence of continous numbers in r with f==0 starting at p.
// A new starting p is returned. The output is converted into sequences
// of X (single r) and X:Y (a sequence between x and r). The function
// returnes with the next f!=0.
//
TString MSequence::GetNumSequence(Int_t &p, const TArrayI &r, const TArrayI &f) const
{
    TString str;

    while (p<r.GetSize() && f[p]==0)
    {
        // serach for the first entry which doesn't fit
        // or has a file number which is != 0
        int i=0;
        while (p+ ++i<r.GetSize())
            if (r[p+i]-r[p]!=i || f[p+i]!=0)
                break;

        // None found (i-1==0)
        if (i-1==0)
            return str;

        // The last run found in the sequence (e.g. 5.0) is followed
        // by an identical run number but file != 0 (e.g. 5.1)
        if (p+i<f.GetSize() && r[p+i]==r[p] && f[p+i]!=0)
            i--;

        // Now we have to check again whether found no valid entry
        if (i-1==0)
            return str;

        str += MString::Format(" %d", r[p]);
        // p now points to the last entry which fits and
        // has a file number == 0
        p += i-1;
        // Only one      found (i-1==1)
        // More tahn one found (i-1>1)
        str += i-1==1 ? " " : ":";
        str += MString::Format("%d", r[p]);

        // One step forward
        p++;
    }

    return str;
}

// --------------------------------------------------------------------------
//
// Print the runs in a compressed wa. Returns the RunXXXXXXX string
// simplyfing the file setup
//
TString MSequence::PrintRuns(ostream &out, const char *pre, const char *name, const TArrayI &r, const TArrayI &f) const
{
    if (r.GetSize()==0)
        return "";

    out << pre << name;
    if (f.GetSize()==0)
        const_cast<TArrayI&>(f).Set(r.GetSize());

#ifdef DEBUG
    for (int i=0; i<r.GetSize(); i++)
        out << "  " << r[i] << "." << f[i];
    out << endl;
    return "";
#endif

    TString str;

    Int_t pos = 0;
    while (pos<r.GetSize())
    {
        TString rc = GetNumSequence(pos, r, f);
        if (!rc.IsNull())
        {
            out << rc;
            continue;
        }

        Int_t n = GetSubArray(pos, r.GetSize(), (Int_t*)r.GetArray());
        // end reached
        if (n<0)
            break;

        // This can happen if it is the last entry
        if (n==1)
        {
            out << " " << r[pos];
            if (f[pos]>=0)
                out << "." << f[pos];
        }
        else
        {
            // Check for sequence
            Bool_t isseq = kTRUE;
            for (int i=1; i<n; i++)
                if (f[pos+i]-f[pos]!=i)
                    isseq=kFALSE;

            if (isseq)
            {
                out << " " << r[pos] << ".";
                if (f[pos]>=0)
                    out << f[pos];
                out << ":" << f[pos+n-1];
            }
            else
            {
                out << " " << r[pos] << "+";

                str += '\n';
                str += pre;
                str += MString::Format("Run%08d:", r[pos]);
                str += GetNumSequence(pos, n, f);
            }
        }

        pos += n;
    }

    out << endl;

    return str;
}

// --------------------------------------------------------------------------
//
// Print the numbers in the classical way (one run after the other)
//
void MSequence::PrintRunsClassic(ostream &out, const char *pre, const char *name, const TArrayI &r) const
{
    out << pre << name;
    for (int i=0; i<r.GetSize(); i++)
        out << " " << r[i];
    out << endl;
}

// --------------------------------------------------------------------------
//
// Print the contents of the sequence
//
void MSequence::Print(ostream &out, Option_t *o) const
{
    const TString opt(o);

    const TString pre = opt.Contains("prefixed") ? MString::Format("Sequence%08d.", fSequence) : "";

    if (!IsValid())
    {
        out << pre << "Sequence: " << fFileName << " <invalid>" << endl;
        return;
    }
    if (!fFileName.IsNull())
        out << "# FileName: " << fFileName << endl;
    if (!fDataPath.IsNull())
        out << "# DataPath: " << fDataPath << endl;
    out << endl;
    if (pre.IsNull())
        out << "Sequence:       " << fSequence << endl;
    if (fTelescope!=1)
        out << "Telescope:      " << fTelescope << endl;
    if (fMonteCarlo)
        out << pre << "MonteCarlo:     Yes" << endl;
    if (fPeriod!=(UInt_t)-1)
        out << pre << "Period:         " << fPeriod << endl;
    if (fNight!=MTime())
        out << pre << "Night:          " << fNight.GetStringFmt("%Y-%m-%d") << endl;
    out << endl;
    if (fLightCondition!=kNA)
    {
        out << pre << "LightCondition: ";
        switch (fLightCondition)
        {
        case kNA:       out << "n/a" << endl;      break;
        case kNoMoon:   out << "NoMoon" << endl;   break;
        case kTwilight: out << "Twilight" << endl; break;
        case kMoon:     out << "Moon" << endl;     break;
        case kDay:      out << "Day" << endl;      break;
        }
    }

    if (fStart!=MTime())
        out << pre << "Start:          " << fStart.GetSqlDateTime() << endl;
    if (fLastRun!=(UInt_t)-1)
        out << pre << "LastRun:        " << fLastRun << endl;
    if (fDrsSequence)
        out << pre << "DrsSequence:    " << fDrsSequence << endl;
    if (fNumEvents!=(UInt_t)-1)
        out << pre << "NumEvents:      " << fNumEvents << endl;
    if (!fProject.IsNull())
        out << pre << "Project:        " << fProject << endl;
    if (!fSource.IsNull())
        out << pre << "Source:         " << fSource << endl;
    if (!fTriggerTable.IsNull())
        out << pre << "TriggerTable:   " << fTriggerTable << endl;
    if (!fHvSettings.IsNull())
        out << pre << "HvSettings:     " << fHvSettings << endl;
    out << endl;

    TString str;
    if (!HasSubRuns() && opt.Contains("classic"))
    {
        PrintRunsClassic(out, pre, "Runs:     ", fRuns);
        PrintRunsClassic(out, pre, "CalRuns:  ", fCalRuns);
        PrintRunsClassic(out, pre, "PedRuns:  ", fPedRuns);
        PrintRunsClassic(out, pre, "DataRuns: ", fDatRuns);
        PrintRunsClassic(out, pre, "DrsRuns:  ", fDrsRuns);
        PrintRunsClassic(out, pre, "DrsFiles: ", fDrsFiles);
        PrintRunsClassic(out, pre, "Exclude:  ", fExclRuns);
    }
    else
    {
        // FIXME: This needs more check!
        // FORBID the + syntax in "Runs"
        // Could we make "Runs" an automatic summary of all runs?
        // Should we add a check forbidding the same run in
        // Cal/Ped/Data? What about the Exclude?
        /*str +=*/ PrintRuns(out, pre, "Runs:     ", fRuns,     fRunsSub);
        str += PrintRuns(out, pre, "CalRuns:  ", fCalRuns,  fCalRunsSub);
        str += PrintRuns(out, pre, "PedRuns:  ", fPedRuns,  fPedRunsSub);
        str += PrintRuns(out, pre, "DatRuns:  ", fDatRuns,  fDatRunsSub);
        str += PrintRuns(out, pre, "DrsRuns:  ", fDrsRuns,  fDrsRunsSub);
        str += PrintRuns(out, pre, "DrsFiles: ", fDrsFiles, fDrsFilesSub);
        str += PrintRuns(out, pre, "Exclude:  ", fExclRuns, fExclRunsSub);
    }

//    if (!fDataPath.IsNull())
//        out << endl << pre << "DataPath: " << fDataPath << endl;

    if (!str.IsNull())
        out << str << endl;

    out << endl;

    if (!fComment.IsNull())
        out << pre << "Comment: " << fComment << endl;

    if (GetDrsFile())
        out << "# DrsFile: " << GetDrsFile() << endl;
}

// --------------------------------------------------------------------------
//
// Print the contents of the sequence to gLog
//
void MSequence::Print(Option_t *o) const
{
    gLog << all;
    Print(gLog, o);
}

// --------------------------------------------------------------------------
//
// Print the contents of the sequence to the file with name filename
//
Bool_t MSequence::WriteFile(const char *name, const Option_t *o) const
{
    ofstream fout(name);
    if (!fout)
    {
        gLog << err << "ERROR - Cannot open file " << name << ": ";
        gLog << strerror(errno) << endl;
        return kFALSE;
    }

    Print(fout, o);

    if (!fout)
    {
        gLog << err << "ERROR - Writing file " << name << ": ";
        gLog << strerror(errno) << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// check if the run/file is contained in the arrays.
//
Bool_t MSequence::IsContained(const TArrayI &arr, const TArrayI &sub, UInt_t run, UInt_t file) const
{
    // Backward compatibilty
    if (sub.GetSize()==0)
    {
        for (int i=0; i<arr.GetSize(); i++)
            if (run==(UInt_t)arr[i])
                return kTRUE;
    }
    else
    {
        for (int i=0; i<arr.GetSize(); i++)
            if (run==(UInt_t)arr[i] && file==(UInt_t)sub[i])
                return kTRUE;
    }
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Exclude all runs which are found in the list, e.g. "100 102 105"
//
void MSequence::ExcludeRuns(TString runs)
{
    // FIXME: Decode stream!!!

    TArrayI data, sub;
    Split(runs, data, sub);
    for (int i=0; i<data.GetSize(); i++)
        ExcludeFile(data[i], sub[i]);
}

// --------------------------------------------------------------------------
//
// Return the excluded runs (to be more precise:the excluded files)
// in a string
//
const TString MSequence::GetExcludedRuns() const
{
    TString rc;
    for (int i=0; i<fExclRuns.GetSize(); i++)
    {
        rc += fExclRuns[i];
        if (fExclRunsSub.GetSize()>0)
        {
            rc += ".";
            rc += fExclRunsSub[i];
        }
        rc += " ";
    }
    return rc(0, rc.Length()-1);
}

// --------------------------------------------------------------------------
//
// If you want to change or set the night manually.
// The Format is
//     SetNight("yyyy-mm-dd");
//
void MSequence::SetNight(const char *txt)
{
    TString night(txt);

    if (night.IsNull())
    {
        fNight  = MTime();
        fPeriod = UInt_t(-1);
        return;
    }

    night += " 00:00:00";
    fNight.SetSqlDateTime(night);

    fPeriod = fNight.GetMagicPeriod();
}

// --------------------------------------------------------------------------
//
// Read a sequence from gDirectory. If the default is given we try
// to find a sequence with name GetName. If a key with name name
// exists in the file read this key. Otherwise loop over all keys
// and return the first key matching the TPRegex (regular expression)
// matching defined by the argument.
//
Int_t MSequence::Read(const char *name)
{
    if (!name || !gDirectory)
        return MParContainer::Read(name);

    if (gDirectory->GetListOfKeys()->FindObject(name))
        return MParContainer::Read(name);

    TPRegexp regexp(name);

    TIter NextK(gDirectory->GetListOfKeys());
    TObject *key = 0;
    while ((key=NextK()))
        if (TString(key->GetName())(regexp)==key->GetName())
            return MParContainer::Read(key->GetName());

    *fLog << warn << "WARNING - No key in " << gDirectory->GetName() << " matching " << name << "." << endl;
    return 0;
}

// --------------------------------------------------------------------------
//
// If the sequence name seq is just a digit it is inflated to a full
// path following the datacenter standard.
//
// Returns if file accessible or not.
//
Bool_t MSequence::InflateSeq(TString &seq, Bool_t ismc)
{
    TObjArray *arr = seq.Tokenize(':');
    if (arr->GetEntries()<3)
    {
        TString num1 = (*arr)[0]->GetName();
        TString num2 = arr->GetEntries()==2 ? (*arr)[1]->GetName() : "";

        if (num1.IsDigit() && num2.IsDigit())
        {
            gLog << err << "ERROR - InflateSeq 'telescope:sequence' not yet implemented." << endl;
            delete arr;
            return kFALSE;
        }

        if (num1.IsDigit() && num2.IsNull())
        {
            const Int_t numseq = seq.Atoi();
            seq = "/magic/";
            if (ismc)
                seq += "montecarlo/";
            seq += MString::Format("sequences/%04d/sequence%08d.txt", numseq/10000, numseq);
            gLog << inf << "Inflated sequence file: " << seq << endl;
        }
    }
    delete arr;

    if (!gSystem->AccessPathName(seq, kFileExists))
        return kTRUE;

    gLog << err << "Sorry, sequence file '" << seq << "' doesn't exist." << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Search starting at position p in array arr and return the number
// of elemets which are identical to the starting entry (the starting entry
// is also counted)
//
Int_t MSequence::GetSubArray(Int_t p, Int_t n, Int_t *arr)
{
    Int_t *ptr0 = arr+p;

    Int_t *ptr = ptr0;
    Int_t *end = arr+n;

    while (ptr<end && *ptr==*ptr0)
        ptr++;

    return ptr-ptr0;
}

// --------------------------------------------------------------------------
//
// Sort the array arr2 starting at position p for following entries
// for which arr1 is equal. If not at least two entries are found which
// can be sorted return -1.
//
// The absolute index of the next entry in arr1 is returned.
//
Int_t MSequence::SortArraySub(Int_t p, Int_t n, Int_t *arr1, Int_t *arr2)
{
    Int_t *ptr2 = arr2+p;

    Int_t cnt = GetSubArray(p, n, arr1);
    if (cnt==0)
        return -1;

    TArrayI srt(cnt, ptr2);
    MJob::SortArray(srt);

    memcpy(ptr2, srt.GetArray(), srt.GetSize()*sizeof(Int_t));

    return p+srt.GetSize();
}

void MSequence::SortArrays(TArrayI &arr1, TArrayI &arr2)
{
    if (arr1.GetSize()!=arr2.GetSize())
        return;

    TArrayI idx(arr1.GetSize());

    TArrayI srt1(arr1);
    TArrayI srt2(arr2);

    TMath::Sort(arr1.GetSize(), srt1.GetArray(), idx.GetArray(), kFALSE);

    for (int i=0; i<arr1.GetSize(); i++)
    {
        arr1[i] = srt1[idx[i]];
        arr2[i] = srt2[idx[i]];
    }

    Int_t p = 0;
    while (p>=0)
        p = SortArraySub(p, arr1.GetSize(), arr1.GetArray(), arr2.GetArray());
}
