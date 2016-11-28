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
!   Author(s): Thomas Bretz, 7/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MReadFiles
//
/////////////////////////////////////////////////////////////////////////////
#include "MReadFiles.h"

#include <errno.h>

#include <TObjString.h>

#include "MLog.h"
#include "MLogManip.h"

#include "izstream.h"
#include "MDirIter.h"

ClassImp(MReadFiles);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Creates an array which stores the file names of
// the files which should be read. If a filename is given it is added
// to the list.
//
MReadFiles::MReadFiles(const char *fname, const char *name, const char *title)
    : fNumFile(0), fNumLine(0), fParList(0), fIn(0)
{
    fName  = name  ? name  : "MRead";
    fTitle = title ? title : "Reads a Reflector output file";

    //
    // remember file name for opening the file in the preprocessor
    //
    fFileNames.SetOwner();

    if (fname)
        AddFile(fname);
}

// --------------------------------------------------------------------------
//
// Delete the filename list and the input stream if one exists.
//
MReadFiles::~MReadFiles()
{
    if (fIn)
        delete fIn;
}

// --------------------------------------------------------------------------
//
// Call Close() and reset fNumLine and fNumFile
//
Bool_t MReadFiles::Rewind()
{
    Close();

    fNumFile = 0;
    fNumLine = 0;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read a line from the file, return kFALSE if stream is at the end,
// kTRUE in case of success. Increase fNumLine in case of success.
//
Bool_t MReadFiles::ReadLine(TString &line)
{
    line.ReadLine(*fIn);
    if (!*fIn)
        return kFALSE;

    fNumLine++;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Close an open stream and set fIn to NULL
//
void MReadFiles::Close()
{
    if (fIn)
        delete fIn;

    fIn = NULL;
    fFileName = "";
}

// --------------------------------------------------------------------------
//
// This opens the next file in the list and deletes its name from the list.
//
Bool_t MReadFiles::OpenNextFile(Int_t num)
{
    // First close an open file, if any
    Close();

    if (num>=0)
        fNumFile=num;

    // Check for the existence of a next file to read
    if (fNumFile >= (UInt_t)fFileNames.GetSize())
    {
        *fLog << inf << GetDescriptor() << ": No unread files anymore..." << endl;
        return kFALSE;
    }

    // open the file which is the first one in the chain
    fFileName = fFileNames.At(fNumFile)->GetName();

    *fLog << inf << "Open file: " << fFileName << endl;

    // open a stream to a zipped or unzipped file
    fIn = new izstream(fFileName);
    if (!*fIn)
    {
        *fLog << err << "Cannot open file " << fFileName << ": ";
        *fLog << strerror(errno) << endl;
        return kFALSE;
    }

    //*fLog << inf;
    //fLog->Separator(fFileName);

    fNumFile++;
    fNumLine=0;

    // return stream status
    return bool(*fIn);
}

// --------------------------------------------------------------------------
//
// Open next file (the first one) and call AnalyzeHeader.
// Rewind() afterwards.
//
Int_t MReadFiles::PreProcess(MParList *plist)
{
    fParList = plist;

    Rewind();

    if (!OpenNextFile())
        return kFALSE;

    if (!AnalyzeHeader(*plist))
        return kFALSE;

    Rewind();

    return kTRUE;
}

Int_t MReadFiles::Process()
{
    // if no file open or there was a failure
    if (!fIn || !*fIn)
    {
        // try to open the next file
        if (!OpenNextFile())
            return kFALSE;

        // read the header from the file
        if (!ReadHeader())
            return kERROR;

        // after reading the header ReInit tasklist
        if (!ReInit(fParList))
            return kERROR;
    }

    // Read one event
    return ReadEvent();
}

Int_t MReadFiles::PostProcess()
{
    Close();
    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Check if the file is accessible
//
Bool_t MReadFiles::CheckFile(TString name) const
{
    gSystem->ExpandPathName(name);

    if (IsFileValid(name))
        return kTRUE;

    *fLog << err;
    *fLog << "ERROR - File " << name<< " not accessible!" << endl;
    return kFALSE;;
}

// --------------------------------------------------------------------------
//
// Add this file as the last entry in the chain. Wildcards as accepted
// by MDirIter are allowed.
//
Int_t MReadFiles::AddFile(const char *txt, int)
{
    const TString dir  = gSystem->DirName(txt);
    const TString base = gSystem->BaseName(txt);

    Int_t num = 0;

    MDirIter Next(dir, base);
    while (1)
    {
        const TString fname = Next();
        if (fname.IsNull())
            break;

        if (!CheckFile(fname))
            continue;

        if (fFileNames.FindObject(fname))
            *fLog << warn << "WARNING - " << fname << " already in list." << endl;

        *fLog << inf2 << fname << " added to list." << endl;
        fFileNames.AddLast(new TObjString(fname));
        num++;
    }

    if (num>1)
        *fLog << inf2 << num << " files added to list." << endl;

    if (num==0)
        *fLog << warn << "WARNING - No files found at " << txt << endl;

    return num;
}
