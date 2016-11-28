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
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */


void threshold(TString filename="data/camera.root", TString outname="")
{
    //
    // This macro fill the container MHMcEnergies using the task
    // MMcThresholdCalc and shows the results.
    //
    MParList parlist;

    MTaskList tasklist;
    parlist.AddToList(&tasklist);

    //
    // Setup the parameter list
    // - You need create the container MHMcEnergies.
    //    + You need to put the number of trigger conditions when
    //      you declarete the MHMcEnergies
    //    + If you don't put any dimension to MHMcEnergies it works
    //      taking only the trigger information from MMcTrig
    //
    const UInt_t numtrigcond = 0;

    UInt_t from = numtrigcond>0 ?           1 : -numtrigcond;
    UInt_t to   = numtrigcond>0 ? numtrigcond : -numtrigcond;

    Int_t dim = to-from+1;

    //
    // Create numtriggerconditions histograms of type MHMcEnergy
    // and store the histograms in an TObjArray
    //
    TObjArray hists(MParList::CreateObjList("MHMcEnergy", from, to));
    hists.SetOwner();

    //
    // Check if the list really contains the right number of histograms
    //
    if (hists.GetEntriesFast() != dim)
        return;

    //
    // Add the histograms to the paramater list.
    //
    parlist.AddToList(&hists);

    //
    // Setup the task list
    // - You need the read and the MMcThresholdCalc tasks
    // - You have to fill the histograms for the Energy threshold
    //    + You need to put the number of trigger conditions when
    //      you declarete the MMcThresholdCalc
    //    + If you don't put any dimension to MMcThresholdCalc it works
    //      like one dimension MMcThresholdCalc
    //
    MReadTree read("Events", filename);
    MMcThresholdCalc calc(numtrigcond);

    tasklist.AddToList(&read);
    tasklist.AddToList(&calc);

    MEvtLoop evtloop;
    evtloop.SetParList(&parlist);

    //
    // Begin the loop (if the loop wasn't succesfull
    // don't try to draw the results
    //
    MProgressBar bar;
    evtloop.SetProgressBar(&bar);
    if (!evtloop.Eventloop())
        return;

    //
    // Now you can display the results
    //
    hists.Print();

    TIter Next(&hists);
    TObject *obj;
    while ((obj=Next()))
        obj->DrawClone();

    // Write histogram to a file in case an output filename has been supplied:
    if (outname.IsNull())
        return;

    TFile f(outname, "recreate");
    if (!f)
        return;

    parlist.FindObject("MHMcEnergy")->Write();
}
