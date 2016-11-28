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
!   Author(s): Thomas Bretz  01/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MF                                                              
//
// With this filter you can filter in all variables from Mars parameter
// containers.
//
// In the constructor you can give the filter rule, like
//   "MHillas.fLength < 15"
// Where MHillas is the name of the parameter container in the parameter
// list and fLength is the name of the data member which should be used
// for the filter rule. If the name of the container is use specified
// (MyHillas) the name to give would be:
//   "MyHillas.fLength < 15"
//
// Also more difficult rules are allowed, like:
//   "MHillas.fWidth<0.5 && MHillas.fLength<0.6"
//
// You can also use parantheses:
//   "MHillas.fSize>200 || (MHillas.fWidth<0.5 && MHillas.fLength<0.6)"
//
// For more details on available functions and expressions see MDataChain!
//
// The allowed logigal conditionals are:
//   &&: logical and
//   ||: logical or
//   ^:  exclusive or
//
// As conditional signs, for now, only:
//   <, >
// are allowed.
//
//
// Warning: There is no priority rule build in. So better use parantheses
//   to get correct results. The rule is parsed/evaluated from the left
//   to the right, which means:
//
//   "MHillas.fSize>200 || MHillas.fWidth<0.5 && MHillas.fLength<0.6"
//
//    is parses as
//
//   "(MHillas.fSize>200 || MHillas.fWidth<0.5) && MHillas.fLength<0.6"
//
//
// FIXME: The possibility to use also complete filters is missing.
//        Maybe we can use gInterpreter->Calc("") for this.
//        gROOT->ProcessLineFast("line");
//
//
// Class Version 2:
// ----------------
//  + Bool_t fDefault
//
/////////////////////////////////////////////////////////////////////////////
#include "MFDataPhrase.h"

#include <fstream>        // ofstream, ...

#include <TMethodCall.h>

#include "MParList.h"

#include "MDataPhrase.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MFDataPhrase);

using namespace std;

const TString MFDataPhrase::gsDefName  = "MFDataPhrase";
const TString MFDataPhrase::gsDefTitle = "Filter setup by a text-rule";

