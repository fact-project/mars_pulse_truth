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
!   Author(s): Javier Lopez 05/2001 <mailto:jlopez@ifae.es>
!   Author(s): Thomas Bretz 06/2001 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
//  MMcThresholdCalc
//                                                                         
//  Input Containers:
//   MMcEvt, MMcTrig;*
//
//  Output Containers:
//   MHMcEnergies
//
/////////////////////////////////////////////////////////////////////////////

#include "MMcThresholdCalc.h"

#include <math.h>

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MMcEvt.hxx"
#include "MMcTrig.hxx"

#include "MHMcEnergy.h"

ClassImp(MMcThresholdCalc);

using namespace std;

const Float_t MMcThresholdCalc::fSqrt2 = sqrt(2.);

// --------------------------------------------------------------------------
//
// Default Constructor.
//
// Specify the number of trigger conditions you want to use.
// The default is 0.
//
// dim < 0: use only condition number dim (eg "MMcTrig;3")
// dim = 0: use only condition without a number ("MMcTrig")
// dim > 0: use conditions up to dim (from "MMcTrig;1" to "MMcTrig;dim")
//
MMcThresholdCalc::MMcThresholdCalc(const Int_t dim, const char* name,
                                   const char* title)
    : fMcTrig(NULL), fEnergy(NULL)
{
    fName  = name  ? name  : "MMcThresholdCalc";
    fTitle = title ? title : "Task to calculate the energy threshold from Monte Carlo";

    fFirst = dim>0 ?   1 : -dim;
    fLast  = dim>0 ? dim : -dim;

    fNum = fLast-fFirst+1;

    AddToBranchList("MMcEvt.fEnergy");
    AddToBranchList("MMcTrig", "fNumFirstLevel", fFirst, fLast);
}

// -------------------------------------------------------------------------
//
// Destructor.
//
MMcThresholdCalc::~MMcThresholdCalc()
{
    if (fMcTrig)
        delete fMcTrig;

    if (fEnergy)
        delete fEnergy;
}

// --------------------------------------------------------------------------
//
// connect Monte Carlo data with this task
//
Int_t MMcThresholdCalc::PreProcess(MParList* pList)
{
    //
    // This task takes into accout if the root file has one trigger
    // condition (MMcTrig) or severl of them (MMcTrig;#.)

    fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << err << dbginf << "MMcEvt not found... aborting." << endl;
        return kFALSE;
    }

    UInt_t num;

    fMcTrig = new TObjArray(pList->FindObjectList("MMcTrig", fFirst, fLast));
    num = fMcTrig->GetEntriesFast();
    if (num != fNum)
    {
        *fLog << err << dbginf << fNum << " MMcTrig objects requested, ";
        *fLog << num << " are available... aborting." << endl;
        return kFALSE;
    }

    fEnergy = new TObjArray(pList->FindCreateObjList("MHMcEnergy", fFirst, fLast));
    num = fMcTrig->GetEntriesFast();
    if (num != fNum)
    {
        *fLog << err << dbginf << fNum << " MHMcEnergy objects requested, ";
        *fLog << num << " are available... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The histograms are filled with log10 of the energy for triggered
// events and weighted with 1/E because it is needed the dN/dE vs. logE
// distribution to get the energy threshold.
//
Int_t MMcThresholdCalc::Process()
{
    const Float_t energy   = fMcEvt->GetEnergy();
    const Float_t lg10     = log10(energy);
    const Float_t reciproc = 1./energy;

    for (UInt_t i=0; i<fNum; i++)
        if (GetTrig(i)->GetFirstLevel()>0)
            GetHEnergy(i)->Fill(lg10, reciproc);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// fit the energy distribution to get the threshold
// Some iterations are done to be sure the fit parameters converge.
//
Int_t MMcThresholdCalc::PostProcess()
{
    for (UInt_t i=0; i<fNum; i++)
    {
        MHMcEnergy &hist = *GetHEnergy(i);

        Float_t peak;
        Float_t sigma;

        hist.Fit(1, 3);

        peak  = hist.GetGaussPeak();
        sigma = hist.GetGaussSigma();
        hist.Fit(peak - 2.   *sigma, peak + 2.   *sigma);

        peak  = hist.GetGaussPeak();
        sigma = hist.GetGaussSigma();
        hist.Fit(peak - fSqrt2*sigma, peak + fSqrt2*sigma);

        hist.SetReadyToSave();
    }
    return kTRUE;
}

