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
!   Author(s): Thomas Bretz, 9/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MJOptimizeCuts
//
// Class for otimizing the parameters of the supercuts
//
// Minimization Control
// ====================
//
//   To choose the minimization algorithm use:
//         void SetOptimizer(Optimizer_t o);
//
//   Allowed options are:
//        enum Optimizer_t
//        {
//            kMigrad,      // Minimize by the method of Migrad
//            kSimplex,     // Minimize by the method of Simplex
//            kMinimize,    // Migrad + Simplex (if Migrad fails)
//            kMinos,       // Minos error determination
//            kImprove,     // Local minimum search
//            kSeek,        // Minimize by the method of Monte Carlo
//            kNone         // Skip optimization
//        };
//
//   For more details on the methods see TMinuit.
//
//
//   You can change the behaviour of the minimization using
//
//        void SetNumMaxCalls(UInt_t num=0);
//        void SetTolerance(Float_t tol=0);
//
//   While NumMaxCalls is the first, Tolerance the second arguement.
//   For more details start root and type
//
//        gMinuit->mnhelp("command")
//
//   while command can be
//        * MIGRAD
//        * SIMPLEX
//        * MINIMIZE
//        * MINOS
//        * IMPROVE
//        * SEEK
//
//   The default (num==0 and tol==0) should always give you the
//   corresponding defaults used in Minuit.
//
//
// FIXME: Implement changing cut in hadronness...
// FIXME: Show MHSignificance on MStatusDisplay during filling...
// FIXME: Choose step-size percentage as static data membewr
// FIXME: Choose minimization method
//
/////////////////////////////////////////////////////////////////////////////
#include "MJOptimizeCuts.h"

#include <TClass.h>

#include "MHMatrix.h"

// Environment
#include "MLog.h"
#include "MLogManip.h"
#include "MStatusDisplay.h"

// Eventloop
#include "MReadTree.h"
#include "MParList.h"
#include "MTaskList.h"

// Parameter container
#include "MGeomCamFACT.h"
#include "MParameters.h"
#include "MFilterList.h"

// histograms
#include "../mhflux/MHAlpha.h"

// Tasks
#include "MF.h"
#include "MFillH.h"
#include "MContinue.h"
#include "MMatrixLoop.h"

#include "MFMagicCuts.h"

ClassImp(MJOptimizeCuts);

using namespace std;

//------------------------------------------------------------------------
//
MHAlpha *MJOptimizeCuts::CreateNewHist(const char *name) const
{
    TClass *cls = gROOT->GetClass(fNameHist);
    if (!cls)
    {
        *fLog << err << "Class " << fNameHist << " not found in dictionary... abort." << endl;
        return NULL;
    }
    if (!cls->InheritsFrom(MHAlpha::Class()))
    {
        *fLog << err << "Class " << fNameHist << " doesn't inherit from MHAlpha... abort." << endl;
        return NULL;
    }

    MHAlpha *h = (MHAlpha*)cls->New();
    if (h && name)
        h->SetName(name);

    return h;
}

//------------------------------------------------------------------------
//
Bool_t MJOptimizeCuts::RunOnOffCore(MHAlpha &histon, MHAlpha &histof, const char *fname, MFilter *filter, MAlphaFitter *fit, const char *tree)
{
    SetTitle(Form("OptimizeCuts: %s", fname));

    if (fDisplay)
        fDisplay->SetTitle(fTitle);

    fLog->Separator("Preparing On/Off-optimization");

    MParList parlist;

    MGeomCamFACT geom; // For GetConvMm2Deg
    parlist.AddToList(&geom);

    MHMatrix m("M");
    AddRulesToMatrix(m);
    parlist.AddToList(&m);

    const Int_t idxdatatype = m.AddColumn("DataType.fVal");

    histon.SkipHistTime();
    histon.SkipHistTheta();
    //histon.SkipHistEnergy();
    histof.SkipHistTime();
    histof.SkipHistTheta();
    //histof.SkipHistEnergy();
    histon.ForceUsingSize();
    histof.ForceUsingSize();
    histon.InitMapping(&m, 1);
    histof.InitMapping(&m, 1);

    if (filter)
    {
        if (filter->InheritsFrom(MFMagicCuts::Class()))
            ((MFMagicCuts*)filter)->InitMapping(&m);
        else
        {
            *fLog << err << "ERROR - Currently only MFMagicCuts is supported." << endl;
            return kFALSE;
        }
    }

    parlist.AddToList(&histon);
    parlist.AddToList(&histof);

    if (fname)
    {
        MReadTree read(tree);
        read.DisableAutoScheme(); // AutoScheme doesn't seem to be faster!
        read.AddFile(fname);
        if (!FillMatrix(read, parlist))
            return kFALSE;
    }
    else
    {
        MParameterI par("DataType");
        parlist.AddToList(&par);

        gLog.Separator("Reading On-Data");
        par.SetVal(1);
        MReadTree readon(tree);
        readon.DisableAutoScheme(); // AutoScheme doesn't seem to be faster!
        AddSequences(readon, fNamesOn);
        if (!FillMatrix(readon, parlist))
            return kFALSE;

        gLog.Separator("Reading Off-Data");
        par.SetVal(0);
        MReadTree readoff(tree);
        readoff.DisableAutoScheme(); // AutoScheme doesn't seem to be faster!
        AddSequences(readoff, fNamesOff);
        if (!FillMatrix(readoff, parlist))
            return kFALSE;
    }

    MTaskList tasklist;
    parlist.Replace(&tasklist);
    if (fit)
        parlist.AddToList(fit);

    MFilterList list;
    SetupFilters(list, filter);

    MContinue contin(&list);
    parlist.AddToList(&list);

    MFillH fillof(&histof, "", "FillHistOff");
    MFillH fillon(&histon, "", "FillHistOn");

    MF f0(Form("M[%d]<0.5", idxdatatype), "FilterOffData");
    MF f1(Form("M[%d]>0.5", idxdatatype), "FilterOnData");

    fillof.SetFilter(&f0);
    fillon.SetFilter(&f1);

    MMatrixLoop loop(&m);

    tasklist.AddToList(&loop);
    tasklist.AddToList(&list);
    tasklist.AddToList(&contin);
    tasklist.AddToList(&f0);
    tasklist.AddToList(&f1);
    tasklist.AddToList(&fillof);
    tasklist.AddToList(&fillon);

    // Optimize with the tasklist in this parameterlist
    if (!Optimize(parlist))
        return kFALSE;

    *fLog << inf << "Finished processing of " << fname << endl;

    // Copy the result back to be accessible by the user
    if (fit)
        histon.GetAlphaFitter().Copy(*fit);

    // Print the result
    histon.GetAlphaFitter().Print("result");

    // Store result if requested
    TObjArray cont;
    cont.Add(&contin);
    return WriteContainer(cont, fNameOut);
}

