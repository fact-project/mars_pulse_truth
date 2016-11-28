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
//  plot2.C
//  =======
//
//  This macro shows how to fill and display a histogram using Mars
//
//  The advantage of using Mars histograms instead of root-trees is that
//  you can fill values in your histogram which is calculated in the
//  eventloop.
//
//  In this particular sample we fill a histogram with width vs length
//  of gammas and hadrons. At the end we display both in a single plot.
//
//  The input is a star-macro already conatining image parameters.
//
///////////////////////////////////////////////////////////////////////////

void plot2()
{
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

    // First Task: read in a file created with star.C
    MReadMarsFile  read("Events", "star.root");
    read.DisableAutoScheme();
    tlist.AddToList(&read);

    // Create a filter for the gamma events
    MFParticleId fgamma("MMcEvt", '=', MMcEvt::kGAMMA);
    tlist.AddToList(&fgamma);

    // Create a filter for the non-gamma events
    MFParticleId fhadrons("MMcEvt", '!', MMcEvt::kGAMMA);
    tlist.AddToList(&fhadrons);

    // -------------------------------------------------------
    //
    // set the name of the variable to plot and the binning
    //
    MGeomCamMagic cam;
    plist.AddToList(&cam);

    // Set the variables (converted to deg)
    TString vary("MHillas.fWidth*MGeomCam.fConvMm2Deg");
    TString varx("MHillas.fLength*MGeomCam.fConvMm2Deg");

    // Set the binning
    MBinning binsy("BinningMH3Y");
    MBinning binsx("BinningMH3X");
    binsy.SetEdges(11, 0, 0.3);
    binsx.SetEdges(11, 0, 0.6);
    plist.AddToList(&binsx);
    plist.AddToList(&binsy);
    //
    // -------------------------------------------------

    // Create two 2D histograms and add them to the list
    MH3 h3g(varx, vary);
    MH3 h3h(varx, vary);

    plist.AddToList(&h3g);
    plist.AddToList(&h3h);

    // Create a task to fill one histogram with the gamma data
    MFillH fillg(&h3g);
    fillg.SetFilter(&fgamma);
    tlist.AddToList(&fillg);

    // Create a task to fill the other one with the non gamma data
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
    MProgressBar bar;
    evtloop.SetProgressBar(&bar);
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    // Create a default canvas
    MH::MakeDefCanvas("Plot");

    // setup some style options
    h3h.GetHist().SetMarkerColor(kRed);
    h3h.GetHist().SetLineColor(kRed);
    h3h.GetHist().SetFillStyle(4000);

    // show a contour plot of both histograms
    h3h.GetHist().DrawCopy("cont3");
    h3g.GetHist().DrawCopy("cont3same");

    return;

    //
    // Use this (or something similar) if you want to plot the profile
    // histograms
    //
    TProfile *p = ((TH2&)h3g.GetHist()).ProfileX();
    p->Draw("same");
    p->SetBit(kCanDelete);

    TProfile *p = ((TH2&)h3h.GetHist()).ProfileX();
    p->SetLineColor(kRed);
    p->SetFillStyle(4000);
    p->Draw("same");
    p->SetBit(kCanDelete);
}
