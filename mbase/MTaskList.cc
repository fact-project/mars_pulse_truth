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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MTaskList
//
// Collection of tasks.
//
// A tasklist is necessary to run the eventloop. It contains the scheduled
// tasks, which should be executed in your program.
//
// To add a task use AddToList.
//
// The tasklist itself is a task, too. You can add a tasklist to another
// tasklist. This makes sense, if you want to filter the execution of
// more than one task of your tasklist using the same filter.
//
// The tasks in the list are idetified by their names. If more than one
// task has the same name, the tasklist will still work correctly, but
// you might run into trouble trying to get a pointer to a task by name
// from the list.
//
// Remark: The Process function is only executed if the class of your task
//         overloads Process() or if the task itself is a MTask. This
//         means if you have a task without Process() (only PreProcess
//         and PostProcess no time is lost during execution)
//
// Warning:
//  Be carefull if you are writing your tasklist
//  (eg. MWriteRootFile("file.root", "MTaskList")) to a file. You may
//  not be able to initialize a new working tasklist from a file if
//   a) Two Paramerer containers with the same names are existing in the
//      MParList.
//   b) You used a container somewhere which is not part of MParList.
//      (eg. You specified a pointer to a MH container in MFillH which is
//      not added to the parameter list.
//
/////////////////////////////////////////////////////////////////////////////
#include "MTaskList.h"

#include <fstream>           // ofstream, SavePrimitive

#include <TSystem.h>         // gSystem
#include <TOrdCollection.h>  // TOrdCollection

#include "MLog.h"
#include "MLogManip.h"

#include "MIter.h"
#include "MFilter.h"
#include "MParList.h"
#include "MInputStreamID.h"

#include "MStatusDisplay.h"

ClassImp(MTaskList);

using namespace std;

//#define DEBUG_PROCESS
#undef DEBUG_PROCESS

const TString MTaskList::gsDefName  = "MTaskList";
const TString MTaskList::gsDefTitle = "A list for tasks to be executed";

// --------------------------------------------------------------------------
//
// the name for the task list must be the same for all task lists
// because the task list (at the moment) is identified by exactly
// this name in the parameter list (by MEvtLoop::SetParList)
//
MTaskList::MTaskList(const char *name, const char *title) : fTasks(0), fParList(0), fNumPasses(0), fNumPass(0)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    fTasks = new TList;

    gROOT->GetListOfCleanups()->Add(fTasks);
    gROOT->GetListOfCleanups()->Add(&fTasksProcess);
    fTasks->SetBit(kMustCleanup);
    fTasksProcess.SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
//   CopyConstructor
//   creates a new TaskList and put the contents of an existing
//   TaskList in the new TaskList.
//
MTaskList::MTaskList(const MTaskList &ts) : MTask(), fNumPasses(ts.fNumPasses)
{
    fTasks->AddAll(ts.fTasks);
}

// --------------------------------------------------------------------------
//
//  If the 'IsOwner' bit is set (via SetOwner()) all tasks are deleted
//  by the destructor
//
MTaskList::~MTaskList()
{
    if (TestBit(kIsOwner))
        fTasks->SetOwner();

    delete fTasks;
}

// --------------------------------------------------------------------------
//
//  If the 'IsOwner' bit is set (via SetOwner()) all containers are deleted
//  by the destructor
//
void MTaskList::SetOwner(Bool_t enable)
{
    enable ? SetBit(kIsOwner) : ResetBit(kIsOwner);
}


// --------------------------------------------------------------------------
//
//  Set the logging stream for the all tasks in the list and the tasklist
//  itself.
//
void MTaskList::SetLogStream(MLog *log)
{
    fTasks->R__FOR_EACH(MTask, SetLogStream)(log);
    MTask::SetLogStream(log);
}

// --------------------------------------------------------------------------
//
//  Set the display for the all tasks in the list and the tasklist itself.
//
void MTaskList::SetDisplay(MStatusDisplay *d)
{
    fTasks->R__FOR_EACH(MTask, SetDisplay)(d);
    MTask::SetDisplay(d);
}

void MTaskList::SetAccelerator(Byte_t acc)
{
    fTasks->R__FOR_EACH(MTask, SetAccelerator)(acc);
    MTask::SetAccelerator(acc);
}

