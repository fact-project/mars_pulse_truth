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

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   MFTriggerLvl1                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MFTriggerLvl1.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MMcTrig.hxx"

ClassImp(MFTriggerLvl1);

using namespace std;

// --------------------------------------------------------------------------
//
MFTriggerLvl1::MFTriggerLvl1(const char *cname, const char type, const Int_t val,
                             const char *name, const char *title) : fMcTrig(NULL)
{
    fContName = cname;
    Init(type, val, name, title);
}

// --------------------------------------------------------------------------
//
MFTriggerLvl1::MFTriggerLvl1(MMcTrig *mctrig, const char type, const Int_t val,
                             const char *name, const char *title) : fMcTrig(mctrig)
{
    Init(type, val, name, title);
}

// --------------------------------------------------------------------------
//
void MFTriggerLvl1::Init(const char type, const Int_t val,
                         const char *name, const char *title)
{
    fName  = name  ? name  : "MFTriggerLvl1";
    fTitle = title ? title : "Filter using number of 1st level triggers";

    fFilterType = (type=='<' ? kELowerThan : kEGreaterThan);

    if (type!='<' && type!='>')
        *fLog << warn << dbginf << "Warning: Neither '<' nor '>' specified... using '>'." << endl;

    fValue = val;

    AddToBranchList(Form("%s.fNumFirstLevel", (const char*)fContName));
}

// --------------------------------------------------------------------------
//
Bool_t MFTriggerLvl1::IsExpressionTrue() const
{
    return fResult;
}

// --------------------------------------------------------------------------
//
Int_t MFTriggerLvl1::PreProcess(MParList *pList)
{
    if (fMcTrig)
        return kTRUE;

    fMcTrig = (MMcTrig*)pList->FindObject(fContName);
    if (fMcTrig)
        return kTRUE;

    *fLog << err << dbginf << fContName << " [MMcTrig] not found... aborting." << endl;
        return kFALSE;
}

// --------------------------------------------------------------------------
//
Int_t MFTriggerLvl1::Process()
{
    const Int_t lvl1 = fMcTrig->GetFirstLevel();

    switch (fFilterType)
    {
    case kELowerThan:
        fResult = (lvl1 < fValue);
        break;
    case kEGreaterThan:
        fResult = (lvl1 > fValue);
        break;
    }

    return kTRUE;
}

void MFTriggerLvl1::StreamPrimitive(ostream &out) const
{
    if (fMcTrig)
        fMcTrig->SavePrimitive(out);

    out << "   MFTriggerLvl1 " << GetUniqueName() << "(";

    if (fMcTrig)
        out << "&" << fMcTrig->GetUniqueName();
    else
        out << "\"" << fContName << "\"";

    out << ", '" << (fFilterType==kELowerThan?"<":">") << "', " << fValue << ");" << endl;

}