// --------------------------------------------------------------------------
//
// Default Constructor. Don't use.
//
MFDataPhrase::MFDataPhrase()
    : fData(NULL), fAllowEmpty(kFALSE), fDefault(kTRUE)
{
    fName  = gsDefName.Data();
    fTitle = gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
// Constructor. For the text describing the filter rule please see
// the class description above.
//
MFDataPhrase::MFDataPhrase(const char *text, const char *name, const char *title)
    : fData(NULL), fAllowEmpty(kFALSE), fDefault(kTRUE)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    if (!text)
        return;

    *fLog << inf << "Parsing filter rule... " << flush;
    fData=new MDataPhrase(text);
    *fLog << inf << "found: ";
    fData->Print();
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Destructor. Delete filters.
//
MFDataPhrase::~MFDataPhrase()
{
    if (fData)
        delete fData;
}

// --------------------------------------------------------------------------
//
// PreProcess all filters.
//
Int_t MFDataPhrase::PreProcess(MParList *plist)
{
    if (!fData)
    {
        if (IsAllowEmpty())
            return kTRUE;

        *fLog << err << dbginf << "No filter rule available." << endl;
        return kFALSE;
    }

//    fData->SetAccelerator(GetAccelerator());

    if (!fData->PreProcess(plist))
    {
        *fLog << err << dbginf << "PreProcessing filters in ";
        *fLog << fName << " failed." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Process all filters.
//
Int_t MFDataPhrase::Process()
{
    fResult = fData ? fData->GetValue()>0.5 : fDefault;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Postprocess all filters.
//
Int_t MFDataPhrase::PostProcess()
{
    return kTRUE;
}

TString MFDataPhrase::GetRule() const
{
    return fData ? fData->GetRule() : MFilter::GetRule();
}

void MFDataPhrase::SetVariables(const TArrayD &arr)
{
    if (fData)
        fData->SetVariables(arr);
}

// --------------------------------------------------------------------------
//
// Return the result of the filter rule. If no filter available the
// condition is always true
//
Bool_t MFDataPhrase::IsExpressionTrue() const
{
    return fResult;
}

// --------------------------------------------------------------------------
//
// Return a comma seperated list of all data members used in the chain.
// This is mainly used in MTask::AddToBranchList
//
TString MFDataPhrase::GetDataMember() const
{
  return fData ? fData->GetDataMember() : (TString)"";
}

void MFDataPhrase::StreamPrimitive(ostream &out) const
{
    out << "   MF " << GetUniqueName();

    if (!fData)
    {
        out << ";" << endl;
        return;
    }

    out << "(\"" << fData->GetRule() << "\"";
        if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << "(\"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
    }
    out << ");" << endl;

}

// --------------------------------------------------------------------------
//
// Print the rule.
// if now filter is set up '<n/a>' is printed.
//
void MFDataPhrase::Print(Option_t *opt) const
{
    *fLog << all << underline << GetDescriptor() << endl;
    if (fData)
        fData->Print();
    else
        *fLog << "<n/a>" << endl;
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Check for corresponding entries in resource file and setup filters.
//
// Assuming your MF-filter is called (Set/GetName): MyFilter
//
// First you can setup whether the filter is inverted or not:
//     MyFilter.Inverted: yes, no  <default=no>
//
// Now setup the condition, eg:
//     MyFilter.Condition: MHillas.fSize<1000
// or
//     MyFilter.Condition: MHillas.fSize>500 && MHillas.fSize<10000
//
// If you want to use more difficult filters you can split the
// condition into subcondistions. Subcondistions are identified
// by {}-brackets. Avoid trailing 0's! For example:
//
//     MyFilter.Condition: MHillas.fSize>500 && {0} && {1}
//     MyFilter.0: MHillas.fSize>1000
//     MyFilter.1: MHillas.fSize<10000
//
// The numbering must be continous and start with 0. You can use
// a subcondition more than once. All {}-brackets are simply replaced
// by the correspodning conditions. The rules how conditions can
// be written can be found in the class description MDataPhrase.
//
Int_t MFDataPhrase::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Int_t rc1 = MFilter::ReadEnv(env, prefix, print);
    if (rc1==kERROR)
        return kERROR;

    if (IsEnvDefined(env, prefix, "Condition", print))
    {
        TString rule = GetEnvValue(env, prefix, "Condition", "");
        rule.ReplaceAll(" ", "");

        Int_t idx=0;
        while (1)
        {
            TString cond;
            if (IsEnvDefined(env, prefix, Form("%d", idx), print))
            {
                cond += "(";
                cond += GetEnvValue(env, prefix, Form("%d", idx), "");
                cond += ")";
            }

            if (cond.IsNull())
                break;

            rule.ReplaceAll(Form("{%d}", idx), cond);
            idx++;
        }

        if (fData)
        {
            delete fData;
            fData = 0;
        }

        if (rule.IsNull())
        {
            *fLog << warn << GetDescriptor() << " ::ReadEnv - WARNING: Empty condition found." << endl;
            SetAllowEmpty();
            return kTRUE;
        }

        SetAllowEmpty(kFALSE);

        fData = new MDataPhrase(rule);
        /*
        if (!(fF=ParseString(rule, 1)))
        {
            *fLog << err << "MFDataPhrase::ReadEnv - ERROR: Parsing '" << rule << "' failed." << endl;
            return kERROR;
        }
        */
        if (print)
        {
            *fLog << inf << "found: ";
            fData->Print();
            *fLog << endl;
        }

        rc1 = kTRUE;
    }
    else
    {
        if (!fData)
        {
            *fLog << warn << GetDescriptor() << " ::ReadEnv - WARNING: No condition found." << endl;
            SetAllowEmpty();
            return kFALSE;
        }
    }

    const Int_t rc2 = fData->ReadEnv(env, prefix, print);
    if (rc2==kERROR)
        return kERROR;

    // This is a workaround, because MFilter::ReadEnv and fF->ReadEnv
    // might check for "Inverted" in this case both gets inverted
    // and double-inversion is no inversion
    /*
    if (IsEnvDefined(env, prefix, "Inverted", print))
        if (GetEnvValue(env, prefix, "Inverted", kFALSE)==kTRUE)
            SetInverted(kFALSE);*/

    return rc1 || rc2;
}
