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
// traindisp.C
// ===========
//
// Trains a random forest for disp estimation.
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

void traindisp()
{
    MDataSet set("mcsponde/mcdataset-for-training.txt");
    set.SetNumAnalysis(1);            // Necessary

    // alternatively use:
    //MDataSet set("mcctesttrain.txt", "/magic/montacrlo/sequences", "/magic/montecarlo/star");

    MJTrainDisp opt;
    //opt.SetDebug();

    // These are control parameters how to train the random forest (use with care!)
    //opt.SetNumTrees(100);
    //opt.SetNdSize(5);
    //opt.SetNumTry(0);

    // --- Change the theta-cut for the displayed cut-effieincy ---
    //opt.SetThetaCut(0.16);

    // ------- Parameters to train Random Forest --------
    opt.AddParameter("MHillas.fLength");
    opt.AddParameter("MHillas.fWidth");
    opt.AddParameter("MHillas.fSize");
    opt.AddParameter("MNewImagePar.fLeakage1");
    opt.AddParameter("MPointingPos.fZd");
    opt.AddParameter("MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)");
    //opt.AddParameter("MNewImagePar.fLeakage2");
    //opt.AddParameter("MNewImagePar.fConc");
    //opt.AddParameter("MNewImagePar.fConc1");
    //opt.AddParameter("MNewImagePar.fUsedArea");
    //opt.AddParameter("MNewImagePar.fCoreArea");
    //opt.AddParameter("MNewImagePar.fNumUsedPixels");
    //opt.AddParameter("MNewImagePar.fNumCorePixels");
    //opt.AddParameter("MImagePar.fSizeSinglePixels");
    //opt.AddParameter("MHillasExt.fM3Long*sign(MHillasSrc.fCosDeltaAlpha)");
    //opt.AddParameter("MHillasExt.fAsym*sign(MHillasSrc.fCosDeltaAlpha)");

    // -------------------- Run ----------------------------

    MStatusDisplay *d = new MStatusDisplay;
    opt.SetDisplay(d);

    /*
     // -------------------- Magic-Cuts ----------------------
     MFMagicCuts cuts;
     cuts.SetHadronnessCut(MFMagicCuts::kArea);
     cuts.SetThetaCut(MFMagicCuts::kNone);

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

     opt.AddTestCut(&cuts);

     // -------------------- Other cuts ----------------------
     opt.AddPreCut("MNewImagePar.fLeakage1<0.0001");
     opt.AddPreCut("MHillas.fSize>200");
     opt.AddPreCut("MHillasSrc.fDCA*MGeomCam.fConvMm2Deg<0.3");
     opt.AddPreCut("MPointingPos.fZd<25");
     opt.AddTestCut("MHillasExt.fM3Long   *sign(MHillasSrc.fCosDeltaAlpha)*MGeomCam.fConvMm2Deg>-0.07");
     opt.AddTestCut("MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/MGeomCam.fConvMm2Deg<(MHillasSrc.fDist*MGeomCam.fConvMm2Deg-0.5)*7.2");

     // ---------------------- Histogram  --------------------
     MHn hist("MyHist", "Dist Residual (Disp-Dist)");

     MBinning bins(50, -0.5, 0.5);

     hist.AddHist("MHillasExt.fM3Long*sign(MHillasSrc.fCosDeltaAlpha)*3.37e-3", "Disp.fVal-MHillasSrc.fDist*3.37e-3");
     hist.InitName("ResM3l;M3Long;ResidualDist");
     hist.InitTitle(";M3l [\\circ];Disp-Dist [\\circ];");
     hist.SetDrawOption("colz profx");
     hist.SetBinnings(&bins);

     hist.AddHist("MHillasExt.fAsym*sign(MHillasSrc.fCosDeltaAlpha)*3.37e-3", "Disp.fVal-MHillasSrc.fDist*3.37e-3");
     hist.InitName("ResAsym;M3Long;ResidualDist");
     hist.InitTitle(";Asym [\\circ];Disp-Dist [\\circ];");
     hist.SetDrawOption("colz profx");
     hist.SetBinnings(&bins);

     MFillH fill(&hist, "", "FillMyHist");
     //fill.SetWeight(); // Enable weights to be used to fill this histogram
     opt.AddTestTask(&fill);

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

     // ------- A trick for testing the parametrization -------
     // This will overwrite the random forest calculated disp.
     // Therefore you can test our disp-parametrization.
     MParameterCalc calc("(1.15136 +"
                         "0.0681437*MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/3.37e-3 +"
                         "0.0507821*(log10(MHillas.fSize)-1.51279)^2*(log10(MHillas.fSize)>1.51279) +"
                         "2.62932*MNewImagePar.fLeakage1)*"
                         "(1-MHillas.fWidth/MHillas.fLength)");
     calc.SetNameParameter("Disp");
     opt.AddPostTask(&calc);

     // ------------------------------------------------------
    */

     // To allow overwrite of existing files
     // opt.SetOverwrite();

     // The number is the number of events read from the file
     // randomly and is the number of events before cuts
     opt.Train("rf-disp.root", set, 30000);
}
