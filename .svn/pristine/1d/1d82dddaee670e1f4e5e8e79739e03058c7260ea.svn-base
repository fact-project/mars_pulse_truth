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
!   Author(s): Thomas Bretz et al, 08/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */


void starplot(const char *filename="Gamma_*.root")
{
    //
    // This is a demonstration program which plots the Hillas
    // parameter from a file created with star.C
    //

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

    MGeomApply geomapl;

    MFillH hfill1("MHHillas", "MHillas");
    MFillH hfill2("MHHillasExt");
    MFillH hfill3("MHStarMap", "MHillas");
    MFillH hfill4("HistExtSource [MHHillasExt]", "MHillasSrc");
    MFillH hfill5("HistSource [MHHillasSrc]", "MHillasSrc");

    tlist.AddToList(&read);
    tlist.AddToList(&geomapl);
    tlist.AddToList(&hfill1);
    tlist.AddToList(&hfill2);
    tlist.AddToList(&hfill3);
    tlist.AddToList(&hfill4);
    tlist.AddToList(&hfill5);

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
}

