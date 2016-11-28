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
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MReadMarsFile
//
// This task works more or less like MReadTree, but in addition PreProcess
// reads all the information from the 'RunHeader' tree.
//
// Warning: Until now this works only for 'one run header per file'
//
/////////////////////////////////////////////////////////////////////////////
#include "MReadMarsFile.h"

#include <fstream>

#include <TFile.h>
#include <TTree.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MTaskList.h"

#include "MRawRunHeader.h"

#include "MMcRunHeader.hxx"

ClassImp(MReadMarsFile);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default constructor. Don't use it.
//
MReadMarsFile::MReadMarsFile() : fRun(NULL)
{
    fName  = "MRead";
    fTitle = "Read tree and run headers from Mars file.";
}

// --------------------------------------------------------------------------
//
//  Default constructor. It creates a MReadTree object to read the
//  RunHeaders and disables Auto Scheme for this tree.
//
MReadMarsFile::MReadMarsFile(const char *tname, const char *fname,
                             const char *name, const char *title)
    : MReadTree(tname, fname)
{
    fName  = name  ? name  : "MRead";
    fTitle = title ? title : "Read tree and run headers from Mars file.";

    //
    // open the input stream
    //
    fRun = new MReadTree("RunHeaders", fname, "RunHeaders");

    //
    // This disables the auto scheme. because reading new runheader is done
    // at a low frequency we don't loose time if we always read all
    // runheaders
    //
    fRun->DisableAutoScheme();
}

// --------------------------------------------------------------------------
//
// Destructor. Deleted the MReadTree object for the RunHeaders
//
MReadMarsFile::~MReadMarsFile()
{
    delete fRun;
}

Byte_t MReadMarsFile::IsFileValid(const char *name)
{
    ifstream fin(name);
    if (!fin)
        return 0;

    Char_t c[4];
    fin.read(c, 4);
    if (!fin)
        return 0;

    if (!(c[0]=='r'&& c[1]=='o' && c[2]=='o' && c[3]=='t'))
        return 0;

    TFile f(name, "READ");

    TTree *t = (TTree*)f.Get("Events");
    if (!t)
        return 0;

    // FIXME: Replace numbers by enum! Maybe use bits?
    if (t->FindBranch("MRawEvtData."))
        return t->FindBranch("MMcEvt.") ? 2 : 1;

    if (t->FindBranch("MCerPhotEvt."))
        return t->FindBranch("MMcEvt.") ? 4 : 3;

    return 0;
}

// --------------------------------------------------------------------------
//
//  see MReadTree::AddFile, too. The file is also added to the chain for
//  the run headers. If adding file gives a different result for both
//  chains -1 is returned, otherwise the number of files which were added.
//
Int_t MReadMarsFile::AddFile(const char *fname, Int_t entries)
{
    //
    // FIXME! A check is missing whether the file already exists or not.
    //
    //
    // returns the number of file which were added
    //
    if (!fRun)
        return -1;

    Int_t n1 = fRun->AddFile(fname);
    Int_t n2 = MReadTree::AddFile(fname, entries);

    return n1 != n2 ? -1 : n1;
}

// --------------------------------------------------------------------------
//
//  Sort the files by their file-names
//
void MReadMarsFile::SortFiles()
{
    fRun->SortFiles();
    MReadTree::SortFiles();
}

// --------------------------------------------------------------------------
//
//  This overload MReadTree::Notify. Before the MReadTree Notify
//  TObjects are called the RunHeaders of the next files are read.
//  
//  WARNING: This doesn't work correctly yet, if the files are not read in
//           increasing order.
//
Bool_t MReadMarsFile::Notify()
{
    UInt_t runtype = 0xffff;

    const MRawRunHeader *rawheader = (MRawRunHeader*)fParList->FindObject("MRawRunHeader");
    if (rawheader)
        runtype = rawheader->GetRunType();

    //
    // Try to read the new run headers. If reading the new run header
    // was successfull call the ReInits
    //
    const Int_t idx = GetFileIndex();
    fRun->SetEventNum(idx<0?0:idx); // Assumption: One Entry per File!

    const Int_t rc = fRun->Process();
    if (rc!=kTRUE)
    {
        *fLog << err << "ERROR - Cannot read new runheaders #" << idx;
        *fLog << " after reading event #" << GetNumEntry();
        *fLog << " (MReadTree::Process() returned rc=" << rc << ")" << endl;
        return kFALSE;
    }

    if (!MReadTree::Notify())
        return kFALSE;

    if (rawheader && runtype!=0xffff && runtype != rawheader->GetRunType())
    {
        *fLog << warn << "WARNING - You are mixing files with different run types (";
        *fLog << runtype << ", " << rawheader->GetRunType() << ")" << endl;
    }

    if (!rawheader->IsValid())
    {
        *fLog << warn << "WARNING - The run header (MRawRunHeader) read from the file returns not IsValid()." << endl;
        *fLog << "          Please check if the file contents are ok." << endl;
        rawheader->Print("header");
    }

    const MMcRunHeader *mcheader = (MMcRunHeader*)fParList->FindObject("MMcRunHeader");
    if (mcheader)
    {
        if (mcheader->GetCamVersion()==50)
        {
            *fLog << warn << "WARNING - You are using a file created with Camera 0.5." << endl;
            *fLog << "In this camera version some events have undefined Impact-Values" << endl;
            *fLog << "(MMcEvt::fImpact) Please don't use it for MC studies using the" << endl;
            *fLog << "impact parameter." << endl;
        }
    }

    MTaskList *tlist = (MTaskList*)fParList->FindObject("MTaskList");
    if (!tlist)
    {
        *fLog << err << dbginf << "ERROR - MTaskList not found in Parameter List." << endl;
        return kFALSE;
    }

    if (tlist->ReInit())
        return kTRUE;

    //MReadTree::Notify();

    *fLog << err << "ERROR - ReInit of '" << tlist->GetDescriptor() << "' failed." << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
//  PreProcessed the MReadTree to read the run headers and its base class.
//  see MReadTree::PreProcess for more information
//
Int_t MReadMarsFile::PreProcess(MParList *pList)
{
    if (!fRun)
    {
        *fLog << err << "Error - Cannot use MReadMarsFile instantiated with default constructor... aborting." << endl;
        return kFALSE;
    }

    fParList = pList;

    if (!fRun->PreProcess(pList))
    {
        *fLog << err << "Error - PreProcessing MReadMarsFile::fRun... aborting." << endl;
        return kFALSE;
    }

    // Check if really one file conatains one RunHeader
    // (FIXME: Do the check more properly)
    if ((Int_t)fRun->GetEntries() < fRun->GetNumFiles())
    {
        *fLog << err << "Error - Number of files exceeds number of RunHeaders... aborting." << endl;
        return kFALSE;
    }

    if ((Int_t)fRun->GetEntries() > fRun->GetNumFiles())
    {
        *fLog << err << "Error - Number of RunHeaders exceeds number of files... aborting." << endl;
        return kFALSE;
    }

    fRun->SetEventNum(0); 
    const Int_t rc = fRun->Process();
    if (rc!=kTRUE)
    {
        *fLog << err << "ERROR - Cannot read runheaders of first file.";
        *fLog << " (MReadTree::Process() returned rc=" << rc << ")" << endl;
        return kFALSE;
    }

    return MReadTree::PreProcess(pList);
}
