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
!   Author(s): Thomas Bretz, 8/2010 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// traincuts.C
// ===========
//
// For more details see the class description of MJTrainCuts
//
/////////////////////////////////////////////////////////////////////////////
void traincuts()
{
    // Instantiate class
    MJTrainCuts opt;
    //opt.SetDebug();

    // These are control parameters how to train the random forest (use with care!)
    //opt.SetNumTrees(100);
    //opt.SetNdSize(5);
    //opt.SetNumTry(0);

    // Setup datasets
    MDataSet seton ("dataset_on.txt");
    MDataSet setoff("dataset_off.txt");

    opt.SetDataSetOn(seton);
    opt.SetDataSetOff(setmix);

    // Define the parameters you want the random forest to use
    Int_t p00 = opt.AddParameter("MHillas.fSize");
    Int_t p01 = opt.AddParameter("MHillas.GetArea*MGeomCam.fConvMm2Deg^2");
    Int_t p02 = opt.AddParameter("MHillasSrc.fDist*MGeomCam.fConvMm2Deg");
    Int_t p03 = opt.AddParameter("MHillasExt.fTimeSpreadWeighted");
    Int_t p04 = opt.AddParameter("MHillasExt.fSlopeSpreadWeighted");

    Int_t p05 = opt.AddParameter("MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/MGeomCam.fConvMm2Deg");
    Int_t p06 = opt.AddParameter("MHillasExt.fM3Long*sign(MHillasSrc.fCosDeltaAlpha)*MGeomCam.fConvMm2Deg");

    // A selection of more parameters
    //    Int_t p07 = opt.AddParameter("MNewImagePar.fLeakage1");
    //    Int_t p08 = opt.AddParameter("MNewImagePar.fLeakage2");
    //    Int_t p09 = opt.AddParameter("MNewImagePar.fConcCOG");
    //    Int_t p10 = opt.AddParameter("MNewImagePar.fConcCore");
    //    Int_t p11 = opt.AddParameter("MNewImagePar.fConc1");
    //    Int_t p12 = opt.AddParameter("MNewImagePar.fUsedArea*MGeomCam.fConvMm2Deg^2");
    //    Int_t p13 = opt.AddParameter("MNewImagePar.fCoreArea*MGeomCam.fConvMm2Deg^2");

    // Define the binning for all the parameters
    //    opt.AddBinning(p00, MBinning(40,   10, 10000, "", "log"));  // Size
    //    opt.AddBinning(p01, MBinning(40,    0, 0.3));               // Area
    //    opt.AddBinning(p02, MBinning(50,    0, 2.5));               // Dist
    //    opt.AddBinning(p03, MBinning(50,    0, 2.5));               // TimeSpreadW
    //    opt.AddBinning(p04, MBinning(50,    0, 1.0));               // SlopeSpreadW
    //    opt.AddBinning(p05, MBinning(51,  -12,  12));               // SlopeLong
    //    opt.AddBinning(p06, MBinning(51, -0.5, 0.5));               // M3Long
    //    opt.AddBinning(p07, MBinning(50,    0, 0.5));               // Leakage1
    //    opt.AddBinning(p08, MBinning(50,    0, 0.5));               // Leakage2
    //    opt.AddBinning(p09, MBinning(50,    0,   1));               // ConcCOG
    //    opt.AddBinning(p10, MBinning(50,    0,   1));               // ConcCore
    //    opt.AddBinning(p11, MBinning(50,    0,   1));               // Conc1
    //    opt.AddBinning(p12, MBinning(50,    0, 0.3));               // UsedArea
    //    opt.AddBinning(p13, MBinning(50,    0, 0.3));               // CoreArea

    // Define the histograms you want to see by AddHist(x[,y[,z]]);
    opt.AddHist(p00, p01);    // Area vs Size
    opt.AddHist(p02, p03);    // TimeSpreadW vs Dist
    opt.AddHist(p02, p04);    // SlopeSpreadW vs Dist
    // opt.AddHist(p09);;          // ConcCOG

    // Define additional cuts which are applied after reading the data
    //opt.AddPreCut("MNewImagePar.fConcCOG>0.1");
    //opt.AddPreCut("MHillasSrc.fDist*MGeomCam.fConvMm2Deg<2.0");

    // -------------------- Run ----------------------------

    MStatusDisplay *d = new MStatusDisplay;
    opt.SetDisplay(d);

    // ------------------------------------------------------

    // Use random forest regression method
    // opt.EnableRegression();

    // Allows overwrite of existing files
    // opt.SetOverwrite();
    opt.Process("rf-cuts.root");
}