// --------------------------------------------------------------------------
//
//  Set the serial number for the all tasks in the list and the tasklist
//  itself.
//
void MTaskList::SetSerialNumber(Byte_t num)
{
    fTasks->R__FOR_EACH(MTask, SetSerialNumber)(num);
    MTask::SetSerialNumber(num);
}

Bool_t MTaskList::CheckAddToList(MTask *task, /*const char *type,*/ const MTask *where) const
{
    //
    // Sanity check
    //
    if (!task)
    {
        *fLog << err << "ERROR - task argument=NULL." << endl;
        return kFALSE;
    }

    //
    // Get Name of new task
    //
    const char *name = task->GetName();

    //
    // Check if the new task is already existing in the list
    //
    const TObject *objn = fTasks->FindObject(name);
    const TObject *objt = fTasks->FindObject(task);

    if (objn || objt)
    {
        //
        // If the task is already in the list ignore it.
        //
        if (objt || objn==task)
        {
            *fLog << warn << dbginf << "Warning: Task '" << task->GetName() << ", 0x" << (void*)task;
            *fLog << "' already existing in '" << GetName() << "'... ignoring." << endl;
            return kTRUE;
        }

        //
        // Otherwise add it to the list, but print a warning message
        //
        *fLog << warn << dbginf << "Warning: Task '" << task->GetName();
        *fLog << "' already existing in '" << GetName() << "'." << endl;
        *fLog << "You may not be able to get a pointer to this task by name." << endl;
    }

    if (!where)
        return kTRUE;

    if (fTasks->FindObject(where))
        return kTRUE;

    *fLog << err << dbginf << "Error: Cannot find task after which the new task should be scheduled!" << endl;
    return kFALSE;
}


