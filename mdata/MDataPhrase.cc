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
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MDataPhrase
//
// A MDataPhrase is a wrapper for TFormula. It supports access to data-
// members and/or member functions acessible from the parameter list
// via MDataMember. It supports access to elements of a MMatrix through
// the parameter list via MDataElement and it sipports variables set
// by SetVariables via MDataValue.
//
// The parsing is done by TFormula. For more information which functions
// are supported see TFormula, TFormulaPrimitives and TFormulaMathInterface.
//
// The support is done by replacing the parameters with access to the
// parameter list by parameters like [0], [1],... When evaluating
// the TFormula first the parameters are evaluated and passed to
// TFormula. Each parameter is evaluated only once and, if necessary,
// passed more than once to TFormula. To store the MData* classes used
// for parameter access a TObjArray is used. Its advantage is, that
// it has an UncheckedAt function which saves some time, because
// no obsolete sanity checks must be done accessing the array.
//
// Because everything supported by TFormula is also supported by
// MDataPhrase also conditional expression are supported.
//
// For supported functions see TFormulaPrimitive and TMathInterface.
//
// Examples:
//
//   Gaus, Gausn, Landau, Landaun, Pol0-Pol10, Pow2-Pow5
//
// In the constructor you can give rule, like
//   "HillasSource.fDist / MHillas.fLength"
// Where MHillas/HillasSource is the name of the parameter container in
// the parameter list and fDist/fLength is the name of the data members
// in the containers. The result will be fDist divided by fLength.
//
// In case you want to access a data-member which is a data member object
// you can acces it with (Remark: it must derive from MParContainer):
//   "MCameraLV.fPowerSupplyA.fVoltagePos5V"
// (THIS FEATURE IS CURRENTLY NOT SUPPORTED)
//
// You can also use parantheses:
//   "HillasDource.fDist / (MHillas.fLength + MHillas.fWidth)"
//
// Additional implementations:
//
//   isnan(x)  return 1 if x is NaN (Not a Number) otherwise 0
//   finite(x) return 1 if the number is a valid double (not NaN, inf)
//
// NaN (Not a Number) means normally a number which is to small to be
// stored in a floating point variable (eg. 0<x<1e-56 or similar) or
// a number which function is not defined (like asin(1.5))
//
// inf is the symbol for an infinite number.
//
//
// Constants
// ---------
//
// Most constants you might need can be found in TMath, eg:
//  TMath::Pi(), TMath::TwoPi(), TMath::Ln10(), TMath::LogE()
//  TMath::RadToDeg(), TMath::DegToRad();
//
//
// Variable Parameters
// ------------------------
// If you want to use variables, eg for fits you can use [0], [1], ...
// These values are initialized with 0 and set by calling
// SetVariables(), eg
//   [0]*MHillas.fArea
//
//
// Multi-argument functions
// ------------------------
// You can use multi-argument functions, too. Example:
//   "TMath::Hypot(MHillas.fMeanX, MHillas.MeanY)"
//   "pow(MHillas.fMeanX*MHillas.MeanY, -1.2)"
//
//
//
// To Do:
//  - The possibility to use other objects inheriting from MData
//    is missing.
//
/////////////////////////////////////////////////////////////////////////////
#include "MDataPhrase.h"

#include <TMath.h>
#include <TArrayI.h>
#include <TPRegexp.h>
#include <TFormula.h>
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,26,00)
#include <TFormulaPrimitive.h>
#endif

#include "MLog.h"
#include "MLogManip.h"

#include "MArrayD.h"

#include "MDataValue.h"
#include "MDataMember.h"
#include "MDataElement.h"

ClassImp(MDataPhrase);

using namespace std;

// --------------------------------------------------------------------------
//
// Check for the existance of the expression [idx] in the string
// phrase. If existing a corresponding new MDataValue is added to
// fMembers and index is increased by one.
//
// This makes the use of user-defined variables in the phrase possible.
//
Int_t MDataPhrase::CheckForVariable(const TString &phrase, Int_t idx)
{
    TString mods;
    TArrayI pos;

    while (1)
    {
        // \\A: matches the beginning of the string like ^ does
        // \\Z: matches the end       of the string like $ does
        // \\W: matches any non-word character [^a-zA-Z_0-9]
        TPRegexp reg(Form("(\\A|\\W)\\[0*%d\\](\\W|\\Z)", idx));
        if (reg.Match(phrase, mods, 0, 130, &pos)==0)
            break;

        // [idx] already existing. Add a corresponding MDataValue
        fMembers.AddLast(new MDataValue(0, idx));
        idx++;
    }

    return idx;
}

