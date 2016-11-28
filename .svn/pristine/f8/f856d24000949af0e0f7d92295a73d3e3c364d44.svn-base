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
!   Author(s): Thomas Bretz, 1/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MDataSet
//
//  This class describes a collection of sequences.
//
//  Such an input file looks like:
//
//     crab.txt:
//     ---------
//       AnalysisNumber: 1
//
//       Name: SecondCrab
//
//       SequencesOn:  35222 35229
//       SequencesOff: 36817
//
//       SequencePath: /magic/sequences
//       DataPath:     /magic/data/star
//
//       Sequence00035222.File: sequences/sequence035222.txt
//       Sequence00036817.File: sequences/sequence036817.txt
//
//       Sequence00035222.Dir: /data2/wuerzburg/Crab-Analyse/images/035222
//       Sequence00036817.Dir: /data2/wuerzburg/Crab-Analyse/images/036817
//
//       Sequence00036817.Exclude: 36818 36819
//
//       WobbleMode: No
//       MonteCarlo: No
//
//       SourceName: CrabNebula
//       Catalog: /magic/datacenter/setup/magic_favorites_dc.edb
//
//
// Reading the file is based on TEnv. For more details see also
// the class reference of TEnv.
//
//
// AnalysisNumber: The analysis number is an artifical number used to name
//  the output files automatically if the names are not overwritten in the
//  corresponding programs.
//
// SequencePath: In case it is not specified the datacenter default path is
//  used. If it is given it is the place at which the sequence files
//  are searched, if not overwritten by either a program command line
//  option (aka. a argument to the constructor) or a resource for
//  an individual sequence file. Note, that the four digits high-level
//  directories to sort the sequences are added to the given path.
//
// DataPath: In case it is not specified the datacenter default path is
//  used. If it is given it is the place at which the data files
//  are searched, if not overwritten by either a program command line
//  option (aka. a argument to the constructor) or a resource for
//  an individual data path.  Note, that the four digits high-level
//  directories to sort the sequences are added to the given path.
//
// SequencesOn/Off: The sequence number are used to concatenate the filenames
//  of the sequences using the file structure used in the datacenter. Each
//  sequence can be added to the on and off data at the same time but only
//  once.
//
//  If you have different file names you can overwrite the default file names
//  using Sequence%08d.File (make sure you have 8 digits!)
//
//  If the file name is given as "-", e.g.
//       Sequence00035222.File: -
//  the current dataset file is read as sequence file. In this case all
//  resources defined for a sequence file should be prefixed by
//  "Sequence%08d."; if not prefixed the default without the prefix
//  is looked up. Be aware that the "Sequence" resource for sequence-files
//  (for more details see MSequence) is ignored and the already defined
//  number is used instead and thus should be identical to the sequence
//  number - which normally is not necessary.
//
//  In standard coditions (datacenter file system) paths are concatenated
//  by using the information in the sequence files (date, etc). You can
//  overwrite the directories in which the sequence-files (eg I-files) are
//  stored using Sequence%08d.Dir (make sure you have 8 digits!)
//
//  Using the resource Sequence%08d.Exclude you can exclude a list of
//  runs from a sequence.
//
// WobbleMode: This is just a flag which is passed to the program
//  end eveluated (or not) by the individual program, which takes this
//  dataset as an input. For example this is necessary in ganymed to do
//  some wobble mode special setup. If no resource is given (or it is
//  set to "auto") wobble mode if set if the datset contains no off-
//  sequences.
//
// MonetCarlo: This is just a flag which is passed to the program
//  end eveluated (or not) by the individual program, which takes this
//  dataset as an input. For example this tells ganymed to skip some
//  parts accessing time stamps which are not available in the MCs.
//
// SequencePath/DataPath: This determined were the sequences and data-files
//  are stored. The priorities are as follows:
//   0) Build in default path:          /magic/sequences   /magic/data/star
//   1) Def.path from dataset file:       SequencePath         DataPath
//   2) Indiv.path from dataset file:  12345.SequencePath   12345.DataPath
//   3) Path from command line:            Argument in the constructors
//   4) Path for an indiv. dataset:         Sequences00022555.File/Dir
//
// Catalog: This is the xephem-style file from the central control
//  containing the observed sky positions.
//
// SourceName: The source name, as defined in the catalog, of the object
//  the data corresponds to.
//
// Name: A name is stored for your convinience
//
// Comment: It is just stored.
//
// General Remark: MDataSet doesn't implement any action on the given
// resources. Whether a resource is used, necessary or ignored, and
// what will happen if you change the resource is always defined by the
// executed macro or program.
//
// Resource file entries are case sensitive!
//
//
// Collection of datsets
// ---------------------
//
//  You can combine more than one datset in a file (if your program
//  supports this) Such a dataset file could look like this:
//
//       35222.SequencesOn: 35222
//       35222.SequencesOff: 36817
//
//       65778.SequencesOn: 65778
//
//       12345.SequencesOn: 12345
//
//       12345.SequencePath: /home/nobody/specialsequences
//       65778.DataPath:     /home/nobody/specialstars
//
//       SequencePath: /home/nobody/defaultsequences
//       DataPath:     /home/nobody/defaultstars
//
//       Sequence00035222.File: sequences/sequence035222.txt
//       Sequence00035222.Dir:  /data2/wuerzburg/Crab-Analyse/images/035222
//
//       35222.WobbleMode: Off
//       WobbleMode: On
//
//
// IMPORTANT:
//   * Run filenames must begin with a string which allows correct
//     ordering in time, otherwise synchronization might fail.
//   * Sequence filenames should also have names allowing to order them
//     in time, but it is not necessary.
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
// Class Version 3:
// ----------------
//  + fFileName
//  + fDataSet
//
// Class Version 2:
// ----------------
//  + fMonteCarlo
//  + fWobbleMode
//  - fIsWobbleMode
//
/////////////////////////////////////////////////////////////////////////////
#include "MDataSet.h"

