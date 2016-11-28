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
!   Author(s): Antonio Stamerra  02/2003 <mailto:antonio.stamerra@pi.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   MFEnergySlope                                                         //
//                                                                         //
//  A filter to select MC events (generated with a energy slope MCSlope)   //
//   with a different energy slope NewSlope set by the user.               //
//                                                                         //
//  The new slope is set by the user with SetSlope().                      //
//  Only negative slopes are admitted; positive ones are internally        //
//   converted.                                                            //
//  Events are selected following the new energy power slope, and the      //
//   sample is normalized to the number of events at:                      //
//    1. McMaxEnergy, if abs(NewSlope) < abs(McSlope);                     //
//    2. McMinEnergy, if abs(NewSlope) > abs(McSlope);                     //
//   Mc{Max,Min}Energy are set with SetMcMinEnergy() and SetMcMaxEnergy(); //
//    with GeV values.                                                     //
//  Default values are the min/max energy of the MC sample.                //
//                                                                         //
//  With this filter the statistics of the MC sample is reduced.           //
//  camera ver.0.6 and reflector ver.0.6 are required to fetch             //
//   the correct MC information                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MFEnergySlope.h"

#include <fstream>

#include <TMath.h>
#include <TRandom.h>

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MMcEvt.hxx"
#include "MMcCorsikaRunHeader.h"

ClassImp(MFEnergySlope);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor
//
MFEnergySlope::MFEnergySlope(const char *name, const char *title):
    fNewSlope(-1), fMcMinEnergy(-1.), fMcMaxEnergy(-1.)
{
    fName  = name  ? name  : "MFEnergySlope";
    fTitle = title ? title : "Filter to select energy with given slope";
}

// --------------------------------------------------------------------------
//
//     Constructor
//
MFEnergySlope::MFEnergySlope(Float_t slope, const char *name, const char *title):
    fNewSlope(TMath::Abs(slope)), fMcMinEnergy(-1.), fMcMaxEnergy(-1.)
{
    fName  = name  ? name  : "MFEnergySlope";
    fTitle = title ? title : "Filter to select energy with given slope";
}

// --------------------------------------------------------------------------
//
//     Constructor
//
MFEnergySlope::MFEnergySlope(Float_t slope, Float_t emin, const char *name, const char *title):
    fNewSlope(TMath::Abs(slope)), fMcMinEnergy(emin), fMcMaxEnergy(-1.)
{
    fName  = name  ? name  : "MFEnergySlope";
    fTitle = title ? title : "Filter to select energy with given slope";
}

// --------------------------------------------------------------------------
//
//   Preprocess
//  
//  MC slope and min/max energy are read
//  Normalization factor is computed
//
Int_t MFEnergySlope::PreProcess(MParList *pList)
{
    if (fNewSlope<0)
    {
        *fLog << err << "New slope still undefined... aborting." << endl;
        return kFALSE;
    }

    fEvt = (MMcEvt*)pList->FindObject("MMcEvt");
    if (!fEvt)
    {
        *fLog << err << "MMcEvt not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//   Preprocess
//  
//  MC slope and min/max energy are read
//  Normalization factor is computed
//
Bool_t MFEnergySlope::ReInit(MParList *pList)
{
    MMcCorsikaRunHeader *runheader = (MMcCorsikaRunHeader*)pList->FindObject("MMcCorsikaRunHeader");
    if (!runheader)
    {
        *fLog << err << "MMcCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    //
    // Read info from0 the MC sample (it must be generated with
    //   reflector ver.0.6 and camera ver. 0.6)
    //
    fMcSlope = runheader->GetSlopeSpec();
    if (fMcMinEnergy<0)
        fMcMinEnergy = runheader->GetELowLim();
    if (fMcMinEnergy<0)
        fMcMaxEnergy = runheader->GetEUppLim();

    // Slope is used with positive values in the code
    if (fNewSlope < 0)
        fNewSlope *= -1;
    if (fMcSlope < 0)
        fMcSlope *= -1;

    // Calculate normalization factor
    fN0 = TMath::Power(fNewSlope>fMcSlope ? fMcMinEnergy : fMcMaxEnergy, fNewSlope-fMcSlope);

    // Print some infos
    *fLog << inf;
    *fLog << "Fetched MC info:" << endl;
    *fLog << "  Change E Slope from " << -fMcSlope << " (" << fMcMinEnergy << " < E < ";
    *fLog << fMcMaxEnergy << ") to " << -fNewSlope << endl;
    *fLog << "  Norm factor: " << fN0 << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//  
//  Select events randomly according to the MC ("old") and required ("new") 
//    energy slope.
//  Old E slope is fMcSlope
//  New E slope is set by the user (fval; fNewSlope)
//  If old and new energy slope are the same skip the selection. 
//  The MC energy slope and lower and upper limits are taken from the
//  run header (requires reflector ver.>0.6 and camera ver.>0.6) 
//
Int_t MFEnergySlope::Process()
{
    fResult = kTRUE;

    // Energy slopes are the same: skip it
    if (fNewSlope == fMcSlope)
        return kTRUE;

    //  The value of the normalized spectrum is compared with a
    //   random value in [0,1];
    //   if the latter is higher the event is accepted
    const Float_t energy = fEvt->GetEnergy();

    /*
     //
     // If energy bounds different from MC ones are selected, then
     // events outside these bounds are rejected, as their slope has
     // not been changed.
     //
     if (energy > fMcMaxEnergy || energy < fMcMinEnergy)
     {
        fResult = kFALSE;
        return kTRUE;
     }
     */

    const Float_t Nexp = fN0 * pow(energy, fMcSlope-fNewSlope);
    const Float_t Nrnd = gRandom->Uniform();

    fResult = Nexp > Nrnd;

    return kTRUE;
}


// --------------------------------------------------------------------------
//
//   MFEnergySlope.NewSlope: -2.8
//
Int_t MFEnergySlope::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NewSlope", print))
    {
        rc = kTRUE;
        SetNewSlope(GetEnvValue(env, prefix, "NewSlope", fNewSlope));
    }
    return rc;
}
