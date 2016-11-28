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
!   Author(s): Thomas Bretz 8/2010 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MJTrainImpact
//
//
// Example:
// --------
//
//    // SequencesOn are used for training, SequencesOff for Testing
//    MDataSet set("mctesttrain.txt");
//    set.SetNumAnalysis(1);  // Must have a number
//    MJTrainImpact opt;
//    //opt.SetDebug();
//    opt.AddParameter("MHillas.fSize");
//    opt.AddParameter("MHillasSrc.fDist");
//    MStatusDisplay *d = new MStatusDisplay;
//    opt.SetDisplay(d);
//    opt.AddPreCut("MHillasSrc.fDCA*MGeomCam.fConvMm2Deg<0.3");
//    opt.Train("rf-impact.root", set, 30000); // Number of train events
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
#include "MJTrainImpact.h"

#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

// tools
#include "MDataSet.h"
#include "MTFillMatrix.h"
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

// histograms
#include "MHn.h"
#include "MBinning.h"
#include "MHEnergyEst.h"

// filter
#include "MFilterList.h"

ClassImp(MJTrainImpact);

using namespace std;

Bool_t MJTrainImpact::Train(const char *out, const MDataSet &set, Int_t num)
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

    MFilterList list;
    if (!list.AddToList(fPreCuts))
        *fLog << err << "ERROR - Calling MFilterList::AddToList for fPreCuts failed!" << endl;
    if (!list.AddToList(fTestCuts))
        *fLog << err << "ERROR - Calling MFilterList::AddToList for fTestCuts failed!" << endl;

    MContinue cont(&list);
    cont.SetInverted();

    // -------------------------------------------------------------
    MBinning binsS(40,  10,     100000, "BinningSize",             "log");
    MBinning binsE(40,  10,     100000, "BinningEnergy",           "log");
//    MBinning binsF(40,  10,     100000, "BinningEnergyEst",        "log");
    MBinning binsG(90, -14,     14,     "BinningSlope",            "lin");
    MBinning binsR(50,-100,     100,    "BinningImpactResidual",   "lin");
    MBinning binsL(50,   0,     0.3,    "BinningLeakage",          "lin");
    MBinning binsT(51,  -0.005, 0.505,  "BinningTheta",            "asin");
    MBinning binsD(70,   0,     2.4,    "BinningDist",             "lin");
    MBinning binsC(50,   1e-2,  1,      "BinningConc",             "log");
    MBinning binsI(32,   0,     800,    "BinningImpact",           "lin");

    plist.AddToList(&binsG);
    plist.AddToList(&binsS);
    plist.AddToList(&binsR);
    plist.AddToList(&binsE);
