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
!   Author(s): Thomas Bretz, 04/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MDataFormula
//
// A MDataFormula is a wrapper for TFormula. It should have access to all
// multi-argument functions, to formulas and to single-argument functions
// unknown by MDataChain intrinsically.
//
// eg.
//  MDataFormula("myfunc(MParams.fValue)");
//  MDataFormula("TMath::Hypot(MParams.fX, MParams.fY)");
//  MDataFormula("TMath::Pow(MParams.fX, 7.5)");
//  MDataFormula("TMath::Pow(MParams.fX, [6])");
//
// Using this should be rather slow. If you can create a 'plain' Data-Chain
// this should be much faster.
//
// To do:
//  - implement SetVariables to be used in functions like "gaus(0)" or
//    "pol0(1)"
//
/////////////////////////////////////////////////////////////////////////////
#include "MDataFormula.h"

#include <TArrayD.h>
#include <TFormula.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MDataChain.h"

ClassImp(MDataFormula);

using namespace std;

TString MDataFormula::Parse(const char *rule)
{
    TString txt(rule);

    Int_t pos1 = txt.First('(');
    if (pos1<0)
    {
        *fLog << err << dbginf << "Syntax Error: '(' not found in " << txt << endl;
        return "";
    }

    Int_t pos2 = txt.Last(')');
    if (pos2<0)
    {
        *fLog << err << dbginf << "Syntax Error: ')' not found in " << txt << endl;
        return "";
    }

    if (pos2<pos1+1)
    {
        *fLog << err << dbginf << "Syntax Error: '(' not behind ')' in " << txt << endl;
        return "";
    }

    //TObject *func = gROOT->GetListOfFunctions()->FindObject(txt(0, pos1));

    TString args = txt(pos1+1, pos2-pos1);
    Int_t num = 0;

    while (args.Length()>0 && num<5)
    {
        const Int_t last = args.First(',');
        const Int_t len = last<0 ? args.Last(')') : last;
        if (len<0)
        {
            *fLog << err << dbginf << "Syntax Error: ',' found after ')'" << endl;
            return "";
        }

        const TString arg = args(0, len);

        MDataChain *chain = new MDataChain(arg);
        if (!chain->IsValid())
        {
            delete chain;
            return "";
        }
        fMembers.AddLast(chain);

        args.Remove(0, len+1);
        pos1 += len;

        txt.Replace(txt.Index(arg), len, Form("[%d]", num++));
    }

    return txt;
}

// --------------------------------------------------------------------------
//
//   Default Constructor. Not for usage!
//
MDataFormula::MDataFormula(const char *rule, const char *name, const char *title) : fFormula(0)
{
    fName  = name  ? name  : "MDataFormula";
    fTitle = title ? title : rule;

    fMembers.SetOwner();

    if (!rule)
        return;

    const TString txt=Parse(rule);
    if (txt.IsNull())
    {
        fMembers.Delete();
        return;
    }

    fFormula = new TFormula;
    // Must have a name otherwise all axis labels disappear like a miracle
    fFormula->SetName(fName.IsNull()?"TFormula":fName.Data());

    if (fFormula->Compile(txt))
    {
        *fLog << err << dbginf << "Syntax Error: TFormula::Compile failed for " << txt << endl;
        delete fFormula;
        fFormula = 0;
        return;
    }

    gROOT->GetListOfFunctions()->Remove(fFormula);
}

// --------------------------------------------------------------------------
//
//   Destructor
//
MDataFormula::~MDataFormula()
{
    if (fFormula)
        delete fFormula;
}

// --------------------------------------------------------------------------
//
//   CopyConstructor
//
MDataFormula::MDataFormula(MDataFormula &ts)
{
    TFormula *f = ts.fFormula;

    fName  = "MDataFormula";
    fTitle = f ? f->GetExpFormula() : (TString)"";

    fFormula = f ? (TFormula*)f->Clone() : 0;
    gROOT->GetListOfFunctions()->Remove(fFormula);

    fMembers.SetOwner();

    TObject *o = NULL;
    TIter Next(&ts.fMembers);
    while ((o=Next()))
        fMembers.AddLast(o->Clone());
}

// --------------------------------------------------------------------------
//
//  Evaluates and returns the result of the member list.
//  The expression is evaluated step by step, eg:
//  ((member[0] # member[1]) # member[3]) # member[4])
//  The '#' stands for the boolean operation which is specified in
//  the constructor.
//
Double_t MDataFormula::GetValue() const
{
    TArrayD val(fMembers.GetSize());

    MData *member = NULL;

    Int_t i=0;
    TIter Next(&fMembers);
    while ((member=(MData*)Next()))
        val[i++] = member->GetValue();

    return fFormula->EvalPar(0, val.GetArray());
}

Bool_t MDataFormula::IsValid() const
{
    TIter Next(&fMembers);

    MData *data = NULL;
    while ((data=(MData*)Next()))
        if (!data->IsValid())
            return kFALSE;

    return fFormula ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
// Checks whether at least one member has the ready-to-save flag.
//
Bool_t MDataFormula::IsReadyToSave() const
{
    TIter Next(&fMembers);

    MData *data = NULL;

    while ((data=(MData*)Next()))
        if (data->IsReadyToSave())
            return kTRUE;

    return kFALSE;
}

// --------------------------------------------------------------------------
//
// PreProcesses all members in the list
//
Bool_t MDataFormula::PreProcess(const MParList *plist)
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
// Builds a rule from all the list members. This is a rule which could
// be used to rebuild the list using the constructor of a MDataChain
//
TString MDataFormula::GetRule() const
{
    if (!fFormula)
        return "<empty>";

    TString rule = fFormula->GetTitle(); //fFormula->GetExpFormula();

    MData *member = NULL;

    Int_t i=0;
    TIter Next(&fMembers);
    while ((member=(MData*)Next()))
    {
        TString r = member->GetRule();
        r.ReplaceAll("]", "\\]");
        rule.ReplaceAll(Form("[%d]", i++), r);
    }
    rule.ReplaceAll("\\]", "]");

    return rule;
}
/*
// --------------------------------------------------------------------------
//
// Return a comma seperated list of all data members used in the chain.
// This is mainly used in MTask::AddToBranchList
//
TString MDataFormula::GetDataMember() const
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
*/
void MDataFormula::SetVariables(const TArrayD &arr)
{
    fMembers.R__FOR_EACH(MData, SetVariables)(arr);
}

