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
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MDataChain
// ==========
//
// With this chain you can concatenate simple mathematical operations on
// members of mars containers.
//
//
// Rules
// -----
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
//
// You can also use parantheses:
//   "HillasDource.fDist / (MHillas.fLength + MHillas.fWidth)"
//
//
// Operators
// ---------
//
// The allowed operations are: +, -, *, /, %, ^ and ** for ^
//
// While a%b returns the floating point reminder of a/b.
// While a^b returns a to the power of b
//
// Priority
// --------
//
//  The priority of the operators is build in as:
//    ^ (highest),  %, *, /, +, -
//
//  Priorities are evaluated before parsing the rule by inserting
//  artificial paranthesis. If you find ANY problem with this please
//  report it immediatly! It is highly recommended to check the result!
//
// You can also use mathmatical operators, eg:
//   "5*log10(MMcEvt.fEnergy*MHillas.fSize)"
//
// The allowed operators are:
//   exp(x)    e^x
//   log(x)    natural logarithm of x
//   pow10(x)  10^x
//   log2(x)   logarithm of x to base two
//   log10(x)  logarithm of x to base ten
//   cos(x)    cosine of x
//   sin(x)    sine of x
//   tan(x)    tangent of x
//   cosh(x)   hyperbolic cosine of x
//   sinh(x)   hyperbolic sine of x
//   tanh(x)   hyperbolic tangent of x
//   acosh(x)  arc hyperbolic cosine of x
//   asinh(x)  arc hyperbolic sine of x
//   atanh(x)  arc hyperbolic tangent of x
//   acos(x)   arc cosine (inverse cosine) of x
//   asin(x)   arc sine (inverse sine) of x
//   atan(x)   arc tangent (inverse tangent) of x
//   sqrt(x)   square root of x
//   sqr(x)    square of x
//   abs(x)    absolute value of x, |x|
//   floor(x)  round down to the nearest integer (floor(9.9)=9)
//   ceil(x)   round up   to the nearest integer (floor(9.1)=10)
//   round(x)  round      to the nearest integer
//   r2d(x)    transform radians to degrees
//   d2r(x)    transform degrees to radians
//   rand(x)   returns a uniform deviate on the interval ( 0, x ].
//             (gRandom->Uniform(x) is returned)
//   randp(x)  returns gRandom->Poisson(x)
//   rande(x)  returns gRandom->Exp(x)
//   randi(x)  returns gRandom->Integer(x)
//   randg(x)  returns gRandom->Gaus(0, x)
//   randl(x)  returns gRandom->Landau(0, x)
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
// Constants are implemented in ParseDataMember, namely:
//   kPi:       TMath::Pi()
//   kRad2Deg:  180/kPi
//   kDeg2Rad:  kPi/180
//
// You can also defined constants which are defined in TMath by:
//   kLn10       for   static Double_t TMath::Ln10();
//   kLogE       for   static Double_t TMath::LogE();
//   kRadToDeg   for   static Double_t TMath::RadToDeg();
//   kDegToRad   for   static Double_t TMath::DegToRad();
//   ...
//
// Remark:
//  In older root versions only Pi() and E() are implemented
//  in TMath.
//
//
// Variable Parameters
// ------------------------
// If you want to use variables, eg for fits you can use [0], [1], ...
// These values are initialized with 0 and set by calling
// SetVariables().
//
//
// Multi-argument functions
// ------------------------
// You can use multi-argument functions, too. The access is implemented
// via TFormula, which slows down thing a lot. If you can avoid usage
// of such expression you accelerate the access a lot. Example:
//   "TMath::Hypot(MHillas.fMeanX, MHillas.MeanY)"
//   "pow(MHillas.fMeanX*MHillas.MeanY, -1.2)"
// It should be possible to use all functions which are accessible
// via the root-dictionary.
//
//
// REMARK:
//  - All the random functions are returning 0 if gRandom==0
//  - You may get better results if you are using a TRandom3
//
// To Do:
//  - The possibility to use other objects inheriting from MData
//    is missing.
//  - By automatic pre-adding parantheses to the rule it would be possible
//    to implement priorities for operators.
//
/////////////////////////////////////////////////////////////////////////////