// --------------------------------------------------------------------------
//
// Replace all expressions expr (found by a regular expression \\b%s\\b
// with %s being the expression) in the string phrase by [idx].
//
// The length of [idx] is returned.
//
Int_t MDataPhrase::Substitute(TString &phrase, const TString &expr, Int_t idx) const
{
    const TString arg = Form("[%d]", idx);

    TPRegexp reg(expr);

    TString mods;
    TArrayI pos;

    Int_t p = 0;
    while (1)
    {
        // check whether argument is found
        if (reg.Match(phrase, mods, p, 130, &pos)==0)
            break;

        // Replace expression by argument [idx]
        phrase.Replace(pos[0], pos[1]-pos[0], arg, arg.Length());

        // Jump behind the new string which was just inserted
        p = pos[0]+arg.Length();
    }
    return arg.Length();
}

TString MDataPhrase::Parse(TString phrase)
{
    // This is for backward compatibility with MDataChain
    phrase.ReplaceAll("gRandom->", "TRandom::");
    phrase.ReplaceAll("kRad2Deg",  "TMath::RadToDeg()");
    phrase.ReplaceAll("kDeg2Rad",  "TMath::DegToRad()");
    phrase.ReplaceAll(" ", "");

    int idx=0;
    int p =0;

    TString mods;
    TArrayI pos;
/*
    // Find all functions...
    // The first \\w+ should also allow ::
    TPRegexp reg = TPRegexp("[[:word:]:]+\\([^()]*\\)");
    while (1)
    {
        //idx = CheckForVariables(phrase, idx);

        if (reg.Match(phrase, mods, p, 130, &pos)==0)
            break;

        const Int_t len = pos[1]-pos[0];

        // This is the full function with its argument(s)
        const TString term = phrase(pos[0], len);

        // Now get rid of the argument(s)
        const TPRegexp reg3("\\([^()]+\\)");

        TString func(term);
        reg3.Substitute(func, "");

        // Seems to be a special case whic is not handles by
        // TFormulaPrimitive by known by TFormula
        if (func.BeginsWith("TRandom::"))
        {
            p = pos[0]+pos[1];
            continue;
        }

        // check whether the function is available
        if (TFormulaPrimitive::FindFormula(func))
        {
            p = pos[0]+pos[1];
            continue;
        }

        cout << "Unknown: " << func  << endl;
        // p = pos[0]+pos[1];
        return;
    }
*/

    p   = 0;

    // Find all data-members in expression such as
    // MTest.fDataMember. Because also floating point
    // numbers can contain a dot the result has to
    // be checked carefully
    // \\w: matches any word character [a-zA-Z_0-9]
    TPRegexp reg = TPRegexp("\\w+[.]\\w+");
    TPRegexp ishex("^0x[[:xdigit:]]+$");

    while (1)
    {
        // If some indices are already existing
        // initialize them by a flexible MDataValue
        idx = CheckForVariable(phrase, idx);

        // Check whether expression is found
        if (reg.Match(phrase, mods, p, 130, &pos)==0)
            break;

        // Get expression from phrase
        const TString expr = phrase(pos[0], pos[1]-pos[0]);

        // Also hex-numbers and floats fullfill our condition...
        // FIXME: WHY HEX NUMBERS?
        if (!expr(ishex).IsNull() || expr.IsFloat())
        {
            p = pos[1];
            continue;
        }

        // Add a corresponding MDataMember to our list
        fMembers.AddLast(new MDataMember(expr));

        // Find other occurances of arg by this regexp
        // and start next search behind first match
        const TString regexp = Form("\\b%s\\b", expr.Data());
        p = pos[0] + Substitute(phrase, regexp, idx);

        // Step forward to the next argument
        idx++;
    }

    p = 0;

    // Now check for matrix elemets as M[5]
    reg = TPRegexp("\\w+\\[\\d+\\]");
    while (1)
    {
        // If some indices are already existing
        // initialize them by a MDataValue
        idx = CheckForVariable(phrase, idx);

        // Check whether expression is found
        if (reg.Match(phrase, mods, p, 130, &pos)==0)
            break;

        // Get expression from phrase
        TString expr = phrase(pos[0], pos[1]-pos[0]);

        // Add a corresponding MDataMember to our list
        fMembers.AddLast(new MDataElement(expr));

        // Make the expression "Regular expression proofed"
        expr.ReplaceAll("[", "\\[");
        expr.ReplaceAll("]", "\\]");

        // Find other occurances of arg by this regexp
        // and start next search behind first match
        const TString regexp = Form("\\b%s", expr.Data());
        p = pos[0] + Substitute(phrase, regexp, idx);

        // Step forward to the next argument
        idx++;
    }
    /*

    // * HOW DO WE PROCESS THE FILTERS?
    // * DO WE NEED THIS FOR MData derived classes? Is there any need for it?
    // * MAYBE FIRST FILTERS (MF?) MUST REPLACE {name[class]} BEFORE
    //   THE DATA PHRASSE IS CREATED?
    // --> MFDataPhrase must have a list of MFilter. In Process first
    //     all self created MFilter are processed (see MF). Then
    //     they are evaluated and the result is given to the MDataPhrase.
    //     Can this be done using MDataMember? We replace {Name[class]}
    //     by Name.IsExpressionTrue and we need a way that MDataPhrase
    //     gets the coresponding pointer.
    // --> Alternatively we can create a MDataPhrase which allows
    //     Pre/Processing
    //
    // We convert {Name[Class]} to Name.IsExpressionTrue. To process these
    // data/filters we need a request from MFDataPhrase (new virtual
    // memeber function?)
    //
    // {} Is alreaqdy used in ReadEnv.
    //
    // Enhance ReadEnv to allow Cut1.5 to be just a class.
    //
    // The difference between MFDataPhrase is
    //   MFDataPhrase only knows MDataPhrase, while
    //   MF also knows a filter-class.
    //

    p = 0;

    // And now we check for other phrases or filters
    // They are defined by a [, a pribtable character and
    // any numer of word characters (a-zA-Z0-9_) and a closing ]
    reg = TPRegexp("{[A-Za-z}\\w+(\\[[A-Za-z]\\w+\\])?}");
    while (1)
    {
        // If some indices are already existing
        // initialize them by a MDataValue
        idx = CheckForVariable(phrase, idx);

        // Check whether expression is found
        if (reg.Match(phrase, mods, p, 130, &pos)==0)
            break;

        // Get expression from phrase
        TString expr = phrase(pos[0], pos[1]-pos[0]);

        // Zerlegen: {Name[Class]}

        // Create a new MData object of kind
        MData *dat = (MData*)GetNewObject(cls, MData::Class());
        if (!dat)
            return "";
        dat->SetName(name);

        // Add a corresponding MDataMember to our list
        fMembers.AddLast(dat);

        // Make the expression "Regular expression proofed"
        expr.ReplaceAll("[", "\\[");
        expr.ReplaceAll("]", "\\]");

        // Find other occurances of arg by this regexp
        // and start next search behind first match
        p = pos[0] + Substitute(phrase, expr, idx);

        // Step forward to the next argument
        idx++;
    }
 */
    // Now we have to check if there are additional indices [idx]
    // This is mainly important if the rule has indices only!
    while (1)
    {
        const Int_t newidx = CheckForVariable(phrase, idx);
        if (newidx == idx)
            break;
    }

    return phrase;
}

