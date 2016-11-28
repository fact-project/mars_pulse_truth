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
!   Author(s): Thomas Bretz, 12/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// evtrate.C
// =========
//
// Example macro how to calulate the eventrate (per event) and display
// the result versus time.
//
// As an input you need a merpped raw-data file containing correct times.
// The output is the plot: Eventrate vs. time.
//
///////////////////////////////////////////////////////////////////////////

void evtrate()
{
    // Setup parameter- and tasklist
    MParList plist;
    MTaskList tlist;
    plist.AddToList(&tlist);

    // Setup reading task
    MReadMarsFile read("Events");
    read.DisableAutoScheme();
    read.AddFile("test-time.root");

    // Setup event rate calculator
    MEventRateCalc calc;
    // Setup number of events to be averaged
    calc.SetNumEvents(200);

    // Setup histogram to be filles with rate
    MHVsTime rate("MEventRate.fRate");

    // Setup task to fill the histogram
    MFillH fill(&rate, "MTime");

    // Setup tasklist
    tlist.AddToList(&read);
    tlist.AddToList(&calc);
    tlist.AddToList(&fill);

    // Execute your eventloop
    MEvtLoop loop;
    loop.SetParList(&plist);

    if (!loop.Eventloop())
        return;

    // print some execution statistics
    tlist.PrintStatistics();

    // Draw result
    rate.DrawClone();
}
