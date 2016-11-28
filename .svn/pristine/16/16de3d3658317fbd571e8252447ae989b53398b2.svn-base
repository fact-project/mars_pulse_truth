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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MReadReports
//
// Read from a file events from different trees ordered in time, eg:
//
// Having a file with:
//
//      Tree1         Tree2         Tree3
//      ------------  ------------  -----------
//      (0) MTime[0]
//                    (0) MTime[1]
//      (1) MTime[2]
//      (2) MTime[3]
//                                  (0) MTime[1]
//      (3) MTime[4]
//
// MReadReports will read the events in the tree in the following order:
//   <0> (0) from Tree1
//   <1> (0) from Tree2
//   <2> (1) from Tree1
//   <3> (2) from Tree1
//   <4> (0) from Tree3
//   <5> (3) from Tree1
//   ...
//
// To tell MReadReports which Trees to read use: MReadReports::AddTree()
// To schedule a file for reading use MReadReports::AddFile()
//
// All calls to AddTree _must_ be before the calls to AddFile!
//
// After reading from a tree with the name 'TreeName' the stream id of
// the main tasklist ('MTaskList' found in MParList in PreProcess) is
// set to this name. This means that only tasks having this stream id
// are executed.
//
/////////////////////////////////////////////////////////////////////////////
#include "MReadReports.h"

#include <TChain.h>
#include <TChainElement.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MTime.h"
#include "MParList.h"
#include "MTaskList.h"

#include "MReadMarsFile.h"

ClassImp(MReadReports);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Set fName and fTitle. Instatiate fTrees and fChains.
// Call SetOwner for fTrees and fChains
//
MReadReports::MReadReports() : fEnableAutoScheme(kFALSE)
{
    fName  = "MRead";
    fTitle = "Reads events and reports from a root file ordered in time";

    fTrees  = new MTaskList("MReadReports");
    fChains = new TList;

    fTrees->SetOwner();
    fChains->SetOwner();
}

// --------------------------------------------------------------------------
//
// Destructor, delete everything which was allocated by this task...
//
MReadReports::~MReadReports()
{
    TObject *o=0;
    TIter NextC(fChains);
    while ((o=NextC()))
        delete *GetTime((TChain*)o);

    delete fTrees;
    delete fChains;
}

// --------------------------------------------------------------------------
//
// Return the number of entries in all trees.
//
UInt_t MReadReports::GetEntries()
{
    UInt_t n=0;

    TIter NextT(fTrees->GetList());
    MReadTree *tree=0;
    while ((tree=(MReadTree*)NextT()))
        n += tree->GetEntries();

    return n;
}

// --------------------------------------------------------------------------
//
// In case of a Master Tree GetFileName() of the MReadMarsFile is returned.
// If no master is available "<MReadReports>" is returned.
//
TString MReadReports::GetFullFileName() const
{
    if (!TestBit(kHasMaster))
        return "<MReadReports>";

    TIter NextT(fTrees->GetList());
    MReadTree *tree=0;
    while ((tree=(MReadTree*)NextT()))
        if (tree->InheritsFrom("MReadMarsFile"))
            return tree->GetFileName();

    return "<n/a>";

}

void MReadReports::AddToBranchList(const char *name)
{
    MTask::AddToBranchList(name);
}

// --------------------------------------------------------------------------
//
// Schedule the contents of this tree for reading. As a default the time
// branch which is used for the ordering is assumed to by "MTime"+tree.
// If this is not the case you can overwrite the default specifying the
// name in time.
//
// All calls to AddTree _must_ be BEFORE the calls to AddFile!
//
// To be done: A flag(?) telling whether the headers can be skipped.
//
void MReadReports::AddTree(const char *tree, const char *time, Type_t master)
{
    /*
    if (fTrees->GetNumTasks()>0)
    {
        *fLog << warn << "WARNING - AddTree must be called before AddFile... ignored." << endl;
        *fLog << dbg << fTrees->GetNumTasks() << endl;
        return kFALSE;
    }
    */

    if (master==kMaster && TestBit(kHasMaster))
    {
        *fLog << warn << GetDescriptor() << " already has a master tree... ignored." << endl;
        master = kStandard;
    }

    MReadTree *t = master==kMaster ? new MReadMarsFile(tree) : new MReadTree(tree);
    t->SetName(tree);
    t->SetTitle(time?time:"");
    if (master==kMaster)
        SetBit(kHasMaster);
    if (master==kRequired)
        t->SetBit(kIsRequired);

    if (!fEnableAutoScheme)
        t->DisableAutoScheme();

    //FIXME!
    //t->DisableAutoScheme();

    fTrees->AddToList(t);
    //    return kTRUE;
}

