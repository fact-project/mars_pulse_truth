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
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MTask
//
//   Base class for all tasks which can perfomed in a tasklist
//   For each event processed in the eventloop all the different
//   tasks in the tasklist will be processed.
//
//   So all tasks must inherit from this baseclass.
//
//   The inheritance from MInputStreamID is used to indicate the
//   type of event that this task is for. If it is "All" it is executed
//   independantly of the actual ID of the task list.
//
//   Inside this abstract class, there are three fundamental function:
//
//   - PreProcess():   executed before the eventloop starts. Here you
//                     can initiate different things, open files, etc.
//                     As an argument this function gets a pointer to the
//                     parameter list. You can stop the execution by
//                     returning kFALSE instead of kTRUE. If an error
//                     occured and you return kFALSE make sure, that
//                     any action is closed correctly and all newly
//                     created object are deleted. The PostProcess in
//                     such a case won't be executed by the Tasklist or
//                     Eventloop.
//
//   - Process():      executed for each event in the eventloop. Do it
//                     one task after the other (as they occur in the
//                     tasklist). Only the tasks with a Stream ID
//                     which matches the actual ID of the tasklist
//                     are executed. A task can return kFALSE to
//                     stop the execuition of the tasklist or
//                     kCONTINUE to skip the pending tasks. If you want
//                     to stop the eventloop and wants the eventloop to
//                     return the status 'failed' return kERROR.
//
//   - ReInit()        The idea is, that
//                       a) we have one file per run
//                       b) each file contains so called run-headers which
//                          stores information 'per run', eg MRawRunHeader
//                          or the bad pixels
//                       c) this information must be evaluated somehow each
//                          time a new file is opened.
//
//                     If you use MReadMarsFile or MCT1ReadPreProc it is
//                     called each time a new file has been opened and the
//                     new run headers have been read before the first
//                     event of these file is preprocessed.
//
//   - PostProcess():  executed after the eventloop. Here you can close
//                     output files, start display of the run parameter,
//                     etc. PostProcess is only executed in case of
//                     PreProcess was successfull (returned kTRUE)
//
//
//  Remark: Using a MTask in your tasklist doesn't make much sense,
//          because it is doing nothing. However it is a nice tool
//          to count something (exspecially if used together with a
//          filter)
//
//
//  Version 1:
//  ----------
//   - first version
//
//  Version 2:
//  ----------
//   - added fSerialNumber
//
/////////////////////////////////////////////////////////////////////////////
#include "MTask.h"

#include <fstream>

#include <TClass.h>
#include <TBaseClass.h> // OverwritesProcess
#include <TStopwatch.h> // TStopwatch

#include "MString.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MFilter.h"
#include "MStatusDisplay.h"

#undef DEBUG_PROCESS
//#define DEBUG_PROCESS

ClassImp(MTask);

using namespace std;

MTask::MTask(const char *name, const char *title)
    : fFilter(NULL), fSerialNumber(0), fIsPreprocessed(kFALSE),
    fStopwatch(0), fNumExecutions(0), fNumExec0(0), fAccelerator(0)
{
    fName  = name  ? name  : "MTask";
    fTitle = title ? title : "Base class for all tasks (dummy task).";

    fListOfBranches = new TList;
    fListOfBranches->SetOwner();

    fStopwatch = new TStopwatch;
}

// --------------------------------------------------------------------------
//
// Destructor. Delete fStopwatch and fListOfBranches
//
MTask::~MTask()
{
    delete fStopwatch;
    delete fListOfBranches;
}

// --------------------------------------------------------------------------
//
// Initialize fFilter with filter and if not null add the result of
// GetDataMember from the filter to the branch list.
//
void MTask::SetFilter(MFilter *filter)
{
    fFilter=filter;
    if (!filter)
        return;

    fFilter->SetBit(kMustCleanup);    // Better is better ;-)
    AddToBranchList(filter->GetDataMember());
}

// --------------------------------------------------------------------------
//
// This adds a branch to the list for the auto enabeling schmeme.
// This makes it possible for MReadTree to decide which branches
// are really needed for the eventloop. Only the necessary branches
// are read from disk which speeds up the calculation enormously.
//
// You can use TRegExp expressions like "*.fEnergy", but the
// recommended method is to call this function for exactly all
// branches you want to have, eg:
//  AddToBranchList("MMcTrig.fNumFirstLevel");
//  AddToBranchList("MMcTrig;1.fNumFirstLevel");
//  AddToBranchList("MMcTrig;2.fNumFirstLevel");
//
// We agreed on the convetion, that all branches are stored with
// a trailing dot '.' so that always the Master Branch name
// (eg. MMcTrig) is part of the branch name.
//
// Remark: The common place to call AddToBranchList is the
//         constructor of the derived classes (tasks)
//
void MTask::AddToBranchList(const char *b)
{
    if (fListOfBranches->FindObject(b))
        return;

    fListOfBranches->Add(new TNamed(b, ""));
}