#include "MDataChain.h"

#include <ctype.h>        // isalnum, ...
#include <stdlib.h>       // strtod, ...

#include <TMath.h>
#include <TClass.h>
#include <TRegexp.h>
#include <TRandom.h>
#include <TMethodCall.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MDataList.h"
#include "MDataValue.h"
#include "MDataMember.h"
#include "MDataFormula.h"
#include "MDataElement.h"

ClassImp(MDataChain);

using namespace std;

// --------------------------------------------------------------------------
//
//  Constructor which takes a rule and a surrounding operator as argument
//
MDataChain::MDataChain(const char *rule, OperatorType_t op)
    : fOperatorType(op)
{
    fName  = "MDataChain";
    fTitle = rule;

    fMember=ParseString(rule);
}

// --------------------------------------------------------------------------
//
// Constructor taking a rule as an argument. For more details see
// class description
//
MDataChain::MDataChain(const char *rule, const char *name, const char *title)
    : fMember(NULL), fOperatorType(kENoop)
{
    fName  = name  ? name  : "MDataChain";
    fTitle = title ? title : rule;

    if (TString(rule).IsNull())
        return;

    *fLog << inf << "Parsing rule... " << flush;
    if (!(fMember=ParseString(rule)))
    {
        *fLog << err << dbginf << "Parsing '" << rule << "' failed." << endl;
        return;
    }
    *fLog << inf << "found: " << GetRule() << endl;
}

// --------------------------------------------------------------------------
//
// PreProcesses all members in the list
//
Bool_t MDataChain::PreProcess(const MParList *pList)
{
    return fMember ? fMember->PreProcess(pList) : kFALSE;
}

// --------------------------------------------------------------------------
//
// Checks whether at least one member has the ready-to-save flag.
//
Bool_t MDataChain::IsReadyToSave() const
{
    *fLog << all << "fM=" << fMember << "/" << (int)fMember->IsReadyToSave() << " " << endl;
    return fMember ? fMember->IsReadyToSave() : kFALSE;
}

// --------------------------------------------------------------------------
//
// Destructor. Delete filters.
//
MDataChain::~MDataChain()
{
    if (fMember)
        delete fMember;
}

// --------------------------------------------------------------------------
//
// Returns the number of alphanumeric characters (including '.' and ';')
// in the given string
//
Int_t MDataChain::IsAlNum(TString txt)
{
    int l = txt.Length();
    for (int i=0; i<l; i++)
    {
        if (!isalnum(txt[i]) && txt[i]!='.' && txt[i]!=':' && txt[i]!=';' &&
	    /*txt[i]!='['&&txt[i]!=']'&&*/
            ((txt[i]!='-' && txt[i]!='+') || i!=0))
            return i;
    }

    return l;
}

Int_t MDataChain::GetBracket(TString txt, char open, char close)
{
    Int_t first=1;
    for (int cnt=0; first<txt.Length(); first++)
    {
        if (txt[first]==open)
            cnt++;
        if (txt[first]==close)
            cnt--;
        if (cnt==-1)
            break;
    }
    return first;
}