#include <string.h>  // necessary for Fedora core 2 with kernel 2.6.9-1.667 #1 and gcc 3.4.2
#include <errno.h>   // necessary for Fedora core 2 with kernel 2.6.9-1.667 #1 and gcc 3.4.2

#include <stdlib.h>
#include <fstream>

#include <TEnv.h>
#include <TChain.h>
#include <TRegexp.h>
#include <TSystem.h> // TSystem::ExpandPath

#include "MLog.h"
#include "MLogManip.h"

#include "MRead.h"
#include "MJob.h"
#include "MEnv.h"
#include "MAstro.h"
#include "MString.h"
#include "MDirIter.h"
#include "MSequence.h"
#include "MPointingPos.h"

ClassImp(MDataSet);

using namespace std;

const TString MDataSet::fgCatalog = "/magic/datacenter/setup/magic_favorites.edb";

// --------------------------------------------------------------------------
//
// Copy the sequence numbers from the TString runs into the TArrayI data
// Sequences which are twice in the list are only added once. In this case
// a warning is emitted.
//
void MDataSet::Split(TString &runs, TArrayI &data) const
{
    const TRegexp regexp("[0-9]+");

    data.Set(0);

    runs.ReplaceAll("\t", " ");
    runs = runs.Strip(TString::kBoth);

    while (!runs.IsNull())
    {
        const TString num = runs(regexp);

        if (num.IsNull())
        {
            *fLog << warn << "WARNING - Sequence is NaN (not a number): '" << runs << "'" << endl;
            break;
        }

        const Int_t seq = atoi(num.Data());
        const Int_t n   = data.GetSize();

        // skip already existing entries
        int i;
        for (i=0; i<n; i++)
            if (data[i] == seq)
                break;

        if (i<n)
            *fLog << warn << "WARNING - Sequence #" << seq << " already in list... skipped." << endl;
        else
        {
            // set new entry
            data.Set(n+1);
            data[n] = seq;
        }

        // remove entry from string
        runs.Remove(0, runs.First(num)+num.Length());
    }

    MJob::SortArray(data);
}