// --------------------------------------------------------------------------
//
// Clear Formula and corresponding data members
//
void MDataPhrase::Clear(Option_t *)
{
    fMembers.Delete();
    if (!fFormula)
        return;

    delete fFormula;
    fFormula = NULL;
}

// --------------------------------------------------------------------------
//
// Set a new phrase/rule. Returns kTRUE on sucess, kFALSE otherwise
//
Bool_t MDataPhrase::SetRule(const TString &rule)
{
    Clear();

    const TString txt=Parse(rule);
    if (txt.IsNull())
    {
        Clear();
        return kFALSE;
    }

    fFormula = new TFormula;

    // Must have a name otherwise all axis labels disappear like a miracle
    fFormula->SetName(fName.IsNull()?"TFormula":fName.Data());
    if (fFormula->Compile(txt))
    {
        *fLog << err << dbginf << "Syntax Error: TFormula::Compile failed..."<< endl;
        *fLog << " Full Rule:   " << rule << endl;
        *fLog << " Parsed Rule: " << txt << endl;
        Clear();
        return kFALSE;
    }

    gROOT->GetListOfFunctions()->Remove(fFormula);

    return kTRUE;
}

namespace MFastFun {
   //
   // Namespace with basic primitive functions registered by TFormulaPrimitive
   // all function registerd by TFormulaPrimitive can be used in TFormula
   //
    Double_t Nint(Double_t x){return TMath::Nint(x);}
    Double_t Sign(Double_t x){return x<0?-1:+1;}
    Double_t IsNaN(Double_t x){return TMath::IsNaN(x);}
    Double_t Finite(Double_t x){return TMath::Finite(x);}
}

