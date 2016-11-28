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
!   Author(s): Thomas Bretz 11/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2005-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MJTrainDisp
//
//
// Example:
// --------
//
//    // SequencesOn are used for training, SequencesOff for Testing
//    MDataSet set("mctesttrain.txt");
//    set.SetNumAnalysis(1);  // Must have a number
//    MJTrainDisp opt;
//    //opt.SetDebug();
//    opt.AddParameter("MHillas.fLength");
//    opt.AddParameter("MHillas.fWidth");
//    MStatusDisplay *d = new MStatusDisplay;
//    opt.SetDisplay(d);
//    opt.AddPreCut("MHillasSrc.fDCA*MGeomCam.fConvMm2Deg<0.3");
//    opt.Train("rf-disp.root", set, 30000); // Number of train events
//
//    // Two of the displayed histograms show the cut efficiency for
//    // a given Theta-Cut. The default is 0.215. It can be overwritten
//    // by
//    opt.SetThetaCut(0.165);
//
//
// Random Numbers:
// ---------------
//   Use:
//         if(gRandom)
//             delete gRandom;
//         gRandom = new TRandom3();
//   in advance to change the random number generator.
//
////////////////////////////////////////////////////////////////////////////
#include "MJTrainDisp.h"

#include <TH2.h>
#include <TLine.h>
#include <TCanvas.h>

#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

// tools
#include "MDataSet.h"
#include "MTFillMatrix.h"
#include "MChisqEval.h"
#include "MStatusDisplay.h"

// eventloop
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

// tasks
#include "MReadMarsFile.h"
#include "MContinue.h"
#include "MFillH.h"
#include "MRanForestCalc.h"
#include "MParameterCalc.h"

// container
#include "MParameters.h"

// histograms
#include "MBinning.h"
#include "MH3.h"
#include "MHn.h"
#include "MHThetaSq.h"

// filter
#include "MFilterList.h"

ClassImp(MJTrainDisp);

using namespace std;

const TString MJTrainDisp::fgTrainParameter = "MHillasSrc.fDist*MGeomCam.fConvMm2Deg";

// --------------------------------------------------------------------------
//
// Display a result histogram either vs. size or energy
// FIXME: Could be moved into a new histogram class.
//
void MJTrainDisp::DisplayHist(TCanvas &c, Int_t i, MH3 &mh3) const
{
    MH::SetPalette("pretty");

    TH2 &hist = *static_cast<TH2*>(mh3.GetHist().Clone());
    hist.SetBit(TH1::kNoStats);
    hist.SetDirectory(0);

    TLine line;
    line.SetLineStyle(kDashed);
    line.SetLineWidth(1);

    c.cd(i+4);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    //gPad->SetFillColor(kWhite);
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();
    //gPad->SetLeftMargin(0.12);
    //gPad->SetRightMargin(0.12);

    const Int_t bin = 3;
    const Float_t cutval = hist.GetYaxis()->GetBinUpEdge(bin);

    TH1D heff;
    heff.SetName(Form("Eff%s", hist.GetName()));
    heff.SetTitle(Form("Cut efficiency vs. %s for \\vartheta<%.3f", hist.GetName(), TMath::Sqrt(cutval)));
    heff.SetDirectory(0);
    heff.SetXTitle(hist.GetXaxis()->GetTitle());
    heff.SetYTitle("Efficiency");

    MH::SetBinning(heff, hist);

    for (int x=0; x<=hist.GetNbinsX()+1; x++)
    {
        const Double_t n0 = hist.Integral(x, x, -1, -1);
        if (n0>0)
            heff.SetBinContent(x, hist.Integral(x, x, -1, bin)/n0);
    }

    heff.SetMinimum(0);
    heff.SetMaximum(1);
    heff.DrawCopy();

    line.DrawLine(hist.GetXaxis()->GetXmin(), 0.5,
                  hist.GetXaxis()->GetXmax(), 0.5);

    c.cd(i+0);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    //gPad->SetFillColor(kWhite);
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();
    //gPad->SetLeftMargin(0.12);
    //gPad->SetRightMargin(0.12);

    for (int x=0; x<=hist.GetNbinsX(); x++)
    {
        const Float_t n = hist.GetBinContent(1); // + hist.GetBinContent(2)
        if (n==0)
            continue;

        for (int y=0; y<=hist.GetNbinsY(); y++)
            hist.SetBinContent(x, y, 100*hist.GetBinContent(x,y)/n);
    }

    hist.SetMaximum(100);
    hist.DrawCopy("colz");

    line.DrawLine(hist.GetXaxis()->GetXmin(), cutval,
                  hist.GetXaxis()->GetXmax(), cutval);

    c.cd(i+2);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    //gPad->SetFillColor(kWhite);
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();
    //gPad->SetLeftMargin(0.12);
    //gPad->SetRightMargin(0.12);

    for (int x=0; x<=hist.GetNbinsX(); x++)
    {
        const Float_t n = hist.Integral(x, x, -1, 9999);
        if (n==0)
            continue;

        for (int y=0; y<=hist.GetNbinsY(); y++)
            hist.SetBinContent(x, y, 100*hist.GetBinContent(x,y)/n);
    }

    hist.SetMaximum(25);
    hist.DrawCopy("colz");

    line.DrawLine(hist.GetXaxis()->GetXmin(), cutval,
                  hist.GetXaxis()->GetXmax(), cutval);
}