// --------------------------------------------------------------------------
//
// Compare a given text with the known operators. If no operator match
// kENoop is retunred, otherwise the corresponding OperatorType_t
//
MDataChain::OperatorType_t MDataChain::ParseOperator(TString txt) const
{
    txt.ToLower();

    if (txt=="abs")    return kEAbs;
    if (txt=="fabs")   return kEAbs;
    if (txt=="log")    return kELog;
    if (txt=="log2")   return kELog2;
    if (txt=="log10")  return kELog10;
    if (txt=="sin")    return kESin;
    if (txt=="cos")    return kECos;
    if (txt=="tan")    return kETan;
    if (txt=="sinh")   return kESinH;
    if (txt=="cosh")   return kECosH;
    if (txt=="tanh")   return kETanH;
    if (txt=="asin")   return kEASin;
    if (txt=="acos")   return kEACos;
    if (txt=="atan")   return kEATan;
    if (txt=="asinh")  return kEASinH;
    if (txt=="acosh")  return kEACosH;
    if (txt=="atanh")  return kEATanH;
    if (txt=="sqrt")   return kESqrt;
    if (txt=="sqr")    return kESqr;
    if (txt=="exp")    return kEExp;
    if (txt=="pow10")  return kEPow10;
    if (txt=="sgn")    return kESgn;
    if (txt=="floor")  return kEFloor;
    if (txt=="r2d")    return kERad2Deg;
    if (txt=="d2r")    return kEDeg2Rad;
    if (txt=="rand")   return kERandom;
    if (txt=="randp")  return kERandomP;
    if (txt=="rande")  return kERandomE;
    if (txt=="randi")  return kERandomI;
    if (txt=="randg")  return kERandomG;
    if (txt=="randl")  return kERandomL;
    if (txt=="isnan")  return kEIsNaN;
    if (txt=="finite") return kEFinite;
    if (txt[0]=='-')   return kENegative;
    if (txt[0]=='+')   return kEPositive;

    return kENoop;
}

// --------------------------------------------------------------------------
//
// Here the names of data members are interpreted. This can be used to
// check for constants.
//
MData *MDataChain::ParseDataMember(TString txt)
{
    //txt.ToLower();

    if (txt=="kRad2Deg") return new MDataValue(kRad2Deg);
    if (txt=="kDeg2Rad") return new MDataValue(1./kRad2Deg);

    if (!txt.BeginsWith("k"))
        return new MDataMember(txt.Data());

    const TString name = txt(1, txt.Length());
#if ROOT_VERSION_CODE < ROOT_VERSION(4,01,00)
    TMethodCall call(TMath::Class(), name, "");
#else
    static TClass *const tmath = TClass::GetClass("TMath");
    TMethodCall call(tmath, name, "");
#endif

    switch (call.ReturnType())
    {
    case TMethodCall::kLong:
        Long_t l;
        call.Execute(l);
        return new MDataValue(l);
    case TMethodCall::kDouble:
        Double_t d;
        call.Execute(d);
        return new MDataValue(d);
    default:
        break;
    }

    return new MDataMember(txt.Data());
}

// --------------------------------------------------------------------------
//
// Remove all whitespaces
// Replace all kind of double +/- by a single + or -
//
void MDataChain::SimplifyString(TString &txt) const
{
    txt.ReplaceAll(" ",  "");
    txt.ReplaceAll("**", "^");

    while (txt.Contains("--") || txt.Contains("++") ||
           txt.Contains("+-") || txt.Contains("-+"))
    {
        txt.ReplaceAll("--", "+");
        txt.ReplaceAll("++", "+");
        txt.ReplaceAll("-+", "-");
        txt.ReplaceAll("+-", "-");
    }
}

// --------------------------------------------------------------------------
//
// Add Parenthesis depending on the priority of the operators. The
// priorities are defined by the default of the second argument.
//
void MDataChain::AddParenthesis(TString &test, const TString op) const
{
    // Signiture of an exponential number 1e12
    static const TRegexp rexp("[.0123456789]e[+-][0123456789]");

    const TString low = op(1, op.Length());
    if (low.Length()==0)
        return;

    int offset = 0;
    while (1)
    {
        const TString check = test(offset, test.Length());
        if (check.IsNull())
            break;

        const Ssiz_t pos = check.First(op[0]);
        if (pos<0)
            break;

        int cnt=0;

        int i;
        for (i=pos-1; i>=0; i--)
        {
            if (check[i]==')')
                cnt++;
            if (check[i]=='(')
                cnt--;
            if (cnt>0 || low.First(check[i])<0)
                continue;

            // Check if operator belongs to an exponential number
            const TString sub(check(i-2, 4));
            if (!sub(rexp).IsNull())
                continue;
            break;
        }

        cnt=0;
        int j;
        for (j=pos; j<check.Length(); j++)
        {
            if (check[j]=='(')
                cnt++;
            if (check[j]==')')
                cnt--;
            if (cnt>0 || low.First(check[j])<0)
                continue;

            // Check if operator belongs to an exponential number
            const TString sub(check(j-2, 4));
            if (!sub(rexp).IsNull())
                continue;
            break;
        }

        const TString sub = test(offset+i+1, j-i-1);

        // Check if it contains operators,
        // otherwise we can simply skip it
        if (sub.First("%^/*+-")>=0)
        {
            test.Insert(offset+j,   ")");
            test.Insert(offset+i+1, "(");
            offset += 2;
        }
        offset += j+1;
    }

    AddParenthesis(test, low);
}

