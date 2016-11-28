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
!   Author(s): Thomas Bretz 3/2007 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFEvtNumber
//
/////////////////////////////////////////////////////////////////////////////
#include "MFEvtNumber.h"

#include <TFile.h>
#include <TTree.h>

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"

ClassImp(MFEvtNumber);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor
//
MFEvtNumber::MFEvtNumber(const char *name, const char *title):
    fFileName(""), fTreeName("Events"), fSelector("")
{
    fName  = name  ? name  : "MFEvtNumber";
    fTitle = title ? title : "Filter to select events by run- and evt-number";
}

// --------------------------------------------------------------------------
//
// return the event id. It is a binary compilation of run- and evt-number
//
ULong_t MFEvtNumber::GetEvtId() const
{
    return Compile(fRun->GetRunNumber(), fEvt->GetDAQEvtNumber());
}

// --------------------------------------------------------------------------
//
//   Preprocess
//  
//  MC slope and min/max energy are read
//  Normalization factor is computed
//
Int_t MFEvtNumber::PreProcess(MParList *plist)
{
    fResult = kTRUE;

    if (fFileName.IsNull())
    {
        *fLog << inf << "No input file given... skipped." << endl;
        return kSKIP;
    }

    fRun = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!fRun)
    {
        *fLog << err << "MRawRunHeader not found ... aborting." << endl;
        return kFALSE;
    }
    fEvt = (MRawEvtHeader*)plist->FindObject("MRawEvtHeader");
    if (!fEvt)
    {
        *fLog << err << "MRawEvtHeader not found ... aborting." << endl;
        return kFALSE;
    }

    TFile file(fFileName);
    if (file.IsZombie())
    {
        *fLog << err << "Cannot open file " << fFileName << "... aborting." << endl;
        return kFALSE;
    }

    TTree *t = dynamic_cast<TTree*>(file.Get(fTreeName));
    if (!t)
    {
        *fLog << err << "Tree " << fTreeName << " not found in file " << fFileName << "... aborting." << endl;
        return kFALSE;
    }

    t->SetEstimate(t->GetEntries());

    if (t->Draw("RunNumber.fVal:EvtNumber.fVal", fSelector, "goff")<0)
    {
        *fLog << err << "Could not retrieve event-list from tree " << fTreeName << " in file " << fFileName << " selecting " << fSelector << "... aborting." << endl;
        return kFALSE;
    }

    const Long64_t n = t->GetSelectedRows();

    if (n<=0)
    {
        *fLog << err << "Could not retrieve event-list from tree " << fTreeName << " in file " << fFileName << " selecting " << fSelector << "... aborting." << endl;
        return kFALSE;
    }

    const Double_t *v1 = t->GetV1();
    const Double_t *v2 = t->GetV2();


    *fLog << inf << "Found " << n << " events fulfilling " << fSelector << "." << endl;

    for (Long64_t i=0; i<n; i++)
        fList.Add(Compile((ULong64_t)v1[i], (ULong64_t)v2[i]), 1);

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
Int_t MFEvtNumber::Process()
{
    if (fFileName.IsNull())
        return kTRUE;

    fResult = fList.GetValue(GetEvtId()) ? kTRUE : kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//   MFEvtNumber.FileName: filename.root
//   MFEvtNumber.TreeName: Events
//   MFEvtNumber.Selector: ThetaSquared<0.04
//
Int_t MFEvtNumber::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "FileName", print))
    {
        rc = kTRUE;
        SetFileName(GetEnvValue(env, prefix, "FileName", fFileName));
    }
    if (IsEnvDefined(env, prefix, "TreeName", print))
    {
        rc = kTRUE;
        SetFileName(GetEnvValue(env, prefix, "TreeName", fTreeName));
    }
    if (IsEnvDefined(env, prefix, "Selector", print))
    {
        rc = kTRUE;
        SetSelector(GetEnvValue(env, prefix, "Selector", fSelector));
    }
    return rc;
}
