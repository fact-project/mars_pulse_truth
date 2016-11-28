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
!   Author(s): Thomas Bretz, 6/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MTaskInteractive
//
// If you want to create a new task inside a macro you will have to compile
// your macro using macro.C++, because the root interpreter cannot use
// uncompiled classes. To workaround this problem you can write simple
// funcions (which can be handled by CINT) and use MTaskInteractive.
//
// This is a simple way to develop new code in a macro without need
// to compile it.
//
// Example:
//    Int_t Process()
//    {
//       gLog << "Processing..." << endl;
//       return kTRUE;
//    }
//
//    void main()
//    {
//       MTaskInteractive task;
//       task.SetProcess(Process);
//       MTaskList list;
//       list.AddToList(&task);
//    }
//
//
//  Input Containers:
//   -/-
//
//  Output Containers:
//   -/-
//
/////////////////////////////////////////////////////////////////////////////
#include "MTaskInteractive.h"

#include <Api.h>
#include <TMethodCall.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MTaskInteractive);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. Takes name and title of the interactive task
//
MTaskInteractive::MTaskInteractive(const char *name, const char *title) :
    fPreProcess(NULL), fProcess(NULL), fPostProcess(NULL), fReInit(NULL)
{
    fName  = name  ? name  : "MTaskInteractive";
    fTitle = title ? title : "Interactive task";

    fCall[0] = 0;
    fCall[1] = 0;
    fCall[2] = 0;
    fCall[3] = 0;
}

// --------------------------------------------------------------------------
//
// Destructor. Free all resources.
//
MTaskInteractive::~MTaskInteractive()
{
    Free(0);
    Free(1);
    Free(2);
    Free(3);
}

// --------------------------------------------------------------------------
//
// Calls the function and returns its return value.
// Called by PreProcess, Process, PostProcess and ReInit.
//
inline Int_t MTaskInteractive::Return(Int_t no, void *params)
{
    // Static function called when SetFCN is called in interactive mode
    if (!fCall[no])
    {
        gLog << err << dbginf << "Return(" << no << ") - TMethodCall not set." << endl;
        return kFALSE;
    }

    Long_t result;
    fCall[no]->SetParamPtrs(params);
    fCall[no]->Execute(result);

    return result;
}

// --------------------------------------------------------------------------
//
// Generalized function to set the functions of your interactive task.
// Called by SetPreProcess, SetProcess, SetPostProcess and SetReInit
//
Bool_t MTaskInteractive::Set(void *fcn, Int_t no, const char *params)
{
    // this function is called by CINT instead of the function above
    if (!fcn)
        return kFALSE;

    char *funcname = G__p2f2funcname(fcn);
    if (!funcname)
        return kFALSE;

    Free(no);

    fCall[no] = new TMethodCall;
    fCall[no]->InitWithPrototype(funcname, params);

    gLog << inf << GetDescriptor() << ": Using " << funcname << " as ";
    switch (no)
    {
    case 0:
        gLog << "PreProcess";
        break;
    case 1:
        gLog << "Process";
        break;
    case 2:
        gLog << "PostProcess";
        break;
    case 3:
        gLog << "ReInit";
        break;

    }
    gLog << "-function." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
//
void MTaskInteractive::Free(Int_t no)
{
    if (!fCall[no])
        return;
    delete fCall[no];
    fCall[no] = 0;
}
