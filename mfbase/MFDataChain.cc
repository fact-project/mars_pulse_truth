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
!   Author(s): Thomas Bretz  11/2002 <mailto:tbretz@astro.uni-wueruburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MFDataChain                                                              
//
// With this filter you can filter in all variables from Mars parameter
// containers using rules (for more details see MDataPhrase).
//
// In the constructor you can give the filter variable, like
//   "sqrt(MHillas.fLength*MHillas.fLength)"
// Where MHillas is the name of the parameter container in the parameter
// list and fLength is the name of the data member which should be used
// for the filter rule. If the name of the container is use specified
// (MyHillas) the name to give would be:
//   "MyHillas.fLength"
//
// For example:
//   MFDataChain filter("sqr(MHillas.fLength)", '<', 150);
//   MFDataChain filter("MHillas.fLength", '<', "MHillas.fWidth");
//
/////////////////////////////////////////////////////////////////////////////
#include "MFDataChain.h"

#include <fstream>

#include <TMethodCall.h>

#include "MParList.h"

#include "MDataValue.h"
#include "MDataPhrase.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MFDataChain);

using namespace std;

MFDataChain::MFDataChain(const char *name, const char *title)
    : fCond(0)
{
    fName  = name  ? name  : "MFDataChain";
    fTitle = title ? title : "Filter using any data member of a class";
}

// --------------------------------------------------------------------------
//
MFDataChain::MFDataChain(const char *rule, const char type, const Double_t val,
                         const char *name, const char *title)
    : fData(rule), fCond(new MDataValue(val))
{
    fName  = name  ? name  : "MFDataChain";
    fTitle = title ? title : "Filter using any data member of a class";

    //AddToBranchList(member);

    fFilterType = (type=='<' ? kELowerThan : kEGreaterThan);

    if (type!='<' && type!='>')
        *fLog << warn << dbginf << "Warning: Neither '<' nor '>' specified... using '>'." << endl;
}

MFDataChain::MFDataChain(const char *rule, const char type, const char *cond,
                         const char *name, const char *title)
    : fData(rule), fCond(new MDataPhrase(cond))
{
    fName  = name  ? name  : "MFDataChain";
    fTitle = title ? title : "Filter using any data member of a class";

    //AddToBranchList(member);

    fFilterType = (type=='<' ? kELowerThan : kEGreaterThan);

    if (type!='<' && type!='>')
        *fLog << warn << dbginf << "Warning: Neither '<' nor '>' specified... using '>'." << endl;
}

MFDataChain::~MFDataChain()
{
    if (fCond)
        delete fCond;
}

// --------------------------------------------------------------------------
//
Int_t MFDataChain::PreProcess(MParList *plist)
{
    if (!fCond)
    {
        *fLog << "No condition available - don't call default constructor!" << endl;
        return kFALSE;
    }
    return fData.PreProcess(plist) && fCond->PreProcess(plist);
}

// --------------------------------------------------------------------------
//
Int_t MFDataChain::Process()
{
    const Double_t data = fData.GetValue();
    const Double_t cond = fCond->GetValue();

    switch (fFilterType)
    {
    case kELowerThan:
        fResult = (data < cond);
        return kTRUE;
    case kEGreaterThan:
        fResult = (data > cond);
        return kTRUE;
    }

    return kFALSE;
}

void MFDataChain::Print(Option_t *) const
{
    *fLog << GetRule() << flush;
}

// --------------------------------------------------------------------------
//
//  Crahses if default constructor called.
//
void MFDataChain::StreamPrimitive(ostream &out) const
{
    out << "   MFDataChain " << GetUniqueName() << "(\"";
    out << fData.GetRule() << "\", '";
    out << (fFilterType==kELowerThan?"<":">");
    out << "', ";

    if (fCond->InheritsFrom(MDataValue::Class()))
        out << ((MDataValue*)fCond)->GetValue();
    else
        out << "\"" << fCond->GetRule() << "\"";

    out << ");" << endl;
}

// --------------------------------------------------------------------------
//
//  Crahses if default constructor called.
//
TString MFDataChain::GetRule() const
{
    TString ret(fData.GetRule());
    ret += fFilterType==kELowerThan?"<":">";

    TString str;
    str += fCond->GetRule();
    ret += str.Strip(TString::kBoth);
    return ret;
}

// --------------------------------------------------------------------------
//
//  Crahses if default constructor called.
//
TString MFDataChain::GetDataMember() const
{
    if (!fData.IsValid())
        return "";

    TString ret(fData.GetDataMember());
    ret += ",";
    ret += fCond->GetDataMember();
    return ret;
}
