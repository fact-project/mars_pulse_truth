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
!   Author(s): Thomas Bretz  10/2001 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  MPrint                                                                  //
//                                                                          //
//  This task calls the Print() function of a parameter container, to       //
//  print some information event by event.                                  //
//  Overload TObject::Print()                                               //
//                                                                          //
//  Input Containers:                                                       //
//   MParContainer                                                          //
//                                                                          //
//  Output Containers:                                                      //
//   -/-                                                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#include "MPrint.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

ClassImp(MPrint);

using namespace std;

// --------------------------------------------------------------------------
//
// Initializes name and title of the object. It is called by all
// constructors.
//
void MPrint::Init(const char *name, const char *title)
{
    fName  = name  ? name  : "MPrint";
    fTitle = title ? title : "Task to print a parameter container";

    fObject = NULL;
}

MPrint::MPrint()
{
    Init(0, 0);
    SetBit(kSeparator);
}

// --------------------------------------------------------------------------
//
//  Constructor. Remembers the name to search for in the parameter list
//  of the object to be printed. The object must be derived from TObject and
//  TObject::Print(Option_t *) const
//  must be overloaded. You can also set an option string to use
//  when calling TObject::Print
//  If you want that the MPrint instance is removed from the tasklist
//  if the container to be printed is not found in the PreProcess, call:
//     MPrint::EnableSkip();
//
MPrint::MPrint(const char *obj, const char *option,
               const char *name, const char *title)
{
    Init(name, title);
    SetOption(option);

    fObjName = obj;
}

// --------------------------------------------------------------------------
//
//  Constructor. Remember the pointer of the object which has to be
//  printed. The object must be derived from TObject and
//  TObject::Print(Option_t *) const
//  must be overloaded. You can also set an option string to use
//  when calling TObject::Print
//  if the container to be printed is not found in the PreProcess, call:
//     MPrint::EnableSkip();
//
MPrint::MPrint(const TObject *obj, const char *option,
               const char* name, const char *title)
{
    Init(name, title);
    SetOption(option);

    fObject  = obj;
    fObjName = obj->GetName();
}

// --------------------------------------------------------------------------
//
//  Checks the parameter list for the existance of the parameter container. If
//  the name of it was given in the constructor.
//
Int_t MPrint::PreProcess(MParList *pList)
{
    //
    // The pointer is already given by the user.
    //
    if (fObject || TestBit(kSeparator))
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
    if (TestBit(kSkip))
    {
        *fLog << warn << fObjName << " not found... removing task from list." << endl;
        return kSKIP;
    }
    else
    {
        *fLog << err << fObjName << " not found... aborting." << endl;
        return kFALSE;
    }
}

// --------------------------------------------------------------------------
//
//  Calls overloaded TObject::Print
//
Int_t MPrint::Process()
{
    gLog << flush << all;
    if (TestBit(kSeparator))
        gLog.Separator() << endl;
    else
        fObject->Print(fOption);
    return kTRUE;
} 

