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
!   Author(s): Thomas Bretz, 4/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// derotatedc.C
// ============
//
// Derotate a MCamEvent and fill a histogram with derotated data
// (sky-plot).
//
// As an input you need a merpped root file conataining DC information
// from a camera report file. To be able to derotate you also need
// aproproitate time-stamps and the corresponding pointing information.
//
// All events are filled into a 2D histograms - derotated.
//
// The example shows the usage of MHCamEventRot. The output is the derotated
// sky-plot.
//
/////////////////////////////////////////////////////////////////////////////

void derotatedc()
{
    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    // Define Observatory location (for derotation)
    MObservatory obs;
    plist.AddToList(&obs);

    // Set the camera geometry (for histogram size)
    MGeomCamMagic cam;
    plist.AddToList(&cam);

    // setup pointing position
    MPointingPos ppos;
    ppos.SetSkyPosition(MAstro::Hms2Hor(5, 34, 31.9), MAstro::Dms2Deg(22, 0, 52.0));
    plist.AddToList(&ppos);

    // Define which file to read
    MReadTree read("Currents", "../dc.root");
    read.DisableAutoScheme();

    // Derotated histogram to fill
    MHCamEventRot hist;

    // Set name of time container corresponding to your data
    hist.SetNameTime("MTimeCurrents");

    // Setup fill task
    MFillH fill(&hist, "MCameraDC");

    // Set a draw option for your 2D histogram
    //fill.SetDrawOption("colz");

    // Setup tasklist
    tlist.AddToList(&read);
    tlist.AddToList(&fill);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    MStatusDisplay *d = new MStatusDisplay;
    evtloop.SetDisplay(d);

    //
    // Execute your analysis
    //
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();
}
