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
!   Author(s): Thomas Bretz, 07/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFilterList
//
// A filter list can be used to concatenate filter (derived from MFilter)
// by a logical or bitwise operator. For more details see the constructor.
//
// The list is setup by adding filters to the list calling AddToList().
// For example in the case of the default constructor ("&&") all results
// are logically and'd together and the result of this and is returned.
//
// Because the meaning of all filters can be inverted calling SetInverted()
// which is defined in the base class MFilter you can use this list to
// invert the meaning of a filter, by eg:
//
//    MF anyfilter("MHillas.fAlpha");
//
//    MFilterList alist;
//    alist.AddToList(&anyfilter);
//
//    alist.SetInverted();
//
//  or do
//
//    MFilterList alist(&anyfilter);
//
//
// Adding the filterlist to the eventloop will process all contained filters.
// Doing this as early as possible is a simple way of processing all filters.
//
// If you want to make the list delete all contained filters you may make
// the list owner of the filters by calling SetOwner()
//
/////////////////////////////////////////////////////////////////////////////
#include "MFilterList.h"

#include <fstream>

#include <TString.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MIter.h"

ClassImp(MFilterList);

using namespace std;

static const TString gsDefName  = "MFilterList";
static const TString gsDefTitle = "List combining filters logically.";

// --------------------------------------------------------------------------
//
// Wrapper to simplify constructors.
//
void MFilterList::Init(const char *name, const char *title)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    gROOT->GetListOfCleanups()->Add(&fFilters);
    fFilters.SetBit(kMustCleanup);

    fFilterType = kEAnd;
}

// --------------------------------------------------------------------------
//
//   Default Constructor.
//
//   Specify the boolean operation which is used to evaluate the
//   result of this list. If no operation is specified "land" is
//   used.
//
//   Options:
//      and,  &  : is a bitwise and
//      or,   |  : is a bitwise or
//      xor,  ^  : is a bitwise exclusive or
//      land, && : is a logical and
//      lor,  || : is a logical or
//
//   The bitwise operators are implemented for convinience. They may not
//   make much sense. Maybe IsExpressionTrue should change its return
//   type from Bool_t to Int_t.
//
MFilterList::MFilterList(const char *type, const char *name, const char *title)
{
    Init(name, title);

    TString str(type);

    if (!str.CompareTo("OR", TString::kIgnoreCase)  || !str.CompareTo("|"))
        fFilterType = kEOr;
    if (!str.CompareTo("XOR", TString::kIgnoreCase) || !str.CompareTo("^"))
        fFilterType = kEXor;
    if (!str.CompareTo("LAND", TString::kIgnoreCase) || !str.CompareTo("&&"))
        fFilterType = kELAnd;
    if (!str.CompareTo("LOR", TString::kIgnoreCase) || !str.CompareTo("||"))
        fFilterType = kELOr;
}

// --------------------------------------------------------------------------
//
//   Constructor.
//
// Setup an '&&' filter list, adds the filter to the list and
// call MFilterList::SetInverted()
//
// To be used as a logical NOT.
//
MFilterList::MFilterList(MFilter *f, const char *name, const char *title)
{
    Init(name, title);

    SetInverted();
    AddToList(f);
}

// --------------------------------------------------------------------------
//
//   CopyConstructor
//
MFilterList::MFilterList(MFilterList &ts)
{
    fFilters.AddAll(&ts.fFilters);
    fFilterType = ts.fFilterType;
}