// --------------------------------------------------------------------------
//
// After resolving the sequence filename and directory either from the
// default (/magic/data/sequences/0004/sequence00004000.txt) or from
// the corresponding entries in the dataset file.
// The entries are sorted by filename.
//
void MDataSet::ResolveSequences(const TEnv &env, const TString &prefix, const TArrayI &num, TList &list) const
{
    TString sequences = fPathSequences;
    TString data      = fPathDataFiles;

    for (int i=0; i<num.GetSize(); i++)
    {
        TString name = GetEnvValue3(env, prefix, "Sequence%d.File",    num[i]);
        TString dir  = GetEnvValue3(env, prefix, "Sequence%d.Dir",     num[i]);
        TString excl = GetEnvValue3(env, prefix, "Sequence%d.Exclude", num[i]);

        // Set default sequence file and dir name
        if (name.IsNull())
            name = MString::Format("%s%04d/sequence%08d.txt", sequences.Data(), num[i]/10000, num[i]);
        if (dir.IsNull())
            dir = MString::Format("%s%04d/%08d", data.Data(), num[i]/10000, num[i]);

        // Check if sequence information is contained in Dataset file
        const Bool_t useds = name=="-";

        // FIXME: The sequence number from the sequence file is assigned!!!
        MSequence *seq = new MSequence(useds?fFileName:name, dir, num[i]);
        seq->ExcludeRuns(excl);

        if (seq->IsValid() && seq->GetSequence()!=(UInt_t)num[i])
            *fLog << warn << "WARNING - Sequence number " << num[i] << " in dataset file doesn't match number " << seq->GetSequence() << " in sequence file!" << endl;

        list.Add(seq);
    }

    // For the synchronization we must make sure, that all sequences are
    // in the correct order...
    // list.Sort();
}

Bool_t MDataSet::GetWobbleMode(const TEnv &env, const TString &prefix) const
{
    TString wob = GetEnvValue2(env, prefix, "WobbleMode", "auto");

    wob.ToLower();
    wob=wob.Strip(TString::kBoth);

    if (wob=="auto")
        return !HasOffSequences();

    return GetEnvValue2(env, prefix, "WobbleMode", kFALSE);
}

// --------------------------------------------------------------------------
//
// Read a dataset from the file fname. If num is != -1 all resources are
// prefixed with the number. This allows constrcutions like:
//
//   89123.SequencesOn: 1 2 3 4
//   89130.SequencesOn: 5 6 7 8
//
// For one dataset:
//
//   89123.DataPath: /magic/data/star
//
// For all other datasets:
//
//   DataPath: /magic/data/star
//
// For one sequence of one datasets:
//
//   89123.Sequence00000002.Dir: /magic/data/star
//
// and therefore allows storage of several datsets in one file with
// a defaults for non specific resources.
//
// sequences and data are the path to the sequences (/magic/sequences)
// and the data (/magic/data/star) respectively. Both can be overwritten
// be a default from the dataset file or a resource for an individual
// sequence.
//
void MDataSet::Init(const char *fname, const UInt_t num, TString sequences, TString &data)
{
    fName  = "MDataSet";
    fTitle = "DataSet file";

    // Store given file name as name
    fFileName = fname;

    // Delete the stored Sequences automatically at destruction
    fSequencesOn.SetOwner();
    fSequencesOff.SetOwner();

    // Expand the file name (eg $MARS or ~ are expanded)
    gSystem->ExpandPathName(fFileName);

    // Check its accessibility
    const Bool_t access = !gSystem->AccessPathName(fFileName, kFileExists);
    if (!access)
    {
        gLog << err << "ERROR - Dataset file " << fname << " not accessible!" << endl;
        fNumAnalysis = (UInt_t)-1;
        return;
    }

    // Determin the prefix to access this resource
    const TString prefix = num==(UInt_t)-1 ? "" : MString::Format("%d", num);

    // Open and read the file
    MEnv env(fFileName);

    // Get analysis number and name
    fNumAnalysis = GetEnvValue2(env, prefix, "AnalysisNumber", (Int_t)num);
    fDataSet     = GetEnvValue2(env, prefix, "Name", GetBaseName());

    // Get sequences from file
    TString str;
    str = GetEnvValue2(env, prefix, "SequencesOn",  "");
    Split(str, fNumSequencesOn);
    str = GetEnvValue2(env, prefix, "SequencesOff", "");
    Split(str, fNumSequencesOff);

    // Get other resources
    fNameSource   = GetEnvValue2(env, prefix, "SourceName", "");
    fCatalog      = GetEnvValue2(env, prefix, "Catalog",    fgCatalog);
    fMonteCarlo   = GetEnvValue2(env, prefix, "MonteCarlo", kFALSE);
    fComment      = GetEnvValue2(env, prefix, "Comment",    "");

    fWobbleMode   = GetWobbleMode(env, prefix); // needs the number of off sequences

    fNameSource = fNameSource.Strip(TString::kBoth);
    fCatalog    = fCatalog.Strip(TString::kBoth);

    // Check for sequence and data path (GetDefPath needs the monte carlo flag)
    const TString defpathseq  = GetEnvValue2(env, prefix, "SequencePath", GetDefPathSequences());
    const TString defpathdata = GetEnvValue2(env, prefix, "DataPath",     GetDefPathDataFiles());

    SetupDefaultPath(sequences, defpathseq);
    SetupDefaultPath(data,      defpathdata);

    fPathSequences = sequences;
    fPathDataFiles = data;

    // Resolve sequences
    ResolveSequences(env, prefix, fNumSequencesOn,  fSequencesOn);
    ResolveSequences(env, prefix, fNumSequencesOff, fSequencesOff);

    // --- Now "touch" resources which are not yet stored in MDataSet ---
    env.Touch("RunTime");

    // --- Print "untouch" resources ---
    if (env.GetNumUntouched()>0)
    {
        gLog << warn << "WARNING - At least one resource in the dataset-file has not been touched!" << endl;
        env.PrintUntouched();
    }
}

