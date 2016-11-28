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
!   Author(s): Thomas Bretz, 5/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Rudy Bock, 5/2002 <mailto:rkb@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
//  plot.C
//  ======
//
//  This macro shows how to fill and display a histogram using Mars
//
//  The advantage of using Mars histograms instead of root-trees is that
//  you can fill values in your histogram which is calculated in the
//  eventloop.
//
//  In this particular sample we fill a histogram with the size parameter
//  of gammas and one with hadron's size. At the end we display both in a
//  single plot.
//
//  The input is a star-macro already conatining image parameters.
//
///////////////////////////////////////////////////////////////////////////

void plot()
{
    // Create a status display for graphical output
    MStatusDisplay *d = new MStatusDisplay;
    d->SetLogStream(&gLog);

    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    //
    // Now setup the tasks and tasklist:
    // ---------------------------------
    //

    // First Task: Read file with image parameters
    // (created with the star.C macro)
    MReadMarsFile  read("Events", "MC_OFF1.root");
    read.AddFile("MC_ON1.root");
    read.DisableAutoScheme();
    tlist.AddToList(&read);

    // Create a filter for Gammas
    MFParticleId fgamma("MMcEvt", '=', MMcEvt::kGAMMA);
    tlist.AddToList(&fgamma);

    // Create a filter for Non-Gammas
    MFParticleId fhadrons("MMcEvt", '!', MMcEvt::kGAMMA);
    tlist.AddToList(&fhadrons);

    // -------------------------------------------------------
    //
    // set the name of the variable to plot and the binning
    //
    TString var("Hillas.fSize");

    MBinning bins("BinningMH3X");
    bins.SetEdgesLog(50, 100, 20000);
    plist.AddToList(&bins);
    //
    // -------------------------------------------------------

    // Create a histogram for the data from gammas and from non-gammas
    MH3 h3g(var);
    MH3 h3h(var);

    // Add the histograms to the parameter container list
    plist.AddToList(&h3g);
    plist.AddToList(&h3h);

    // Create a task which fills one histogram with the gamma-data
    MFillH fillg(&h3g);
    fillg.SetFilter(&fgamma);
    tlist.AddToList(&fillg);

    // Create a task which fills the other histogram with the non-gamma-data
    MFillH fillh(&h3h);
    fillh.SetFilter(&fhadrons);
    tlist.AddToList(&fillh);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //
    // Execute your analysis
    //
    evtloop.SetDisplay(d);
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    // Create a pad, check if MStatusDisplay was not closed meanwhile
    if (evtloop.GetDisplay())
        d->AddTab("Size");
    else
        MH::MakeDefCanvas("Plot");

    // x-axis to logarithmic scale
    gPad->SetLogx();

    // Setup some style options of the two histograms
    // and draw a copy of both
    h3h.GetHist().SetLineColor(kRed);
    MH::DrawCopy(h3h.GetHist(), h3g.GetHist(), "Size");

    // Now create a new histogram, fill it with the division of the
    // two histograms and draw also a copy of it
    TH1D h;
    MH::SetBinning(&h, &bins);
    h.Divide(&h3g.GetHist(), &h3h.GetHist());
    h.SetLineColor(kGreen);
    h.DrawCopy("same");
}
