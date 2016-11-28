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
!   Author(s): Thomas Bretz  07/2001 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  MClone                                                                  //
//                                                                          //
//  This task clones a given paramter container. You can either specify     //
//  the name of the container which should be cloned or a pointer to the    //
//  container. If you specify a name the preprocessing tries to find the    //
//  corresponding container in the parameter list.                          //
//  Cloning in this context means duplicating the object in memory. This    //
//  may be used if you change an object in the eventloop (eg. the image     //
//  cleaning is changing the image) and you want to compare both 'version'  //
//  of this object afterwards.                                              //
//  The cloned object can be accessed by using MClone::GetClone.            //
//  To clone the container more than once use several instances of MClone.  //
//  The object does only exist until a new object is cloned. It is deleted  //
//  in the destructor.                                                      //
//                                                                          //
//  To use MClone you must make sure, that TObject::Clone is correctly      //
//  working for this class (maybe you have to overload it)                  //
//                                                                          //
//  Input Containers:                                                       //
//   MParContainer                                                          //
//                                                                          //
//  Output Containers:                                                      //
//   -/-                                                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#include "MClone.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

ClassImp(MClone);

using namespace std;

// --------------------------------------------------------------------------
//
// Initializes name and title of the object. It is called by all
// constructors.
//
void MClone::Init(const char *name, const char *title)
{
    fName  = name  ? name  : "MClone";
    fTitle = title ? title : "Task to clone a parameter container for later usage";

    fForceClone = kFALSE;

    fClone  = NULL;
    fObject = NULL;
}

// --------------------------------------------------------------------------
//
//  Constructor. Remembers the name to search for in the parameter list.
//
MClone::MClone(const char *obj, const char *name, const char *title)
{
    Init(name, title);

    fObjName = obj;
}

// --------------------------------------------------------------------------
//
//  Constructor. Remember the pointer of the object which has to be cloned.
//
MClone::MClone(const TObject *obj, const char *name, const char *title)
{
    Init(name, title);

    fObject  = obj;
    fObjName = obj->GetName();
}

// --------------------------------------------------------------------------
//
//  Destructor. Deletes the cloned object.
//
MClone::~MClone()
{
    Clear();
}

// --------------------------------------------------------------------------
//
//  Checks the parameter list for the existance of the parameter container. If
//  the name of it was given in the constructor.
//
Int_t MClone::PreProcess(MParList *pList)
{
    //
    // The pointer is already given by the user.
    //
    if (fObject)
        return kTRUE;

    //
    // Try to find the parameter container with the given name in the list
    //
    fObject = pList->FindObject(fObjName);
    if (fObject)
        return kTRUE;

    //
    // If it couldn't get found stop Eventloop
    //
    *fLog << err << dbginf << fObjName << " not found... aborting." << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
//  Delete the cloned object if one is existing
//
void MClone::Clear(Option_t *)
{
    //
    // Check if an object has been initialized
    //
    if (!fClone)
        return;

    //
    // Delete it and set the pointer to NULL for the sanity check (above)
    //
    delete fClone;
    fClone = NULL;
}

// --------------------------------------------------------------------------
//
//  Deletes an existing clone and clones the object (parameter container)
//  again.
//
Int_t MClone::Process()
{
    if (fClone && !fForceClone)
    {
        fObject->Copy(*fClone);
        return kTRUE;
    }

    Clear();

    fClone = fObject->Clone();
    return kTRUE;
}