// --------------------------------------------------------------------------
//
// Constructor. See Read for more details.
//
MDataSet::MDataSet(const char *fname, TString sequences, TString data)
{
    Init(fname, (UInt_t)-1, sequences, data);
}

// --------------------------------------------------------------------------
//
// Constructor. See Read for more details.
//
MDataSet::MDataSet(const char *fname, Int_t num, TString sequences, TString data)
{
    Init(fname, num, sequences, data);
}

//---------------------------------------------------------------------------
//
// Return the name of the file
//
const char *MDataSet::GetBaseName() const
{
    return gSystem->BaseName(fFileName);
}

//---------------------------------------------------------------------------
//
// Return the directory of the file
//
const char *MDataSet::GetFilePath() const
{
    return gSystem->DirName(fFileName);
}

// --------------------------------------------------------------------------
//
// Return '+' if both can be accessed, '-' otherwise.
//
void MDataSet::PrintFile(ostream &out, const MSequence &seq)
{
    const Char_t access =
        !gSystem->AccessPathName(seq.GetFileName(), kFileExists) &&
        !gSystem->AccessPathName(seq.GetDataPath(), kFileExists) ? '+' : '-';

    out << "#  " << access << " " << seq.GetFileName() << " <" << seq.GetDataPath() << ">" << endl;
}

// --------------------------------------------------------------------------
//
// Helper to print a seqeunce in Print()
//
void MDataSet::PrintSeq(ostream &out, const MSequence &seq) const
{
    const Bool_t useds = seq.GetFileName()==fFileName;

    out << "Sequence" << MString::Format("%08d", seq.GetSequence()) << ".File:   " << (useds?"-":seq.GetFileName()) << endl;
    out << "Sequence" << MString::Format("%08d", seq.GetSequence()) << ".Dir:    " << seq.GetDataPath() << endl;
    if (!useds && seq.GetNumExclRuns()>0)
        out << "Sequence" << MString::Format("%08d", seq.GetSequence()) << ".Exclude: " << seq.GetExcludedRuns() << endl;

    if (useds)
    {
        out << endl;
        seq.Print(out, "prefixed");
        out << endl << "# ---" << endl;
    }
}