// --------------------------------------------------------------------------
//
// Default constructor. Set a rule (phrase), see class description for more
// details. Set a name and title. If no title is given it is set to the rule.
//
MDataPhrase::MDataPhrase(const char *rule, const char *name, const char *title) : fFormula(0)
{
    // More in TFormulaPrimitive.cxx
    // More in TFormulaMathInterface
    if (!TFormulaPrimitive::FindFormula("isnan"))
    {
        TFormulaPrimitive::AddFormula(new TFormulaPrimitive("log2",   "log2",   (TFormulaPrimitive::GenFunc10)TMath::Log2));
        TFormulaPrimitive::AddFormula(new TFormulaPrimitive("fabs",   "fabs",   (TFormulaPrimitive::GenFunc10)TMath::Abs));
        TFormulaPrimitive::AddFormula(new TFormulaPrimitive("floor",  "floor",  (TFormulaPrimitive::GenFunc10)TMath::Floor));
        TFormulaPrimitive::AddFormula(new TFormulaPrimitive("ceil",   "ceil",   (TFormulaPrimitive::GenFunc10)TMath::Ceil));

        TFormulaPrimitive::AddFormula(new TFormulaPrimitive("nint",   "nint",   (TFormulaPrimitive::GenFunc10)MFastFun::Nint));
        TFormulaPrimitive::AddFormula(new TFormulaPrimitive("round",  "round",  (TFormulaPrimitive::GenFunc10)MFastFun::Nint));
        TFormulaPrimitive::AddFormula(new TFormulaPrimitive("sgn",    "sgn",    (TFormulaPrimitive::GenFunc10)MFastFun::Sign));

        TFormulaPrimitive::AddFormula(new TFormulaPrimitive("isnan",  "isnan",  (TFormulaPrimitive::GenFunc10)MFastFun::IsNaN));
        TFormulaPrimitive::AddFormula(new TFormulaPrimitive("finite", "finite", (TFormulaPrimitive::GenFunc10)MFastFun::Finite));
    }

    //    TFormulaPrimitive is used to get direct acces to the function pointers
    //    GenFunc     -  pointers  to the static function
    //    TFunc       -  pointers  to the data member functions
    //
    //    The following sufixes are currently used, to describe function arguments:
    //    ------------------------------------------------------------------------
    //    G     - generic layout - pointer to double (arguments), pointer to double (parameters)
    //    10    - double
    //    110   - double, double
    //    1110  - double, double, double

    fName  = name  ? name  : "MDataPhrase";
    fTitle = title ? title : rule;

    fMembers.SetOwner();

    if (rule)
        SetRule(rule);
}

// --------------------------------------------------------------------------
//
//   Destructor
//
MDataPhrase::~MDataPhrase()
{
    if (fFormula)
        delete fFormula;
}