// --------------------------------------------------------------------------
//
// Display the result histograms in a new tab.
//
void MJTrainDisp::DisplayResult(MH3 &hsize, MH3 &henergy)
{
    TCanvas &c = fDisplay->AddTab("Disp");
    c.Divide(2,3);

    DisplayHist(c, 1, hsize);
    DisplayHist(c, 2, henergy);
}

// --------------------------------------------------------------------------
//
// Run Disp optimization
//
Bool_t MJTrainDisp::Train(const char *out, const MDataSet &set, Int_t num)
{
    SetTitle(Form("Train%s: %s", fNameOutput.Data(), out));

    if (fDisplay)
        fDisplay->SetTitle(fTitle);

    if (!set.IsValid())
    {
        *fLog << err << "ERROR - DataSet invalid!" << endl;
        return kFALSE;
    }

    if (!HasWritePermission(out))
        return kFALSE;

    *fLog << inf;
    fLog->Separator(GetDescriptor());

    // --------------------- Setup files --------------------
    MReadMarsFile readtrn("Events");
    MReadMarsFile readtst("Events");
    readtrn.DisableAutoScheme();
    readtst.DisableAutoScheme();

    if (!set.AddFilesOn(readtrn))
    {
        *fLog << err << "ERROR - Adding SequencesOn." << endl;
        return kFALSE;
    }
    if (!set.AddFilesOff(readtst))
    {
        *fLog << err << "ERROR - Adding SequencesOff." << endl;
        return kFALSE;
    }

    // ----------------------- Setup Matrix ------------------
    MHMatrix train("Train");
    train.AddColumns(fRules);
    if (fEnableWeights)
        train.AddColumn("MWeight.fVal");
    train.AddColumn(fTrainParameter);

    // ----------------------- Fill Matrix RF ----------------------
    MTFillMatrix fill(fTitle);
    fill.SetDisplay(fDisplay);
    fill.SetLogStream(fLog);
    fill.SetDestMatrix1(&train, num);
    fill.SetReader(&readtrn);
    fill.AddPreCuts(fPreCuts);
    fill.AddPreCuts(fTrainCuts);
    fill.AddPreTasks(fPreTasks);
    fill.AddPostTasks(fPostTasks);
    if (!fill.Process())
        return kFALSE;

    // ------------------------ Train RF --------------------------
    MRanForestCalc rf("Train", fTitle);
    rf.SetNumTrees(fNumTrees);
    rf.SetNdSize(fNdSize);
    rf.SetNumTry(fNumTry);
    rf.SetNumObsoleteVariables(1);
    rf.SetLastDataColumnHasWeights(fEnableWeights);
    rf.SetDisplay(fDisplay);
    rf.SetLogStream(fLog);
    rf.SetFileName(out);
    rf.SetDebug(fDebug>1);
    rf.SetNameOutput(fNameOutput);
    rf.SetFunction(fResultFunction);

    /*
    MBinning b(32, 10, 100000, "BinningEnergyEst", "log");

    if (!rf.TrainMultiRF(train, b.GetEdgesD()))    // classification with one tree per bin
        return;

    if (!rf.TrainSingleRF(train, b.GetEdgesD()))   // classification into different bins
        return;
    */
    if (!rf.TrainRegression(train))                  // regression (best choice)
        return kFALSE;

    // --------------------- Display result ----------------------

    gLog.Separator("Test");

    MH::SetPalette("pretty");

    MParList  plist;
    MTaskList tlist;
    plist.AddToList(this);
    plist.AddToList(&tlist);
    //plist.AddToList(&b);

    MParameterD par("ThetaSquaredCut");
    par.SetVal(fThetaCut*fThetaCut);
    plist.AddToList(&par);

    MAlphaFitter fit;
    fit.SetPolynomOrder(0);
    fit.SetSignalFitMax(0.8);
    fit.EnableBackgroundFit(kFALSE);
    fit.SetSignalFunction(MAlphaFitter::kThetaSq);
    fit.SetMinimizationStrategy(MAlphaFitter::kGaussSigma);
    plist.AddToList(&fit);

    MFilterList list;
    if (!list.AddToList(fPreCuts))
        *fLog << err << "ERROR - Calling MFilterList::AddToList for fPreCuts failed!" << endl;
    if (!list.AddToList(fTestCuts))
        *fLog << err << "ERROR - Calling MFilterList::AddToList for fTestCuts failed!" << endl;

    MContinue cont(&list);
    cont.SetInverted();

    const char *rule = "(MHillasSrc.fDist*MGeomCam.fConvMm2Deg)^2 + (Disp.fVal)^2 - (2*MHillasSrc.fDist*MGeomCam.fConvMm2Deg*Disp.fVal*cos(MHillasSrc.fAlpha*kDeg2Rad))";

    MParameterCalc calcthetasq(rule, "MThetaSqCalc");
    calcthetasq.SetNameParameter("ThetaSquared");

    MChisqEval eval;
    eval.SetY1("sqrt(ThetaSquared.fVal)");

    // ----------- Setup binnings ----------------
    MBinning binsS(50,  10,    100000, "BinningSize",         "log");
    MBinning binsE(70,  10,    31623,  "BinningEnergy",       "log");
    MBinning binsG(50, -10,    10,     "BinningSlope",        "lin");
    MBinning binsX(50, -1,     1,      "BinningResidualDist", "lin");
    MBinning binsL(50,  0,     0.3,    "BinningLeakage",      "lin");
    MBinning binsT(51, -0.005, 0.505,  "BinningTheta",        "asin");
    MBinning binsC(50,  1e-2,  1,      "BinningConc",         "log");
    MBinning binsW(50,  0,     0.5,    "BinningLength",       "lin");
    MBinning binsM(50,  0,     0.3,    "BinningWidth",        "lin");
    MBinning binsV(75,  0, par.GetVal()*25, "BinningThetaSq", "lin");

    plist.AddToList(&binsG);
    plist.AddToList(&binsS);
    plist.AddToList(&binsX);
    plist.AddToList(&binsE);
    plist.AddToList(&binsL);
    plist.AddToList(&binsT);
    plist.AddToList(&binsC);
    plist.AddToList(&binsV);
    plist.AddToList(&binsW);
    plist.AddToList(&binsM);

    // ----------- Setup some histograms ----------------

    MHThetaSq hist;
    hist.SkipHistTime();
    hist.SkipHistTheta();
    hist.SkipHistEnergy();

    // To speed it up we could precalculate it.
    const char *res = "Disp.fVal-MHillasSrc.fDist*MGeomCam.fConvMm2Deg";

    MHn hres1("Disp1", "Xi Residual (Dist/Disp)");
    hres1.AddHist("MHillas.fSize", res);
    hres1.InitName("ResSize;Size;ResidualDist");
    hres1.InitTitle(";S [phe];Disp-Dist [\\circ];");
    hres1.SetDrawOption("colz profx");
    hres1.AddHist("MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/MGeomCam.fConvMm2Deg", res);
    hres1.InitName("ResSlope;Slope;ResidualDist");
    hres1.InitTitle(";Slope;Disp-Dist [\\circ];");
    hres1.SetDrawOption("colz profx");
    hres1.AddHist("MNewImagePar.fLeakage1", res);
    hres1.InitName("ResLeak;Leakage;ResidualDist");
    hres1.InitTitle(";Leak;Disp-Dist [\\circ];");
    hres1.SetDrawOption("colz profx");
    hres1.AddHist("MPointingPos.fZd", res);
    hres1.InitName("ResTheta;Theta;ResidualDist");
    hres1.InitTitle(";Zd [\\circ];Disp-Dist [\\circ];");
    hres1.SetDrawOption("colz profx");

    MHn hres2("Disp2", "Dist Residual (Disp-Dist)");
    hres2.AddHist("MHillas.fLength*MGeomCam.fConvMm2Deg", res);
    hres2.InitName("ResLength;Length;ResidualDist");
    hres2.InitTitle(";L [\\circ];Disp-Dist [\\circ];");
    hres2.SetDrawOption("colz profx");
    hres2.AddHist("MNewImagePar.fConc1", res);
    hres2.InitName("ResConc1;Conc;ResidualDist");
    hres2.InitTitle(";C;Disp-Dist [\\circ];");
    hres2.SetDrawOption("colz profx");
    hres2.AddHist("MHillas.fWidth*MGeomCam.fConvMm2Deg", res);
    hres2.InitName("ResWidth;Width;ResidualDist");
    hres2.InitTitle(";W [\\circ];Disp-Dist [\\circ];");
    hres2.SetDrawOption("colz profx");
    hres2.AddHist("MMcEvt.fEnergy", res);
    hres2.InitName("ResEmc;Energy;ResidualDist");
    hres2.InitTitle(";E_{mc} [GeV];Disp-Dist [\\circ];");
    hres2.SetDrawOption("colz profx");

    MH3 hdisp1("MHillas.fSize",  "ThetaSquared.fVal");
    MH3 hdisp2("MMcEvt.fEnergy", "ThetaSquared.fVal");
    hdisp1.SetName("Size;Size;ThetaSq");
    hdisp2.SetName("Energy;Energy;ThetaSq");
    hdisp1.SetTitle("\\vartheta distribution vs. Size:Size [phe]:\\vartheta^2 [\\circ]");
    hdisp2.SetTitle("\\vartheta distribution vs. Energy:Energy [GeV]:\\vartheta^2 [\\circ]");

    // -------------- Setup fill tasks ----------------

    MFillH fillh(&hist, "", "FillThetaSq");
    MFillH fillh2a(&hres1, "", "FillResiduals1");
    MFillH fillh2b(&hres2, "", "FillResiduals2");
    MFillH fillh2c(&hdisp1, "", "FillSize");
    MFillH fillh2d(&hdisp2, "", "FillEnergy");
    fillh2c.SetBit(MFillH::kDoNotDisplay);
    fillh2d.SetBit(MFillH::kDoNotDisplay);

    // --------------- Setup weighting -------------------

    if (fEnableWeights)
    {
        fillh.SetWeight();
        fillh2a.SetWeight();
        fillh2b.SetWeight();
        fillh2c.SetWeight();
        fillh2d.SetWeight();
        eval.SetNameWeight();
    }

    // --------------- Setup tasklist -------------------

    tlist.AddToList(&readtst);
    tlist.AddToList(fPreTasks);
    tlist.AddToList(&cont);
    tlist.AddToList(&rf);
    tlist.AddToList(&calcthetasq);
    tlist.AddToList(fPostTasks);
    tlist.AddToList(&fillh);
    tlist.AddToList(&fillh2a);
    tlist.AddToList(&fillh2b);
    tlist.AddToList(&fillh2c);
    tlist.AddToList(&fillh2d);
    tlist.AddToList(fTestTasks);
    tlist.AddToList(&eval);

    // ------------- Setup/run eventloop -----------------

    MEvtLoop loop(fTitle);
    loop.SetLogStream(fLog);
    loop.SetDisplay(fDisplay);
    loop.SetParList(&plist);
    //if (!SetupEnv(loop))
    //    return kFALSE;

    if (!loop.Eventloop())
        return kFALSE;

    // ---------------- Prepare result -------------------

    // Print the result
    *fLog << inf;
    *fLog << "Rule: " << rule << endl;
    *fLog << "Disp: " << fTrainParameter << endl;
    hist.GetAlphaFitter().Print("result");

    // The user has closed the display
    if (!fDisplay)
        return kTRUE;

    DisplayResult(hdisp1, hdisp2);

    TObjArray arr;
    arr.Add(const_cast<MDataSet*>(&set));
    if (fDisplay)
        arr.Add(fDisplay);

    SetPathOut(out);
    return WriteContainer(arr, 0, "UPDATE");
}

