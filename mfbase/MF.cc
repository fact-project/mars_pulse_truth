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
!   Copyright: MAGIC Software Development, 2000-2005
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
// If you want to use mathematic expressions (as defined in MDataChain)
// you must encapsulate it in {}-parantheses, eg:
//   "{log10(MHillas.fSize)}>3"
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
//   -------->  '==' is NOT supported!
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
/////////////////////////////////////////////////////////////////////////////
#include "MF.h"

#include <ctype.h>        // isalnum, ...
#include <stdlib.h>       // strtod, ...
#include <fstream>        // ofstream, ...

#include <TMethodCall.h>

#include "MParList.h"

//#include "MFilterList.h"
#include "MFDataPhrase.h"
//#include "MFDataMember.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MF);

using namespace std;

const TString MF::gsDefName  = "MF";
const TString MF::gsDefTitle = "Filter setup by a text-rule";

// --------------------------------------------------------------------------
//
// Default Constructor. Don't use.
//
MF::MF() : fF(NULL), fAllowEmpty(kFALSE)
{
    fName  = gsDefName.Data();
    fTitle = gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
// Constructor. For the text describing the filter rule please see
// the class description above.
//
MF::MF(const char *text, const char *name, const char *title) : fAllowEmpty(kFALSE)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    *fLog << inf << "Parsing filter rule... " << flush;
    fF = new MFDataPhrase(text);
    /*
    if (!(fF=ParseString(text, 1)))
    {
        *fLog << err << dbginf << "Parsing '" << text << "' failed." << endl;
        return;
    }
    */
    *fLog << inf << "found: ";
    fF->Print();
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Destructor. Delete filters.
//
MF::~MF()
{
    if (fF)
        delete fF;
}
/*
// --------------------------------------------------------------------------
//
// Returns the number of alphanumeric characters (including '.')
// in the given string
//
Int_t MF::IsAlNum(TString txt) const
{
    int l = txt.Length();
    for (int i = 0; i<l; i++)
        if (!isalnum(txt[i]) && txt[i]!='.' && txt[i]!=';')
            return i;

    return l;
}

MFilter *MF::ParseRule(TString &txt, MFilter *filter0, Int_t level) const
{
    // For backward compatibility
    txt.ReplaceAll("{", "(");
    txt.ReplaceAll("}", ")");

    const Int_t fg = txt.First('>');
    const Int_t lg = txt.First('>');
    const Int_t fl = txt.First('<');
    const Int_t ll = txt.First('<');

    if (fg<0 && fl<0)
    {
        MFilter *f = (MFilter*)GetNewObject(txt, MFilter::Class());
        if (!f)
        {
            *fLog << err << dbginf << "Syntax Error: '" << txt << "' is neither a MFilter nor conditional sign found." << endl;
            return NULL;
        }
        // FIXME: Search object through list...?
        txt = "";
        return f;
    }

    if (fg>=0 && fl>=0)
    {
        *fLog << err << dbginf << "Syntax Error: Two coditional signs found in " << txt << endl;
        *fLog << "Currently you have to enclose all conditions in parantheses, like: \"(x<y) && (z<5)\"" << endl;
        return NULL;
    }
    if (fg!=lg || fl!=ll)
    {
        *fLog << err << dbginf << "Syntax Error: Coditional sign found twice " << txt << endl;
        return NULL;
    }

    const Int_t cond = fg<0 ? fl : fg;

    const TString rule1 = txt(0, cond);
    const TString rule2 = txt(cond+1, txt.Length());

    Int_t lvl = gLog.GetDebugLevel();
    gLog.SetDebugLevel(1);
    MFilter *f = new MFDataChain(rule1.Data(), txt[cond], rule2.Data());
    f->SetName(Form("Chain%02d%c", level, txt[cond]));
    gLog.SetDebugLevel(lvl);

    txt = "";
    return f;
}
// --------------------------------------------------------------------------
//
// Parse a text string. Returns a corresponding filter of filter list.
//
MFilter *MF::ParseString(TString txt, Int_t level)
{
    MFilter *filter0=NULL;

    TString type;
    int nlist = 0;

    while (!txt.IsNull())
    {
        MFilter *newfilter = NULL;

        txt = txt.Strip(TString::kBoth);

        //fLog << all << setw(level) << " " << "Text: " << level << " '" << txt << "'" << endl;

        switch (txt[0])
        {
        case '(':
            {
                //
                // Search for the corresponding parantheses
                //
                Int_t first=1;
                for (int cnt=0; first<txt.Length(); first++)
                {
                    if (txt[first]=='(')
                        cnt++;
                    if (txt[first]==')')
                        cnt--;

                    if (cnt==-1)
                        break;
                }

                if (first==txt.Length())
                {
                    *fLog << err << dbginf << "Syntax Error: ')' missing." << endl;
                    if (filter0)
                        delete filter0;
                    return NULL;
                }

                //
                // Make a copy of the 'interieur' and delete the substringä
                // including the parantheses
                //
                TString sub = txt(1, first-1);
                txt.Remove(0, first+1);

                //
                // Parse the substring
                //
                newfilter = ParseString(sub, level+1);
                if (!newfilter)
                {
                    *fLog << err << dbginf << "Parsing '" << sub << "' failed." << endl;
                    if (filter0)
                        delete filter0;
                    return NULL;
                }
            }
            break;

        case ')':
            *fLog << err << dbginf << "Syntax Error: Too many ')'" << endl;
            if (filter0)
                delete filter0;
            return NULL;


        case '&':
        case '|':
        case '^':
            if (filter0)
            {
                //
                // Check for the type of the conditional
                //
                TString is = txt[0];
                txt.Remove(0, 1);

                if (is==txt[0] && is!='^')
                {
                    is += txt[0];
                    txt.Remove(0, 1);
                }

                //
                // If no filter is available or the available filter
                // is of a different conditional we have to create a new
                // filter list with the new conditional
                //
                if (!filter0->InheritsFrom(MFilterList::Class()) || type!=is)
                {
                    MFilterList *list = new MFilterList(is);
                    list->SetName(Form("List_%s_%d", (const char*)is, 10*level+nlist++));

                    list->SetOwner();
                    list->AddToList(filter0);
                    filter0 = list;

                    type = is;
                }
                continue;
            }

            *fLog << err << dbginf << "Syntax Error: First argument of condition missing." << endl;
            if (filter0)
                delete filter0;
            return NULL;

        default:
            newfilter = ParseRule(txt, filter0, level++);
            if (!newfilter)
            {
                if (filter0)
                    delete filter0;
                return NULL;
            }
        }

        if (!filter0)
        {
            filter0 = newfilter;
            continue;
        }

        if (!filter0->InheritsFrom(MFilterList::Class()))
            continue;

        ((MFilterList*)filter0)->AddToList(newfilter);
    }

    return filter0;
}
*/

// --------------------------------------------------------------------------
//
// PreProcess all filters.
//
Int_t MF::PreProcess(MParList *plist)
{
    if (!fF)
    {
        if (IsAllowEmpty())
            return kTRUE;

        *fLog << err << dbginf << "No filter rule available." << endl;
        return kFALSE;
    }

    fF->SetAccelerator(GetAccelerator());

    if (!fF->CallPreProcess(plist))
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
Int_t MF::Process()
{
    return fF ? fF->CallProcess() : kTRUE;
}

// --------------------------------------------------------------------------
//
//  Postprocess all filters.
//
Int_t MF::PostProcess()
{
    return fF ? fF->CallPostProcess() : kTRUE;
}

// --------------------------------------------------------------------------
//
// Return the result of the filter rule. If no filter available the
// condition is always true
//
Bool_t MF::IsExpressionTrue() const
{
    return fF ? fF->IsConditionTrue() : kTRUE;
}

// --------------------------------------------------------------------------
//
// Return a comma seperated list of all data members used in the chain.
// This is mainly used in MTask::AddToBranchList
//
TString MF::GetDataMember() const
{
  return fF ? fF->GetDataMember() : (TString)"";
}

void MF::StreamPrimitive(ostream &out) const
{
    out << "   MF " << GetUniqueName();

    if (!fF)
    {
        out << ";" << endl;
        return;
    }

    out << "(\"" << fF->GetRule() << "\"";
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
void MF::Print(Option_t *opt) const
{
    *fLog << all << underline << GetDescriptor() << endl;
    if (fF)
        fF->Print();
    else
        *fLog << "<n/a>";
    *fLog << endl << endl;
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
// be written can be found in the class description of MF and MDataChain.
//
Int_t MF::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

    /*
     // It is assumed that each filter itself calls MFilter::ReadEnv
     // to process the Inverted directive
     Bool_t rc = MFilter::ReadEnv(env, prefix, print);
     if (rc==kERROR)
        return kERROR;
        */
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

        if (fF)
        {
            delete fF;
            fF = 0;
        }

        if (rule.IsNull())
        {
            *fLog << warn << GetDescriptor() << " ::ReadEnv - WARNING: Empty condition found." << endl;
            SetAllowEmpty();
            return kTRUE;
        }

        SetAllowEmpty(kFALSE);

        const Int_t fg = rule.First('>');
        const Int_t fl = rule.First('<');
        const Int_t fe = rule.First('=');

        if (fg<0 && fl<0 && fe<0)
        {
            MFilter *f = (MFilter*)GetNewObject(rule, MFilter::Class());
            if (!f)
            {
                *fLog << err << dbginf << "Syntax Error: '" << rule << "' is neither a MFilter nor conditional sign found." << endl;
                return kERROR;
            }
            fF = f;
        }
        else
            fF = new MFDataPhrase(rule);
        /*
        if (!(fF=ParseString(rule, 1)))
        {
            *fLog << err << "MF::ReadEnv - ERROR: Parsing '" << rule << "' failed." << endl;
            return kERROR;
        }
        */
        if (print)
        {
            *fLog << inf << "found: ";
            fF->Print();
            *fLog << endl;
        }
    }
    else
    {
        if (!fF)
        {
            *fLog << warn << GetDescriptor() << " ::ReadEnv - WARNING: No condition found." << endl;
            SetAllowEmpty();
            return kFALSE;
        }
    }

    // This is a workaround, because MFilter::ReadEnv and fF->ReadEnv
    // might check for "Inverted" in this case both gets inverted
    // and double-inversion is no inversion
    /*
    if (IsEnvDefined(env, prefix, "Inverted", print))
        if (GetEnvValue(env, prefix, "Inverted", kFALSE)==kTRUE)
            SetInverted(kFALSE);*/

    return fF->ReadEnv(env, prefix, print);
}