// --------------------------------------------------------------------------
//
// Core of the data chain. Here the chain is constructed out of the rule.
//
MData *MDataChain::ParseString(TString txt, Int_t level)
{
    if (level==0)
    {
        SimplifyString(txt);
        AddParenthesis(txt);
    }

    MData *member0=NULL;

    char type=0;
    int nlist = 0;

    while (!txt.IsNull())
    {
        MData *newmember = NULL;

        txt = txt.Strip(TString::kBoth);

        switch (txt[0])
        {
        case '(':
            {
                //
                // Search for the corresponding parantheses
                //
                const Int_t first=GetBracket(txt, '(', ')');
                if (first==txt.Length())
                {
                    *fLog << err << dbginf << "Syntax Error: ')' missing." << endl;
                    if (member0)
                        delete member0;
                    return NULL;
                }

                //
                // Make a copy of the 'interieur' and delete the substring
                // including the brackets
                //
                TString sub = txt(1, first-1);
                txt.Remove(0, first+1);

                //
                // Parse the substring
                //
                newmember = ParseString(sub, level+1);
                if (!newmember)
                {
                    *fLog << err << dbginf << "Parsing '" << sub << "' failed." << endl;
                    if (member0)
                        delete member0;
                    return NULL;
                }
            }
            break;

        case ')':
            *fLog << err << dbginf << "Syntax Error: Too many ')'" << endl;
            if (member0)
                delete member0;
            return NULL;

        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '^':
            if (member0)
            {
                //
                // Check for the type of the symbol
                //
                char is = txt[0];
                txt.Remove(0, 1);

                //
                // If no filter is available or the available filter
                // is of a different symbols we have to create a new
                // data list with the new symbol
                //
                if (/*!member0->InheritsFrom(MDataMember::Class()) ||*/ type!=is)
                {
                    MDataList *list = new MDataList(is);
                    list->SetName(Form("List_%c_%d", is, 10*level+nlist++));

                    list->SetOwner();
                    list->AddToList(member0);

                    member0 = list;

                    type = is;
                }
                continue;
            }

            if (txt[0]!='-' && txt[0]!='+')
            {
                *fLog << err << dbginf << "Syntax Error: First argument of '";
                *fLog << txt[0] << "' opartor missing." << endl;
                if (member0)
                    delete member0;
                return NULL;
            }

            // FALLTHROU

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '[':
        case ']':
            if ((txt[0]!='-' && txt[0]!='+') || isdigit(txt[1]) || txt[1]=='.')
            {
                if (!txt.IsNull() && txt[0]=='[')
                {
                    Int_t first = GetBracket(txt, '[', ']');
                    TString op  = txt(1, first-1);
                    txt.Remove(0, first+1);

                    newmember = new MDataValue(0, atoi(op));
                    break;
                }

                char *end;
                Double_t num = strtod(txt.Data(), &end);
                if (!end || txt.Data()==end)
                {
                    *fLog << err << dbginf << "Error trying to convert '" << txt << "' to value." << endl;
                    if (member0)
                        delete member0;
                    return NULL;
                }

                txt.Remove(0, end-txt.Data());

                newmember = new MDataValue(num);
                break;
            }

            // FALLTHROUH

        default:
            int i = IsAlNum(txt);

            if (i==0)
            {
                *fLog << err << dbginf << "Syntax Error: Name of data member missing in '" << txt << "'" << endl;
                if (member0)
                    delete member0;
                return NULL;
            }

            TString text = txt(0, i);

            txt.Remove(0, i);
            txt = txt.Strip(TString::kBoth);

            if (!txt.IsNull() && txt[0]=='[')
            {
                Int_t first = GetBracket(txt, '[', ']');
                TString op  = txt(1, first-1);
                txt.Remove(0, first+1);

                newmember = new MDataElement(text, atoi(op));
                break;
            }
            if ((txt.IsNull() || txt[0]!='(') && text[0]!='-' && text[0]!='+')
            {
                newmember = ParseDataMember(text.Data());
                break;
            }

            OperatorType_t op = ParseOperator(text);

            Int_t first = GetBracket(txt, '(', ')');
            TString sub = op==kENegative || op==kEPositive ? text.Remove(0,1) + txt : txt(1, first-1);
            txt.Remove(0, first+1);

            if (op==kENoop)
            {
                newmember = new MDataFormula(Form("%s(%s)", (const char*)text, (const char*)sub));
                if (newmember->IsValid())
                    break;

                *fLog << err << dbginf << "Syntax Error: Operator '" << text << "' unknown." << endl;
                if (member0)
                    delete member0;
                return NULL;
            }

            newmember = new MDataChain(sub, op);
            if (!newmember->IsValid())
            {
                *fLog << err << dbginf << "Syntax Error: Error parsing contents '" << sub << "' of operator " << text << endl;
                delete newmember;
                if (member0)
                    delete member0;
                return NULL;
            }
        }

        if (!member0)
        {
            member0 = newmember;
            continue;
        }

        if (!member0->InheritsFrom(MDataList::Class()))
            continue;

        ((MDataList*)member0)->AddToList(newmember);
    }

    return member0;
}