/*
#include "MParameterCalc.h"
#include "MHillasCalc.h"
#include "../mpointing/MSrcPosRndm.h"

Bool_t MJTrainDisp::TrainGhostbuster(const char *out, const MDataSet &set, Int_t num)
{
    SetTitle(Form("TrainGhostbuster: %s", out));

    if (fDisplay)
        fDisplay->SetTitle(fTitle);

    if (!set.IsValid())
    {
        *fLog << err << "ERROR - DataSet invalid!" << endl;
        return kFALSE;
    }

    if (!HasWritePermission(out))
        return kFALSE;

    *fLog << inf;
    fLog->Separator(GetDescriptor());

    // --------------------- Setup files --------------------
    MReadMarsFile readtrn("Events");
    MReadMarsFile readtst("Events");
    readtrn.DisableAutoScheme();
    readtst.DisableAutoScheme();

    if (!set.AddFilesOn(readtrn))
        return kFALSE;
    if (!set.AddFilesOff(readtst))
        return kFALSE;

    // ----------------------- Setup Matrix ------------------
    MHMatrix train("Train");
    train.AddColumns(fRules);
    if (fEnableWeights)
        train.AddColumn("MWeight.fVal");
    train.AddColumn("sign(MHillasSrc.fCosDeltaAlpha)==sign(SignStore.fVal)");

    MParameterCalc calc("MHillasSrc.fCosDeltaAlpha", "SignStore");
    calc.SetNameParameter("SignStore");

    MSrcPosRndm rndm;
    rndm.SetRule(fTrainParameter);
    //rndm.SetDistOfSource(120*3.37e-3);

    MHillasCalc hcalc;
    hcalc.SetFlags(MHillasCalc::kCalcHillasSrc);

    // ----------------------- Fill Matrix RF ----------------------
    MTFillMatrix fill(fTitle);
    fill.SetDisplay(fDisplay);
    fill.SetLogStream(fLog);
    fill.SetDestMatrix1(&train, num);
    fill.SetReader(&readtrn);
    fill.AddPreCuts(fPreCuts);
    fill.AddPreCuts(fTrainCuts);
    fill.AddPreTasks(fPreTasks);
    fill.AddPostTasks(fPostTasks);
    fill.AddPostTask(&calc);
    fill.AddPostTask(&rndm);
    fill.AddPostTask(&hcalc);
    if (!fill.Process())
        return kFALSE;

    // ------------------------ Train RF --------------------------
    MRanForestCalc rf("TrainGhostbuster", fTitle);
    rf.SetNumTrees(fNumTrees);
    rf.SetNdSize(fNdSize);
    rf.SetNumTry(fNumTry);
    rf.SetNumObsoleteVariables(1);
    rf.SetLastDataColumnHasWeights(fEnableWeights);
    rf.SetDisplay(fDisplay);
    rf.SetLogStream(fLog);
    rf.SetFileName(out);
    rf.SetDebug(fDebug>1);
    rf.SetNameOutput("Sign");

    if (!rf.TrainRegression(train))                  // regression (best choice)
        return kFALSE;

    // --------------------- Display result ----------------------

    gLog.Separator("Test");

    MH::SetPalette("pretty");

    MParList  plist;
    MTaskList tlist;
    plist.AddToList(this);
    plist.AddToList(&tlist);
    //plist.AddToList(&b);

    MFilterList list;
    if (!list.AddToList(fPreCuts))
        *fLog << err << "ERROR - Calling MFilterList::AddToList for fPreCuts failed!" << endl;
    if (!list.AddToList(fTestCuts))
        *fLog << err << "ERROR - Calling MFilterList::AddToList for fTestCuts failed!" << endl;

    MContinue cont(&list);
    cont.SetInverted();

    const char *rule = "abs(Sign.fVal-(sign(MHillasSrc.fCosDeltaAlpha)==sign(SignStore.fVal)))";

    //MChisqEval eval;
    //eval.SetY1("sqrt(ThetaSquared.fVal)");

    MH3 hsign1("MHillas.fSize",  rule);
    MH3 hsign2("MMcEvt.fEnergy", rule);
    hsign1.SetTitle("Was ist das? vs. Size:Size [phe]:XXX");
    hsign2.SetTitle("Was ist das? vs. Energy:Enerhy [GeV]:XXXX");

    MBinning binsx( 70, 10, 31623, "BinningMH3X", "log");
    MBinning binsy( 51,  0,     1, "BinningMH3Y", "lin");

    plist.AddToList(&binsx);
    plist.AddToList(&binsy);

    MFillH fillh2a(&hsign1, "", "FillSize");
    MFillH fillh2b(&hsign2, "", "FillEnergy");
    fillh2a.SetDrawOption("profx colz");
    fillh2b.SetDrawOption("profx colz");
    fillh2a.SetNameTab("Size");
    fillh2b.SetNameTab("Energy");

    tlist.AddToList(&readtst);
    tlist.AddToList(&cont);
    tlist.AddToList(&calc);
    tlist.AddToList(&rndm);
    tlist.AddToList(&hcalc);
    tlist.AddToList(&rf);
    tlist.AddToList(&fillh2a);
    tlist.AddToList(&fillh2b);
    //tlist.AddToList(&eval);

    MEvtLoop loop(fTitle);
    loop.SetLogStream(fLog);
    loop.SetDisplay(fDisplay);
    loop.SetParList(&plist);
    //if (!SetupEnv(loop))
    //    return kFALSE;

    if (!loop.Eventloop())
        return kFALSE;

    // Print the result
    *fLog << inf << "Rule: " << rule << endl;

    //DisplayResult(hdisp1, hdisp2);

    SetPathOut(out);
    if (!WriteDisplay(0, "UPDATE"))
        return kFALSE;

    return kTRUE;
}
*/
