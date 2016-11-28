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
!   Author(s): Thomas Bretz  11/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   MGTask                                                                //
//                                                                         //
//   A MTask with several enhancments for a graphical interface.           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "MGTask.h"

#include <TClass.h>
#include <TMethod.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGGroupFrame.h"

ClassImp(MGTask);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default constructor. Initialized fFrame with NULL.
//
MGTask::MGTask(const char *name, const char *title)
    : fFrame(NULL)
{
    fName  = name  ? name  : "MGTask";
    fTitle = title ? title : "Base class for all tasks with graphical I/O.";
}

// --------------------------------------------------------------------------
//
//  Deletes the GUI if one was created.
//
MGTask::~MGTask()
{
    if (fFrame)
        delete fFrame;
}

// --------------------------------------------------------------------------
//
//  Hides the graphical interface if existing and calls
//  MTask::CallPreProcess
//
Int_t MGTask::CallPreProcess(MParList *plist)
{
    HideGui();

    return MTask::CallPreProcess(plist);
}

// --------------------------------------------------------------------------
//
//  Shows the graphical interface if existing and calls
//  MTask::CallPostProcess
//
Int_t MGTask::CallPostProcess()
{
    ShowGui();

    return MTask::CallPostProcess();
}

// --------------------------------------------------------------------------
//
// Get the Widget from the MGGroupFrame (GUI) with the Id id.
//
TObject *MGTask::FindWidget(Int_t id) const
{
    return fFrame->FindWidget(id);
}

// --------------------------------------------------------------------------
//
//  Process a message. Redirect gui events (eg by calling
//  TGButton->Associate()) to the MGGroupFrame when Creating the GUI in
//  CreateGuiElements. And process the messages in the overwritten
//  ProcessMessage function.
//
Bool_t MGTask::ProcessMessage(Int_t msg, Int_t submsg, Long_t param1, Long_t param2)
{
    fLog->setf(ios::showbase);
    *fLog << all << "Task " << GetDescriptor() << " received gui msg " << hex;
    *fLog << msg << " " << submsg << " " << param1 << " " << param2 << endl;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Show the gui (and its gui elements) of the task if existing.
//
void MGTask::ShowGui()
{
    if (fFrame)
        fFrame->MapWindow();
}

// --------------------------------------------------------------------------
//
//  Hide the gui (and its gui elements) of the task if existing.
//
void MGTask::HideGui()
{
    if (fFrame)
        fFrame->UnmapWindow();
}

// --------------------------------------------------------------------------
//
//  Check whether a gui for this task was already created.
//  If not create a MGGroupFrame to group the gui elements and add it
//  to the given frame with the given layout hints.
//  Now call the overwritten function CreateGuiElements to fill the group
//  frame.
//
void MGTask::CreateGui(TGCompositeFrame *f, TGLayoutHints *layout)
{
    //
    // Check whether frame already exists
    //
    if (fFrame)
    {
        *fLog << warn << GetDescriptor() << " Gui already created... skipped." << endl;
        return;
    }

    //
    // Check whether a gui is implemented for this class
    //  - IsA gives a pointer to the dictionary entry of the mostly
    //    inherited class of this Task
    //  - GetMethodAllAny checks for the existance of CreateGuiElements
    //    in the class and all its base classes
    //  - now get the dictionary entry of the corresponding class
    //    (in which the mathos was found)
    //  - if method isn't overwritten the result is the dictionary
    //    entry for MGTask.
    //
    TMethod *method = IsA()->GetMethodAllAny("CreateGuiElements");
    if (method->GetClass() == MGTask::Class())
    {
        *fLog << warn << "Sorry, " << GetDescriptor();
        *fLog << " doesn't override CreateGuiElements." << endl;
        return;
    }

    //
    //  create group frame
    //
    fFrame = new MGGroupFrame(this, f, ClassName());
    f->AddFrame(fFrame, layout);

    //
    // add gui elements
    //
    CreateGuiElements(fFrame);
}
