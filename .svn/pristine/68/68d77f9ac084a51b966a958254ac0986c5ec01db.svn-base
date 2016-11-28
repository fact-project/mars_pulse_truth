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
!   Author(s): Thomas Bretz  6/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MParameterCalc
//
// Task to calculate a parameter stored ina MParametrD by a rule, eg:
//
// MParameterCalc est;
// est.SetRule("0.5 + (1.1*MHillas.fLength) + (2.2*MHillasSrc.fDist) + (3.3*MHillas.fSize) +"
//             "(4.4*MHillas.fSize*MHillas.fLength) + (5.5*MHillasSrc.fDist*MHillas.fLength)");
//
// For description of rules, see MDataPhrase.
//
// Output:
//   fNameParameter [MParameterD] <default=MParameterD>
//
/////////////////////////////////////////////////////////////////////////////
#include "MParameterCalc.h"

#include <TMath.h>

#include "MParList.h"

#include "MDataPhrase.h"
#include "MParameters.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MParameterCalc);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Initialize fData with default rule "MMcEvt.fEnergy"
//
MParameterCalc::MParameterCalc(const char *def, const char *name, const char *title)
    : fData(0), fParameter(0), fNameParameter("MParameterD")
{
    fName  = name  ? name  : "MParameterCalc";
    fTitle = title ? title : "Task to calculate a MParameterD";

    fData = new MDataPhrase(def);
}

// --------------------------------------------------------------------------
//
// delete fData
//
MParameterCalc::~MParameterCalc()
{
    delete fData;
}

// --------------------------------------------------------------------------
//
// Delete fData. Initialize a new MDataPhrase with rule.
// Returns if fData->IsValid()
//
Bool_t MParameterCalc::SetRule(const char *rule)
{
    delete fData;
    fData = new MDataPhrase(rule);

    return fData->IsValid();
}

// --------------------------------------------------------------------------
//
// Forwards SetVariables to fData to allow optimizations.
//
void MParameterCalc::SetVariables(const TArrayD &arr)
{
    fData->SetVariables(arr);
}

// --------------------------------------------------------------------------
//
// FindCreate "MEnergyEst"
// PreProcess fData.
//
Int_t MParameterCalc::PreProcess(MParList *plist)
{
    memset(fCounter, 0, sizeof(ULong_t)*2);

    fParameter = (MParameterD*)plist->FindCreateObj("MParameterD", fNameParameter);
    if (!fParameter)
        return kFALSE;

    *fLog << inf << "Rule for " << fNameParameter << ": " << fData->GetRule() << endl;

    if (!fData->PreProcess(plist))
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Get value from fData and set it to fEnergy. SetReadyToSave for fEnergy.
// Return kCONTINUE if value is NaN (Not a Number)
//
Int_t MParameterCalc::Process()
{
    const Double_t val = fData->GetValue();
    if (TMath::IsNaN(val))
    {
        fCounter[0]++;
        return kCONTINUE;
    }
    if (!TMath::Finite(val))
    {
        fCounter[1]++;
        return kCONTINUE;
    }

    fParameter->SetVal(val);
    fParameter->SetReadyToSave();
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print some execution statistics
//
Int_t MParameterCalc::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    PrintSkipped(fCounter[0], "Skipped due to NaN-result (Not a Number)");
    PrintSkipped(fCounter[1], "Skipped due to inf-result (infinite)");
    *fLog << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print the rule used for energy estimation
//
void MParameterCalc::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << ":";
    if (!fData)
        *fLog << " <n/a>" << endl;
    else
        *fLog << endl << fData->GetRule() << endl;
}

// --------------------------------------------------------------------------
//
// Check for corresponding entries in resource file and setup filters.
// Avoid trailing 0's!
//
// Example:
//   test.C:
//     MParameterCalc est("MyEstimator");
//
//   test.rc:
//     MyEstimator.Rule: {0} + {1}
//     MyEstimator.NameOutput: MParameterD
//     MyEstimator.0: log10(MHillas.fSize)
//     MyEstimator.1: 5.5
//
// For more details see MDataPhrase::ReadEnv
//
Int_t MParameterCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    MDataPhrase *f = new MDataPhrase;
    f->SetName(fName);

    const Bool_t rc = f->ReadEnv(env, prefix, print);
    if (rc!=kTRUE)
    {
        delete f;
        return rc;
    }

    delete fData;
    fData = f;

    if (!f->HasValidRule())
    {
        *fLog << err << "MParameterCalc::ReadEnv - ERROR: Inavlid rule from resource file." << endl;
        return kERROR;
    }

    if (IsEnvDefined(env, prefix, "NameOutput", print))
        fNameParameter = GetEnvValue(env, prefix, "NameOutput", fNameParameter);

    return kTRUE;
}
