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
!   macro comprob.C
!
!   Author(s): Abelardo Moralejo
!   Author(s): Thomas Bretz, 2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
//  This macro demonstrates one way of gamma hadron separation using the
//  composite probability method. To use it you need a star-file (which is
//  a file ouput by the star.C macro containing image parameters)
//
///////////////////////////////////////////////////////////////////////////
void comprob()
{
    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    // Create task list and add it to the parameter list:
    MTaskList tlist;
    plist.AddToList(&tlist);

    // First task in list: read the star file (reference data)
    MReadMarsFile read("Events", "star.root");
    read.DisableAutoScheme();
    tlist.AddToList(&read);

    //
    // Task to do the composite analysis of Hillas parameters:
    // Add(rule, number of bins, lower limit, upper limit)
    // For the meaning of the rule see MDataChain
    //
    MHCompProb compprob(500);
    compprob.Add("MHillas.fWidth",              500,    0, 160);
    compprob.Add("MHillas.fLength",             500,    0, 300);
    compprob.Add("abs(MHillas.fAsym)",          500,    0, 400);
    compprob.Add("MHillasSrc.fDist",            500,    0, 400);
    compprob.Add("abs(MHillasSrc.fHeadTail)",   500,    0, 400);
    compprob.Add("abs(MHillas.fM3Long)",        500,    0, 300);
    compprob.Add("abs(MHillas.fM3Trans)",       500,    0, 150);
    compprob.Add("MHillas.fConc1",              500,    0, 0.6);
    compprob.Add("MHillas.fConc",               500,    0, 0.9);
    compprob.Add("log10(MHillas.fSize)",        500,    0,   5);

    plist.AddToList(&compprob);

    MFillH fill(&compprob, "MMcEvt");

    // Use this if you want to do it for a fixed energy range:
    /*
     MF filter("MMcEvt.fEnergy < 100");
     fill->SetFilter(&filter);
     tlist.AddToList(&filter);
     */

    tlist.AddToList(&fill);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    // Loop over all data to fill the "normal" histograms:
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    gLog.SetDebugLevel(2);

    // Loop to fill the variable bin histograms:
    read.SetEventNum(0);
    if (!evtloop.Eventloop())
        return;

    // ------------------------------------------------------------------

    // Create task list and replace the old task list with the new one
    MTaskList tlist2;
    plist.Replace(&tlist2);

    // First task in list: read star file (test data)
    MReadMarsFile read2("Events", "star2.root");
    read2.DisableAutoScheme();
    tlist2.AddToList(&read2);

    // create task to calculate composite probabilities
    MCompProbCalc calc;
    tlist2.AddToList(&calc);

    // fill probabilities (hadroness) into histogram
    MFillH fill2("MHHadronness");
    tlist2.AddToList(&fill2);

    // Loop to fill the hadronness histograms:
    if (!evtloop.Eventloop())
        return;

    //
    // Display the hadroness histograms and print some informations on
    // the console window
    //
    plist.FindObject("MHHadronness")->DrawClone();
    plist.FindObject("MHHadronness")->Print();
}