// --------------------------------------------------------------------------
//
// Returns the value described by the rule
//
Double_t MDataChain::GetValue() const
{
    if (!fMember)
    {
        //*fLog << warn << "MDataChain not valid." << endl;
        return 0;
    }

    const Double_t val = fMember->GetValue();

    switch (fOperatorType)
    {
    case kEAbs:      return TMath::Abs(val);
    case kELog:      return TMath::Log(val);
    case kELog2:     return TMath::Log2(val);
    case kELog10:    return TMath::Log10(val);
    case kESin:      return TMath::Sin(val);
    case kECos:      return TMath::Cos(val);
    case kETan:      return TMath::Tan(val);
    case kESinH:     return TMath::SinH(val);
    case kECosH:     return TMath::CosH(val);
    case kETanH:     return TMath::TanH(val);
    case kEASin:     return TMath::ASin(val);
    case kEACos:     return TMath::ACos(val);
    case kEATan:     return TMath::ATan(val);
    case kEASinH:    return TMath::ASinH(val);
    case kEACosH:    return TMath::ACosH(val);
    case kEATanH:    return TMath::ATanH(val);
    case kESqrt:     return TMath::Sqrt(val);
    case kESqr:      return val*val;
    case kEExp:      return TMath::Exp(val);
    case kEPow10:    return TMath::Power(10, val);
    case kESgn:      return val<0 ? -1 : 1;
    case kENegative: return -val;
    case kEPositive: return val;
    case kEFloor:    return TMath::Floor(val);
    case kECeil:     return TMath::Ceil(val);
    case kERound:    return TMath::Nint(val);
    case kERad2Deg:  return val*180/TMath::Pi();
    case kEDeg2Rad:  return val*TMath::Pi()/180;
    case kERandom:   return gRandom ? gRandom->Uniform(val)      : 0;
    case kERandomP:  return gRandom ? gRandom->Poisson(val)      : 0;
    case kERandomE:  return gRandom ? gRandom->Exp(val)          : 0;
    case kERandomI:  return gRandom ? gRandom->Integer((int)val) : 0;
    case kERandomG:  return gRandom ? gRandom->Gaus(0, val)   : 0;
    case kERandomL:  return gRandom ? gRandom->Landau(0, val) : 0;
    case kEIsNaN:    return TMath::IsNaN(val);
    case kEFinite:   return TMath::Finite(val);
    case kENoop:     return val;
    }

    *fLog << warn << "No Case for " << fOperatorType << " available." << endl;

    return 0;
}

