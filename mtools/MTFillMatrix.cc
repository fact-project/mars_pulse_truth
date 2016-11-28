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
!   Author(s): Thomas Bretz, 12/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MTFillMatrix
//
// Use this tool to fill eg trainings and test-matrices, while the matrix
// to be filled can be a real matrix (MHMatrix) or a file (MWriteRootFile)
// or both.
//
// First create a reference histogram (MH3). For more details see
// MFEventSelector2 which is used to select events according to the
// reference histogram.
//
// If no reference histogram is available the number of events are
// randomly choosen from the sample with a probability which fits
// the total destination number of events.
//
// Here is an example of how to choose 1000 events somehow distributed in
// size from a file.
// -----------------------------------------------------------------------
//
// MH3 ref("MHillas.fSize");          // choose a predefined size distribution
// // Now setup the distribution
//
// MHMatrix matrix1;                   // create matrix to fill
// matrix.AddColumn("MHillas.fWidth"); // setup columns of your matrix
//
// MReadMarsFile read("myfile.root");  // Setup your 'reader'
// read.DisableAutoScheme();           // make sure everything is read
//
// MTFillMatrix fill(&ref);            // setup MTFillMatrix
// fill.SetNumDestEvents1(1000);       // setup number of events to select
// fill.SetDestMatrix1(&matrix1);      // setup destination matrix
// if (!fill.Process())                // check if everything worked
//    return;
// fill.WriteMatrix1("myoutput.root"); // write matrix to file
//
//
// To get two matrices instead of one (splitted randomly) you can add
// the following before calling Process():
// ------------------------------------------------------------------------
//
// MHMatrix matrix2;
// fill.SetNumDestEvents2(500);        // setup number of events to select
// fill.SetDestMatrix2(&matrix2);      // setup destination matrix
// [...]
// fill.WriteMatrix2("myoutput2.root");
//
//
// To write both matrices into a single file use:
// ----------------------------------------------
//
// fill.WriteMatrices("myfile.root");
//
//
// Task List execution
// ----------------------------------------------
//
// The tasklist is excuted in the follwowing order:
//   - fReader      set by SetReader
//   - fPreTasks    in the order as set with the SetPreTask functions
//   - fPreCuts     in the order as set with the SetPreCut  functions
//   - Selection    the event selection
//   - fPostTasks   in the order as set with the SetPostTask functions
//   - Fill Matrix
//   - Write output
//
/////////////////////////////////////////////////////////////////////////////
#include "MTFillMatrix.h"

#include <TFile.h>
#include <TMath.h>

// environment
#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

// eventloop
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

// tasks
#include "MRead.h"
#include "MFillH.h"
#include "MContinue.h"

// filters
#include "MFDataPhrase.h"
#include "MFilterList.h"
#include "MFEventSelector.h"
#include "MFEventSelector2.h"

ClassImp(MTFillMatrix);

using namespace std;

