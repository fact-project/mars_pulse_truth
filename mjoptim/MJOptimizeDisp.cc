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
!   Author(s): Thomas Bretz, 6/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//
// MJOptimizeDisp
//
// Class for otimizing the disp parameters. For more details see
// MJOptimize.
//
// MJOptimizeDisp minimzes the width of the Theta^2 distribution by
// minimizing <Theta^2> (the RMS^2 of Theta^2), while Theta^2 is
// calculated as:
//        Theta^2 = d^2 + p^2 - 2*d*p*cos(a)
// with:
//        d: MHillasSrc.fDist [deg]
//        p: Disp as calculated by the given rule, eg: [0] (a constant)
//        a: MHillasSrc.fAlpha [rad]
//
// Example:
// --------
//    MJOptimizeDisp opt;
//    opt.SetDebug(2);
//    opt.SetOptimizer(MJOptimize::kMigrad);
//    opt.SetNumEvents(20000);
//    opt.EnableTestTrain();
//    opt.AddParameter("1-(MHillas.fWidth/MHillas.fLength)");
//    opt.SetParameter(0, 1, 0, 2);
//    char *r = "[0]*M[0]"; //Rule to calculate disp
//    MStatusDisplay *d = new MStatusDisplay;
//    opt.SetDisplay(d);
//    opt.RunDisp("ganymed-summary.root", r);
//
/////////////////////////////////////////////////////////////////////////////
#include "MJOptimizeDisp.h"

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
#include "MGeomCamFACT.h"

// histograms
#include "MH3.h"
#include "MBinning.h"
#include "../mhflux/MAlphaFitter.h"
#include "../mhflux/MHThetaSq.h"
#include "../mtools/MChisqEval.h"

// tasks
#include "MReadTree.h"
#include "MMatrixLoop.h"
#include "MParameterCalc.h"
#include "MFillH.h"
#include "MContinue.h"
#include "MWriteRootFile.h"

// filters
#include "MFilterList.h"
#include "MFDataMember.h"

using namespace std;

ClassImp(MJOptimizeDisp);