// --------------------------------------------------------------------------
//
// Builds a rule from all the chain members. This is a rule which could
// be used to rebuild the chain.
//
TString MDataChain::GetRule() const
{
    if (!fMember)
        return "<n/a>";

    TString str;

    Bool_t bracket = fOperatorType!=kENoop && !fMember->InheritsFrom(MDataList::Class());

    switch (fOperatorType)
    {
    case kEAbs:      str += "abs"   ; break;
    case kELog:      str += "log"   ; break;
    case kELog2:     str += "log2"  ; break;
    case kELog10:    str += "log10" ; break;
    case kESin:      str += "sin"   ; break;
    case kECos:      str += "cos"   ; break;
    case kETan:      str += "tan"   ; break;
    case kESinH:     str += "sinh"  ; break;
    case kECosH:     str += "cosh"  ; break;
    case kETanH:     str += "tanh"  ; break;
    case kEASin:     str += "asin"  ; break;
    case kEACos:     str += "acos"  ; break;
    case kEATan:     str += "atan"  ; break;
    case kEASinH:    str += "asinh" ; break;
    case kEACosH:    str += "acosh" ; break;
    case kEATanH:    str += "atanh" ; break;
    case kESqrt:     str += "sqrt"  ; break;
    case kESqr:      str += "sqr"   ; break;
    case kEExp:      str += "exp"   ; break;
    case kEPow10:    str += "pow10" ; break;
    case kESgn:      str += "sgn"   ; break;
    case kENegative: str += "-"     ; break;
    case kEPositive: str += "+"     ; break;
    case kEFloor:    str += "floor" ; break;
    case kECeil:     str += "ceil"  ; break;
    case kERound:    str += "round" ; break;
    case kERad2Deg:  str += "r2d"   ; break;
    case kEDeg2Rad:  str += "d2r"   ; break;
    case kERandom:   str += "rand"  ; break;
    case kERandomP:  str += "randp" ; break;
    case kERandomE:  str += "rande" ; break;
    case kERandomI:  str += "randi" ; break;
    case kERandomG:  str += "randg" ; break;
    case kERandomL:  str += "randl" ; break;
    case kEIsNaN:    str += "isnan" ; break;
    case kEFinite:   str += "finite"; break;
    case kENoop:
        break;
    }

    if (bracket)
        str += "(";

    str += fMember->GetRule();

    if (bracket)
        str += ")";

    return str;
}

// --------------------------------------------------------------------------
//
// Return a comma seperated list of all data members used in the chain.
// This is mainly used in MTask::AddToBranchList
//
TString MDataChain::GetDataMember() const
{
    return fMember ? fMember->GetDataMember() : TString();
}

void MDataChain::SetVariables(const TArrayD &arr)
{
    if (fMember)
        fMember->SetVariables(arr);
}

// --------------------------------------------------------------------------
//
// Check for corresponding entries in resource file and setup data chain.
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
Int_t MDataChain::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (!IsEnvDefined(env, prefix, "Rule", print))
        return rc;

    TString rule = GetEnvValue(env, prefix, "Rule", "");
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

    if (fMember)
    {
        delete fMember;
        fMember = 0;
    }

    if (rule.IsNull())
    {
        *fLog << warn << "MDataChain::ReadEnv - ERROR: Empty rule found." << endl;
        return kERROR;
    }

    if (!(fMember=ParseString(rule)))
    {
        *fLog << err << "MDataChain::ReadEnv - ERROR: Parsing '" << rule << "' failed." << endl;
        return kERROR;
    }

    if (print)
    {
        *fLog << inf << "found: ";
        fMember->Print();
        *fLog << endl;
    }

    return kTRUE;
}
/*
void MDataChain::ReconstructElements()
{
    if (!fMember)
        return;

    if (fMember->InheritsFrom(MDataElement::Class()))
    {
        MData *data = ((MDataElement*)fMember)->CloneColumn();
        delete fMember;
        fMember = data;
    }

    fMember->ReconstructElements();
}
*/