// --------------------------------------------------------------------------
//
// Using this overloaded member function you may cascade several branches
// in acomma seperated list, eg: "MMcEvt.fTheta,MMcEvt.fEnergy"
//
// For moredetailed information see AddToBranchList(const char *b);
//
void MTask::AddToBranchList(const TString &str)
{
    TString s = str;

    while (!s.IsNull())
    {
        Int_t fst = s.First(',');

        if (fst<0)
            fst = s.Length();

        AddToBranchList((const char*)TString(s(0, fst)));

        s.Remove(0, fst+1);
    }
}

// --------------------------------------------------------------------------
//
// Copy constructor. Reset MInputStreamID, copy pointer to fFilter and
// copy the contents of fListOfBranches
//
MTask::MTask(MTask &t) : MInputStreamID()
{
    fFilter = t.fFilter;
    fListOfBranches->AddAll(t.fListOfBranches);
}

// --------------------------------------------------------------------------
//
// Mapper function for PreProcess.
// Sets the preprocessed flag dependend on the return value of PreProcess.
// Resets number of executions and cpu consumtion timer.
// If task has already been preprocessed return kTRUE.
//
Int_t MTask::CallPreProcess(MParList *plist)
{
    if (fIsPreprocessed)
        return kTRUE;

    // This does not reset the counter!
    fStopwatch->Reset();
    fNumExec0 = fNumExecutions;

    *fLog << all << GetDescriptor() << "... " << flush;
    if (fDisplay)
        fDisplay->SetStatusLine2(*this);

    switch (PreProcess(plist))
    {
    case kFALSE:
        return kFALSE;

    case kTRUE:
        fIsPreprocessed = kTRUE;
        return kTRUE;

    case kSKIP:
        return kSKIP;
    }

    *fLog << err << dbginf << "PreProcess of " << GetDescriptor();
    *fLog << " returned an unknown value... aborting." << endl;

    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Mapper function for Process.
// Executes Process dependent on the existance of a filter and its possible
// return value.
// If Process is executed, the execution counter is increased.
// Count cpu consumption time.
//
Int_t MTask::CallProcess()
{
    //
    // Check for the existance of a filter. If a filter is existing
    // check for its value. If the value is kFALSE don't execute
    // this task.
    //
    if (fFilter && !fFilter->IsConditionTrue())
        return kTRUE;

    if (!HasAccelerator(kAccDontTime))
        fStopwatch->Start(kFALSE);

    fNumExecutions++;

#ifdef DEBUG_PROCESS
    *fLog << all << flush << GetName() << "..." << flush;
#endif

    const Int_t rc = Process();

#ifdef DEBUG_PROCESS
    *fLog << all << flush << "done." << endl;
#endif

    if (!HasAccelerator(kAccDontTime))
        fStopwatch->Stop();

    return rc;
}

// --------------------------------------------------------------------------
//
// Mapper function for PreProcess.
// Calls Postprocess dependent on the state of the preprocessed flag,
// resets this flag.
//
Int_t MTask::CallPostProcess()
{
    if (!fIsPreprocessed)
        return kTRUE;

    fIsPreprocessed = kFALSE;

    *fLog << all << GetDescriptor() << "... " << flush;
    if (fDisplay)
        fDisplay->SetStatusLine2(*this);

    return PostProcess();
}

// --------------------------------------------------------------------------
//
// This is reinit function
//
// This function is called asynchronously if the tasks in the tasklist need
// reinitialization. This for example happens when the eventloop switches
// from one group of events to another one (eg. switching between events
// of different runs means reading a new run header and a new run header
// may mean that some value must be reinitialized)
//
// the virtual implementation returns kTRUE
//
Bool_t MTask::ReInit(MParList *)
{
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// This is processed before the eventloop starts
//
// It is the job of the PreProcess to connect the tasks
// with the right container in the parameter list.
//
// the virtual implementation returns kTRUE
//
Int_t MTask::PreProcess(MParList *)
{
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// This is processed for every event in the eventloop
//
// the virtual implementation returns kTRUE
//
Int_t MTask::Process()
{
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// This is processed after the eventloop starts
//
// the virtual implementation returns kTRUE
//
Int_t MTask::PostProcess()
{
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns the name of the object. If the name of the object is not the
// class name it returns the object name and in []-brackets the class name.
// If a serial number is set (!=0) the serial number is added to the
// name (eg. ;1)
//
const TString MTask::GetDescriptor() const
{
    //
    // Because it returns a (const char*) we cannot return a casted
    // local TString. The pointer would - immediatly after return -
    // point to a random memory segment, because the TString has gone.
    //
    if (fName==ClassName())
        return fSerialNumber==0 ? (TString)ClassName() : MString::Format("%s;%d", ClassName(), fSerialNumber);

    return fSerialNumber>0 ?
        MString::Format("%s;%d [%s]", fName.Data(), fSerialNumber, ClassName()) :
        MString::Format("%s [%s]",    fName.Data(), ClassName());
}

// --------------------------------------------------------------------------
//
//  Return the total number of calls to since PreProcess(). If Process() was
//  not called due to a set filter this is not counted.
//
UInt_t MTask::GetNumExecutions() const
{
    return fNumExecutions-fNumExec0;
}

// --------------------------------------------------------------------------
//
//  Return the total number of calls to Process() ever. If Process() was not
//  called due to a set filter this is not counted.
//
UInt_t MTask::GetNumExecutionsTotal() const
{
    return fNumExecutions;
}

// --------------------------------------------------------------------------
//
//  Return total CPU execution time in seconds of calls to Process().
//  If Process() was not called due to a set filter this is not counted.
//
Double_t MTask::GetCpuTime() const
{
    return fStopwatch->CpuTime();
}

// --------------------------------------------------------------------------
//
//  Return total real execution time in seconds of calls to Process().
//  If Process() was not called due to a set filter this is not counted.
//
Double_t MTask::GetRealTime() const
{
    return fStopwatch->RealTime();
}

// --------------------------------------------------------------------------
//
//  Prints the relative time spent in Process() (relative means relative to
//  its parent Tasklist) and the number of times Process() was executed.
//  Don't wonder if the sum of the tasks in a tasklist is not 100%,
//  because only the call to Process() of the task is measured. The
//  time of the support structure is ignored. The faster your analysis is
//  the more time is 'wasted' in the support structure.
//  Only the CPU time is displayed. This means that exspecially task
//  which have a huge part of file i/o will be underestimated in their
//  relative wasted time.
//  For convinience the lvl argument results in a number of spaces at the
//  beginning of the line. So that the structur of a tasklist can be
//  identified. If a Tasklist or task has filter applied the name of the
//  filter is printer in <>-brackets behind the number of executions.
//  Use MTaskList::PrintStatistics without an argument.
//  For tasks which don't overwrite Process() no action is perfomed.
//
void MTask::PrintStatistics(const Int_t lvl, Bool_t title, Double_t time) const
{
    if (!Overwrites("Process") && IsA()!=MTask::Class())
        return;

    *fLog << all << setfill(' ') << setw(lvl) << " ";

    if (GetCpuTime()>0 && time>0 && GetCpuTime()>=0.001*time && !HasAccelerator(kAccDontTime))
        *fLog << Form("%5.1f", GetCpuTime()/time*100) << "% ";
    else
        *fLog << "       ";

    if (HasStreamId())
        *fLog << GetStreamId() << ":";
    *fLog << GetDescriptor();

    if (GetNumExecutions()>0)
        *fLog << "\t" << dec << GetNumExecutions();

    if (fFilter)
        *fLog << " <" << fFilter->GetName() << ">";
    if (title)
        *fLog << "\t" << fTitle;
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// First call MParContainer::SavePrimitive which should stream the primitive
// to the output stream. Then, if a filter is set, stream first the filter
// and afterwards set the filter for this task.
//
void MTask::SavePrimitive(ostream &out, Option_t *)
{
    MParContainer::SavePrimitive(out);
    if (!fFilter)
        return;

    /*
     If we don't stream filter which are not in the task list itself
     (which means: already streamed) we may be able to use
     SavePrimitive as some kind of validity check for the macros

     fFilter->SavePrimitive(out);
     */
    out << "   " << GetUniqueName() << ".SetFilter(&" << fFilter->GetUniqueName() <<");" << endl;
    if (fSerialNumber>0)
        out << "   " << GetUniqueName() << ".SetSerialNumber(" << fSerialNumber <<");" << endl;
}

void MTask::SavePrimitive(ofstream &out, Option_t *o)
{
    SavePrimitive(static_cast<ostream&>(out), o);
}

void MTask::SetDisplay(MStatusDisplay *d)
{
    if (fFilter)
        fFilter->SetDisplay(d);
    MParContainer::SetDisplay(d);
}

// --------------------------------------------------------------------------
//
//  This is used to print the output in the PostProcess/Finalize.
//  Or everywhere else in a nice fashioned and unified way.
//
void MTask::PrintSkipped(UInt_t n, const char *str)
{
    *fLog << " " << setw(7) << n << " (";
    *fLog << Form("%5.1f", 100.*n/GetNumExecutions());
    *fLog << "%) Evts skipped: " << str << endl;
}

// --------------------------------------------------------------------------
//
// If obj==fFilter set fFilter to NULL
// Call MParcontainer::RecursiveRemove
//
void MTask::RecursiveRemove(TObject *obj)
{
    if (obj==fFilter)
        fFilter=NULL;

    if (fFilter)
        fFilter->RecursiveRemove(obj);

    MParContainer::RecursiveRemove(obj);
}