// --------------------------------------------------------------------------
//
// Print the contents of the dataset to the ostream out
//
void MDataSet::Print(ostream &out, Option_t *o) const
{
    if (!IsValid())
    {
        out << "Dataset: " << fFileName << " <invalid - no analysis number available>" << endl;
        return;
    }
    out << "# Path: " << GetFilePath() << endl;
    out << "# Name: " << GetBaseName() << endl;
    out << endl;
    out << "AnalysisNumber: " << fNumAnalysis << endl << endl;

    if (!fDataSet.IsNull())
        out << "Name: " << fDataSet << endl << endl;

    out << "SequencesOn:   ";
    for (int i=0; i<fNumSequencesOn.GetSize(); i++)
        out << " " << fNumSequencesOn[i];
    out << endl;
    if (fNumSequencesOff.GetSize()>0)
    {
        out << "SequencesOff:  ";
        for (int i=0; i<fNumSequencesOff.GetSize(); i++)
            out << " " << fNumSequencesOff[i];
        out << endl;
    }

    out << endl;
    if (!fNameSource.IsNull())
        out << "SourceName: " << fNameSource << endl;
    out << "Catalog: " << fCatalog << endl;

    out << "WobbleMode: " << (fWobbleMode?"Yes":"No") << endl << endl;
    out << "MonteCarlo: " << (fMonteCarlo?"Yes":"No") << endl << endl;

    if (!fComment.IsNull())
        out << "Comment: " << fComment << endl;

    if (fSequencesOn.GetEntries()>0)
        out << endl;

    // FIXME: If file==fName --> print Sequence0000.content

    TIter NextOn(&fSequencesOn);
    TIter NextOff(&fSequencesOff);
    MSequence *seq=0;
    while ((seq=(MSequence*)NextOn()))
        PrintSeq(out, *seq);
    if (fSequencesOff.GetEntries()>0)
        out << endl;
    while ((seq=(MSequence*)NextOff()))
        PrintSeq(out, *seq);

    if (TString(o).Contains("files", TString::kIgnoreCase))
    {
        out << endl;
        out << "# On-Data Files:" << endl;
        NextOn.Reset();
        while ((seq=(MSequence*)NextOn()))
            PrintFile(out, *seq);

        out << endl;
        out << "# Off-Data Files:" << endl;
        NextOff.Reset();
        while ((seq=(MSequence*)NextOff()))
            PrintFile(out, *seq);
    }
}

// --------------------------------------------------------------------------
//
// Print the contents of the dataset to the gLog stream
//
void MDataSet::Print(Option_t *o) const
{
    gLog << all;
    Print(gLog, o);
}

// --------------------------------------------------------------------------
//
// Print the contents of the dataset to the file with name filename
//
void MDataSet::WriteFile(const char *name, const Option_t *o) const
{
    ofstream fout(name);
    if (!fout)
    {
        gLog << err << "Cannot open file " << name << ": ";
        gLog << strerror(errno) << endl;
        return;
    }

    Print(fout, o);
}

// --------------------------------------------------------------------------
//
// Adds all sequences contained in list to the MDirIter. After adding
// everything MDirIter::Sort is called to sort all entries by name.
//
Bool_t MDataSet::AddSequencesFromList(const TList &list, MDirIter &files)
{
    TIter Next(const_cast<TList*>(&list));

    MSequence *seq=0;
    while ((seq=(MSequence*)Next()))
    {
        if (!seq->IsValid())
        {
            gLog << err;
            gLog << "ERROR - MDataSet::AddSequencesFromList: Sequence invalid!" << endl;
            gLog << "  + File: " << seq->GetFileName() << endl;
            gLog << "  + Dir:  " << seq->GetDataPath() << endl;
            return kFALSE;
        }

        if (seq->GetRuns(files, MSequence::kImages)<=0)
            return kFALSE;
    }

    // This is important in case of synchronisation, because the
    // files in the sequences can be interleaved (eg W1, W2)
    // Filenames MUST begin with an appropriate string which allow
    // to order them correctly in time!
    // files.Sort();

    if (gLog.GetDebugLevel()>4)
    {
        gLog << inf << "Files which are searched:" << endl;
        files.Print();
    }
    return kTRUE;
}

Bool_t MDataSet::AddFilesOn(MDirIter &iter) const
{
    return AddSequencesFromList(fSequencesOn, iter);
}

Bool_t MDataSet::AddFilesOff(MDirIter &iter) const
{
    return AddSequencesFromList(fSequencesOff, iter);
}

Bool_t MDataSet::AddFiles(MDirIter &iter) const
{
    const Bool_t rc1 = AddFilesOff(iter);
    const Bool_t rc2 = AddFilesOn(iter);
    return rc1 && rc2;
}

Bool_t MDataSet::AddFilesOn(MRead &read) const
{
    MDirIter files;
    if (!AddFilesOn(files))
        return kFALSE;
    return read.AddFiles(files)>0;
}

Bool_t MDataSet::AddFilesOff(MRead &read) const
{
    MDirIter files;
    if (!AddFilesOff(files))
        return kFALSE;
    return read.AddFiles(files)>0;
}

