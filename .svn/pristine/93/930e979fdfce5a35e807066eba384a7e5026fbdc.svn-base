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
!   Author(s): Thomas Bretz 07/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MChisqEval
//
// Evaluates a chisq from one or two MParameterD calculated in an eventloop.
//
/////////////////////////////////////////////////////////////////////////////
#include "MChisqEval.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MDataPhrase.h"
#include "MParameters.h" // MParameterD

#include "MParList.h"

ClassImp(MChisqEval);

using namespace std;

const TString MChisqEval::gsDefName  = "MChisqEval";
const TString MChisqEval::gsDefTitle = "Evaluate a chisq";

MChisqEval::MChisqEval(const char *name, const char *title)
    : fData0(NULL), fData1(NULL), fWeight(NULL), fNameResult("MinimizationValue")
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

MChisqEval::MChisqEval(MData *y1, const char *name, const char *title)
: fData0(NULL), fData1(NULL), fWeight(NULL), fNameResult("MinimizationValue")
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
    SetY1(y1);
}

MChisqEval::MChisqEval(MData *y1, MData *y2, const char *name, const char *title)
: fData0(NULL), fData1(NULL), fWeight(NULL), fNameResult("MinimizationValue")
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
    SetY1(y1);
    SetY2(y2);
}

MChisqEval::~MChisqEval()
{
    if (fData0 && (fData0->TestBit(kCanDelete) || TestBit(kIsOwner)))
        delete fData0;

    if (fData1 && (fData1->TestBit(kCanDelete) || TestBit(kIsOwner)))
        delete fData1;
}

void MChisqEval::SetY1(MData *data)
{
    // Set MC value
    if (fData0 && (fData0->TestBit(kCanDelete) || TestBit(kIsOwner)))
        delete fData0;
    fData0 = data;
    fData0->SetBit(kCanDelete);
    AddToBranchList(fData0->GetDataMember());
}

void MChisqEval::SetY2(MData *data)
{
    // Set measured/estimated value
    if (fData1 && (fData1->TestBit(kCanDelete) || TestBit(kIsOwner)))
        delete fData1;
    fData1 = data;
    fData1->SetBit(kCanDelete);
    AddToBranchList(fData1->GetDataMember());
}

void MChisqEval::SetY1(const TString data)
{
    SetY1(new MDataPhrase(data));
}

void MChisqEval::SetY2(const TString data)
{
    SetY2(new MDataPhrase(data));
}

Int_t MChisqEval::PreProcess(MParList *plist)
{
    fChisq = 0;
    fSumW  = 0;

    if (!fData0)
        return kFALSE;

    if (!fData0->PreProcess(plist))
        return kFALSE;

    if (fData1)
        if (!fData1->PreProcess(plist))
            return kFALSE;

    if (!fNameWeight.IsNull())
    {
        fWeight = (MParameterD*)plist->FindObject(fNameWeight, "MParameterD");
        if (!fWeight)
            return kFALSE;
    }

    fResult = (MParameterD*)plist->FindCreateObj("MParameterD", fNameResult);
    if (!fResult)
        return kFALSE;

    return kTRUE;
}

Int_t MChisqEval::Process()
{
    const Double_t y1 = fData0->GetValue();
    const Double_t y2 = fData1 ? fData1->GetValue() : 0;

    const Double_t dy = y2-y1;
    const Double_t er = fData1 ? y1*y2 : 1;

    const Double_t w  = fWeight ? fWeight->GetVal() : 1;

    fChisq += w*dy*dy/er;
    fSumW  += w;

    return kTRUE;
}

Int_t MChisqEval::PostProcess()
{
    if (GetNumExecutions()>0)
        fChisq /= fSumW;

    fResult->SetVal(fChisq);

    *fLog << inf << GetDescriptor() << ": Result=" << fChisq << endl;

    return kTRUE;
}

void MChisqEval::StreamPrimitive(ostream &out) const
{
    out << "   MChisqEval " << GetUniqueName() << ";";
    if (fData0)
        out << "   " << GetUniqueName() << ".SetY1(\"" << fData0->GetRule() << "\");" << endl;
    if (fData1)
        out << "   " << GetUniqueName() << ".SetY1(\"" << fData1->GetRule() << "\");" << endl;
}

