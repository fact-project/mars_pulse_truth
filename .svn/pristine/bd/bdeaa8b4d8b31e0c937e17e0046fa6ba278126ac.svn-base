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
//   MDataList
//
/////////////////////////////////////////////////////////////////////////////

#include "MDataList.h"

#include <math.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MDataList);

using namespace std;

// --------------------------------------------------------------------------
//
//   Default Constructor. Not for usage!
//
MDataList::MDataList()
{
    fSign = kENone;

    gROOT->GetListOfCleanups()->Add(&fMembers);
    fMembers.SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
//   Constructor.
//
//   Specify the operation which is used to evaluate the
//   result of this list.
//
//   Options:
//      *,  /,  -,  +
//
MDataList::MDataList(char type)
{
    switch (type)
    {
    case '*':
        fSign = kEMult;
        return;
    case '/':
        fSign = kEDiv;
        return;
    case '-':
        fSign = kEMinus;
        return;
    case '+':
        fSign = kEPlus;
        return;
    case '%':
        fSign = kEModul;
        return;
    case '^':
        fSign = kEPow;
        return;
    default:
        fSign = kENone;
    }

    gROOT->GetListOfCleanups()->Add(&fMembers);
    fMembers.SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
//   CopyConstructor
//
MDataList::MDataList(MDataList &ts)
{
    fMembers.AddAll(&ts.fMembers);
    fSign = ts.fSign;
}

// --------------------------------------------------------------------------
//
//  Evaluates and returns the result of the member list.
//  The expression is evaluated step by step, eg:
//  ((member[0] # member[1]) # member[3]) # member[4])
//  The '#' stands for the boolean operation which is specified in
//  the constructor.
//
Double_t MDataList::GetValue() const
{
    TIter Next(&fMembers);

    MData *member=(MData*)Next();

    if (!member)
        return kTRUE;

    Double_t val = member->GetValue();

    //
    // loop over all members
    //
    switch (fSign)
    {
    case kENone:
        return 0;

    case kEPlus:
        while ((member=(MData*)Next()))
            val += member->GetValue();
        break;

    case kEMinus:
        while ((member=(MData*)Next()))
            val -= member->GetValue();
        break;

    case kEMult:
        while ((member=(MData*)Next()))
            val *= member->GetValue();
        break;

    case kEDiv:
        while ((member=(MData*)Next()))
        {
            const Double_t d = member->GetValue();
            if (d==0)
            {
                *fLog << warn << "Warning: Division by zero: " << member->GetRule() << endl;
                return 0;
            }
            val /= d;
        }
        break;

    case kEModul:
        while ((member=(MData*)Next()))
        {
            const Double_t d = member->GetValue();
            if (d==0)
            {
                *fLog << warn << "Warning: Modulo division by zero: " << member->GetRule() << endl;
                return 0;
            }
            val = fmod(val, d);
        }
        break;
    case kEPow:
        while ((member=(MData*)Next()))
            val = pow(val, member->GetValue());
        break;
    }
    return val;
}

// --------------------------------------------------------------------------
//
// Checks whether at least one member has the ready-to-save flag.
//
Bool_t MDataList::IsReadyToSave() const
{
    TIter Next(&fMembers);

    MData *data = NULL;

    while ((data=(MData*)Next()))
        if (data->IsReadyToSave())
            return kTRUE;

    return kFALSE;
}

Bool_t MDataList::IsValid() const
{
    TIter Next(&fMembers);

    MData *data = NULL;
    while ((data=(MData*)Next()))
        if (!data->IsValid())
            return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// If you want to add a new member to the list call this function with the
// pointer to the member to be added.
//
Bool_t MDataList::AddToList(MData *member)
{
    if (!member)
        return kTRUE;

    if (fMembers.FindObject(member))
    {
        *fLog << warn << dbginf << "Filter already existing... skipped." << endl;
        return kTRUE;
    }

    member->SetBit(kMustCleanup);
    fMembers.Add(member);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// PreProcesses all members in the list
//
Bool_t MDataList::PreProcess(const MParList *plist)
{
    TIter Next(&fMembers);

    MData *member=NULL;

    //
    // loop over all members
    //
    while ((member=(MData*)Next()))
        if (!member->PreProcess(plist))
        {
            *fLog << err << "Error - Preprocessing Data Member ";
            *fLog << member->GetName() << " in " << fName << endl;
            return kFALSE;
        }

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// If you want to use a verbose output ("and") instead of a symbolic ("&")
// one the option string must conatin a "v"
//
/*
void MDataList::Print(Option_t *opt) const
{
    *fLog << "(";

    TIter Next(&fMembers);

    TObject *member=Next();

    //
    // loop over all members
    //
    if (!member)
    {
        *fLog << "<empty>)" << flush;
        return;
    }

    member->Print();

    while ((member=Next()))
    {
        switch (fSign)
        {
        case kENone:
            break;

        case kEPlus:
            *fLog << "+";
            break;

        case kEMinus:
            *fLog << "-";
            break;

        case kEMult:
            *fLog << "*";
            break;

        case kEDiv:
            *fLog << "/";
            break;
        }

        member->Print();
    }

    *fLog << ")" << flush;
    }
    */

// --------------------------------------------------------------------------
//
// Builds a rule from all the list members. This is a rule which could
// be used to rebuild the list using the constructor of a MDataPhrase
//
TString MDataList::GetRule() const
{
    TIter Next(&fMembers);

    MData *member=(MData*)Next();

    //
    // loop over all members
    //
    if (!member)
        return "(<empty>)";

    TString str = "(";

    str += member->GetRule();

    while ((member=(MData*)Next()))
    {
        switch (fSign)
        {
        case kENone:
            break;

        case kEPlus:
            str += "+";
            break;

        case kEMinus:
            str += "-";
            break;

        case kEMult:
            str += "*";
            break;

        case kEDiv:
            str += "/";
            break;

        case kEModul:
            str += "%";
            break;

        case kEPow:
            str += "^";
            break;
        }

        str += member->GetRule();
    }

    str += ")";

    return str;
}

// --------------------------------------------------------------------------
//
// Return a comma seperated list of all data members used in the chain.
// This is mainly used in MTask::AddToBranchList
//
TString MDataList::GetDataMember() const
{
    TString str;

    TIter Next(&fMembers);

    MData *member=(MData*)Next();

    if (!member->GetDataMember().IsNull())
        str += member->GetDataMember();

    while ((member=(MData*)Next()))
    {
        if (!member->GetDataMember().IsNull())
        {
            str += ",";
            str += member->GetDataMember();
        }
    }

    return str;
}

void MDataList::SetVariables(const TArrayD &arr)
{
    fMembers.R__FOR_EACH(MData, SetVariables)(arr);
}
