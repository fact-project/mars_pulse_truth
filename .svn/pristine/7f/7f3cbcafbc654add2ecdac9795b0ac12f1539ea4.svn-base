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
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MTaskEnv
//
// This class is used if a task - at runtime - should be replaced or setup
// from a resource file, eg.
//
//
// resources.rc:
//   MyTask: MThisIsMyClass
//   MyTask.Resource1: yes
//   MyTask.Resource2: 50
//
// macro.C:
//   MTaskList list;
//
//   MDefaultTask def;
//   MTaskEnv taskenv("MyTask");
//   taskenv.SetDefault(&def);
//
//   list.AddToList(&taskenv);
//   [...]
//   evtloop.ReadEnv("resource.rc");
//
// In this case MTaskEnv will act like MDefaultTask if nothing is found in the
// resource file. If the task is setup via the resource file like in the
// example above it will act like a MThisIsMyClass setup with Resource1 and
// Resource2.
//
//
// You can also skip the task completely if you setup (in lower case letters!)
//
// resources.rc:
//   MyTask: <dummy>
//
//
// A third option is to setup a MTaskEnv to be skipped except a task is
// initialized through the resource file:
//   MTaskEnv taskenv("MyTask");
//   taskenv.SetDefault(0);
//
//
//  Input Containers:
//   -/-
//
//  Output Containers:
//   -/-
//
/////////////////////////////////////////////////////////////////////////////
#include "MTaskEnv.h"

#include <TClass.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MTaskEnv);

using namespace std;
// --------------------------------------------------------------------------
//
// Default Constructor. Takes name and title of the interactive task
//
MTaskEnv::MTaskEnv(const char *name, const char *title) : fTask(0)
{
    fName  = name  ? name  : "MTaskEnv";
    fTitle = title ? title : "Task setup from Environment file";
}

MTaskEnv::~MTaskEnv()
{
    if (TestBit(kIsOwner))
        delete fTask;
}

MTask *MTaskEnv::GetTask(const char *name) const
{
    MTask *task = (MTask*)GetNewObject(name, MTask::Class());
    if (!task)
        return NULL;

    task->SetName(fName);
    task->SetTitle(fTitle);

    return task;
}

void MTaskEnv::SetDefault(const char *def)
{
    if (TestBit(kIsOwner) && fTask)
        delete fTask;

    fTask = GetTask(def);
    if (!fTask)
        *fLog << err << dbginf << "ERROR - No default Task setup..." << endl;

    SetBit(kIsOwner);
}

Bool_t MTaskEnv::ReInit(MParList *list)
{
    *fLog << fTask->ClassName() << " <MTaskEnv>... " << flush;
    return fTask->ReInit(list);
}

Int_t MTaskEnv::PreProcess(MParList *list)
{
    if (TestBit(kIsDummy))
    {
        *fLog << inf << "Dummy Task... skipped." << endl;
        return kSKIP;
    }

    if (!fTask)
    {
        *fLog << err << GetDescriptor() << " - ERROR: No task setup." << endl;
        return kFALSE;
    }

    *fLog << fTask->ClassName() << " <MTaskEnv>... " << flush;

    fTask->SetAccelerator(GetAccelerator());
    return fTask->CallPreProcess(list);
}

Int_t MTaskEnv::Process()
{
    return fTask->CallProcess();
}

Int_t MTaskEnv::PostProcess()
{
    *fLog << fTask->ClassName() << " <MTaskEnv>... " << flush;
    return fTask->CallPostProcess();
}

Int_t MTaskEnv::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (!IsEnvDefined(env, prefix, print))
        return fTask ? fTask->ReadEnv(env, prefix, print) : kFALSE;

    TString task = GetEnvValue(env, prefix, "");
    task.ReplaceAll("\015", "");
    task = task.Strip(TString::kBoth);

    if (task=="<dummy>")
    {
        if (TestBit(kIsOwner) && fTask)
            delete fTask;
        fTask = 0;
        SetBit(kIsDummy);
        return kTRUE;
    }

    fTask = GetTask(task.Data());
    if (!fTask)
    {
        *fLog << err << GetDescriptor() << " - ERROR: No task matching '";
        *fLog << task << "' could be setup." << endl;
        return kERROR;
    }

    ResetBit(kIsDummy);
    SetBit(kIsOwner);

    return fTask->ReadEnv(env, prefix, print);
}

Bool_t MTaskEnv::WriteEnv(TEnv &env, TString prefix, Bool_t print) const
{
    return fTask->WriteEnv(env, prefix, print);
}