Bool_t MDataSet::AddFiles(MRead &read) const
{
    const Bool_t rc1 = AddFilesOff(read);
    const Bool_t rc2 = AddFilesOn(read);
    return rc1 && rc2;
}

Int_t MDataSet::AddFilesToChain(MDirIter &files, TChain &chain)
{
    Int_t num=0;
    while (1)
    {
        const TString fname = files.Next();
        if (fname.IsNull())
            break;

        const Int_t n = chain.Add(fname);
        if (n<=0)
            return kFALSE;
        num += n;
    }
    return num;
}

Bool_t MDataSet::AddFilesOn(TChain &chain) const
{
    MDirIter files;
    if (!AddSequencesFromList(fSequencesOn, files))
        return kFALSE;
    return AddFilesToChain(files, chain)>0;
}

Bool_t MDataSet::AddFilesOff(TChain &chain) const
{
    MDirIter files;
    if (!AddSequencesFromList(fSequencesOff, files))
        return kFALSE;
    return AddFilesToChain(files, chain)>0;
}

Bool_t MDataSet::AddFiles(TChain &read) const
{
    const Bool_t rc1 = AddFilesOff(read);
    const Bool_t rc2 = AddFilesOn(read);
    return rc1 && rc2;
}

Bool_t MDataSet::GetSourcePos(MPointingPos &pos) const
{
    if (!HasSource())
    {
        gLog << err << "ERROR - MDataSet::GetSourcePos called, but no source available." << endl;
        return kFALSE;
    }

    TString catalog(fCatalog);
    gSystem->ExpandPathName(catalog);

    ifstream fin(catalog);
    if (!fin)
    {
        gLog << err << "Cannot open file " << catalog << ": ";
        gLog << strerror(errno) << endl;
        return kFALSE;
    }

    TString ra, dec, epoch;

    Int_t n = 0;
    while (1)
    {
        TString line;
        line.ReadLine(fin);
        if (!fin)
        {
            gLog << err << "ERROR - Source '" << fNameSource << "' not found in " << catalog << "." << endl;
            return kFALSE;
        }

        n++;

        TObjArray *arr = line.Tokenize(",");

        if (arr->GetEntries()<6)
        {
            gLog << err << "ERROR - Not enough arguments in line #" << n << " of " << catalog << endl;
            delete arr;
            return kFALSE;;
        }

        const TString name = (*arr)[0]->GetName();

        ra    = (*arr)[2]->GetName();
        dec   = (*arr)[3]->GetName();
        epoch = (*arr)[5]->GetName();

        delete arr;

        if (name.Strip(TString::kBoth)==fNameSource)
            break;
    }

    if (epoch.Strip(TString::kBoth)!=(TString)"2000")
    {
        gLog << err << "ERROR - Epoch not 2000... not supported." << endl;
        return kFALSE;
    }

    Double_t r,d;
    if (!MAstro::Coordinate2Angle(ra, r))
    {
        gLog << err << "ERROR - Interpreting right ascension: " << ra << endl;
        return kFALSE;
    }
    if (!MAstro::Coordinate2Angle(dec, d))
    {
        gLog << err << "ERROR - Interpreting declination: " << dec << endl;
        return kFALSE;
    }

    pos.SetSkyPosition(r, d);
    pos.SetTitle(fNameSource);

    return kTRUE;
}

/*
// --------------------------------------------------------------------------
//
// Calls ReplaceAll(old, news) for all Dir-entries
//
void MDataSet::ReplaceDir(TList &list, const TString &old, const TString &news) const
{
    TIter Next(&list);
    TNamed *name = 0;
    while ((name=(TNamed*)Next()))
    {
        TString dir = name->GetTitle();
        dir.ReplaceAll(old, news);
        name->SetTitle(dir);
    }
}

// --------------------------------------------------------------------------
//
// Calls ReplaceAll(old, news) for all File-entries
//
void MDataSet::ReplaceFile(TList &list, const TString &old, const TString &news) const
{
    TIter Next(&list);
    TNamed *name = 0;
    while ((name=(TNamed*)Next()))
    {
        TString file = name->GetName();
        file.ReplaceAll(old, news);
        name->SetName(file);
    }
}
*/
