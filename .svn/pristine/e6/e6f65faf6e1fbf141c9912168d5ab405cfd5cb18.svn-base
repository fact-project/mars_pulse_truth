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
//  MGList                                                                 //
//                                                                         //
//  This is a TList object which has some enhancements for GUI elements.   //
//  Use GetWidget to search for a GUI element with a given widget id.      //
//  Add checkes for the existances of a GUI element with the same widget   //
//  id (for IDs>=0).                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MGList.h"

#include <iostream>

#include <TROOT.h>
#include <TClass.h>
#include <TGClient.h>
#include <TGWidget.h>
#include <TGPicture.h>

ClassImp(MGList);

using namespace std;

// --------------------------------------------------------------------------
//
//  Add the list to the global list of cleanups
//  objects in the list.
//
MGList::MGList() : TList()
{
    // Make sure that all object deleted are also deleted from this list
    gROOT->GetListOfCleanups()->Add(this);
    // Make sure that this doesn't remain in ListOfCleanups after deletion
    SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
//  Before destroying the list with all its contents free all TGPicture
//  objects in the list.
//
MGList::~MGList()
{
    DeletePictures();
}

// --------------------------------------------------------------------------
//
//  Free/Delete all TGPicture store in list.
//
void MGList::DeletePictures()
{
    TObject *obj;
    TIter Next(this);
    while ((obj=Next()))
    {
        if (!obj->InheritsFrom(TGPicture::Class()))
            continue;

        //
        // Remove the object first. Otherwise we would remove
        // a non existing object...
        //
        Remove(obj);
        if (gClient)
            gClient->FreePicture((TGPicture*)obj);
    }
}

// --------------------------------------------------------------------------
//
//  Delete all pictures. Call TList::Delete.
//
void MGList::Delete(Option_t *option)
{
    DeletePictures();
    TList::Delete(option);
}

// --------------------------------------------------------------------------
//
//  Does a dynamic cast from a TObject to a TGWidget. This is necesary
//  if a class derived from TObject inherits also from TGWidget and
//  you have only a pointer to the TObject available.
//
TGWidget *MGList::GetWidget(TObject *obj) const
{
    //
    // - IsA gives a pointer to the parent class entry in the dictionary
    // - DynamicCast tries to cast obj of class type obj->IsA to one
    //   of its base classes TGWidget
    // - This method is ment for dynamic casts (multi inheritance)
    //
    //  If this functions makes trouble check for the correct inheritance
    //  first via obj->InheritsFrom(TGWidget::Class())
    //
    //  The root implementation is used, because for space reasons
    //  the C++ dynamic_cast<TGWidget*> is turned off by the option
    //  -fno-rtti, which could be used in 'plain' C++
    //

    //
    // FIXME: This should not be necessary but it is, why??
    //
    // TRY: TGPicture *pic = gClient->GetPicture("pic");
    //      cout << pic->IsA()->DynamicCast(TGWidget::Class(), pic) << endl;
    //
    //      Is this another bug in root?
    //
#if ROOT_VERSION_CODE < ROOT_VERSION(3,02,07)
    if (!obj->InheritsFrom(TGWidget::Class()))
        return NULL;
#endif

    return (TGWidget*)obj->IsA()->DynamicCast(TGWidget::Class(), obj);
}

// --------------------------------------------------------------------------
//
//  Returns kTRUE if the object is derived from TGWidget and a widget
//  with the TGWidget id of this object is already in the list.
//  If the object is not derived from TGWidget or no TGWidget object
//  with the same widget id is existing in the list kFALSE is returned.
//  If the TGWidget has an object id < 0 kFALSE is always retuned.
//
Bool_t MGList::IsExisting(TObject *obj) const
{
    const TGWidget *wid = GetWidget(obj);

    //
    // check whether it is a TGWidget
    //
    if (!wid)
        return kFALSE;

    const Int_t id = wid->WidgetId();

    //
    // check whether is has a valid id
    // (not id=-1, which is the standard id)
    //
    if (id < 0)
        return kFALSE;

    //
    // check whether Widget id is already existing in the list
    //
    return FindWidget(id) ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
//  If the given object obj is derived from TGWidget and a TGWidget with
//  the same widget id is already existing in the list the object is
//  ignored, otherwise it is added to the list via TList::Add(TObject *)
//
void MGList::Add(TObject *obj)
{
    if (IsExisting(obj))
    {
        // FIXME: Replace by gLog
        const Int_t id = GetWidget(obj)->WidgetId();
        cout << "Widget with id #" << id << " (";
        cout << FindWidget(id)->ClassName() << ") already in list... ";
        cout << obj->GetName() << " ignored." << endl;
        return;
    }

    TList::Add(obj);
    obj->SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
//  If the given object obj is derived from TGWidget and a TGWidget with
//  the same widget id is already existing in the list the object is
//  ignored, otherwise it is added to the list via
//  TList::Add(TObject *, Option_t *)
//
void MGList::Add(TObject *obj, Option_t *opt)
{
    if (IsExisting(obj))
    {
        Int_t id = GetWidget(obj)->WidgetId();
        cout << "Widget with id #" << id << " (";
        cout << FindWidget(id)->ClassName() << ") already in list... ";
        cout << obj->GetName() << " ignored." << endl;
        return;
    }

    TList::Add(obj, opt);
    obj->SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
//  Adds the picture physically to the list. The list takes care of that
//   - The picture is freed as often as it was retrieved from gClient
//
void MGList::AddPicture(TGPicture *pic, const char *name)
{
    //
    // Check whether the picture exists
    //
    if (!pic)
    {
        cout << "Warning: Requested picture '" << name << "' not found... ignored." << endl;
        cout << "    Please copy " << name << " to $HOME or $HOME/icons or add" << endl;
        cout << "      Unix.*.Gui.IconPath: ~/Path/To/The/Picture" << endl;
        cout << "    to [$HOME/].rootrc."  << endl;
        return;
    }

    //
    // Add the picture to the list
    //
    TList::Add(pic);
    pic->SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
//  This gets a picture from the picture pool of the TGClient-object.
//  The pictures are freed automatically in the dstructor of the list.
//  The picture counts itself how often it got used, so that only
//  the first call to GetPicture will craete it and the last call to
//  FreePicture will destroy it. If you access the picture only via
//  MGList::GetPicture you don't have to care about.
//
//  Don't try to call FreePicture by yourself for a picture gotten by
//  GetPicture. This is independant of the kIsOwner bit.
//
const TGPicture *MGList::GetPicture(const char *name)
{
    if (!gClient)
        return NULL;

    TGPicture *pic = const_cast<TGPicture*>(gClient->GetPicture(name));
    AddPicture(pic, name);
    return pic;
}

// --------------------------------------------------------------------------
//
//  This gets a picture from the picture pool of the TGClient-object.
//  The pictures are freed automatically in the dstructor of the list.
//  The picture counts itself how often it got used, so that only
//  the first call to GetPicture will craete it and the last call to
//  FreePicture will destroy it. If you access the picture only via
//  MGList::GetPicture you don't have to care about.
//
//  Don't try to call FreePicture by yourself for a picture gotten by
//  GetPicture. This is independant of the kIsOwner bit.
//
const TGPicture *MGList::GetPicture(const char *name, Int_t width, Int_t height)
{
    if (!gClient)
        return NULL;

    TGPicture *pic = const_cast<TGPicture*>(gClient->GetPicture(name, width, height));
    AddPicture(pic, name);
    return pic;
}
// --------------------------------------------------------------------------
//
//  Search the list for a object derived from TGidget which has the given
//  widget id. Returns a pointer to this object otherwise NULL.
//  For IDs < 0 the function returns always NULL.
//
TObject *MGList::FindWidget(Int_t id) const
{
    if (id<0)
        return NULL;

    TObject *obj;
    TIter Next(this);
    while ((obj=Next()))
    {
        const TGWidget *wid = GetWidget(obj);

        if (!wid)
            continue;

        if (id == wid->WidgetId())
            return obj;
    }
    return NULL;
}
