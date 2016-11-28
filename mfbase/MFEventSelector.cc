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
!   Author(s): Thomas Bretz, 01/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MFEventSelector
//
// This is a filter to make a selection of events from a file. At the
// present implementation you can only use a random selection.
//
// This filter can be used for a random split...
//
// If you want to fill only 50% of your events into a histogram please use:
//   MFEventSelector sel;
//   sel.SetSelectionRatio(0.5);
//   MFillH filler(...);
//   filler.SetFilter(&sel);
//   tlist.AddToList(&sel);
//   tlist.AddToList(&filler);
//
// To get around 2000 events from all events use (Remark: This will only
// work if the parlist has an entry called MTaskList which has a task
// MRead inheriting from MRead):
//   MFEventSelector sel;
//   sel.SetNumSelectEvts(2000);
//   MFillH filler(...);
//   filler.SetFilter(&sel);
//   tlist.AddToList(&sel);
//   tlist.AddToList(&filler);
//
// If you don't have MRead available you have to set the number of
// total events manually, using sel.SetNumTotalEvts(10732);
//
// The random number is generated using gRandom->Uniform(). You may
// control this procedure using the global object gRandom.
//
// Because of the random numbers this works best for huge samples...
//
// Don't try to use this filter for the reading task: This won't work!
//
// Remark: You can also use the filter together with MContinue
//
//
// FIXME: Merge MFEventSelector and MFEventSelector2
//
/////////////////////////////////////////////////////////////////////////////
#include "MFEventSelector.h"

#include <TRandom.h>

#include "MParList.h"
#include "MTaskList.h"
#include "MRead.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MFEventSelector);

using namespace std;

static const TString gsDefName  = "MFEventSelector";
static const TString gsDefTitle = "Filter to select events";

// --------------------------------------------------------------------------
//
// Constructor. For the text describing the filter rule please see
// the class description above.
//
MFEventSelector::MFEventSelector(const char *name, const char *title)
: fNumTotalEvts(-1), fNumSelectEvts(-1), fSelRatio(-1), fNumSelectedEvts(0)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
// Set a probability with which events are selected. Eg, f=0.5
// will select roughly half of all events.
//
void MFEventSelector::SetSelectionRatio(Float_t f)
{
    if (f < 0)
    {
        *fLog << warn << "MFEventSelector::SetSelectionRatio: WARNING - Probability less than 0... set to 0." << endl;
        f = 0;
    }

    if (f > 1)
    {
        *fLog << warn << "MFEventSelector::SetSelectionRatio: WARNING - Probability greater than 1... set to 1." << endl;
        f = 1;
    }
    fSelRatio = f;
}

// --------------------------------------------------------------------------
//
// PreProcess all filters.
//
Int_t MFEventSelector::PreProcess(MParList *plist)
{
    fNumSelectedEvts = 0;

    // In the case a ratio was set by the user we are done.
    if (fSelRatio>0)
        return kTRUE;

    // If the number of total events wasn't set try to get it
    if (fNumTotalEvts<0)
    {
        MTaskList *tlist = (MTaskList*)plist->FindObject("MTaskList");
        if (!tlist)
        {
            *fLog << err << "Can't determin total number of events... no MTaskList." << endl;
            return kFALSE;
        }

        MRead *read = (MRead*)tlist->FindObject("MRead");
        if (!read)
        {
            *fLog << err << "Can't determin total number of events from 'MRead'." << endl;
            return kFALSE;
        }
        fNumTotalEvts = read->GetEntries();

        SetBit(kNumTotalFromFile);
    }

    if (fNumSelectEvts<0)
    {
        fSelRatio = 1;
        *fLog << inf << "No selection will be done - all events selected." << endl;
        return kTRUE;
    }

    // Calculate selection probability
    fSelRatio = (Double_t)fNumSelectEvts/fNumTotalEvts;

    *fLog << inf << "Selection probability = " << fNumSelectEvts << "/";
    *fLog << fNumTotalEvts << " = " << Form("%.2f", fSelRatio) << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Process all filters, FIXME: Make it fit the requested number of events
// exactly like it is done in MFEventSelector2. This can be done by merging
// both classes!
//
Int_t MFEventSelector::Process()
{
    fResult = gRandom->Uniform() < fSelRatio;

    if (fResult)
        fNumSelectedEvts++;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Postprocess all filters.
//
Int_t MFEventSelector::PostProcess()
{
    //---------------------------------
    if (GetNumExecutions() != 0)
    {
        const Double_t sel = (Double_t)fNumSelectedEvts/GetNumExecutions();
        const UInt_t   non = GetNumExecutions()-fNumSelectedEvts;

        *fLog << inf << dec << setfill(' ') << endl;
        *fLog << GetDescriptor() << " execution statistics:" << endl;

        *fLog << " " << setw(7) << non << " (" << setw(3);
        *fLog << (int)(100*(1-sel)) << "%) Events not selected" << endl;

        *fLog << " " << setw(7) << fNumSelectedEvts << " (";
        *fLog << (int)(100*sel) << "%) Events selected!" << endl;
        *fLog << endl;
    }

    //---------------------------------
    if (TestBit(kNumTotalFromFile))
    {
        fNumTotalEvts = -1;
        if (fNumSelectEvts>0)
            fSelRatio = -1;
    }

    return kTRUE;
}