// --------------------------------------------------------------------------
//
//   CopyConstructor
//
MDataPhrase::MDataPhrase(MDataPhrase &ts)
{
    TFormula *f = ts.fFormula;

    fName  = "MDataPhrase";
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
//
Double_t MDataPhrase::Eval(const Double_t *x) const
{
    const Int_t n = fMembers.GetEntriesFast();

    if (fMembers.GetSize()<n)
    {
        *fLog << err << "ERROR - MDataPhrase::GetValue: Size mismatch!" << endl;
        return 0;
    }

    // This is to get rid of the cost-qualifier for this->fStorage
    Double_t *arr = fStorage.GetArray();

    // Evaluate parameters (the access with TObjArray::UncheckedAt is
    // roughly two times faster than with a TIter and rougly three
    // times than accessing a TOrdCollection. However this might still
    // be quite harmless compared to the time needed by GetValue)
    for (Int_t i=0; i<n; i++)
        arr[i] = static_cast<MData*>(fMembers.UncheckedAt(i))->GetValue();

    // Evaluate function
    return fFormula->EvalPar(x, arr);
}

// --------------------------------------------------------------------------
//
// Returns kTRUE if all members of fMemebers are valid and fFormula!=NULL
//
Bool_t MDataPhrase::IsValid() const
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
Bool_t MDataPhrase::IsReadyToSave() const
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
Bool_t MDataPhrase::PreProcess(const MParList *plist)
{
    if (!fFormula)
    {
        *fLog << err << "Error - So far no valid phrase was setup." << endl;
        return kFALSE;
    }

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

    // For speed reasons MArrayD instead of TArrayD is used
    // (no range check is done when accessing). The storage is
    // allocated (Set) only when its size is changing. If GetValue
    // is called many times this should improve the speed significantly
    // because the necessary memory is already allocated and doesn't need
    // to be freed. (Just a static variable is not enough, because there
    // may be several independant objects of this class)
    fStorage.Set(fMembers.GetSize());

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Builds a rule from all the list members. This is a rule which could
// be used to rebuild the list using the constructor of a MDataChain
//
TString MDataPhrase::GetRule() const
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

// --------------------------------------------------------------------------
//
// This returns the rule as seen/interpreted by the TFormula. Mainly
// for debugging purposes
//
TString MDataPhrase::GetRuleRaw() const
{
    if (!fFormula)
        return "<empty>";

    return fFormula->GetExpFormula();
}

// --------------------------------------------------------------------------
//
// Return the value converted to a Bool_t
//
Bool_t MDataPhrase::GetBool() const
{
    return TMath::Nint(GetValue())!=0;
}

/*
// --------------------------------------------------------------------------
//
// Return a comma seperated list of all data members used in the chain.
// This is mainly used in MTask::AddToBranchList
//
TString MDataPhrase::GetDataMember() const
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
void MDataPhrase::SetVariables(const TArrayD &arr)
{
    fMembers.R__FOR_EACH(MData, SetVariables)(arr);
}

// --------------------------------------------------------------------------
//
// Check for corresponding entries in resource file and setup data phrase.
//
// Assuming your MDataChain is called (Set/GetName): MyData
//
// Now setup the condition, eg:
//     MyData.Rule: log10(MHillas.fSize)
// or
//     MyData.Rule: log10(MHillas.fSize) - 4.1
//
// If you want to use more difficult rules you can split the
// condition into subrules. Subrules are identified
// by {}-brackets. Avoid trailing 0's! For example:
//
//     MyData.Rule: log10(MHillas.fSize) + {0} - {1}
//     MyData.0: 5.5*MHillas.fSize
//     MyData.1: 2.3*log10(MHillas.fSize)
//
// The numbering must be continous and start with 0. You can use
// a subrules more than once. All {}-brackets are simply replaced
// by the corresponding conditions. The rules how conditions can
// be written can be found in the class description of MDataChain.
//
Int_t MDataPhrase::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (!IsEnvDefined(env, prefix, "Rule", print))
        return rc;

    TString rule = GetEnvValue(env, prefix, "Rule", "");

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

    if (rule.IsNull())
    {
        *fLog << warn << "MDataPhrase::ReadEnv - ERROR: Empty rule found." << endl;
        return kERROR;
    }

    if (!SetRule(rule))
        return kERROR;

    if (print)
        *fLog << inf << "found: " << GetRule() << endl;

    return kTRUE;
}
