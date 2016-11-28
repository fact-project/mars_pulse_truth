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
!   Author(s): Thomas Bretz  01/2002 <mailto:tbretz@astro.uni-wueruburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MFDataMember                                                              
//
// With this filter you can filter in all variables from Mars parameter
// containers.
//
// In the constructor you can give the filter variable, like
//   "MHillas.fLength"
// Where MHillas is the name of the parameter container in the parameter
// list and fLength is the name of the data member which should be used
// for the filter rule. If the name of the container is use specified
// (MyHillas) the name to give would be:
//   "MyHillas.fLength"
//
// For example:
//   MFDataMember filter("MHillas.fLength", '<', 150);
//
// You can test '<', '>' and '='. Warning: Using '=' may give strange results
// in case you are comparing floating point values.
//
// In case the data member is detected to be an integer value, both
// the data member and the val given as argument in the constructor
// are castet to Long_t.
//
// To test != use the SetInverted() member function.
//
/////////////////////////////////////////////////////////////////////////////
#include "MFDataMember.h"

#include <fstream>

#include <TMethodCall.h>

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MFDataMember);

using namespace std;

// --------------------------------------------------------------------------
//
MFDataMember::MFDataMember(const char *member, const char type, const Double_t val,
                           const char *name, const char *title)
    : fData(member), fValue(val)
{
    fName  = name  ? name  : "MFDataMember";
    fTitle = title ? title : "Filter using any data member of a class";

    AddToBranchList(member);

    switch (type)
    {
    case '>': fFilterType = kEGreaterThan; break;
    case '<': fFilterType = kELowerThan;   break;
    case '=': fFilterType = kEEqual;       break;
    default:  fFilterType = kEGreaterThan; break;
    }

    if (type!='<' && type!='=' && type!='>')
        *fLog << warn << dbginf << "Warning: Neither '<' nor '=' nor '>' specified... using '>'." << endl;
}

// --------------------------------------------------------------------------
//
Int_t MFDataMember::PreProcess(MParList *plist)
{
    return fData.PreProcess(plist);
}

// --------------------------------------------------------------------------
//
Int_t MFDataMember::Process()
{
    if (fData.IsInt())
    {
        switch (fFilterType)
        {
        case kELowerThan:
            fResult = ((Long_t)fData.GetValue() < (Long_t)fValue);
            return kTRUE;
        case kEGreaterThan:
            fResult = ((Long_t)fData.GetValue() > (Long_t)fValue);
            return kTRUE;
        case kEEqual:
            fResult = ((Long_t)fData.GetValue() == (Long_t)fValue);
            return kTRUE;
        }
    }
    else
    {
        switch (fFilterType)
        {
        case kELowerThan:
            fResult = (fData.GetValue() < fValue);
            return kTRUE;
        case kEGreaterThan:
            fResult = (fData.GetValue() > fValue);
            return kTRUE;
        case kEEqual:
            fResult = (fData.GetValue() == fValue);
            return kTRUE;
        }
    }

    return kFALSE;
}

void MFDataMember::Print(Option_t *) const
{
    *fLog << GetRule() << flush;
}

void MFDataMember::StreamPrimitive(ostream &out) const
{
    out << "   MFDataMember " << GetUniqueName() << "(\"";
    out << fData.GetRule() << "\", '";
    switch (fFilterType)
    {
    case kEGreaterThan: out << '>'; break;
    case kELowerThan:   out << '<'; break;
    case kEEqual:       out << '='; break;
    }
    out << "', " << fValue << ");" << endl;
}

TString MFDataMember::GetRule() const
{
    TString ret = fData.GetRule();
    switch (fFilterType)
    {
    case kEGreaterThan: ret +='>'; break;
    case kELowerThan:   ret +='<'; break;
    case kEEqual:       ret +='='; break;
    }

    TString str;
    str += fValue;

    return ret+str.Strip(TString::kBoth);
}

TString MFDataMember::GetDataMember() const
{
    return fData.GetDataMember();
}
