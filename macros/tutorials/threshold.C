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
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */


void threshold()
{
    //
    // This macro fill the container MHMcEnergies using the task
    // MMcThresholdCalc and shows the results.
    //
    MParList parlist;

    MTaskList tasklist;
    parlist.AddToList(&tasklist);

    // Setup reading task
    MReadMarsFile read("Events");
    read.DisableAutoScheme();

    // Setup files to read or monte carlo dataset to be read
    MDataSet set("~/Software/montecarlo-onoff-lza.txt");
    set.AddFilesOn(read);

    // If you want to get the threshold after cuts setup your cuts
    TArrayD cuts(8);
    cuts[0]=  1.3245;
    cuts[1]=  0.22;
    cuts[2]=  0.215;
    cuts[3]=  5.486;
    cuts[4]=  0.0897;
    cuts[5]= -0.07;
    cuts[6]=  8.2957;
    cuts[7]=  0.8677;

    MFMagicCuts mcuts;
    mcuts.SetThetaCut(MFMagicCuts::kOn);
    mcuts.SetHadronnessCut(MFMagicCuts::kArea);
    mcuts.SetVariables(cuts);

    MContinue cont(&mcuts);
    cont.SetInverted();

    // Setup the threshold histogram to be filled
    MFillH fill("MHThreshold", "", "FillThreshold");
    fill.SetNameTab("Threshold");

    // If you like to change your input spectrum use this...
    MMcSpectrumWeight weight;
    weight.SetNewSlope(-3.5);
    fill.SetWeight();

    // Setup your tasklist
    tasklist.AddToList(&read);     // read data
    tasklist.AddToList(&cont);     // skip background events
    tasklist.AddToList(&weight);   // caluclate spectral weight
    tasklist.AddToList(&fill);     // fill histogram

    // Setup the display
    MStatusDisplay *d = new MStatusDisplay;

    // Setup your eventloop
    MEvtLoop loop("Threshold");
    loop.SetParList(&parlist);
    loop.SetDisplay(d);

    // Start your analysis
    if (!loop.Eventloop())
        return;
}