Bool_t MJOptimizeCuts::RunOnCore(MHAlpha &hist, const char *fname, MFilter *filter, MAlphaFitter *fit)
{
    SetTitle(Form("OptimizeCuts: %s", fname));

    if (fDisplay)
        fDisplay->SetTitle(fTitle);

    fLog->Separator("Preparing On-only-optimization");

    MParList parlist;

    MGeomCamFACT geom; // For GetConvMm2Deg
    parlist.AddToList(&geom);

    MHMatrix m("M");
    AddRulesToMatrix(m);
    parlist.AddToList(&m);

    hist.SkipHistTime();
    hist.SkipHistTheta();
    hist.SkipHistEnergy();
    hist.InitMapping(&m); 

    if (filter && filter->InheritsFrom(MFMagicCuts::Class()))
        ((MFMagicCuts*)filter)->InitMapping(&m);

    MReadTree read("Events");
    read.DisableAutoScheme(); // AutoScheme doesn't seem to be faster!
    if (fname)
        read.AddFile(fname);
    else
        AddSequences(read, fNamesOn);
    if (!FillMatrix(read, parlist))
        return kFALSE;

    MTaskList tasklist;
    parlist.Replace(&tasklist);
    if (fit)
        parlist.AddToList(fit);

    MFilterList list;
    SetupFilters(list, filter);

    MContinue contin(&list);
    parlist.AddToList(&list);

    MFillH fill(&hist);

    MMatrixLoop loop(&m);

    tasklist.AddToList(&loop);
    tasklist.AddToList(&list);
    tasklist.AddToList(&contin);
    tasklist.AddToList(&fill);

    // Optimize with the tasklist in this parameterlist
    if (!Optimize(parlist))
        return kFALSE;

    *fLog << inf << "Finished processing of " << fname << endl;

    // Copy the result back to be accessible by the user
    if (fit)
        hist.GetAlphaFitter().Copy(*fit);
    // Print the result
    hist.GetAlphaFitter().Print("result");

    // Store result if requested
    TObjArray cont;
    cont.Add(&contin);
    if (fDisplay)
        cont.Add(fDisplay);
    return WriteContainer(cont, fNameOut);
}

//------------------------------------------------------------------------
//
Bool_t MJOptimizeCuts::RunOnOff(const char *fname, MFilter *filter, MAlphaFitter *fit, const char *tree)
{
    MHAlpha *histon = CreateNewHist("Hist");
    MHAlpha *histof = CreateNewHist("HistOff");

    if (!histon || !histof)
        return kFALSE;

    const Bool_t rc = RunOnOffCore(*histon, *histof, fname, filter, fit, tree);

    delete histon;
    delete histof;

    return rc;
}

//------------------------------------------------------------------------
//
Bool_t MJOptimizeCuts::RunOn(const char *fname, MFilter *filter, MAlphaFitter *fit)
{
    MHAlpha *histon = CreateNewHist();

    if (!histon)
        return kFALSE;

    const Bool_t rc = RunOnCore(*histon, fname, filter, fit);

    delete histon;
    return rc;
}
