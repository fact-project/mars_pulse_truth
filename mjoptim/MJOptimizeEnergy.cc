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
// MJOptimize
//
// Class for otimizing a rule to estimate the energy. For more details see
// MJOptimize.
//
// Example:
// --------
//
//    MJOptimizeEnergy opt;
//    opt.SetDebug(2);
//    opt.SetOptimizer(MJOptimize::kMigrad);
//    opt.SetNumEvents(20000);
//    opt.EnableTestTrain();
//    opt.AddParameter("MHillas.fSize");
//    opt.SetParameter(0, 1, 0, 2);
//    char *r = "[0]*M[0]"; //Rule to calculate estimated energy
//    MStatusDisplay *d = new MStatusDisplay;
//    opt.SetDisplay(d);
//    opt.RunDisp("ganymed-summary.root", r);
//
/////////////////////////////////////////////////////////////////////////////
#include "MJOptimizeEnergy.h"

#include "MHMatrix.h"

// environment
#include "MLog.h"
#include "MLogManip.h"
#include "MStatusDisplay.h"

// eventloop
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

// parameters
#include "MParameters.h"
#include "MGeomCamMagic.h"

// histograms
#include "../mhflux/MHEnergyEst.h"
#include "../mtools/MChisqEval.h"

// tasks
#include "MReadTree.h"
#include "MMatrixLoop.h"
#include "MEnergyEstimate.h"
#include "MFillH.h"

// filters
#include "MFDataMember.h"

using namespace std;

ClassImp(MJOptimizeEnergy);

//------------------------------------------------------------------------
//
// Read all events from file which do match rules and optimize
// energy estimator.
//
Bool_t MJOptimizeEnergy::RunEnergy(const char *fname, const char *rule, MTask *weights)
{
    SetTitle(Form("OptimizeEnergy: %s", fname));

    if (fDisplay)
        fDisplay->SetTitle(fTitle);

    fLog->Separator("Preparing Energy optimization");

    MParList parlist;

    MParameterI par("DataType");
    par.SetVal(1);
    parlist.AddToList(&par);

    MFDataMember filter("DataType.fVal", '>', 0.5);
    fPreCuts.Add(&filter);

    MGeomCamMagic geom; // For GetConvMm2Deg
    parlist.AddToList(&geom);

    MHMatrix m("M");
    AddRulesToMatrix(m);
    const Int_t map = m.AddColumn("MMcEvt.fEnergy");
    parlist.AddToList(&m);

    MHEnergyEst hist;
    hist.InitMapping(&m); 

    MParameterCalc est(rule, "MParameters");
    est.SetNameParameter("MEnergyEst");
    parlist.AddToList(&est);

    MReadTree read("Events");
    // NECESSARY BECAUSE OF MDataFormula GetRules missing
    read.DisableAutoScheme();
    if (fname)
        read.AddFile(fname);
    else
        AddSequences(read, fNamesOn);
    if (!FillMatrix(read, parlist, kTRUE))
        return kFALSE;

    fPreCuts.Remove(&filter);

    MTaskList tasklist;
    parlist.Replace(&tasklist);

    MFillH fill(&hist);
    if (weights)
        fill.SetWeight();

    MChisqEval eval;
    eval.SetY1(fOptimLog?Form("log10(MEnergyEst.fVal/M[%d])", map):Form("MEnergyEst.fVal-M[%d]", map));
    if (weights)
        eval.SetNameWeight();

    MMatrixLoop loop(&m);

    tasklist.AddToList(&loop);
    tasklist.AddToList(&est);
    if (weights)
        tasklist.AddToList(weights);
    tasklist.AddToList(&fill);
    tasklist.AddToList(&eval);

    // Optimize with the tasklist in this parameterlist
    if (!Optimize(parlist))
        return kFALSE;

    // Print the result
    *fLog << inf << "Finished processing of " << fname << endl;
    *fLog << inf << "With Rule: " << rule << endl;
    hist.Print();

    // Store result if requested
    TObjArray cont;
    cont.Add(&est);
    if (fDisplay)
        cont.Add(fDisplay);
    return WriteContainer(cont, fNameOut);
}
