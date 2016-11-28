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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// MagicHillas.C
// =============
//
// This is a demonstration program which calculates image (Hillas +)
// parameters using as input a Merpp output file (raw data).
// All parameters are written to an output file called hillas.root. Also
// filles histograms are displayed.
// For the calculation an arbitrary signal extractor (MCerPhotAnal2/Calc)
// is used.
//
///////////////////////////////////////////////////////////////////////////

void MagicHillas(const char *filename="~/data/Gamma_20_N*.root")
{
    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    //
    // Use this if you want to change the binning of one of
    // the histograms. You can use:
    // BinningConc, BinningConc1, BinningAsym, BinningM3Long,
    // BinningM3Trans, BinningWidth, BinningLength, BinningDist,
    // BinningHeadTail, BinningAlpha, BinningSize, BinningDelta,
    // BinningPixels and BinningCamera
    //
    // For more information see MBinning and the corresponding
    // histograms
    //
    // MBinning binsalpha("BinningAlpha");
    // binsalpha.SetEdges(90, 0, 90);       // 90 bins from 0 to 90 deg
    // plist.AddToList(&binsalpha);

    // MBinning binssize("BinningSize");
    // binssize.SetEdgesLog(50, 1, 1e7);
    // plist.AddToList(&binssize);

    //
    // Craete the object which hlods the source positions in the camera
    // plain in respect to which the image parameters will be calculated.
    // For real data the containers will be filled by a task.
    //
    MSrcPosCam source;
    source.SetReadyToSave();
    plist.AddToList(&source);

    //
    // Now setup the tasks and tasklist:
    // ---------------------------------
    //
    // The first argument is the tree you want to read.
    //   Events:     Cosmic ray events
    //   PedEvents:  Pedestal Events
    //   CalEvents:  Calibration Events
    //
    MReadMarsFile read("Events", filename);
    read.DisableAutoScheme();

    // Setup a task which makes sure that all used arrays have
    // the correct size
    MGeomApply geomapl;

    // tasks used if MC files are detected to calculate pedestals
    MMcPedestalCopy pcopy;
    MMcPedestalNSBAdd pnsb;

    // calculate the signal in a very simple way
    // for real adat files use MCerPhotAnal2 instead which also
    // calculates the pedestal.
    MCerPhotCalc ncalc;
    //
    //  Alternative photon calculation:
    //  Example: use only 2nd to 6th FADC slices for photon calculation:
    //
    //    const Float_t x[15]={0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    //    const TArrayF w(15,(Float_t*)x);
    //    ncalc.SetWeights(w);
    //

    // setup image cleaning and blind pixel treatment
    MImgCleanStd    clean;
    MBlindPixelCalc blind;

    //
    // Instead of unmapping the pixels you can also (The use of this
    // class is deprecated, it will be replaced by MBadPixels*)
    //
    // blind.SetUseInterpolation();
    // blind.SetUseCetralPixel();
    //

    // setup tasks to calculate image parameters
    MHillasCalc    hcalc;
    MHillasSrcCalc csrc1;

    // setup tasks to fill histograms
    MFillH hfill1("MHHillas", "MHillas");
    MFillH hfill2("MHHillasExt");
    MFillH hfill3("MHStarMap", "MHillas");
    MFillH hfill4("HistExtSource [MHHillasExt]", "MHillasSrc");
    MFillH hfill5("HistSource [MHHillasSrc]", "MHillasSrc");
    MFillH hfill6("MHNewImagePar");

    // setup task to write containers to a file
    MWriteRootFile write("hillas.root");
    write.AddContainer("MHStarMap");
    write.AddContainer("MHHillas");
    write.AddContainer("MHHillasExt");
    write.AddContainer("HistSource");
    write.AddContainer("HistExtSource");
    write.AddContainer("MHNewImagePar");

    // Setup the contents of zour tasklist
    tlist.AddToList(&read);
    tlist.AddToList(&geomapl);
    tlist.AddToList(&pcopy);
    tlist.AddToList(&pnsb);
    tlist.AddToList(&ncalc);
    tlist.AddToList(&clean);
    tlist.AddToList(&blind);

    tlist.AddToList(&hcalc);
    tlist.AddToList(&csrc1);

    tlist.AddToList(&hfill1);
    tlist.AddToList(&hfill2);
    tlist.AddToList(&hfill3);
    tlist.AddToList(&hfill4);
    tlist.AddToList(&hfill5);
    tlist.AddToList(&hfill6);
    tlist.AddToList(&write);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //
    // Execute your analysis
    //
    MProgressBar bar;
    evtloop.SetProgressBar(&bar);
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    //
    // After the analysis is finished we can display the histograms
    //
    plist.FindObject("MHHillas")->DrawClone();
    plist.FindObject("MHHillasExt")->DrawClone();
    plist.FindObject("MHStarMap")->DrawClone();
    plist.FindObject("HistSource")->DrawClone();
    plist.FindObject("HistExtSource")->DrawClone();
    plist.FindObject("MHNewImagePar")->DrawClone();
}

