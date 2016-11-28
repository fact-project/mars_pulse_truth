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
!   Author(s): Thomas Bretz, 11/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== *//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// trainenergy.C
// =============
//
// Trains a random forest for energy estimation.
//
// The additional code below shows how the MagicCuts can be used in
// training and testing. There is also code which allows to change the
// slope of the input spectrum. If a min- or max-break energy is given
// the new slope is only applied in this region. Further possibilities
// for additional cuts are shown.
//
// SequencesOn:   Monte Carlo Sequences used for training
// SequencesOff:  Monte Carlo Sequences used for testing
//
// Use:
//  opt.AddPreCut    to use cut for test and training
//  opt.AddTestCut   to use cut for test only
//  opt.AddTrainCut  to use cut for train only
//  opt.AddPreTask   a task executed before filling the matrix/evaluating the RF
//  opt.AddPostTask  a task executed before training, after evaluating the RF
//
/////////////////////////////////////////////////////////////////////////////
void trainenergy()
{
    MDataSet set("mcsponde/mcdataset-for-training.txt");
    set.SetNumAnalysis(1);            // Necessary

    // alternatively use:
    //MDataSet set("mcctesttrain.txt", "/magic/montacrlo/sequences", "/magic/montecarlo/star");

    MJTrainEnergy opt;
    //opt.SetDebug();

    // These are control parameters how to train the random forest (use with care!)
    //opt.SetNumTrees(100);
    //opt.SetNdSize(5);
    //opt.SetNumTry(0);

    // ------- Parameters to train Random Forest --------
    // The following parameters are the best parameters
    opt.AddParameter("MHillas.fSize");
    opt.AddParameter("MHillasSrc.fDist");
    opt.AddParameter("MPointingPos.fZd");
    opt.AddParameter("MNewImagePar.fLeakage1");
    opt.AddParameter("MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)");
    // The influence of these parameters is unclear
    //opt.AddParameter("MHillasExt.fM3Long*sign(MHillasSrc.fCosDeltaAlpha)");
    //opt.AddParameter("MNewImagePar.fConcCOG");
    //opt.AddParameter("MHillas.GetArea");
    //opt.AddParameter("MNewImagePar.fConc1");
    //opt.AddParameter("MNewImagePar.fConc");
    //opt.AddParameter("MNewImagePar.fUsedArea");
    //opt.AddParameter("MNewImagePar.fCoreArea");
    //opt.AddParameter("MNewImagePar.fLeakage2");

    // Setup how to train the RF. This one gives best results so far
    opt.SetTrainFunc("log(MMcEvt.fEnergy)/log(MHillas.fSize)", "MHillas.fSize^x");
    // opt.SetTrainLog(); // Train in log-energy
    // opt.SetTrainLin(); // Train just in energy

    // -------------------- Run ----------------------------

    MStatusDisplay *d = new MStatusDisplay;
    opt.SetDisplay(d);

    // -------------------- Magic-Cuts ----------------------
    // It is recommended to test with your cuts, but train with all events

    MFMagicCuts cuts;
    cuts.SetHadronnessCut(MFMagicCuts::kArea);
    cuts.SetThetaCut(MFMagicCuts::kOn);

    TArrayD arr(13);
    arr[0]  =  1.15136;
    arr[1]  =  0.215;
    arr[2]  =  0.215468;
    arr[3]  =  5.63973;
    arr[4]  =  0.0836169;
    arr[5]  = -0.07;
    arr[6]  =  7.2;
    arr[7]  =  0.5;
    arr[8]  =  0.0681437;
    arr[9]  =  2.62932;
    arr[10] =  1.51279;
    arr[11] =  0.0507821;

    cuts.SetVariables(arr);

    // opt.AddPreCut(&cuts);    // add cut for training and testing
    // opt.AddTrainCut(&cuts);  // add cut for training only
    opt.AddTestCut(&cuts);   // add cut for testing only

    /*
     // ---------------------- Histogram  --------------------
     MHn hist("MyHist", "Energy Residual (lg E_{est} - lg E_{mc})");

     hist.AddHist("MNewImagePar.fConcCOG", "log10(MEnergyEst.fVal)-log10(MMcEvt.fEnergy)");
     hist.InitName("ResConc;Conc;EnergyResidual");
     hist.InitTitle(";C;\\Delta lg E;");
     hist.SetDrawOption("colz profx");

     MBinning bins(50, 0, 1);
     hist.SetBinnings(&bins);

     MFillH fill(&hist, "", "FillMyHist");
     //fill.SetWeight(); // Enable weights to be used to fill this histogram
     opt.AddTestTask(&fill);

     // -------------------- Other cuts ----------------------
     opt.AddPreCut("MHillasSrc.fDist*MGeomCam.fConvMm2Deg<1.0");
     opt.AddPreCut("MHillas.fSize>200");

     // -------------------- Energy Slope --------------------
     // Note, that weight normally doesn't improve anything here.
     // This is a way to throw away events to a different slope
     MFEnergySlope slope(-4.0); // New slope for mc spectrum
     opt.AddPreCut(&slope);     // throw away events to change slope

     // This is a way to weight the events to a different spectrum
     MMcSpectrumWeight weight;
     weight.SetFormula("pow(X/300, -2.31-0.26*log10(X/300))");
     opt.SetWeights(&weight);

     // ------------------ Zd distribution -------------------
     TFile file("ganymed00001111.root");

     MStatusArray arr;
     if (arr.Read()<=0)
        return;
     TH1D *vstime = (TH1D*)arr.FindObjectInCanvas("Theta",  "TH1D", "OnTime");
     if (!vstime)
         return -1;

     MMcSpectrumWeight weight;
     weight.SetWeightsZd(vstime);
     opt.AddPreTask(&weight);

     // ------------------------------------------------------
    */

     // To allow overwrite of existing files
     // opt.SetOverwrite();

     opt.Train("rf-energy.root", set, 30000);
}
