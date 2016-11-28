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
!   Author(s): Thomas Bretz  04/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MDataMember
//
//   This objects corresponds to the data member of another object.
//   You can either specify the object as a string, eg "MHillas.fWidth"
//   where MHillas is the name of the container in the parameterlist
//   and fWidth is it's data member, or you can specify it by giving
//   the pointer corresponding to the instance of your object and
//   a TMethodCall pointer corresponding to the Member function returning
//   the requested value.
//
/////////////////////////////////////////////////////////////////////////////
#include "MDataMember.h"

#include <fstream>

#include <TMethodCall.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

ClassImp(MDataMember);

using namespace std;

// --------------------------------------------------------------------------
//
//  obj is a pointer to the instance of your class from which the data
//  should be requested. TMethodCall (s. root dox) is a pointer
//  to a TMethodCall object which should be the getter function for
//  the data you want to get.
//
MDataMember::MDataMember(MParContainer *obj, TMethodCall *call)
{
    fObject = obj;
    fCall   = call;

    if (fCall->ReturnType()==TMethodCall::kLong)
        SetBit(kIsInt);
    else
        ResetBit(kIsInt);

    fDataMember = (TString)obj->GetName() + "." + call->GetName();
}

// --------------------------------------------------------------------------
//
//  obj is a pointer to the instance of your class from which the data
//  should be requested. TMethodCall (s. root dox) is a pointer
//  to a TMethodCall object which should be the getter function for
//  the data you want to get.
//
MDataMember::MDataMember(MParContainer *obj, const TString call)
{
    fObject = obj;
    fCall   = obj->GetterMethod(call);

    if (fCall->ReturnType()==TMethodCall::kLong)
        SetBit(kIsInt);
    else
        ResetBit(kIsInt);

    fDataMember = (TString)obj->GetName() + "." + call;
}

// --------------------------------------------------------------------------
//
// Check if accessing fCall is safe
//
Bool_t MDataMember::CheckGet() const
{
    if (!fObject)
    {
        *fLog << err << "ERROR - MDataMember::Get: fObject not initialized ";
        *fLog << "(not PreProcessed)... returning 0." << endl;
        return kFALSE;
    }

    if (!fCall)
    {
        *fLog << err << "ERROR - MDataMemberGet: No TMethodCall for " << fDataMember << " of ";
        *fLog << fObject->GetName() << " available... returning 0." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns the value you requested
//
Double_t MDataMember::GetValue() const
{
    if (!CheckGet())
        return 0;

    switch (fCall->ReturnType())
    {
    case TMethodCall::kLong:
        Long_t l;
        fCall->Execute(fObject, l);
        return (Double_t)l;

    case TMethodCall::kDouble:
        Double_t v;
        fCall->Execute(fObject, v);
        return v;

    default:
        *fLog << err << "ERROR - MDataMember::GetValue: " << fObject->GetName();
        *fLog << "." << fDataMember << " has unkown type (=";
        *fLog << fCall->ReturnType() << ")... returning 0." << endl;
        return 0;
    }
}

// --------------------------------------------------------------------------
//
// Returns the string you requested
//
const char *MDataMember::GetString() const
{
    if (!CheckGet())
        return NULL;

    switch (fCall->ReturnType())
    {
    case TMethodCall::kString:
        {
            char *c=NULL;
            fCall->Execute(fObject, &c);
            return c;
        }

    default:
        *fLog << err << "DataMember " << fDataMember << " of ";
        *fLog << fObject->GetName() << " not a char*... returning NULL." << endl;
        return NULL;
    }
}

// --------------------------------------------------------------------------
//
// If a string was given PreProcess try to resolv the object name and
// tries to get it from the parlist. And also tries to resolve
// the data member (variable) name you requested and tries to get a
// corresponding TMethodCall from the root Dictionary.
// Remark: If your Data Member is called fDataMember the corresponding
//         getter Method in your class must be calles fDataMember
//
Bool_t MDataMember::PreProcess(const MParList *plist)
{
    // The following change has to be checked :
    // In order that fCall is initialized properly
    // the original statement "if (fCall)" is replaced by the statement
    if (fCall && !fObject)
        return kTRUE;

    TString cname(fDataMember);
    TString mname(fDataMember);

    const char *dot = strrchr(cname, '.');

    if (dot)
    {
        const int pos = dot-cname;

        cname.Remove(pos);
        mname.Remove(0, pos+1);
    }

    fObject = (MParContainer*)plist->FindObject(cname);
    if (!fObject)
    {
        *fLog << err << "Object '" << cname << "' not in parameter list... aborting." << endl;
        return kFALSE;
    }

    fCall = fObject->GetterMethod(mname);

    return fCall ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
// Returns the ready-to-save flag of the data member container
//
Bool_t MDataMember::IsReadyToSave() const
{
    return IsValid() ? fObject->IsReadyToSave() : kFALSE;
}

// --------------------------------------------------------------------------
//
// Print the name of the data member without an CR.
//
/*
void MDataMember::Print(Option_t *opt) const
{
    *fLog << fName << flush;
}
*/

// --------------------------------------------------------------------------
//
// Builds a rule which can be used in a MDataPhrase to describe this object
//
TString MDataMember::GetRule() const
{
    return fDataMember;
}

// --------------------------------------------------------------------------
//
// Returns the data member.
// This is mainly used in MTask::AddToBranchList
//
TString MDataMember::GetDataMember() const
{
    return fDataMember;
}
