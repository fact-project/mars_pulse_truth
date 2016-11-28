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
!   Author(s): Markus Meyer 04/2005 <mailto:meyer@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMuonSetup
//
// Storage Container for setup parameter for the muon analysis
//
// - margin
// - ThresholdArcWidth
// - ThresholdArcPhi
//
/////////////////////////////////////////////////////////////////////////////
#include "MMuonSetup.h"

#include <fstream>

#include "MLog.h"

using namespace std;

ClassImp(MMuonSetup);

// --------------------------------------------------------------------------
//
// Default constructor.
//
MMuonSetup::MMuonSetup(const char *name, const char *title)
    : fMargin(0.2), fThresholdArcPhi(5), fThresholdArcWidth(2)
{
    fName  = name  ? name  : "MMuonSetup";
    fTitle = title ? title : "Muon calibration setup parameters";
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MMuonSetup.Margin:            0.2
//   MMuonSetup.ThresholdArcPhi:   30
//   MMuonSetup.ThresholdArcWidth: 2
//
Int_t MMuonSetup::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "Margin", print))
    {
        rc = kTRUE;
        fMargin = GetEnvValue(env, prefix, "Margin", fMargin);
    }
    if (IsEnvDefined(env, prefix, "ThresholdArcPhi", print))
    {
        rc = kTRUE;
        fThresholdArcPhi = GetEnvValue(env, prefix, "ThresholdArcPhi", fThresholdArcPhi);
    }
    if (IsEnvDefined(env, prefix, "ThresholdArcWidth", print))
    {
        rc = kTRUE;
        fThresholdArcWidth = GetEnvValue(env, prefix, "ThresholdArcWidth", fThresholdArcWidth);
    }

    return rc;
}
