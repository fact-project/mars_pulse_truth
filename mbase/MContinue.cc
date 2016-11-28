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
!   Author(s): Thomas Bretz 10/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MContinue
//
// Does nothing than return kCONTINUE in the Process-function
// (use with filters). For more details see the description of the
// constructors.
//
// To invert the meaning of the contained filter call SetInverted()
//
//
//  Input Containers:
//   -/-
//
//  Output Containers:
//   -/-
//
//
// Class Version 2:
// ----------------
//  + Int_t fRc;
//
/////////////////////////////////////////////////////////////////////////////
#include "MContinue.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MF.h"
#include "MString.h"
#include "MParList.h"
#include "MTaskList.h"

ClassImp(MContinue);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
// Use this constructor if a rule (see MF for more details) shell be used.
// MContinue will create a MF object and use it as a filter for the
// instance. The MF-Task is added to the tasklist in front of the MContinue
// instance and also automatically deleted, eg.
//   MContinue cont("MHillas.fSize<20");
//   tasklist.AddToList(&cont);
// would skip all events which fullfill "MHillas.fSize<20" from this point
// in the tasklist.
// It is not necessary to put the filter in the tasklist. The PreProcess
// will search for the filter and if it isn't found in the tasklist it
// is added to the tasklist in front of MContinue.
//
// Use the default constructor (or an empty rule) if you want to read the
// rule from a resource file.
//
MContinue::MContinue(const TString rule, const char *name, const char *title)
    : fTaskList(0), fRc(kCONTINUE)
{
    fName  = name  ? name  : "MContinue";
    fTitle = title ? title : "Task returning kCONTINUE";

    if (rule.IsNull())
        return;

    SetBit(kIsOwner);

    MTask::SetFilter(new MF(rule, TString("MF(")+fName+")"));
}

// --------------------------------------------------------------------------
//
// Use this if you have a filter. Would be the same as if you would call:
//   MContinue cont;
//   cont.SetFilter(f);
// It is not necessary to put the filter in the tasklist. The PreProcess
// will search for the filter and if it isn't found in the tasklist it
// is added to the tasklist in front of MContinue.
//
MContinue::MContinue(MFilter *f, const char *name, const char *title)
    : fTaskList(0), fRc(kCONTINUE)
{
    fName  = name  ? name  : "MContinue";
    fTitle = title ? title : "Task returning kCONTINUE (or any other return code)";

    SetFilter(f);
}

// --------------------------------------------------------------------------
//
//  Delete the filter if it was created automatically
//
MContinue::~MContinue()
{
    if (TestBit(kIsOwner))
        delete GetFilter();
}

// --------------------------------------------------------------------------
//
//  PreProcess tries to find the tasklist MTaskList, adds the filter
//  before this instance to the tasklist and preprocesses the filter.
//
Int_t MContinue::PreProcess(MParList *list)
{
    ResetBit(kFilterIsPrivate);

    if (!GetFilter())
    {
        if (IsAllowEmpty())
        {
            *fLog << warn << GetDescriptor() << " - WARNING: Filter empty... task removed." << endl;
            return kSKIP;
        }

        *fLog << inf << "My filter has vanished... skipping." << endl;
        return kSKIP;
    }

    fTaskList = (MTaskList*)list->FindTaskListWithTask(this);
    if (!fTaskList)
    {
        *fLog << err << dbginf << "ERROR - MTaskList with this not found... abort." << endl;
        return kFALSE;
    }

    if (fTaskList->FindObject(GetFilter()))
    {
        *fLog << inf << dbginf << "The filter is already in the tasklist..." << endl;
        return kTRUE;
    }

    if ((TString)GetFilter()->GetName()==fName)
        GetFilter()->SetName(MString::Format("MF:%s", fName.Data()));

    if (!fTaskList->AddToListBefore(GetFilter(), this))
    {
        *fLog << err << dbginf << "ERROR - Adding filter before MContinue failed... abort." << endl;
        return kFALSE;
    }

    // Make sure, that everything is correctly propageted to the childs
    GetFilter()->SetDisplay(fDisplay);
    GetFilter()->SetLogStream(fLog);
    GetFilter()->SetAccelerator(GetAccelerator());

    // Remeber that the filter is not already in tasklist
    SetBit(kFilterIsPrivate);

    return GetFilter()->CallPreProcess(list);
}

