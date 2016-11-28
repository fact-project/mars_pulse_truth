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
!   Author(s): Thomas Bretz 03/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MParameterD, MParameterDerr, MParameterI
//
// Storage Container for doubles and ints
//
// This classes can be used for intermidiate variables which we don't want
// to have in a special container.
//
//
// MParameterDerr - Version 2:
//  - inherits from MParemeterD now, where fVal comes from
//
/////////////////////////////////////////////////////////////////////////////
#include "MParameters.h"

#include <fstream>

#include "fits.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MParameterD);
ClassImp(MParameterI);
ClassImp(MParameterDerr);
//ClassImp(MParameter);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MParameterD::MParameterD(const char *name, const char *title)
{
    fName  = name  ? name  : "MParameterD";
    fTitle = title ? title : "Storgare container for general parameters (double)";
}

// --------------------------------------------------------------------------
//
// Default constructor.
//
MParameterDerr::MParameterDerr(const char *name, const char *title)
{
    fName  = name  ? name  : "MParameterDerr";
    fTitle = title ? title : "Storgare container for general parameters (double) and its error";
}

// --------------------------------------------------------------------------
//
// Default constructor.
//
MParameterI::MParameterI(const char *name, const char *title)
{
    fName  = name  ? name  : "MParameterI";
    fTitle = title ? title : "Storgare container for general parameters (integer)";
}

// --------------------------------------------------------------------------
//
// Print value of container.
//
void MParameterD::Print(Option_t *) const
{
    *fLog << all << GetDescriptor() << ":  Val=" << fVal << endl;
}

// --------------------------------------------------------------------------
//
// Print value and error of container.
//
void MParameterDerr::Print(Option_t *) const
{
    *fLog << all << GetDescriptor() << ":  Val=" << GetVal() << "  Err=" << fErr << endl;
}

// --------------------------------------------------------------------------
//
// Print value of container.
//
void MParameterI::Print(Option_t *) const
{
    *fLog << all << GetDescriptor() << ":  Val=" << fVal << endl;
}

// --------------------------------------------------------------------------
//
// MParameterD.Val: 55.7
//
Int_t MParameterD::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "Val", print))
    {
        rc = kTRUE;
        fVal = GetEnvValue(env, prefix, "Val", fVal);
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// MParameterD.Val: 55.7
// MParameterD.Err: 12.3
//
Int_t MParameterDerr::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Int_t rc = MParameterD::ReadEnv(env, prefix, print);
    if (rc==kERROR)
        return kERROR;

    if (IsEnvDefined(env, prefix, "Err", print))
    {
        rc = kTRUE;
        fErr = GetEnvValue(env, prefix, "Err", fErr);
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// MParameterD.Val: 42
//
Int_t MParameterI::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "Val", print))
    {
        rc = kTRUE;
        fVal = GetEnvValue(env, prefix, "Val", fVal);
    }
    return rc;
}

Bool_t MParameterD::SetupFits(fits &fin)
{
    return fin.SetRefAddress(Form("%s.fVal", fName.Data()), fVal);
}

Bool_t MParameterDerr::SetupFits(fits &fin)
{
    if (!fin.SetRefAddress(Form("%s.fErr", fName.Data()), fErr))
        return kFALSE;
    return MParameterD::SetupFits(fin);
}

Bool_t MParameterI::SetupFits(fits &fin)
{
    return fin.SetRefAddress(Form("%s.fVal", fName.Data()), fVal);
}