// --------------------------------------------------------------------------
//
// Print Size and contained columns.
// Check whether the number of generated events is compatible with
// the number of requested events.
//
Bool_t MTFillMatrix::CheckResult(MHMatrix *m, Int_t num) const
{
    if (!m || num==0)
        return kTRUE;

    m->Print("SizeCols");

    if (num<0)
        return kTRUE;

    const Int_t generated = m->GetM().GetNrows();
    if (TMath::Abs(generated-num) <= TMath::Sqrt(9.0*num))
        return kTRUE;

    *fLog << warn << "WARNING - No. of generated events (";
    *fLog << generated << ") incompatible with requested events (";
    *fLog << num << ") for " << m->GetDescriptor() << endl;

    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Write the given MHMatrix with its name as default name to a
// file fname.
//
Bool_t MTFillMatrix::WriteMatrix(MHMatrix *m, const TString &fname, Int_t i) const
{
    if (!m)
    {
        *fLog << "ERROR - Unable to write matrix #" << i << " (=NULL)... ignored." << endl;
        return kFALSE;
    }
    if (fname.IsNull())
    {
        *fLog << "ERROR - Unable to write matrix, file name empty." << endl;
        return kFALSE;
    }

    TFile file(fname, "RECREATE", m->GetTitle());
    m->Write();
    return kTRUE;
}

void MTFillMatrix::Init(const char *name, const char *title)
{
    fName  = name  ? name  : "MFillMatrix";
    fTitle = title ? title : "Tool to fill MHMatrix from file";
}

MTFillMatrix::MTFillMatrix(const char *name, const char *title)
: fReference(0), fReader(0), fDestMatrix1(0), fDestMatrix2(0),
  fNumDestEvents1(0), fNumDestEvents2(0), fNumMaxEvents(0),
  fWriteFile1(0), fWriteFile2(0)
{
    Init(name, title);
}

// --------------------------------------------------------------------------
//
// Constructor. Takes an MH3 as argument. This MH3 is the reference
// distribution to fill the matrix. More information can be found
// at MFEventSelector2 which is used to select the events.
//
// If no MH3 *ref is given the events are randomly selected from the
// total sample - this may result in samples which don't have exactly
// the predefined size, but it is much faster.
//
MTFillMatrix::MTFillMatrix(const MH3 *ref, const char *name, const char *title)
: fReference(0), fReader(0), fDestMatrix1(0), fDestMatrix2(0),
  fNumDestEvents1(0), fNumDestEvents2(0), fNumMaxEvents(0),
  fWriteFile1(0), fWriteFile2(0)
{
    Init(name, title);
    if (ref)
        fReference = (MH3*)ref->Clone();
}

MTFillMatrix::~MTFillMatrix()
{
    if (fReference)
        delete fReference;
}

//------------------------------------------------------------------------
//
// Function serving AddPreCuts, AddPreTasks and AddPostTasks
//
void MTFillMatrix::Add(const TList &src, const TClass *cls, TList &dest)
{
    TIter Next(&src);
    TObject *obj=0;
    while ((obj=Next()))
        if (obj->InheritsFrom(cls))
            dest.Add(obj);
}

//------------------------------------------------------------------------
//
// Add a cut which is used to fill the matrix, eg "MMcEvt.fOartId<1.5"
// (The rule is applied, nit inverted: The matrix is filled with
// the events fullfilling the condition)
//
void MTFillMatrix::AddPreCut(const char *rule)
{
    MFilter *f = new MFDataPhrase(rule);
    f->SetBit(kCanDelete);
    AddPreCut(f);
}

//------------------------------------------------------------------------
//
// Add a cut which is used to fill the matrix. If kCanDelete is set
// MJOptimize takes the ownership.
//
void MTFillMatrix::AddPreCut(MFilter *f)
{
    fPreCuts.Add(f);
}

//------------------------------------------------------------------------
//
// Add all entries deriving from MFilter from list to PreCuts.
// The ownership is not affected.
//
void MTFillMatrix::AddPreCuts(const TList &list)
{
    Add(list, MFilter::Class(), fPreCuts);
}

//------------------------------------------------------------------------
//
// Add a task which is executed before the precuts. If kCanDelete is set
// MJOptimize takes the ownership.
//
void MTFillMatrix::AddPreTask(MTask *t)
{
    fPreTasks.Add(t);
}

//------------------------------------------------------------------------
//
// Add all entries deriving from MTask from list to PreTasks.
// The ownership is not affected.
//
void MTFillMatrix::AddPreTasks(const TList &list)
{
    Add(list, MTask::Class(), fPreTasks);
}

//------------------------------------------------------------------------
//
// Add a task which is executed after the precuts. If kCanDelete is set
// MJOptimize takes the ownership.
//
void MTFillMatrix::AddPostTask(MTask *t)
{
    fPostTasks.Add(t);
}

//------------------------------------------------------------------------
//
// Add all entries deriving from MTask from list to PostTasks.
// The ownership is not affected.
//
void MTFillMatrix::AddPostTasks(const TList &list)
{
    Add(list, MTask::Class(), fPostTasks);
}

// --------------------------------------------------------------------------
//
// Fill the matrix (FIXME: Flow diagram missing)
//
Bool_t MTFillMatrix::Process(const MParList &parlist)
{
    if (!fReader)
    {
        *fLog << err << "ERROR - No task to read data was given... abort." << endl;
        return kFALSE;
    }

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Fill " << fDestMatrix1->GetDescriptor() << " with ";
    if (fNumDestEvents1<0)
        *fLog << "all events";
    else
        *fLog << fNumDestEvents1;
    cout << "." << endl;
    if (fDestMatrix2)
    {
        *fLog << "Fill " << fDestMatrix2->GetDescriptor() << " with ";
    if (fNumDestEvents2<0)
        *fLog << "all events";
    else
        *fLog << fNumDestEvents2;
    cout << "." << endl;
    }
    *fLog << "Distribution choosen ";
    if (fReference && fReference->GetHist().GetEntries()>0)
        *fLog << "from " << fReference->GetDescriptor();
    else
        *fLog << "randomly";
    *fLog << endl;

    //
    // Create parameter list and task list, add tasklist to parlist
    //
    //parlist.Print();
    MParList  plist(parlist);
    MTaskList tlist;
    plist.AddToList(&tlist);

    //
    // A selector to select a given number of events from a sample
    //
    // FIXME: Merge MFEventSelector and MFEventSelector2
    MFilter *selector=0;
    if (fNumDestEvents1>0 || fNumDestEvents2>0)
    {
        if (fReference)
        {
            // Case of a reference/nominal distribution
            // The events must be read before selection
            MFEventSelector2 *sel = new MFEventSelector2(*fReference);
            sel->SetNumMax(fNumDestEvents1+fNumDestEvents2);
            sel->SetInverted();

            selector = sel;
        }
        else
        {
            // Case of a random distribution
            // The events can be selected before reading
            MFEventSelector *sel = new MFEventSelector;
            sel->SetNumSelectEvts(fNumDestEvents1+fNumDestEvents2);
            fReader->SetSelector(sel);

            selector = sel;
        }
    }

    //
    // Continue for PreCuts
    //
    MFilterList list;
    list.SetName("PreCuts");
    if (!list.AddToList(fPreCuts))
        *fLog << err << "ERROR - Calling MFilterList::AddToList for fPreCuts failed!" << endl;

    MContinue cont0(&list);
    cont0.SetInverted();

    //
    // Continue for all events which are not (SetInverted())
    // selected by the 'selector'
    //
    MContinue cont(selector);

    //
    // Create a filter doing a random split
    //
    const Double_t prob = (Double_t)fNumDestEvents1/(fNumDestEvents1+fNumDestEvents2);
    MFEventSelector split;
    split.SetSelectionRatio(prob);

    //
    // Create the logical inverted filter for 'split'
    //
    MFilterList invsplit;
    invsplit.AddToList(&split);
    invsplit.SetInverted();

    //
    // The two tasks filling the two matrices
    //
    MFillH fill1(fDestMatrix1);
    MFillH fill2(fDestMatrix2);
    fill1.SetFilter(&split);
    fill2.SetFilter(&invsplit);

    // entries in MTaskList
    tlist.AddToList(fReader);        // Read events
    tlist.AddToList(fPreTasks, 0);   // PreTasks
    if (fPreCuts.GetEntries()>0)
        tlist.AddToList(&cont0);     // PreCuts
    if (fReference && selector)
        tlist.AddToList(&cont);      // select a sample of events
    tlist.AddToList(&invsplit);      // process invsplit (which implicitly processes split)
    tlist.AddToList(fPostTasks, 0);  // PostTasks
    if (fDestMatrix1)
        tlist.AddToList(&fill1);     // fill matrix 1
    if (fDestMatrix2)
        tlist.AddToList(&fill2);     // fill matrix 2
    if (fWriteFile1)
    {
        fWriteFile1->SetFilter(&split);
        tlist.AddToList(fWriteFile1);
    }
    if (fWriteFile2)
    {
        fWriteFile2->SetFilter(&invsplit);
        tlist.AddToList(fWriteFile2);
    }

    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    //
    // Execute the eventloop
    //
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);

    const Bool_t rc = evtloop.Eventloop(fNumMaxEvents);

    if (selector)
        delete selector;

    if (!rc)
    {
        *fLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

    // Check the result of filling...
    CheckResult(fDestMatrix1, fNumDestEvents1);
    CheckResult(fDestMatrix2, fNumDestEvents2);

    *fLog << inf << GetDescriptor() << ": Done." << endl;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Write both matrices to a file. Return kFALSE if writing one of them
// failed or both have the same name.
//
Bool_t MTFillMatrix::WriteMatrices(const TString &fname) const
{
    if (fDestMatrix1 && fDestMatrix2 &&
        (TString)fDestMatrix1->GetName()==(TString)fDestMatrix2->GetName())
    {
        *fLog << "ERROR - Cannot write matrices (both have the same name)... ignored." << endl;
        return kFALSE;
    }

    return WriteMatrix1(fname) && WriteMatrix2(fname);
}

Int_t MTFillMatrix::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NumDestEvents1", print))
    {
        rc = kTRUE;
        SetNumDestEvents1(GetEnvValue(env, prefix, "NumDestEvents1", fNumDestEvents1));
    }
    if (IsEnvDefined(env, prefix, "NumDestEvents2", print))
    {
        rc = kTRUE;
        SetNumDestEvents2(GetEnvValue(env, prefix, "NumDestEvents2", fNumDestEvents2));
    }
    return rc;
}
