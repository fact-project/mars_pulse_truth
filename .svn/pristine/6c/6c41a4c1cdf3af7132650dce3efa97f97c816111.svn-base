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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// sectorvstime.C
// ==============
//
// In this example we plot the mean content of the right and left half of
// camera. As an input we use a class derived from MCamEvent. Here this
// are dc currents read directly from a camera control report file.
//
// The output are two histograms one for each half.
//
/////////////////////////////////////////////////////////////////////////////

void sectorvstime()
{
    // Initialize Mars environment
    MParList  plist;
    MTaskList tlist;
    plist.AddToList(&tlist);

    // Create Magic camera geometry
    MGeomCamMagic cam;
    plist.AddToList(&cam);

    // Which DC file to read?
    MReportFileRead read("/data/MAGIC/Period013/cacodata/2004_01_26/dc_2004_01_26_05_35_10_12117_OffMrk421-1.txt");
    read.SetHasNoHeader();
    read.AddToList("MReportCurrents");

    // Initialize histogram
    MHSectorVsTime hist1;
    hist1.SetNameTime("MTimeCurrents");

    // Define sectors you want to display the mean from
    TArrayI s0(3);
    s0[0] = 6;
    s0[1] = 1;
    s0[2] = 2;

    // Define area index [0=inner, 1=outer]
    TArrayI inner(1);
    inner[0] = 0;

    // Don't call this if you want to have all sectors
    hist1.SetSectors(s0);

    // Don't call this if you want to have all area indices
    hist1.SetAreaIndex(inner);

    // Task to fill the histogram
    MFillH fill1(&hist1, "MCameraDC");

    // Also fill a histogram with the mean of all pixels
    MHCamEvent hist2;
    MFillH fill2(&hist2, "MCameraDC");

    // Setup Tasklist
    tlist.AddToList(&read);
    tlist.AddToList(&fill1);
    tlist.AddToList(&fill2);

    // Setup Eventloop
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    // Run Eventloop
    if (!evtloop.Eventloop())
        return;

    // Print some statistics
    tlist.PrintStatistics();

    // Draw clones of the histograms
    hist1.DrawClone();
    hist2.DrawClone();
}