// --------------------------------------------------------------------------
//
// schedule task for execution, before 'where'.
// 'type' is the event type which should be processed
//
Bool_t MTaskList::AddToListBefore(MTask *task, const MTask *where, const char *type)
{
    if (task==this)
    {
        *fLog << warn << "WARNING - You cannot add a tasklist to itself.  This" << endl;
        *fLog << "          would create infinite recursions...ignored." << endl;
        return kFALSE;

    }

    // FIXME: We agreed to put the task into list in an ordered way.
    if (!CheckAddToList(task, /*type,*/ where))
        return kFALSE;

    const TString stream = type ? (TString)type : task->GetStreamId();

    *fLog << inf3 << "Adding " << task->GetName() << " to " << GetName() << " for " << stream << "... " << flush;
    task->SetStreamId(stream);
    task->SetBit(kMustCleanup);
    fTasks->AddBefore((TObject*)where, task);
    *fLog << "done." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// schedule task for execution, after 'where'.
// 'type' is the event type which should be processed
//
Bool_t MTaskList::AddToListAfter(MTask *task, const MTask *where, const char *type)
{
    if (task==this)
    {
        *fLog << warn << "WARNING - You cannot add a tasklist to itself.  This" << endl;
        *fLog << "          would create infinite recursions...ignored." << endl;
        return kFALSE;

    }

    // FIXME: We agreed to put the task into list in an ordered way.
    if (!CheckAddToList(task, /*type,*/ where))
        return kFALSE;

    const TString stream = type ? (TString)type : task->GetStreamId();

    *fLog << inf3 << "Adding " << task->GetName() << " to " << GetName() << " for " << stream << "... " << flush;
    task->SetStreamId(stream);
    task->SetBit(kMustCleanup);
    fTasks->AddAfter((TObject*)where, task);
    *fLog << "done." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// schedule task for execution, 'type' is the event type which should
// be processed
//
Bool_t MTaskList::AddToList(MTask *task, const char *type)
{
    if (task==this)
    {
        *fLog << warn << "WARNING - You cannot add a tasklist to itself.  This" << endl;
        *fLog << "          would create infinite recursions...ignored." << endl;
        return kFALSE;

    }

    // FIXME: We agreed to put the task into list in an ordered way.
    if (!CheckAddToList(task/*, type*/))
        return kFALSE;

    const TString stream = type ? (TString)type : task->GetStreamId();

    *fLog << inf3 << "Adding " << task->GetName() << " to " << GetName() << " for " << stream << "... " << flush;
    task->SetStreamId(stream);
    task->SetBit(kMustCleanup);
    fTasks->Add(task);
    *fLog << "done." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Add all objects in list to the tasklist. If some of them do not
// inherit from MTask return kFALSE, also if AddToList returns an error
// for one of the tasks
//
Bool_t MTaskList::AddToList(const TList &list, const char *tType)
{
    TIter Next(&list);
    TObject *obj=0;
    while ((obj=Next()))
    {
        if (!obj->InheritsFrom(MTask::Class()))
        {
            *fLog << err << "ERROR - Object " << obj->GetName() << " doesn't inherit from MTask..." << endl;
            return kFALSE;
        }

        if (!AddToList(static_cast<MTask*>(obj), tType))
            return kFALSE;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Add all objects in list to the tasklist after task where. If some of
// them do not inherit from MTask return kFALSE, also if AddToListAfter
// returns an error for one of the tasks
//
Bool_t MTaskList::AddToListAfter(const TList &list, const MTask *where, const char *tType)
{
    TIter Next(&list);
    TObject *obj=0;
    while ((obj=Next()))
    {
        if (!obj->InheritsFrom(MTask::Class()))
        {
            *fLog << err << "ERROR - Object " << obj->GetName() << " doesn't inherit from MTask..." << endl;
            return kFALSE;
        }

        if (!AddToListAfter(static_cast<MTask*>(obj), where, tType))
            return kFALSE;

        where = static_cast<MTask*>(obj);
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Add all objects in list to the tasklist before task where. If some of
// them do not inherit from MTask return kFALSE, also if AddToListBefore
// returns an error for one of the tasks
//
Bool_t MTaskList::AddToListBefore(const TList &list, const MTask *where, const char *tType)
{
    TIter Next(&list);
    TObject *obj=0;
    while ((obj=Next()))
    {
        if (!obj->InheritsFrom(MTask::Class()))
        {
            *fLog << err << "ERROR - Object " << obj->GetName() << " doesn't inherit from MTask..." << endl;
            return kFALSE;
        }

        if (!AddToListBefore(static_cast<MTask*>(obj), where, tType))
            return kFALSE;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Find an object in the list.
//  'name' is the name of the object you are searching for.
//
TObject *MTaskList::FindObject(const char *name) const
{
    return fTasks->FindObject(name);
}

// --------------------------------------------------------------------------
//
//  check if the object is in the list or not
//
TObject *MTaskList::FindObject(const TObject *obj) const
{
    return fTasks->FindObject(obj);
}

// --------------------------------------------------------------------------
//
// find recursively a tasklist which contains a task with name task
//
MTaskList *MTaskList::FindTaskList(const char *task)
{
    TIter Next(fTasks);
    TObject *o = 0;
    while ((o=Next()))
    {
        if (strcmp(o->GetName(), task)==0)
           return this;

        MTaskList *l = dynamic_cast<MTaskList*>(o);
        if (!l)
            continue;

        if ((l=l->FindTaskList(task)))
            return l;
    }
    return 0;
}

// --------------------------------------------------------------------------
//
// find recursively a tasklist which contains a task task
//
MTaskList *MTaskList::FindTaskList(const MTask *task)
{
    TIter Next(fTasks);
    TObject *o = 0;
    while ((o=Next()))
    {
       if (o==task)
          return this;

        MTaskList *l = dynamic_cast<MTaskList*>(o);
        if (!l)
            continue;

        if ((l=l->FindTaskList(task)))
            return l;
    }

    return 0;
}

// --------------------------------------------------------------------------
//
//  removes a task from the list (used in PreProcess).
//  if kIsOwner is set the task is deleted. (see SetOwner())
//
void MTaskList::Remove(MTask *task)
{
    if (!task)
        return;

    // First remove it from the list(s) so that a later RecursiveRemove
    // cannot fint the task again
    TObject *obj = fTasks->Remove(task);

    // Now do a recursive remove on all other tasks.
    fTasks->RecursiveRemove(task);

    if (obj && TestBit(kIsOwner))
        delete obj;
}

// --------------------------------------------------------------------------
//
// Call MTask::RecursiveRemove
// Call fTasks->RecursiveRemove
//
void MTaskList::RecursiveRemove(TObject *obj)
{
    MTask::RecursiveRemove(obj);

    if (obj==fTasks)
        fTasks=NULL;

    if (fTasks)
    {
        fTasks->RecursiveRemove(obj);

        // In theory this call is obsolete
        fTasksProcess.RecursiveRemove(obj);
    }
}

// --------------------------------------------------------------------------
//
//  do pre processing (before eventloop) of all tasks in the task-list
//  Only if a task overwrites the Process function the task is
//  added to the fTaskProcess-List. This makes the execution of the
//  tasklist a little bit (only a little bit) faster, bacause tasks
//  doing no Processing are not Processed.
//
Int_t MTaskList::PreProcess(MParList *pList)
{
    *fLog << all << "Preprocessing... " << flush;
    if (fDisplay)
    {
        // Set status lines
        fDisplay->SetStatusLine1("PreProcessing...");
        fDisplay->SetStatusLine2("");
    }

    fParList = pList;

    //
    // Make sure, that the ReadyToSave flag is not reset from a tasklist
    // running as a task in another tasklist.
    //
    const Bool_t noreset = fParList->TestBit(MParList::kDoNotReset);
    if (!noreset)
        fParList->SetBit(MParList::kDoNotReset);

    //
    //  create the Iterator over the tasklist
    //
    TIter Next(fTasks);

    MTask *task=NULL;

    //
    // loop over all tasks for preproccesing
    //
    while ((task=(MTask*)Next()))
    {
        //
        // PreProcess the task and check for it's return value.
        //
        switch (task->CallPreProcess(fParList))
        {
        case kFALSE:
            return kFALSE;

        case kTRUE:
            // Handle GUI events (display changes, mouse clicks)
            if (fDisplay)
                gSystem->ProcessEvents();
            continue;

        case kSKIP:
            Remove(task);
            continue;
        }

        *fLog << err << dbginf << "PreProcess of " << task->GetDescriptor();
        *fLog << " returned an unknown value... aborting." << endl;
        return kFALSE;
    }

    *fLog << all << endl;

    //
    // Reset the ReadyToSave flag.
    //
    if (!noreset)
    {
        fParList->SetReadyToSave(kFALSE);
        fParList->ResetBit(MParList::kDoNotReset);
    }

    //
    // loop over all tasks to fill fTasksProcess
    //
    Next.Reset();
    fTasksProcess.Clear();
    while ((task=(MTask*)Next()))
        if (task->IsA()==MTask::Class() || task->Overwrites("Process"))
            fTasksProcess.Add(task);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  do reinit of all tasks in the task-list
//
Bool_t MTaskList::ReInit(MParList *pList)
{ 
    *fLog << all << "Reinit... " << flush;

    if (!pList)
        pList = fParList;

    //
    // Make sure, that the ReadyToSave flag is not reset from a tasklist
    // running as a task in another tasklist.
    //
    const Bool_t noreset = pList->TestBit(MParList::kDoNotReset);
    if (!noreset)
        pList->SetBit(MParList::kDoNotReset);

    //
    //  create the Iterator over the tasklist
    //
    TIter Next(fTasks);

    MTask *task=NULL;

    //
    // loop over all tasks for reinitialization
    //
    while ((task=(MTask*)Next()))
    {
        // Supress output if not necessary
        if (!task->Overwrites("ReInit"))
            continue;

        *fLog << all << task->GetName() << "... " << flush;

        if (!task->ReInit(pList/*?pList:fParList*/))
        {
            *fLog << err << "ERROR - ReInit of Task '" << task->GetDescriptor() << "' failed." << endl;
            return kFALSE;
        }
    }

    *fLog << all << endl;

    //
    // Reset the ReadyToSave flag.
    //
    if (!noreset)
    {
        pList->SetReadyToSave(kFALSE);
        pList->ResetBit(MParList::kDoNotReset);
    }

    return kTRUE;
}

Int_t MTaskList::ProcessTaskList()
{
    //
    //  create the Iterator for the TaskList
    //
    TIter Next(&fTasksProcess);
    MTask *task=NULL;

#ifdef DEBUG_PROCESS
    *fLog << all << "ProcessTaskList - " << GetDescriptor() << " - start." << endl;
#endif

    //
    // loop over all tasks for processing
    //
    Int_t rc = kTRUE;
    while ( (task=(MTask*)Next()) )
    {
#ifdef DEBUG_PROCESS
        *fLog << all << "Process - " << (void*)task << " " << flush;
        *fLog << task->GetDescriptor() << "... " << flush;
#endif
        //
        // if the task has the wrong stream id skip it.
        //
        if (GetStreamId() != task->GetStreamId() &&
            task->GetStreamId() !=  "All")
            continue;

        //
        // if it has the right stream id execute the CallProcess() function
        // and check what the result of it is.
        // The CallProcess() function increases the execution counter and
        // calls the Process() function dependent on the existance and
        // return value of a filter.
        //
        switch (task->CallProcess())
        {
        case kTRUE:
            //
            // everything was OK: go on with the next task
            //
#ifdef DEBUG_PROCESS
            *fLog << "true." << endl;
#endif
            continue;

        case kFALSE:
            //
            // an error occured: stop eventloop
            //
            rc = kFALSE;
            *fLog << inf << task->GetDescriptor() << " has stopped execution of " << GetDescriptor() << "." << endl;
            break;

        case kERROR:
            //
            // an error occured: stop eventloop and return: failed
            //
            *fLog << err << "Fatal error occured while Process() of " << task->GetDescriptor() << "... stopped." << endl;
            rc = kERROR;
            break;

        case kCONTINUE:
            //
            // something occured: skip the rest of the tasks for this event
            //
#ifdef DEBUG_PROCESS
            *fLog << "continue." << endl;
#endif
            rc = kCONTINUE;
            break;

        default:
            *fLog << err << dbginf << "Unknown return value from MTask::Process()... stopped." << endl;
            rc = kERROR;
            break;
        }
        break;
    }

#ifdef DEBUG_PROCESS
    *fLog << all << "ProcessTaskList - " << GetDescriptor() << " - done." << endl;
#endif

    return rc;
}

// --------------------------------------------------------------------------
//
// do the event execution of all tasks in the task-list.
//
// If a task returns kCONTINUE, kCONTINUE is propagated if the
// number of passes==0, otherwise kTRUE is returned instead.
//
Int_t MTaskList::Process()
{
    //
    // Check whether there is something which can be processed, otherwise
    // stop the eventloop.
    //
    if (fTasksProcess.GetSize()==0)
    {
        *fLog << warn << "Warning: No entries in " << GetDescriptor() << " for Processing." << endl;
        return kFALSE;
    }

    //
    // Reset the ReadyToSave flag.
    // Reset all containers.
    //
    // Make sure, that the parameter list is not reset from a tasklist
    // running as a task in another tasklist.
    //
    const Bool_t noreset = fParList->TestBit(MParList::kIsProcessing);
    if (!noreset)
    {
        fParList->SetBit(MParList::kIsProcessing);
        if (!HasAccelerator(kAccDontReset))
            fParList->Reset();
    }

    // Initialize storage variable for the return code
    Int_t rc = kTRUE;

    // Execute the task list at least once
    const UInt_t cnt = fNumPasses==0 ? 1 : fNumPasses;
    for (fNumPass=0; fNumPass<cnt; fNumPass++)
    {
        // Execute the tasklist once
        rc = ProcessTaskList();
        // In cae of error or a stop-request leave loop
        if (rc==kERROR || rc==kFALSE)
            break;
    }

    // Reset to the default
    if (!noreset)
    {
        fParList->SetReadyToSave(kFALSE);
        fParList->ResetBit(MParList::kIsProcessing);
    }

    // If the number of passes==0 <default> propagete kCONTINUE
    if (rc==kCONTINUE)
        return fNumPasses==0 ? kCONTINUE : kTRUE;

    // Return return code
    return rc;
}

// --------------------------------------------------------------------------
//
//  do post processing (before eventloop) of all tasks in the task-list
//  only tasks which have successfully been preprocessed are postprocessed.
//
Int_t MTaskList::PostProcess()
{
    *fLog << all << "Postprocessing... " << flush;
    if (fDisplay)
    {
        // Set status lines
        fDisplay->SetStatusLine1("PostProcessing...");
        fDisplay->SetStatusLine2("");
    }

    //
    // Make sure, that the ReadyToSave flag is not reset from a tasklist
    // running as a task in another tasklist.
    //
    const Bool_t noreset = fParList->TestBit(MParList::kDoNotReset);
    if (!noreset)
    {
        fParList->SetBit(MParList::kDoNotReset);
        fParList->Reset();
    }

    //
    //  create the Iterator for the TaskList
    //
    TIter Next(fTasks);

    MTask *task=NULL;

    //
    //  loop over all tasks for postprocessing
    //  only tasks which have successfully been preprocessed are postprocessed.
    //
    while ( (task=(MTask*)Next()) )
    {
        if (!task->CallPostProcess())
            return kFALSE;

        // Handle GUI events (display changes, mouse clicks)
        if (fDisplay)
            gSystem->ProcessEvents();
    }

    *fLog << all << endl;

    //
    // Reset the ReadyToSave flag.
    //
    if (!noreset)
    {
        fParList->SetReadyToSave(kFALSE);
        fParList->ResetBit(MParList::kDoNotReset);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Prints the number of times all the tasks in the list has been.
//  For convinience the lvl argument results in a number of spaces at the
//  beginning of the line. So that the structur of a tasklist can be
//  identified. If a Tasklist or task has filter applied the name of the
//  filter is printer in <>-brackets behind the number of executions.
//  Use MTaskList::PrintStatistics without an argument.
//
void MTaskList::PrintStatistics(const Int_t lvl, Bool_t title, Double_t time) const
{
    if (lvl==0)
    {
        *fLog << all << underline << "Process execution Statistics:" << endl;
        *fLog << GetDescriptor();
        if (GetFilter())
            *fLog << " <" << GetFilter()->GetName() << ">";
        if (title)
            *fLog << "\t" << fTitle;
        if (time>=0)
            *fLog << Form(" %5.1f", GetCpuTime()/time*100) << "%";
        else
            *fLog << " 100.0%";
        *fLog << endl;
    }
    else
        MTask::PrintStatistics(lvl, title, time);

    //
    //  create the Iterator for the TaskList
    //
    fTasks->R__FOR_EACH(MTask, PrintStatistics)(lvl+1, title, GetCpuTime());

    if (lvl==0)
        *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Call 'Print()' of all tasks
//
void MTaskList::Print(Option_t *) const
{
    *fLog << all << underline << GetDescriptor() << ":" << endl;

    fTasks->Print();

    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MTaskList::StreamPrimitive(ostream &out) const
{
    out << "   MTaskList " << GetUniqueName();
    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << "(\"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
        out <<")";
    }
    out << ";" << endl << endl;

    MIter Next(fTasks);

    MParContainer *cont = NULL;
    while ((cont=Next()))
    {
        cont->SavePrimitive(out, "");
        out << "   " << GetUniqueName() << ".AddToList(&";
        out << cont->GetUniqueName() << ");" << endl << endl;
    }
}

void MTaskList::GetNames(TObjArray &arr) const
{
    MParContainer::GetNames(arr);
    fTasks->R__FOR_EACH(MParContainer, GetNames)(arr);
}

void MTaskList::SetNames(TObjArray &arr)
{
    MParContainer::SetNames(arr);
    fTasks->R__FOR_EACH(MParContainer, SetNames)(arr);
}

// --------------------------------------------------------------------------
//
// Read the contents/setup of a parameter container/task from a TEnv
// instance (steering card/setup file).
// The key to search for in the file should be of the syntax:
//    prefix.vname
// While vname is a name which is specific for a single setup date
// (variable) of this container and prefix is something like:
//    evtloopname.name
// While name is the name of the containers/tasks in the parlist/tasklist
//
// eg.  Job4.MImgCleanStd.CleaningLevel1:  3.0
//      Job4.MImgCleanStd.CleaningLevel2:  2.5
//
// If this cannot be found the next step is to search for
//      MImgCleanStd.CleaningLevel1:  3.0
// And if this doesn't exist, too, we should search for:
//      CleaningLevel1:  3.0
//
// Warning: The programmer is responsible for the names to be unique in
//          all Mars classes.
//
Int_t MTaskList::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (print)
        *fLog << all << "MTaskList::ReadEnv: " << prefix << " (" << (int)print << ")" << endl;

    MParContainer *cont = NULL;

    MIter Next(fTasks);
    while ((cont=Next()))
    {
        if (cont->InheritsFrom("MTaskList"))
        {
            if (cont->ReadEnv(env, prefix, print)==kERROR)
                return kERROR;
            continue;
        }

        if (cont->TestEnv(env, prefix, print)==kERROR)
            return kERROR;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Write the contents/setup of a parameter container/task to a TEnv
// instance (steering card/setup file).
// The key to search for in the file should be of the syntax:
//    prefix.vname
// While vname is a name which is specific for a single setup date
// (variable) of this container and prefix is something like:
//    evtloopname.name
// While name is the name of the containers/tasks in the parlist/tasklist
//
// eg.  Job4.MImgCleanStd.CleaningLevel1:  3.0
//      Job4.MImgCleanStd.CleaningLevel2:  2.5
//
// If this cannot be found the next step is to search for
//      MImgCleanStd.CleaningLevel1:  3.0
// And if this doesn't exist, too, we should search for:
//      CleaningLevel1:  3.0
//
// Warning: The programmer is responsible for the names to be unique in
//          all Mars classes.
//
Bool_t MTaskList::WriteEnv(TEnv &env, TString prefix, Bool_t print) const
{
    MParContainer *cont = NULL;

    MIter Next(fTasks);
    while ((cont=Next()))
        if (!cont->WriteEnv(env, prefix, print))
            return kFALSE;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Removes a task from the tasklist. Returns kFALSE if the object was not
// found in the list.
//
Bool_t MTaskList::RemoveFromList(MTask *task)
{
    TObject *obj = fTasks->Remove(task);

    //
    // If the task was found in the list try to remove it from the second
    // list, too.
    //
    if (obj)
        fTasksProcess.Remove(task);

    return obj ? kTRUE : kFALSE;

}

// --------------------------------------------------------------------------
//
// Removes all task of the TList from the tasklist. Returns kFALSE if any
// of the objects was not an MTask or not found in the list.
//
Bool_t MTaskList::RemoveFromList(const TList &list)
{
    Bool_t rc = kTRUE;

    TIter Next(&list);
    TObject *obj=0;
    while ((obj=Next()))
    {
        if (!obj->InheritsFrom(MTask::Class()))
        {
            *fLog << err << "ERROR - Object " << obj->GetName() << " doesn't inherit from MTask..." << endl;
            rc = kFALSE;
            continue;
        }

        if (!RemoveFromList(static_cast<MTask*>(obj)))
            rc = kFALSE;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
//  Find an object with the same name in the list and replace it with
//  the new one. If the kIsOwner flag is set and the object was not
//  created automatically, the object is deleted.
//
Bool_t MTaskList::Replace(MTask *task)
{
    //
    //  check if the object (you want to add) exists
    //
    if (!task)
        return kFALSE;

    if (task==this)
    {
        *fLog << warn << "WARNING - You cannot add a tasklist to itself.  This" << endl;
        *fLog << "          would create infinite recursions...ignored." << endl;
        return kFALSE;

    }

    MTask *obj = (MTask*)FindObject(task->GetName());
    if (!obj)
    {
        *fLog << warn << "No object with the same name '";
        *fLog << task->GetName() << "' in list... adding." << endl;
        return AddToList(task);
    }

    if (task==obj)
        return kTRUE;

    *fLog << inf2 << "Replacing " << task->GetName() << " in " << GetName() << " for " << obj->GetStreamId() << "... " << flush;
    task->SetStreamId(obj->GetStreamId());
    task->SetBit(kMustCleanup);
    fTasks->AddAfter((TObject*)obj, task);
    *fLog << "done." << endl;

    RemoveFromList(obj);

    if (TestBit(kIsOwner))
        delete obj;

    //*fLog << inf << "MTask '" << task->GetName() << "' found and replaced..." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Can be used to create an iterator over all tasks, eg:
//   MTaskList tlist;
//   TIter Next(tlist); // Be aware: Use a object here rather than a pointer!
//   TObject *o=0;
//   while ((o=Next()))
//   {
//       [...]
//   }
//
MTaskList::operator TIterator*() const
{
    return new TListIter(fTasks);
}