// --------------------------------------------------------------------------
//
//  Evaluates and returns the result of the filter list.
//  The expression is evaluated step by step, eg:
//  ((filter[0] # filter[1]) # filter[3]) # filter[4])
//  The '#' stands for the boolean operation which is specified in
//  the constructor.
//
Bool_t MFilterList::IsExpressionTrue() const
{
    TIter Next(&fFilters);

    MFilter *filter=(MFilter*)Next();

    if (!filter)
        return kTRUE;

    Bool_t rc = filter->IsConditionTrue();

    //
    // loop over all filters
    //
    switch (fFilterType)
    {
    case kEAnd:
        while ((filter=(MFilter*)Next()))
            rc &= filter->IsConditionTrue();
        break;

    case kEOr:
        while ((filter=(MFilter*)Next()))
            rc |= filter->IsConditionTrue();
        break;

    case kEXor:
        while ((filter=(MFilter*)Next()))
            rc ^= filter->IsConditionTrue();
        break;

    case kELAnd:
        while ((filter=(MFilter*)Next()))
            rc = (rc && filter->IsConditionTrue());
        break;

    case kELOr:
        while ((filter=(MFilter*)Next()))
            rc = (rc || filter->IsConditionTrue());
        break;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// If you want to add a new filter to the list call this function with the
// pointer to the filter to be added. 
//
Bool_t MFilterList::AddToList(MFilter *filter)
{
    if (!filter)
        return kTRUE;

    const char *name = filter->GetName();

    if (fFilters.FindObject(filter))
    {
        *fLog << warn << dbginf << "Filter " << filter << " already existing... skipped." << endl;
        return kTRUE;
    }

    if (fFilters.FindObject(name))
        *fLog << inf << "MFilterList::AddToList - '" << name << "' exists in List already..." << endl;

    *fLog << inf << "Adding " << name << " to " << GetName() << "... " << flush;

    filter->SetBit(kMustCleanup);
    fFilters.Add(filter);

    *fLog << "Done." << endl;

    AddToBranchList(filter->GetDataMember());

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// If you want to add a new filters from a TCollection to the list call
// this function.
//
Bool_t MFilterList::AddToList(const TCollection &col)
{
    TIter Next(&col);
    MFilter *f=0;
    Int_t idx=0;
    while ((f=(MFilter*)Next()))
    {
        if (!f->InheritsFrom(MFilter::Class()))
        {
            *fLog << warn << "WARNING - An object in TCollection doesn't inherit from MFilter... ignored." << endl;
            continue;
        }

        f->SetName(Form("F%d", idx++));
        if (!AddToList(f))
            return kFALSE;
    }
    return kTRUE;
}

void MFilterList::SetAccelerator(Byte_t acc)
{
    fFilters.R__FOR_EACH(MTask, SetAccelerator)(acc);
    MFilter::SetAccelerator(acc);
}

// --------------------------------------------------------------------------
//
// PreProcesses all filters in the list
//
Int_t MFilterList::PreProcess(MParList *pList)
{
    TIter Next(&fFilters);

    MFilter *filter=NULL;

    //
    // loop over all filters
    //
    while ((filter=(MFilter*)Next()))
    {
        filter->SetAccelerator(GetAccelerator());
        if (!filter->CallPreProcess(pList))
        {
            *fLog << err << "Error - Preprocessing Filter ";
            *fLog << filter->GetName() << " in " << fName << endl;
            return kFALSE;
        }
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// ReInit all filters in the list
//
Bool_t MFilterList::ReInit(MParList *pList)
{
    TIter Next(&fFilters);

    MFilter *filter=NULL;

    //
    // loop over all filters
    //
    while ((filter=(MFilter*)Next()))
    {
        if (!filter->ReInit(pList))
        {
            *fLog << err << "Error - ReInit Filter ";
            *fLog << filter->GetName() << " in " << fName << endl;
            return kFALSE;
        }
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Processes (updates) all filters in the list.
//
Int_t MFilterList::Process()
{
    TIter Next(&fFilters);

    MFilter *filter=NULL;

    //
    // loop over all filters
    //
    while ((filter=(MFilter*)Next()))
        if (!filter->CallProcess())
            return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// PostProcesses all filters in the list.
//
Int_t MFilterList::PostProcess()
{
    TIter Next(&fFilters);

    MFilter *filter=NULL;

    //
    // loop over all filters
    //
    while ((filter=(MFilter*)Next()))
        if (!filter->CallPostProcess())
            return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// If you want to use a verbose output ("and") instead of a symbolic ("&")
// one the option string must conatin a "v"
//
void MFilterList::Print(Option_t *opt) const
{
    *fLog << all << GetRule(opt) << flush;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MFilterList::StreamPrimitive(ostream &out) const
{
    out << "   MFilterList " << ToLower(fName) << "(\"";

    switch (fFilterType)
    {
    case kEAnd:
        out << "&";
        break;

    case kEOr:
        out  << "|";
        break;

    case kEXor:
        out  << "^";
        break;

    case kELAnd:
        out << "&&";
        break;

    case kELOr:
        out << "||";
        break;
    }

    out << "\"";

    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << ", \"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
    }
    out << ");" << endl << endl;

    MIter Next(&fFilters);

    MParContainer *cont = NULL;
    while ((cont=Next()))
    {
        cont->SavePrimitive(out, "");

        out << "   " << ToLower(fName) << ".AddToList(&";
        out << cont->GetUniqueName() << ");" << endl << endl;
    }
}

TString MFilterList::GetRule(Option_t *opt) const
{
    TString str(opt);
    const Bool_t verbose = str.Contains("V", TString::kIgnoreCase);

    TString ret = "(";

    TIter Next(&fFilters);

    MFilter *filter=(MFilter*)Next();

    //
    // loop over all filters
    //
    if (!filter)
        return "<empty>";

    ret += filter->GetRule();

    while ((filter=(MFilter*)Next()))
    {
        switch (fFilterType)
        {
        case kEAnd:
            ret += (verbose?" and ":" & ");
            break;

        case kEOr:
            ret += (verbose?" or ":" | ");
            break;

        case kEXor:
            ret += (verbose?" xor ":" ^ ");
            break;

        case kELAnd:
            ret += (verbose?" land ":" && ");
            break;

        case kELOr:
            ret += (verbose?" lor ":" || ");
            break;
        }

        ret += filter->GetRule();
    }

    return ret+")";
}

// --------------------------------------------------------------------------
//
// Return the data members existing in this array in a comma-seperated list
// (This is mainly used for MTask::AddToBranchList)
//
TString MFilterList::GetDataMember() const
{
    TString str;

    TIter Next(&fFilters);
    MFilter *filter = NULL;
    while ((filter=(MFilter*)Next()))
    {
        if (filter->GetDataMember().IsNull())
            continue;

        str += ",";
        str += filter->GetDataMember();
    }
    return str;
}

// --------------------------------------------------------------------------
//
// Loop over all set filters and distribute arr to their SetVariables.
// Make sure, that the values are unique (not two filters using the
// same index in the array with different meanings)
//
void MFilterList::SetVariables(const TArrayD &arr)
{
    fFilters.R__FOR_EACH(MFilter, SetVariables)(arr);
}
