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
!   Author(s): Thomas Bretz  11/2001 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MGGroupFrame                                                            //
//                                                                         //
// This is a group frame derived from TGGroupFrame with some               //
// enhancements:                                                           //
//  - It holds a list which deletes all it's members in ~MGGroupFrame      //
//    (use AddToList to add an object.                                     //
//  - You can access the members of the list by their widget id (if they   //
//    have one)                                                            //
//  - All messages from associated GUI elemets are redirectd to a given    //
//    task.                                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MGGroupFrame.h"

#include "MGList.h"
#include "MGTask.h"

ClassImp(MGGroupFrame);

// --------------------------------------------------------------------------
//
//  For a detailed description check TGGroupFrame::TGGroupFrame.
//  All messages from associated GUI elements are redirected to the given
//  task.
//  Creates a MGList object to store Widgets. Use AddToList to Add
//  objects.
//
MGGroupFrame::MGGroupFrame(MGTask *task,
                           const TGWindow *p, TGString *title,
                           UInt_t options, GContext_t norm,
                           FontStruct_t font, ULong_t back)
    : TGGroupFrame(p, title, options, norm, font, back), TGWidget(-1)
{
    fWidgetFlags = 0;
    fMsgWindow   = p;
    fTask        = task;

    fList = new MGList;
}

// --------------------------------------------------------------------------
//
//  For a detailed description check TGGroupFrame::TGGroupFrame.
//  All messages from associated GUI elements are redirected to the given
//  task.
//  Creates a MGList object to store Widgets. Use AddToList to Add
//  objects.
//
MGGroupFrame::MGGroupFrame(MGTask *task,
                           const TGWindow *p, const char *title,
                           UInt_t options, GContext_t norm,
                           FontStruct_t font, ULong_t back)
: TGGroupFrame(p, title, options, norm, font, back), TGWidget(-1)
{
    fWidgetFlags = 0;
    fMsgWindow   = p;
    fTask        = task;

    fList = new MGList;
}

// --------------------------------------------------------------------------
//
//  Deleted the MGList object and all its members.
//
MGGroupFrame::~MGGroupFrame()
{
    fList->SetOwner();
    delete fList;
}

// --------------------------------------------------------------------------
//
//  Add an object to the MGList. All the objects in MGList are deleted
//  in the destructor MGGroupFrame automatically.
//  You can add all object which have to exist until the Group Frame is
//  destroyed (eg. TGLayoutElements)
//  If an object is also derived from TGWidget you can get a pointer
//  to it by its widget Id (use FindWidget)
//
void MGGroupFrame::AddToList(TObject *obj)
{
    fList->Add(obj);
}

// --------------------------------------------------------------------------
//
//  To get an object which has a widget id (is derived from TGWidget) by
//  its widget id use FindWidget with the widget id of the object you
//  are searching for. If no object with this widget id exists NULL
//  is returned. See also MGList::FindWidget(Int_t)
//
TObject *MGGroupFrame::FindWidget(Int_t id) const
{
    return fList->FindWidget(id);
}

// --------------------------------------------------------------------------
//
//  If you associate an object with this MGGroupFrame object, like in the
//  following example:
//
//   MGGroupFrame *f = new MGGroupFrame(...);
//   TGTextEntry *entry = new TGTextEntry(...);
//   entry->Associate(f);
//
//  All messages send to the group frame by the GUI elent are redirected
//  to the task corresponing to the MGGroupFrame.
//
Bool_t MGGroupFrame::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)
{
    return fTask->ProcessMessage(GET_MSG(msg), GET_SUBMSG(msg), param1, param2);
}