MReadTree *MReadReports::GetReader(const char *tree) const
{
    return (MReadTree*)fTrees->FindObject(tree);
}

// --------------------------------------------------------------------------
//
// Schedule a file or several files (using widcards) for reading.
//
// All calls to AddTree _must_ be BEFORE the calls to AddFile!
//
Int_t MReadReports::AddFile(const char *fname, Int_t entries)
{
    Int_t n=0;

    TIter NextT(fTrees->GetList());
    MReadTree *tree=0;
    while ((tree=(MReadTree*)NextT()))
        n += tree->AddFile(fname, entries);

    return n;
}

// --------------------------------------------------------------------------
//
// Count the number of required trees and store the number if fNumRequired.
// Reset the kIsProcessed bit.
//
void MReadReports::ForceRequired()
{
    fNumRequired = 0;

    TIter Next(fTrees->GetList());
    TObject *o=0;
    while ((o=Next()))
        if (o->TestBit(kIsRequired))
        {
            o->ResetBit(kIsProcessed);
            fNumRequired++;
        }

    *fLog << dbg << "Number of required trees: " << fNumRequired << endl;
}

// --------------------------------------------------------------------------
//
// Find MTaskList and store a pointer to it in fList.
// Delete all entries in fChains.
// Create all chains to read the time in the trees in advance.
// Enable only the time-branch in this chains.
// PreProcess fTrees (a MTaskList storing MReadTree tasks for reading)
//
Int_t MReadReports::PreProcess(MParList *plist)
{
    fChains->Delete();

    Int_t i=0;

    TIter NextT(fTrees->GetList());
    MReadTree *tree=0;
    while ((tree=(MReadTree*)NextT()))
    {
        if (!((TChain*)tree->fChain)->GetFile())
        {
            *fLog << warn << "No files or no tree '" << tree->GetName() << "'... skipped." << endl;
            fTrees->RemoveFromList(tree);
            continue;
        }

        if (tree->GetEntries()==0)
        {
            *fLog << warn << "No events in tree '" << tree->GetName() << "'... skipped." << endl;
            fTrees->RemoveFromList(tree);
            continue;
        }

        TString tn(tree->GetTitle());
        if (tn.IsNull())
        {
            tn += "MTime";
            tn += tree->GetName();
            tn += ".";
        }

        TString tn2(tn);
        tn2 += "*";

        // FIXME: Should be tree->AddToBranchList such that
        //        each run a new 'table' is created, but
        //        MRead is searching for MTaskList in the
        //        parameter list.
        //AddToBranchList((const char*)tn2);

        //
        // SetBranchStatus wants to have a pointer to a pointer
        //
        MTime **tx = new MTime*;
        *tx = 0;//new MTime;

        TChain *c=new TChain(tree->GetName());

        c->SetBranchStatus("*", 0);
        c->SetBranchAddress(tn, tx);
        c->SetBranchStatus(tn2, 1);

        c->Add((TChain*)tree->fChain);
        c->GetEntry(0);

        fChains->Add(c);

        i++;
    }

    if (i==0)
    {
        *fLog << err << "Files do not contain any valid tree... abort." << endl;
        return kFALSE;
    }

    fPosEntry.Set(i);
    fPosEntry.Reset();

    // Force that with the next call to Process the required events are read
    ForceRequired();
    //fFirstReInit=kTRUE;

    // Preprocess all tasks in fTrees
    return fTrees->CallPreProcess(plist);
}

// --------------------------------------------------------------------------
//
// If this is not the first ReInit after PreProcess force the required
// trees to be read first (call FirstRequired())
//
/*
Bool_t MReadReports::ReInit(MParList *plist)
{
    if (!fFirstReInit)
        ForceRequired();
    fFirstReInit=kFALSE;
    return kTRUE;
}
*/