// --------------------------------------------------------------------------
//
// Propagate display to filter (if set)
//
void MContinue::SetDisplay(MStatusDisplay *d)
{
    if (GetFilter())
        GetFilter()->SetDisplay(d);

    MTask::SetDisplay(d);
}

// --------------------------------------------------------------------------
//
// Propagate log stream to filter (if set)
//
void MContinue::SetLogStream(MLog *lg)
{
    if (GetFilter())
        GetFilter()->SetLogStream(lg);

    MTask::SetLogStream(lg);
}

// --------------------------------------------------------------------------
//
// If the filter was added to the tasklist automatically it is removed
// from the tasklist.
//
Int_t MContinue::PostProcess()
{
    if (!TestBit(kFilterIsPrivate))
        return kTRUE;

    if (fTaskList->RemoveFromList(GetFilter()))
        return kTRUE;

    *fLog << err << "ERROR: MContinue::PostProcess - Cannot remove Filter from tasklist" << endl;

    return kFALSE;
}

// --------------------------------------------------------------------------
//
// If a filter is setup, call its 'IsInverted' to invert its meaning
// (boolean "not")
//
void MContinue::SetInverted(Bool_t i)
{
    if (GetFilter())
        GetFilter()->SetInverted(i);
}

// --------------------------------------------------------------------------
//
// If a filter is setup, its IsInverted status is returned. If now filter
// has been setup yet, kFALSE is returned.
//
Bool_t MContinue::IsInverted() const
{
    return GetFilter() ? GetFilter()->IsInverted() : kFALSE;
}

void MContinue::Print(Option_t *) const
{
    *fLog << all << GetDescriptor() << ":";
    if (GetFilter())
        *fLog << " <" << GetFilter()->GetDescriptor() << ">";
    *fLog << endl;
    if (GetFilter())
        GetFilter()->Print();
}

// --------------------------------------------------------------------------
//
// Check for corresponding entries in resource file and setup filters.
// Avoid trailing 0's!
//
// Example:
//   test.C:
//     MContinue cont("", "MyContinue");
//
//   test.rc:
//     MyContinue.Condition: {0} && {1}
//     MyContinue.Inverted: yes
//     MyContinue.0: MHillas.fSize>1000
//     MyContinue.1: MHillas.fSize<10000
//   or (the syntax might change in the future!)
//     MyContinue.Condition: <MMyClass>
//     MMyClass.Variable1: ...
//     MMyClass.Variable2: ...
//
// For more details see MF::ReadEnv
//
Int_t MContinue::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    MFilter *f = GetFilter();
    if (IsEnvDefined(env, prefix, "Condition", print))
    {
        TString txt = GetEnvValue(env, prefix, "Condition", "");
        txt = txt.Strip(TString::kBoth);
        if (txt.BeginsWith("<") && txt.EndsWith(">"))
        {
            const TString name = txt(1, txt.Length()-2);
            f = (MFilter*)GetNewObject(name, MFilter::Class());
            if (!f)
                return kERROR;
        }
    }

    if (!f)
        f = new MF;

    f->SetName(fName);

    const Bool_t rc = f->ReadEnv(env, prefix, print);

    if (rc!=kTRUE)
    {
        if (f!=GetFilter())
            delete f;
        return rc;
    }

    if (f != GetFilter())
    {
        if (TestBit(kIsOwner))
            delete GetFilter();
        SetBit(kIsOwner);
    }

    MTask::SetFilter(f);

    return kTRUE;
}