//------------------------------------------------------------------------
//
// Read all events from file which do match rules and optimize
// disp estimator.
//
Bool_t MJOptimizeDisp::RunDisp(const char *fname, const char *rule, MTask *weights)
{
    SetTitle(Form("OptimizeDisp: %s", fname));

    if (fDisplay)
        fDisplay->SetTitle(fTitle);

    fLog->Separator("Preparing Disp optimization");

    MParList parlist;

    MParameterI par1("DataType");
    par1.SetVal(1);
    parlist.AddToList(&par1);

    MParameterD par2("ThetaSquaredCut");
    par2.SetVal(0.2);
    parlist.AddToList(&par2);

    MAlphaFitter fit;
    fit.SetPolynomOrder(0);
    fit.SetSignalFitMax(0.8);
    fit.EnableBackgroundFit(kFALSE);
    fit.SetSignalFunction(MAlphaFitter::kThetaSq);
    fit.SetMinimizationStrategy(MAlphaFitter::kGaussSigma);
    parlist.AddToList(&fit);

    // To avoid this hard-coded we have to switch to MReadMarsFile
    MGeomCamFACT geom; // For GetConvMm2Deg
    parlist.AddToList(&geom);

    MHMatrix m("M");
    AddRulesToMatrix(m);
    parlist.AddToList(&m);

    const Int_t num1 = m.AddColumn("MHillasSrc.fDist*MGeomCam.fConvMm2Deg");
    const Int_t num2 = m.AddColumn("MHillasSrc.fAlpha*TMath::DegToRad()");
    const Int_t num3 = m.AddColumn("MHillas.fSize");

    MHThetaSq hist;
    hist.SkipHistTime();
    hist.SkipHistTheta();
    //hist.SkipHistEnergy();
    //hist.ForceUsingSize();
    hist.InitMapping(&m, 1);

    MFDataMember filter("DataType.fVal", '>', 0.5);
    fPreCuts.Add(&filter);

    MParameterCalc calc1(rule, "MParameters");
    calc1.SetNameParameter("Disp");
    parlist.AddToList(&calc1);

    const char *disp = "Disp.fVal";
    const char *n1   = Form("M[%d]", num1);

    const char *rule2 = Form("(%s*%s) + (%s*%s) - (2*%s*%s*cos(M[%d]))",
                             n1, n1, disp, disp, n1, disp, num2);

    MParameterCalc calc2(rule2, "MThetaSqCalc");
    calc2.SetNameParameter("ThetaSquared");

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
    eval.SetY1(fUseThetaSq?"ThetaSquared.fVal":"sqrt(ThetaSquared.fVal)");
    if (weights)
        eval.SetNameWeight();

    MMatrixLoop loop(&m);

    const char *n3   = Form("M[%d]", num3);
    MH3 hdisp(n3, "sqrt(ThetaSquared.fVal)");
    hdisp.SetTitle("\\vartheta^{2} distribution vs. Size:Size [phe]:\\vartheta^{2} [\\circ^{2}]");

    MBinning binsx(100, 10, 100000, "BinningMH3X", "log");
    MBinning binsy(100, 0,  2,      "BinningMH3Y", "lin");

    parlist.AddToList(&binsx);
    parlist.AddToList(&binsy);

    MFillH fillh2(&hdisp);
    fillh2.SetDrawOption("blue profx");

    tasklist.AddToList(&loop);
    tasklist.AddToList(&calc1);
    tasklist.AddToList(&calc2);
    if (weights)
        tasklist.AddToList(weights);
    //tasklist.AddToList(&fill);
    //tasklist.AddToList(&fillh2);
    tasklist.AddToList(&eval);

    // Optimize with the tasklist in this parameterlist
    if (!Optimize(parlist))
        return kFALSE;

    // Print the result
    *fLog << inf;
    *fLog << "Finished processing of " << fname << endl;
    *fLog << "With Rule: " << rule << endl;
    //hist.GetAlphaFitter().Print("result");

    if (fNameOut.IsNull())
        return kTRUE;

    fLog->Separator("Writing result");

    MTaskList tlist;
    MParList plist;
    plist.AddToList(&tlist);
    plist.AddToList(&geom);

    MReadTree read2("Events");
    read2.DisableAutoScheme();
    read2.AddFiles(read);
    tlist.AddToList(&read2);

    MFilterList flist;
    fPreCuts.Add(&filter);
    flist.AddToList(fPreCuts);
    flist.SetName("PreCuts");
    flist.SetInverted(kTRUE);

    MContinue cont(&flist);
    tlist.AddToList(&cont);

    MParameterCalc calcx("0", "CountEvents");
    tlist.AddToList(&calcx);

    // FIXME: Handle fTestTrain<0
    MContinue contt("TMath::Odd(CountEvents.fNumExecutions)>0.5", "ContTrain");
    plist.AddToList(&calcx);
    tlist.AddToList(&contt);

    TString r(rule);

    TIter Next(&fRules);
    TObject *o=0;
    Int_t num = 0;
    while ((o=Next()))
    {
        r.ReplaceAll(Form("M[%d]", num), Form("M%d.fVal", num));

        MParameterCalc *c = new MParameterCalc(o->GetName(), Form("CalcM%d", num));
        c->SetNameParameter(Form("M%d", num++));
        c->SetBit(kCanDelete);
        tlist.AddToList(c);
    }

    MParameterCalc calcr(r, "CalcDisp");
    calcr.SetVariables(GetParameters());
    calcr.SetNameParameter("Disp");

    MParameterCalc calct("MHillasSrc.fDist^2*MGeomCam.fConvMm2Deg^2 + Disp.fVal^2 - 2*MHillasSrc.fDist*MGeomCam.fConvMm2Deg*Disp.fVal*cos(MHillasSrc.fAlpha*TMath::DegToRad())", "CalcTheta");
    calct.SetNameParameter("ThetaSquared");

    tlist.AddToList(&calcr);
    tlist.AddToList(&calct);

    tlist.AddToList(fTestTasks);

    MWriteRootFile write(fNameOut);
    write.AddContainer("MHillas",      "Events");
    write.AddContainer("MHillasSrc",   "Events");
    write.AddContainer("MHillasExt",   "Events");
    write.AddContainer("MImagePar",    "Events");
    write.AddContainer("MNewImagePar", "Events");
    write.AddContainer("Disp",         "Events");
    write.AddContainer("ThetaSquared", "Events");
    write.AddContainer("MMcEvt",       "Events");
    write.AddContainer("DataType",     "Events");
    write.AddContainer("Weight",       "Events");
    write.AddContainer("FileId",       "Events");
    write.AddContainer("EvtNumber",    "Events");
    tlist.AddToList(&write);

    MEvtLoop loop2;
    loop2.SetDisplay(fDisplay);
    loop2.SetParList(&plist);
    if (!loop2.Eventloop())
        return kFALSE;

    // Store result if requested
    TObjArray arr;
    if (fDisplay)
        arr.Add(fDisplay);
    arr.Add(&calc1);
    return WriteContainer(arr, fNameOut);
}
