/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz,  1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MSimBundlePhotons
//
// This task sets a new tag (index) for all photons in a list MPhotonEvent
// based on a look-up table.
//
// Input:
//  MPhotonEvent
//  MPhotonStatistics
//
// Output
//  MPhotonEvent
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimBundlePhotons.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MArrayI.h"

#include "MParList.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

ClassImp(MSimBundlePhotons);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimBundlePhotons::MSimBundlePhotons(const char* name, const char *title)
: fEvt(0), fStat(0)//, fFileName("mreflector/dwarf-apdmap.txt")
{
    fName  = name  ? name  : "MSimBundlePhotons";
    fTitle = title ? title : "Task to bundle (re-index) photons according to a look-up table";
}

// --------------------------------------------------------------------------
//
// Check for the needed containers and read the oruting table.
//
Int_t MSimBundlePhotons::PreProcess(MParList *pList)
{
    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }

    fStat = (MPhotonStatistics*)pList->FindObject("MPhotonStatistics");
    if (!fStat)
    {
        *fLog << err << "MPhotonStatistics not found... aborting." << endl;
        return kFALSE;
    }

    if (fFileName.IsNull())
        return kSKIP;

    // Read the look-up table
    fLut.Delete();
    if (!fFileName.IsNull() && fLut.ReadFile(fFileName)<0)
        return kFALSE;

    // If the table is empty remove this task from the tasklist
    if (fLut.IsEmpty())
    {
        *fLog << inf << "Look-up table to bundle photons empty... skipping." << endl;
        return kSKIP;
    }

    // Now invert the tablee. Otherwise we have to do a lot of
    // searching for every index.
    fLut.Invert();

    // Make sure that each line has exactly one row
    if (!fLut.HasConstantLength() && fLut.GetMaxEntries()!=1)
    {
        *fLog << err << fFileName << " wrongly formatted." << endl;
        return kFALSE;
    }

    *fLog << inf << "Using look-up table from " << fFileName << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Re-index all photons according to the look-up table.
//
Int_t MSimBundlePhotons::Process()
{
    // Make sure that we don't get a seg-fault
    /*
    if (fStat->GetMaxIndex()>=fLut.GetEntriesFast())
    {
        *fLog << err;
        *fLog << "ERROR - MSimBundlePhotons::Process: Maximum pixel index stored" << endl;
        *fLog << "        in tag of MPhotonData exceeds the look-up table length." << endl;
        return kERROR;
    }*/

    // FIXME: Add a range check comparing the min and max tag with
    // the number of entries in the routing table

    // Get total number of photons
    const Int_t num = fEvt->GetNumPhotons();

    // If there are no photons we can do nothing
    if (num==0)
        return kTRUE;

    // Get maximum index allowed
    const Int_t max = fLut.GetEntriesFast();

    // Initialize a counter for the final number of photons.
    Int_t cnt=0;

    // Loop over all photons
    for (Int_t i=0; i<num; i++)
    {
        // Get i-th photon from array
        MPhotonData &ph = (*fEvt)[i];

        // Get pixel index (tag) from photon
        const Int_t tag = ph.GetTag();

        // Check if the photon was tagged at all and
        // whether the corresponding lut entry exists
        if (tag<0 || tag>=max)
            continue;

        // Get the routing assigned to this index
        const MArrayI &row = fLut.GetRow(tag);

        // Sanity check: Check if we were routed to a
        // valid entry if not throw away this photon.
        if (row.GetSize()==0)
            continue;

        // Get corresponding entry from routing table
        const Int_t &idx = row[0];

        // Check if we were routed to a valid entry ("not connected")
        // if not throw away this photon.
        if (idx<0)
            continue;

        // Set Tag to new index
        ph.SetTag(idx);

        // Copy photon to its now position in array and increade counter
        (*fEvt)[cnt++] = ph;
    }

    // Shrink the list of photons to its new size
    fEvt->Shrink(cnt);

    // Set new maximum index (Note, that this is the maximum index
    // available in the LUT, which does not necessarily correspond
    // to, e.g., the number of pixels although it should)
    fStat->SetMaxIndex(fLut.GetMaxIndex());

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// FileName: lut.txt
//
Int_t MSimBundlePhotons::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "FileName", print))
    {
        rc = kTRUE;
        fFileName = GetEnvValue(env, prefix, "FileName", fFileName);
    }

    return rc;
}