//    plist.AddToList(&binsF);
    plist.AddToList(&binsL);
    plist.AddToList(&binsT);
    plist.AddToList(&binsD);
    plist.AddToList(&binsC);
    plist.AddToList(&binsI);

    //MHEnergyEst hist;

    // To speed it up we could precalculate it.
    const char *res = "(MImpactEst.fVal-MMcEvt.fImpact)/100";

    MHn hres1("Impact1", "Impact Residual (I_{est} - I_{mc})");
    hres1.AddHist("MHillas.fSize", res);
    hres1.InitName("ResSize;Size;ImpactResidual");
    hres1.InitTitle(";S [phe];\\Delta I;");
    hres1.SetDrawOption("colz profx");
    hres1.AddHist("MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/MGeomCam.fConvMm2Deg", res);
    hres1.InitName("ResSlope;Slope;ImpactResidual");
    hres1.InitTitle(";Slope;\\Delta I;");
    hres1.SetDrawOption("colz profx");
    hres1.AddHist("MNewImagePar.fLeakage1", res);
    hres1.InitName("ResLeak;Leakage;ImpactResidual");
    hres1.InitTitle(";Leak;\\Delta I;");
    hres1.SetDrawOption("colz profx");
    hres1.AddHist("MHillasSrc.fDist*MGeomCam.fConvMm2Deg", res);
    hres1.InitName("ResDist;Dist;ImpactResidual");
    hres1.InitTitle(";D [\\circ];\\Delta I;");
    hres1.SetDrawOption("colz profx");

    MHn hres2("Impact2", "Impact Residual (I_{est} - I_{mc})");
    hres2.AddHist("MMcEvt.fEnergy", res);
    hres2.InitName("ResEmc;Energy;ImpactResidual");
    hres2.InitTitle(";E_{mc} [m];\\Delta I;");
    hres2.SetDrawOption("colz profx");
    hres2.SetAutoRange(kFALSE, kFALSE, kFALSE);
    hres2.AddHist("MPointingPos.fZd", res);
    hres2.InitName("ResTheta;Theta;ImpactResidual");
    hres2.InitTitle(";Zd [\\circ];\\Delta I;");
    hres2.SetDrawOption("colz profx");
    hres2.AddHist("MImpactEst.fVal/100", res);
    hres2.InitName("ResIest;Impact;ImpactResidual");
    hres2.InitTitle(";I_{est} [m];\\Delta I;");
    hres2.SetDrawOption("colz profx");
    hres2.SetAutoRange(kFALSE, kFALSE, kFALSE);
    hres2.AddHist("MNewImagePar.fConc1", res);
    hres2.InitName("ResConc1;Conc;ImpactResidual");
    hres2.InitTitle(";C;\\Delta I;");
    hres2.SetDrawOption("colz profx");

    MHn hres3("Resolution", "Impact Resolution");
    hres3.AddHist("MImpactEst.fVal/100", "(MMcEvt.fImpact/MImpactEst.fVal-1)^2", MH3::kProfile);
    hres3.InitName("ResIest;Impact;");
    hres3.InitTitle(";I_{est} [m];Resolution   \\sqrt{<(I_{mc}/I_{est}-1)^{2}>};");
    hres3.SetConversion("sqrt(x)");

    hres3.AddHist("MHillas.fSize", "(MMcEvt.fImpact/MImpactEst.fVal-1)^2", MH3::kProfile);
    hres3.InitName("ResSize;Size;");
    hres3.InitTitle(";S [phe];Resolution   \\sqrt{<(I_{mc}/I_{est}-1)^{2}>};");
    hres3.SetConversion("sqrt(x)");
/*
    hres3.AddHist("MMcEvt.fEnergy", "(MImpactEst.fVal/MMcEvt.fImpact-1)^2", MH3::kProfile);
    hres3.InitName("ResEmc;EnergyEst;");
    hres3.InitTitle(";E_{mc} [GeV];Resolution \\sqrt{<(I_{mc}/I_{est}-1)^{2}>};");
    hres3.SetConversion("sqrt(x)");
  */
    hres3.AddHist("MMcEvt.fEnergy", "(MMcEvt.fImpact/MImpactEst.fVal-1)^2", MH3::kProfile);
    hres3.InitName("ResEnergy;Energy;");
    hres3.InitTitle(";E_{mc} [m];Resolution   \\sqrt{<(I_{mc}/I_{est}-1)^{2}>};");
    hres3.SetConversion("sqrt(x)");

    hres3.AddHist("MPointingPos.fZd", "(MMcEvt.fImpact/MImpactEst.fVal-1)^2", MH3::kProfile);
    hres3.InitName("ResTheta;Theta;");
    hres3.InitTitle(";\\Theta [\\circ];Resolution   \\sqrt{<(I_{mc}/I_{est}-1)^{2}>};");
    hres3.SetConversion("sqrt(x)");

    //MFillH fillh0(&hist);
    MFillH fillh1(&hres1, "", "FillResiduals1");
    MFillH fillh2(&hres2, "", "FillResiduals2");
    MFillH fillh3(&hres3, "", "FillResolution");

    if (fEnableWeights)
    {
        //fillh0.SetWeight();
        fillh1.SetWeight();
        fillh2.SetWeight();
        fillh3.SetWeight();
    }

    tlist.AddToList(&readtst);
    tlist.AddToList(fPreTasks);
    tlist.AddToList(&cont);
    tlist.AddToList(&rf);
    tlist.AddToList(fPostTasks);
    //tlist.AddToList(&fillh0);
    tlist.AddToList(&fillh1);
    tlist.AddToList(&fillh2);
    tlist.AddToList(&fillh3);
    tlist.AddToList(fTestTasks);

    MEvtLoop loop(fTitle);
    loop.SetLogStream(fLog);
    loop.SetDisplay(fDisplay);
    loop.SetParList(&plist);
    //if (!SetupEnv(loop))
    //   return kFALSE;

    if (!loop.Eventloop())
        return kFALSE;

    TObjArray arr;
    arr.Add(const_cast<MDataSet*>(&set));
    if (fDisplay)
        arr.Add(fDisplay);

    SetPathOut(out);
    return WriteContainer(arr, 0, "UPDATE");
}