// --------------------------------------------------------------------------
//
// Return the number of the tree which is the next one to be read.
// The condition for this decision is the time-stamp.
//
Int_t MReadReports::FindNextTime()
{
    TIter NextC(fChains);
    TChain *c=0;

    Int_t nmin=0;
    MTime tmin(**GetTime((TChain*)NextC()));

    Int_t i=0;

    while ((c=(TChain*)NextC()))
    {
        MTime &t = **GetTime(c);
        i++;

        if (t >= tmin)
            continue;

        tmin = t;
        nmin = i;
    }
    return nmin;
}

// --------------------------------------------------------------------------
//
// Return the number of the tree which is the next one to be read.
// The condition for this decision kIsRequired but not kIsProcessed is set.
//
Int_t MReadReports::FindNextRequired()
{
    Int_t n = 0;

    TIter Next(fTrees->GetList());
    TObject *o=0;
    while ((o=Next()))
    {
        if (o->TestBit(kIsRequired) && !o->TestBit(kIsProcessed))
        {
            o->SetBit(kIsProcessed);
            fNumRequired--;
            *fLog << dbg << "Reading from tree " << n << " " << o->GetName() << endl;
            return n;
        }
        n++;
    }

    return -1;
}

// --------------------------------------------------------------------------
//
// Return the MTime corresponding to this TChain...
//
MTime** MReadReports::GetTime(TChain *c) const
{
    TChainElement *e=(TChainElement*)c->GetStatus()->At(1);
    return (MTime**)e->GetBaddress();
}

// --------------------------------------------------------------------------
//
// Check which is the next tree to read from. Read an event from this tree.
// Sets the StreamId accordingly.
//
Int_t MReadReports::Process()
{
    while (fChains->GetSize())
    {
        // Find the next tree to read from checking the time-stamps
        // of the next events which would be read
        const Int_t nmin=FindNext();
        if (nmin<0)
        {
            *fLog << err << "MReadReports::Process: ERROR - Determination of next tree failed... abort." << endl;
            return kERROR;
        }

        // Read the event from this tree
        MTask *task = static_cast<MTask*>(fTrees->GetList()->At(nmin));
        const Int_t rc = task->CallProcess();

        // Read the time-stamp of the next event
        TChain *chain = static_cast<TChain*>(fChains->At(nmin));
        const Int_t cnt = chain->GetEntry(++fPosEntry[nmin]);

        // In case there is no further time-stamp of an error reading the
        // event we remove this time-stamp from the list of time-stamps to
        // be checked for reading the next events, because there is none.
        if (cnt<=0 || rc==kFALSE)
        {
            *fLog << inf << "Removing chain " << chain->GetName() << " from list" << flush;

            // Find index of chain to be removed
            const Int_t idx = fChains->IndexOf(chain);
            *fLog << " (" << idx << ")" << flush;

            delete *GetTime(chain);        // Delete MTime*
            *fLog << "." << flush;
            delete fChains->Remove(chain); // Remove chain from TList
            *fLog << "." << flush;

            // Change array accordingly
            if (fPosEntry.GetSize()>1)
            {
                for (int i=idx; i<fPosEntry.GetSize()-1; i++)
                    fPosEntry[i] = fPosEntry[i+1];
                fPosEntry[fPosEntry.GetSize()-1] = 0;
            }

            // FIXME: Maybe MTaskList should have a member function to
            //        reorder the tasks?

            // Move this task to the end of the list so that nmin still
            // corresponds to the correct task in the list.
            const_cast<TList*>(fTrees->GetList())->Remove(task);
            *fLog << "." << flush;
            const_cast<TList*>(fTrees->GetList())->AddLast(task);
            *fLog << "done." << endl;
        }

        // If something else than kFALSE (means: stop reading from this
        // tree) has happened we return the return code of the processing
        if (rc!=kFALSE)
            return rc;
    }

    return kFALSE;
}

// --------------------------------------------------------------------------
//
// PostProcess all MReadTree tasks in fTrees.
//
Int_t MReadReports::PostProcess()
{
    return fTrees->CallPostProcess();
}

// --------------------------------------------------------------------------
//
// PrintStatistics of this task and of the MReadTree tasks in fTress
//
void MReadReports::PrintStatistics(const Int_t lvl, Bool_t title, Double_t time) const
{
    MRead::PrintStatistics(lvl, title, time);
    fTrees->PrintStatistics(lvl+1, title, GetCpuTime());
}
