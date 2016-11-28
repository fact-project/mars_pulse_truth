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
!   Author(s): Thomas Bretz 1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MParEnv
//
// A wrapper container which allows to set a new container from the
// resource file. Via SetClassName you can define which type of class
// you want to allow (the given object must inherit from it).
//
// For further details see MParEnv::ReadEnv
//
/////////////////////////////////////////////////////////////////////////////
#include "MParEnv.h"

#include <TClass.h>
#include <TInterpreter.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MParEnv);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MParEnv::MParEnv(const char *name, const char *title) : fCont(0), fClassName("MParContainer"), fIsOwner(kFALSE)
{
    fName  = name  ? name  : "MParEnv";
    fTitle = title ? title : "A wrapper container which allows to set a new container from the resource file";
}

// --------------------------------------------------------------------------
//
// Delete fOwner if we own the instance.
//
MParEnv::~MParEnv()
{
    if (fIsOwner)
        delete fCont;
}

// --------------------------------------------------------------------------
//
// Print the descriptor of this instance.
// Calls Print() of fCont if fCont is set.
//
void MParEnv::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << endl;
    if (fCont)
        fCont->Print();
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Set the class name fCont must inherit from. Check if the class name
// inherits from MParContainer.
// In case of success returns kTRUE, kFALSE otherwise.
//
Bool_t MParEnv::SetClassName(const char *name)
{
    TClass *cls = GetClass(name, fLog);
    if (!cls)
        return kFALSE;

    if (!cls->InheritsFrom(MParContainer::Class()))
    {
        *fLog << err << "ERROR - " << name << " must inherit from MParContainer." << endl;
        return kFALSE;
    }

    fClassName = name;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Deletes fCont if we are owner.
// Sets fCont to 0.
// Checks if c inherits from fClassName.
// Sets fIsOwner to kFALSE.
// Sets fCont to c.
// Sets name of c to fName.
//
void MParEnv::SetCont(MParContainer *c)
{
    if (fIsOwner && fCont)
        delete fCont;
    fCont=0;

    if (!c)
        return;

    TClass *cls = GetClass(fClassName, fLog);
    if (!c->InheritsFrom(cls))
    {
        *fLog << err << "ERROR - MParEnv::SetCont: " << c->ClassName() << "  doesn't inherit from " << fClassName << endl;
        return;
    }

    fIsOwner = kFALSE;

    fCont = c;
    fCont->SetName(fName);
}

// --------------------------------------------------------------------------
//
// Checks for Class and Constructor.
//
// First we check for
//
//   MParEnv.Class: MMyContainer
//
// which would create a new instance of MMyContainer which must inherit from
// fClassName. The instance is created via TObject::New()
//
// If this was not found we check for
//
//   MParEnv.Constructor: MMyContainer(14, "Hallo", "Test");
//
// which calls the given constructor via gInterpreter->ProcessLine. Also here
// the result must inherit from fClassName.
//
// After successfull creation of the new class (or if a class was set already
// before and not overwritten in the resource file) the container's ReadEnv
// is called.
//
// WARNING: Especially in the second case oyu can do weird thing which
//          in principle can result in crashes. Please be extra-carefull.
//
Int_t MParEnv::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    MParContainer *cont = 0;
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "Class", print))
    {
        rc = kTRUE;
        TString cls = GetEnvValue(env, prefix, "Class", "");
        cont = GetNewObject(cls, fClassName);
    }

    if (!cont && IsEnvDefined(env, prefix, "Constructor", print))
    {
        rc = kTRUE;
        if (!gInterpreter)
        {
            *fLog << err << "ERROR - gInterpreter==NULL." << endl;
            return kERROR;
        }

        TString line = GetEnvValue(env, prefix, "Constructor", "");

        cont = (MParContainer*)gInterpreter->ProcessLine(TString("new ")+line);
    }

    if (!rc && !fCont) // FIXME: fAllowEmpty
        return kFALSE;

    if (rc)
    {
        SetCont(cont);
        if (!fCont)
        {
            if (cont)
                delete cont;
            return kERROR;
        }
        fIsOwner = kTRUE;
    }

    const Int_t res = fCont->ReadEnv(env, prefix, print);
    if (res==kERROR)
        return kERROR;

    return rc ? rc : res;
}
